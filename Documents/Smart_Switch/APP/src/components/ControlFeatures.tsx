"use client";

import React, { useState } from "react";
import { Shield, ShieldAlert, Timer } from "lucide-react";

export const ControlFeatures = () => {
  const [childLock, setChildLock] = useState(false);

  return (
    <div className="w-full max-w-sm mx-auto mt-8 px-4 flex flex-col gap-4">
      {/* Safety Lock Toggle */}
      <div className="flex items-center justify-between p-4 bg-[#1a1c23] rounded-2xl shadow-[4px_4px_8px_#121318,-4px_-4px_8px_#22252e]">
        <div className="flex items-center gap-3">
          {childLock ? (
            <ShieldAlert className="text-red-400" size={24} />
          ) : (
            <Shield className="text-emerald-400" size={24} />
          )}
          <div>
            <p className="text-sm font-semibold text-gray-200">Safety Lock</p>
            <p className="text-xs text-gray-500">Disable physical button</p>
          </div>
        </div>
        
        {/* Toggle Switch */}
        <button
          onClick={() => setChildLock(!childLock)}
          className={`relative w-12 h-6 rounded-full transition-colors duration-300 ${
            childLock ? "bg-red-500/80 shadow-[inset_2px_2px_4px_rgba(0,0,0,0.5)]" : "bg-[#121318] shadow-[inset_2px_2px_4px_#0a0a0d,inset_-2px_-2px_4px_#1a1c23]"
          }`}
        >
          <div
            className={`absolute top-1 left-1 w-4 h-4 rounded-full bg-gray-200 transition-transform duration-300 ${
              childLock ? "translate-x-6" : "translate-x-0"
            }`}
          />
        </button>
      </div>

      {/* Timer / Schedule */}
      <div className="flex items-center justify-between p-4 bg-[#1a1c23] rounded-2xl shadow-[4px_4px_8px_#121318,-4px_-4px_8px_#22252e]">
        <div className="flex items-center gap-3">
          <Timer className="text-blue-400" size={24} />
          <div>
            <p className="text-sm font-semibold text-gray-200">Auto-Off Timer</p>
            <p className="text-xs text-gray-500">Set automatic shutdown</p>
          </div>
        </div>
        
        <button className="px-3 py-1 text-xs font-semibold text-gray-300 bg-[#1a1c23] rounded-lg shadow-[2px_2px_4px_#121318,-2px_-2px_4px_#22252e] active:shadow-[inset_2px_2px_4px_#121318,inset_-2px_-2px_4px_#22252e] transition-all">
          Set Timer
        </button>
      </div>
    </div>
  );
};
