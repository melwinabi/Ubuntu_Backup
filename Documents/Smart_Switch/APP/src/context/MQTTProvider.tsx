"use client";

import React, { createContext, useContext, useEffect, useState, useRef } from "react";
import mqtt, { MqttClient } from "mqtt";

export type SwitchState = "OFF" | "ON" | "LOADING";

export interface MQTTEvent {
  time: string;
  topic: string;
  payload: string;
}

interface MQTTContextType {
  isConnected: boolean;
  switchState: SwitchState;
  events: MQTTEvent[];
  handleToggle: () => void;
  uptime: string;
}

const MQTTContext = createContext<MQTTContextType | undefined>(undefined);

export const MQTTProvider = ({ children }: { children: React.ReactNode }) => {
  const [client, setClient] = useState<MqttClient | null>(null);
  const [isConnected, setIsConnected] = useState(false);
  const [switchState, setSwitchState] = useState<SwitchState>("OFF");
  const [events, setEvents] = useState<MQTTEvent[]>([]);
  const [uptime, setUptime] = useState("0h 0m");
  const uptimeInterval = useRef<NodeJS.Timeout | null>(null);

  // Fallback timer just in case we don't get an ACK from hardware
  const fallbackTimer = useRef<NodeJS.Timeout | null>(null);

  useEffect(() => {
    // Start uptime timer
    const startTime = Date.now();
    uptimeInterval.current = setInterval(() => {
      const diff = Date.now() - startTime;
      const hours = Math.floor(diff / (1000 * 60 * 60));
      const mins = Math.floor((diff / (1000 * 60)) % 60);
      setUptime(`${hours}h ${mins}m`);
    }, 60000); // update every minute

    // Connect to the local MQTT broker over WebSockets
    // Note: Browsers require WebSockets (ws://) instead of raw TCP (mqtt://). Default WS port is often 9001 or 8083.
    const mqttClient = mqtt.connect("ws://192.168.29.79:9001", {
      clientId: `smart_switch_web_${Math.random().toString(16).slice(2, 8)}`,
      keepalive: 30,
      protocolId: "MQTT",
      protocolVersion: 4,
      clean: true,
      reconnectPeriod: 1000,
      connectTimeout: 30 * 1000,
    });

    setClient(mqttClient);

    mqttClient.on("connect", () => {
      setIsConnected(true);
      mqttClient.subscribe("switch/1/state");
    });

    mqttClient.on("reconnect", () => {
      setIsConnected(false);
    });

    mqttClient.on("offline", () => {
      setIsConnected(false);
    });

    mqttClient.on("message", (topic, message) => {
      const payload = message.toString();
      const time = new Date().toLocaleTimeString("en-US", { hour12: false });
      
      setEvents((prev) => {
        const newEvents = [{ time, topic, payload: `-> ${payload}` }, ...prev];
        return newEvents.slice(0, 5); // Keep last 5 events
      });

      if (topic === "switch/1/state") {
        if (payload === "1" || payload === "ON") {
          setSwitchState("ON");
          if (fallbackTimer.current) clearTimeout(fallbackTimer.current);
        } else if (payload === "0" || payload === "OFF") {
          setSwitchState("OFF");
          if (fallbackTimer.current) clearTimeout(fallbackTimer.current);
        }
      }
    });

    return () => {
      if (uptimeInterval.current) clearInterval(uptimeInterval.current);
      if (fallbackTimer.current) clearTimeout(fallbackTimer.current);
      mqttClient.end();
    };
  }, []);

  const handleToggle = () => {
    if (!client || !isConnected) return;

    // Immediately set to LOADING to indicate processing
    const targetState = switchState === "ON" ? "0" : "1";
    setSwitchState("LOADING");

    // Publish command
    client.publish("switch/1/cmd", `CMD: ${targetState}`);

    const time = new Date().toLocaleTimeString("en-US", { hour12: false });
    setEvents((prev) => {
      const newEvents = [{ time, topic: "switch/1/cmd", payload: `<- CMD: ${targetState}` }, ...prev];
      return newEvents.slice(0, 5);
    });

    // Fallback: If hardware doesn't ACK within 3 seconds, revert state
    if (fallbackTimer.current) clearTimeout(fallbackTimer.current);
    fallbackTimer.current = setTimeout(() => {
      setSwitchState((prev) => {
        if (prev === "LOADING") {
          return targetState === "1" ? "OFF" : "ON"; // revert
        }
        return prev;
      });
    }, 3000);
  };

  return (
    <MQTTContext.Provider value={{ isConnected, switchState, events, handleToggle, uptime }}>
      {children}
    </MQTTContext.Provider>
  );
};

export const useMQTT = () => {
  const context = useContext(MQTTContext);
  if (context === undefined) {
    throw new Error("useMQTT must be used within an MQTTProvider");
  }
  return context;
};
