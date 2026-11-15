"use client";

import React from "react";
import { useMQTT } from "../context/MQTTProvider";
import { ScrollText } from "lucide-react";

export const LoggingSection = () => {
  const { events } = useMQTT();

  return (
    <div className="w-full max-w-sm mx-auto mt-8 px-4 mb-8">
      <div className="p-4 bg-[#1a1c23] rounded-2xl shadow-[inset_4px_4px_8px_#121318,inset_-4px_-4px_8px_#22252e]">
        <div className="flex items-center gap-2 mb-3">
          <ScrollText className="text-gray-400" size={16} />
          <h3 className="text-xs font-semibold text-gray-400 tracking-wider">MQTT LOGS</h3>
        </div>
        
        <div className="h-32 overflow-y-auto space-y-2 pr-2 scrollbar-thin scrollbar-thumb-gray-700">
          {events.length === 0 ? (
            <p className="text-xs text-gray-500 italic text-center mt-8">Waiting for events...</p>
          ) : (
            events.map((event, i) => (
              <div key={i} className="text-xs font-mono bg-[#121318] p-2 rounded-lg text-gray-300 shadow-[inset_1px_1px_2px_#0a0a0d]">
                <span className="text-gray-500">[{event.time}]</span>{" "}
                <span className="text-blue-400">Topic:</span> {event.topic}{" "}
                <span className="text-emerald-400 font-bold">{event.payload}</span>
              </div>
            ))
          )}
        </div>
      </div>
    </div>
  );
};
