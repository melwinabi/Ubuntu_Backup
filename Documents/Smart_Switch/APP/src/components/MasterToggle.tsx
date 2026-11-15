"use client";

import React from "react";
import { useMQTT } from "../context/MQTTProvider";
import { Power } from "lucide-react";

export const MasterToggle = () => {
  const { switchState, handleToggle, isConnected } = useMQTT();

  const isON = switchState === "ON";
  const isLoading = switchState === "LOADING";

  return (
    <div className="flex flex-col items-center justify-center py-12">
      <button
        onClick={handleToggle}
        disabled={!isConnected || isLoading}
        className={`relative flex items-center justify-center w-48 h-48 rounded-full transition-all duration-300 ease-in-out ${
          isON
            ? "bg-[#1a1c23] shadow-[inset_8px_8px_16px_#121318,inset_-8px_-8px_16px_#22252e] border-2 border-emerald-500/30 glow-emerald"
            : "bg-[#1a1c23] shadow-[8px_8px_16px_#121318,-8px_-8px_16px_#22252e] border-2 border-transparent"
        } ${isLoading ? "opacity-70 cursor-wait" : "cursor-pointer"} ${
          !isConnected ? "opacity-50 cursor-not-allowed" : ""
        }`}
      >
        <div
          className={`absolute w-36 h-36 rounded-full flex items-center justify-center ${
            isON
              ? "bg-[#1a1c23] shadow-[8px_8px_16px_#121318,-8px_-8px_16px_#22252e]"
              : "bg-[#1a1c23] shadow-[inset_4px_4px_8px_#121318,inset_-4px_-4px_8px_#22252e]"
          }`}
        >
          <Power
            size={48}
            className={`transition-all duration-300 ${
              isON ? "text-emerald-400 drop-shadow-[0_0_8px_rgba(52,211,153,0.8)]" : "text-gray-500"
            } ${isLoading ? "animate-pulse text-amber-400" : ""}`}
          />
        </div>
      </button>

      <div className="mt-8 text-center">
        <h2
          className={`text-2xl font-semibold tracking-wider transition-colors duration-300 ${
            isON ? "text-emerald-400 drop-shadow-[0_0_5px_rgba(52,211,153,0.5)]" : "text-gray-400"
          } ${isLoading ? "text-amber-400" : ""}`}
        >
          {isLoading ? "PROCESSING..." : isON ? "ACTIVE" : "POWER OFF"}
        </h2>
        <p className="text-gray-500 text-sm mt-2">
          {!isConnected ? "Disconnected" : "Tap to toggle state"}
        </p>
      </div>
    </div>
  );
};
