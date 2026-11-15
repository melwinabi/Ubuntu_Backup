import { TrendingUp, Coins, Activity } from "lucide-react";

export default function LiveMarketRatesCard({ metalsPrices }: { metalsPrices: any }) {
  const { gold, silver, nifty } = metalsPrices;

  return (
    <div className="glass-card rounded-2xl p-6">
      <div className="flex items-center justify-between mb-6 border-b border-slate-900 pb-4">
        <div>
          <h2 className="text-lg font-bold text-slate-100 flex items-center gap-2">
            <Activity className="w-5 h-5 text-emerald-400" />
            <span>Live Market Rates</span>
          </h2>
          <p className="text-xs text-slate-400 mt-0.5">Current Indian market prices updated via Yahoo Finance.</p>
        </div>
      </div>

      <div className="space-y-4">
        {/* NIFTY 50 */}
        <div className="flex items-center justify-between p-4 rounded-xl bg-slate-900/40 border border-slate-900">
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-xl bg-emerald-500/10 border border-emerald-500/20 text-emerald-400 flex items-center justify-center">
              <TrendingUp className="w-5 h-5" />
            </div>
            <div>
              <p className="text-sm font-semibold text-slate-200">NIFTY 50</p>
              <p className="text-xs text-slate-500">Indian Stock Index</p>
            </div>
          </div>
          <div className="text-right">
            <p className="text-sm font-bold text-slate-100">
              ₹ {nifty?.toLocaleString("en-IN", { minimumFractionDigits: 2, maximumFractionDigits: 2 })}
            </p>
            <span className="text-[10px] text-green-400 flex items-center justify-end gap-1">
              <span className="w-1.5 h-1.5 rounded-full bg-green-500 animate-pulse"></span>
              Live
            </span>
          </div>
        </div>

        {/* 24K Gold */}
        <div className="flex items-center justify-between p-4 rounded-xl bg-slate-900/40 border border-slate-900">
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-xl bg-amber-500/10 border border-amber-500/20 text-amber-400 flex items-center justify-center">
              <Coins className="w-5 h-5" />
            </div>
            <div>
              <p className="text-sm font-semibold text-slate-200">24K Gold</p>
              <p className="text-xs text-slate-500">Retail Price per Gram</p>
            </div>
          </div>
          <div className="text-right">
            <p className="text-sm font-bold text-slate-100">
              ₹ {gold?.toLocaleString("en-IN", { minimumFractionDigits: 2, maximumFractionDigits: 2 })}
            </p>
            <span className="text-[10px] text-green-400 flex items-center justify-end gap-1">
              <span className="w-1.5 h-1.5 rounded-full bg-green-500 animate-pulse"></span>
              Live
            </span>
          </div>
        </div>

        {/* Silver */}
        <div className="flex items-center justify-between p-4 rounded-xl bg-slate-900/40 border border-slate-900">
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-xl bg-slate-400/10 border border-slate-400/20 text-slate-300 flex items-center justify-center">
              <Coins className="w-5 h-5" />
            </div>
            <div>
              <p className="text-sm font-semibold text-slate-200">Silver</p>
              <p className="text-xs text-slate-500">Retail Price per Gram</p>
            </div>
          </div>
          <div className="text-right">
            <p className="text-sm font-bold text-slate-100">
              ₹ {silver?.toLocaleString("en-IN", { minimumFractionDigits: 2, maximumFractionDigits: 2 })}
            </p>
            <span className="text-[10px] text-green-400 flex items-center justify-end gap-1">
              <span className="w-1.5 h-1.5 rounded-full bg-green-500 animate-pulse"></span>
              Live
            </span>
          </div>
        </div>

      </div>
    </div>
  );
}
