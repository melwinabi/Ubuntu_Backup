import { useState, useEffect } from "react"
import mqtt from "mqtt"

export function useMqtt(brokerUrl: string, topics: string[]) {
  const [messages, setMessages] = useState<Record<string, string>>({})
  const [status, setStatus] = useState<"connecting" | "connected" | "error">("connecting")
  const [client, setClient] = useState<mqtt.MqttClient | null>(null)

  useEffect(() => {
    if (!brokerUrl || topics.length === 0) return

    const mqttClient = mqtt.connect(brokerUrl)
    setClient(mqttClient)

    mqttClient.on("connect", () => {
      setStatus("connected")
      if (mqttClient.connected) {
        mqttClient.subscribe(topics, (err) => {
          if (err) {
            console.error("MQTT Subscribe Error:", err)
            setStatus("error")
          }
        })
      }
    })

    mqttClient.on("message", (topic, message) => {
      setMessages((prev) => ({
        ...prev,
        [topic]: message.toString(),
      }))
    })

    mqttClient.on("error", (err) => {
      console.error("MQTT Connection Error:", err)
      setStatus("error")
    })

    return () => {
      mqttClient.end()
    }
  }, [brokerUrl, JSON.stringify(topics)])

  const publish = (topic: string, message: string) => {
    if (client && status === "connected") {
      client.publish(topic, message)
    }
  }

  return { messages, status, publish }
}
