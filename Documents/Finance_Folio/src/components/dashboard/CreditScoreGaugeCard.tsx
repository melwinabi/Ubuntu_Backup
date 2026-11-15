"use client";

import { Gauge, Calendar, AlertCircle } from "lucide-react";
import { format } from "date-fns";

interface CreditScoreGaugeCardProps {
  latestScore: {
    score: number;
    recordedOn: Date;
    bureau: string;
  } | null;
}

export default function CreditScoreGaugeCard({ latestScore }: CreditScoreGaugeCardProps) {
  const score = latestScore?.score ?? 0;
  
  // Determine score quality and color class
  let quality = "N/A";
  let colorClass = "text-slate-500";
  let bgClass = "bg-slate-500/10 border-slate-500/20";
  let scorePercent = 0; // range 300 to 900 maps to 0 to 100%

  if (score > 0) {
    scorePercent = ((score - 300) / 600) * 100;
    if (score >= 750) {
      quality = "Excellent";
      colorClass = "text-green-400";
      bgClass = "bg-green-500/10 border-green-500/20";
    } else if (score >= 700) {
      quality = "Good";
      colorClass = "text-emerald-400";
      bgClass = "bg-emerald-500/10 border-emerald-500/20";
    } else if (score >= 650) {
      quality = "Fair";
      colorClass = "text-amber-400";
      bgClass = "bg-amber-500/10 border-amber-500/20";
    } else {
      quality = "Needs Attention";
      colorClass = "text-red-400";
      bgClass = "bg-red-500/10 border-red-500/20";
    }
  }

  return (
    <div className="glass-card rounded-2xl p-6 flex flex-col justify-between">
      <div className="flex items-center justify-between border-b border-slate-900 pb-4 mb-6">
        <div>
          <h2 className="text-lg font-bold text-slate-100 flex items-center gap-2">
            <Gauge className="w-5 h-5 text-indigo-400" />
            <span>Credit Score</span>
          </h2>
          <p className="text-xs text-slate-400 mt-0.5">Your credit health status.</p>
        </div>
      </div>

      {score === 0 ? (
        <div className="flex flex-col items-center justify-center py-10 text-center text-slate-500">
          <AlertCircle className="w-8 h-8 text-slate-700 mb-2" />
          <p className="text-sm font-medium">No score history</p>
          <p className="text-xs text-slate-600 mt-0.5">Please add a credit score entry to track history.</p>
        </div>
      ) : (
        <div className="flex items-center gap-6">
          {/* Circular progress display */}
          <div className="relative flex items-center justify-center w-24 h-24 rounded-full border-4 border-slate-900">
            {/* Inner ring mapping score percentage */}
            <svg className="w-full h-full transform -rotate-90">
              <circle
                cx="48"
                cy="48"
                r="40"
                className="stroke-slate-950"
                strokeWidth="6"
                fill="transparent"
              />
              <circle
                cx="48"
                cy="48"
                r="40"
                className={`stroke-current ${
                  score >= 750
                    ? "text-green-500"
                    : score >= 700
                    ? "text-emerald-400"
                    : score >= 650
                    ? "text-amber-400"
                    : "text-red-500"
                }`}
                strokeWidth="6"
                fill="transparent"
                strokeDasharray={`${2 * Math.PI * 40}`}
                strokeDashoffset={`${2 * Math.PI * 40 * (1 - scorePercent / 100)}`}
                strokeLinecap="round"
              />
            </svg>
            <div className="absolute flex flex-col items-center justify-center">
              <span className="text-2xl font-bold text-slate-100">{score}</span>
              <span className="text-[9px] font-semibold text-slate-500 uppercase tracking-wider">
                {latestScore?.bureau}
              </span>
            </div>
          </div>

          <div className="space-y-1">
            <div className={`inline-flex items-center px-2.5 py-0.5 rounded-full border text-xs font-semibold ${bgClass} ${colorClass}`}>
              {quality}
            </div>
            <div className="flex items-center gap-1 text-xs text-slate-500 mt-2">
              <Calendar className="w-3.5 h-3.5" />
              <span>Updated: {format(new Date(latestScore!.recordedOn), "dd MMM yyyy")}</span>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
