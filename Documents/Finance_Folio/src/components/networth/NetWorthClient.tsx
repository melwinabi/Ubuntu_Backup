"use client";

import { useState } from "react";
import { triggerRecalculateNetWorth, deleteNetWorthSnapshot } from "@/actions/networth";
import { 
  TrendingUp, 
  TrendingDown, 
  PiggyBank, 
  Calendar, 
  Trash2, 
  AlertCircle,
  Plus,
  Coins,
  CreditCard,
  Wallet,
  ArrowUpRight,
  ChevronRight
} from "lucide-react";
import { format } from "date-fns";
import { 
  AreaChart, 
  Area, 
  XAxis, 
  YAxis, 
  CartesianGrid, 
  Tooltip, 
  ResponsiveContainer 
} from "recharts";

interface NetWorthClientProps {
  initialSnapshots: any[];
  currentPreview: {
    netWorth: number;
    assets: number;
    liabilities: number;
    cash: number;
    investments: number;
    loans: number;
  };
}

export default function NetWorthClient({ initialSnapshots, currentPreview }: NetWorthClientProps) {
  const [snapshots, setSnapshots] = useState(initialSnapshots);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  // Chart data formatting
  const chartData = snapshots.map((s) => ({
    date: format(new Date(s.snapshotDate), "MMM yyyy"),
    value: s.netWorth,
  }));

  // Action: Take Snapshot
  const handleCaptureSnapshot = async () => {
    setLoading(true);
    setError(null);
    try {
      const res = await triggerRecalculateNetWorth();
      if (res.success && res.data) {
        setSnapshots((prev) => {
          const updated = [...prev, res.data];
          return updated.sort((a, b) => new Date(a.snapshotDate).getTime() - new Date(b.snapshotDate).getTime());
        });
      } else {
        setError(res.error || "Failed to capture snapshot");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Delete Snapshot
  const handleDeleteSnapshot = async (id: string) => {
    if (!confirm("Are you sure you want to delete this historical net worth record?")) return;
    try {
      const res = await deleteNetWorthSnapshot(id);
      if (res.success) {
        setSnapshots((prev) => prev.filter((s) => s.id !== id));
      } else {
        alert(res.error || "Failed to delete snapshot");
      }
    } catch (err: any) {
      alert(err.message || "An error occurred");
    }
  };

  return (
    <div className="space-y-8">
      {/* Header */}
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4">
        <div>
          <h1 className="text-3xl font-extrabold tracking-tight text-slate-100">Net Worth Engine</h1>
          <p className="text-slate-400 mt-1">Audit computed assets vs liabilities and record snapshots.</p>
        </div>
        <button
          onClick={handleCaptureSnapshot}
          disabled={loading}
          className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-violet-650 hover:bg-violet-600 font-medium text-sm text-white shadow-lg shadow-violet-600/10 transition-all hover:scale-[1.01]"
        >
          {loading ? (
            <svg className="animate-spin h-4 w-4 text-white" fill="none" viewBox="0 0 24 24">
              <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4" />
              <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z" />
            </svg>
          ) : (
            <Plus className="w-4 h-4" />
          )}
          <span>Record Snapshot</span>
        </button>
      </div>

      {error && (
        <div className="p-4 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm flex items-start gap-3">
          <AlertCircle className="w-5 h-5 shrink-0 mt-0.5" />
          <p>{error}</p>
        </div>
      )}

      {/* Stats Summary Panel */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {/* Net Worth */}
        <div className="glass-card rounded-2xl p-6 relative overflow-hidden bg-gradient-to-br from-violet-900/10 to-indigo-950/10 border border-violet-500/10">
          <div className="absolute top-0 right-0 w-24 h-24 bg-violet-600/5 rounded-full blur-2xl" />
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Computed Net Worth</p>
          <p className="text-3xl font-black text-slate-100 mt-2 font-mono">
            ₹{currentPreview.netWorth.toLocaleString("en-IN", { minimumFractionDigits: 2 })}
          </p>
          <div className="flex items-center gap-1 text-[11px] text-slate-500 mt-3">
            <span>Real-time values aggregate</span>
          </div>
        </div>

        {/* Total Assets */}
        <div className="glass-card rounded-2xl p-6">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Total Assets</p>
          <p className="text-3xl font-bold text-green-400 mt-2 font-mono">
            ₹{currentPreview.assets.toLocaleString("en-IN", { minimumFractionDigits: 2 })}
          </p>
          <div className="flex items-center gap-3 text-[10px] text-slate-500 mt-3 pt-3 border-t border-slate-900">
            <span className="flex items-center gap-0.5"><Wallet className="w-3 h-3 text-amber-500" /> Cash: ₹{currentPreview.cash.toLocaleString("en-IN", { maximumFractionDigits: 0 })}</span>
            <span className="flex items-center gap-0.5"><Coins className="w-3 h-3 text-cyan-500" /> Inv: ₹{currentPreview.investments.toLocaleString("en-IN", { maximumFractionDigits: 0 })}</span>
          </div>
        </div>

        {/* Liabilities */}
        <div className="glass-card rounded-2xl p-6">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Total Liabilities</p>
          <p className="text-3xl font-bold text-red-400 mt-2 font-mono">
            ₹{currentPreview.liabilities.toLocaleString("en-IN", { minimumFractionDigits: 2 })}
          </p>
          <div className="flex items-center gap-1 text-[10px] text-slate-500 mt-3 pt-3 border-t border-slate-900">
            <span className="flex items-center gap-0.5"><CreditCard className="w-3 h-3 text-indigo-400" /> Credit Cards Outstanding</span>
          </div>
        </div>
      </div>

      {/* Main Grid */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Trend Area Chart - spans 2 cols */}
        <div className="lg:col-span-2 glass-card rounded-2xl p-6 flex flex-col justify-between min-h-[350px]">
          <div>
            <h3 className="font-bold text-slate-200 flex items-center gap-2 mb-2">
              <PiggyBank className="w-5 h-5 text-violet-400" />
              <span>Net Worth Growth Curve</span>
            </h3>
            <p className="text-xs text-slate-400">Progression trend line computed from saved monthly snapshots.</p>
          </div>

          {snapshots.length < 2 ? (
            <div className="flex-1 flex flex-col items-center justify-center py-20 text-center text-slate-500">
              <PiggyBank className="w-10 h-10 text-slate-800 mb-2" />
              <p className="text-sm font-medium">Insufficient snapshot archives.</p>
              <p className="text-xs text-slate-600 mt-0.5">Please record at least two monthly snapshots to plot Progression.</p>
            </div>
          ) : (
            <div className="h-64 w-full mt-6">
              <ResponsiveContainer width="100%" height="100%">
                <AreaChart data={chartData} margin={{ top: 10, right: 10, left: -10, bottom: 0 }}>
                  <defs>
                    <linearGradient id="networthColor" x1="0" y1="0" x2="0" y2="1">
                      <stop offset="5%" stopColor="#8b5cf6" stopOpacity={0.2}/>
                      <stop offset="95%" stopColor="#8b5cf6" stopOpacity={0}/>
                    </linearGradient>
                  </defs>
                  <CartesianGrid strokeDasharray="3 3" stroke="rgba(255,255,255,0.03)" vertical={false} />
                  <XAxis dataKey="date" tick={{ fill: "#64748b", fontSize: 10 }} axisLine={false} tickLine={false} />
                  <YAxis tick={{ fill: "#64748b", fontSize: 10 }} axisLine={false} tickLine={false} />
                  <Tooltip
                    contentStyle={{ background: "#0f172a", border: "1px solid #1e293b", borderRadius: "8px" }}
                    labelStyle={{ color: "#94a3b8", fontSize: "11px" }}
                    itemStyle={{ color: "#a78bfa", fontWeight: "bold" }}
                  />
                  <Area
                    type="monotone"
                    dataKey="value"
                    stroke="#8b5cf6"
                    strokeWidth={3}
                    fillOpacity={1}
                    fill="url(#networthColor)"
                  />
                </AreaChart>
              </ResponsiveContainer>
            </div>
          )}
        </div>

        {/* Snapshot list */}
        <div className="glass-card rounded-2xl p-6 space-y-4">
          <h3 className="font-bold text-slate-200 text-base">Recorded Snapshots</h3>
          {snapshots.length === 0 ? (
            <p className="text-xs text-slate-500">No snapshot records found. Record today&apos;s stats to seed the engine!</p>
          ) : (
            <div className="space-y-3 max-h-80 overflow-y-auto pr-1">
              {snapshots.slice().reverse().map((s) => (
                <div key={s.id} className="flex items-center justify-between p-3 rounded-xl bg-slate-900/40 border border-slate-900">
                  <div className="space-y-0.5">
                    <p className="text-sm font-bold text-slate-200 font-mono">
                      ₹{s.netWorth.toLocaleString("en-IN", { maximumFractionDigits: 0 })}
                    </p>
                    <p className="text-[10px] text-slate-500">
                      {format(new Date(s.snapshotDate), "dd MMM yyyy")}
                    </p>
                  </div>
                  <button
                    onClick={() => handleDeleteSnapshot(s.id)}
                    className="p-1.5 text-slate-500 hover:text-red-400 hover:bg-red-500/5 rounded-lg border border-transparent hover:border-red-500/10 transition-all"
                  >
                    <Trash2 className="w-3.5 h-3.5" />
                  </button>
                </div>
              ))}
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
