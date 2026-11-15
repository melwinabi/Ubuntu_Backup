"use client";

import { MQTTProvider } from "../context/MQTTProvider";
import { MasterToggle } from "../components/MasterToggle";
import { StatusMetadata } from "../components/StatusMetadata";
import { ControlFeatures } from "../components/ControlFeatures";
import { LoggingSection } from "../components/LoggingSection";

export default function Home() {
  return (
    <MQTTProvider>
      <main className="min-h-screen bg-[#1a1c23] text-gray-200 flex flex-col items-center pt-8 pb-4">
        {/* Header */}
        <header className="w-full max-w-sm px-4 mb-4">
          <h1 className="text-2xl font-bold tracking-tight text-gray-100">Smart Relay</h1>
          <p className="text-sm text-gray-500 font-medium">Control Dashboard</p>
        </header>

        {/* Central Switch Area */}
        <div className="w-full max-w-sm px-4 relative">
          <div className="absolute inset-0 bg-gradient-to-b from-[#22252e] to-[#1a1c23] opacity-50 rounded-[40px] pointer-events-none"></div>
          <div className="relative py-8 bg-[#1a1c23] rounded-[40px] shadow-[inset_8px_8px_16px_#121318,inset_-8px_-8px_16px_#22252e]">
            <MasterToggle />
          </div>
        </div>

        {/* Status Indicators */}
        <StatusMetadata />

        {/* Controls */}
        <ControlFeatures />

        {/* System Logs */}
        <LoggingSection />
      </main>
    </MQTTProvider>
  );
}
