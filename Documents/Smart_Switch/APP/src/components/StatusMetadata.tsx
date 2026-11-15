"use client";

import React, { useEffect, useState } from "react";
import { useMQTT } from "../context/MQTTProvider";
import { Clock, Activity, Wifi } from "lucide-react";

export const StatusMetadata = () => {
  const { uptime, events, isConnected } = useMQTT();
  const [lastAction, setLastAction] = useState("Never");

  useEffect(() => {
    if (events.length > 0) {
      const last = events[0];
      setLastAction(`Toggled at ${last.time}`);
    }
  }, [events]);

  return (
    <div className="w-full max-w-sm mx-auto grid grid-cols-3 gap-4 mt-8 px-4">
      {/* Uptime */}
      <div className="flex flex-col items-center justify-center p-3 bg-[#1a1c23] rounded-2xl shadow-[4px_4px_8px_#121318,-4px_-4px_8px_#22252e]">
        <Clock className="text-blue-400 mb-2" size={20} />
        <span className="text-xs text-gray-400 font-medium tracking-wide">UPTIME</span>
        <span className="text-sm text-gray-200 mt-1 font-semibold">{uptime}</span>
      </div>

      {/* Signal Strength */}
      <div className="flex flex-col items-center justify-center p-3 bg-[#1a1c23] rounded-2xl shadow-[4px_4px_8px_#121318,-4px_-4px_8px_#22252e]">
        <Wifi className={`${isConnected ? "text-emerald-400" : "text-red-400"} mb-2`} size={20} />
        <span className="text-xs text-gray-400 font-medium tracking-wide">SIGNAL</span>
        <span className="text-sm text-gray-200 mt-1 font-semibold">
          {isConnected ? "Excellent" : "Offline"}
        </span>
      </div>

      {/* Last Action */}
      <div className="flex flex-col items-center justify-center p-3 bg-[#1a1c23] rounded-2xl shadow-[4px_4px_8px_#121318,-4px_-4px_8px_#22252e]">
        <Activity className="text-purple-400 mb-2" size={20} />
        <span className="text-xs text-gray-400 font-medium tracking-wide">ACTION</span>
        <span className="text-xs text-center text-gray-200 mt-1 font-semibold truncate w-full px-1">
          {lastAction}
        </span>
      </div>
    </div>
  );
};
