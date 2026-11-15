"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { creditScoreSchema } from "@/lib/validators";
import { createCreditScore, deleteCreditScore } from "@/actions/creditScores";
import { 
  Plus, 
  Trash2, 
  Calendar, 
  Gauge, 
  TrendingUp,
  TrendingDown,
  AlertCircle,
  X,
  LineChart as LineChartIcon,
  ShieldAlert
} from "lucide-react";
import { format } from "date-fns";
import { 
  LineChart, 
  Line, 
  XAxis, 
  YAxis, 
  CartesianGrid, 
  Tooltip, 
  ResponsiveContainer 
} from "recharts";

interface CreditScoreClientProps {
  initialHistory: any[];
}

const BUREAUS = [
  { value: "CIBIL", label: "CIBIL (TransUnion)" },
  { value: "Experian", label: "Experian" },
  { value: "Equifax", label: "Equifax" },
  { value: "CRIF", label: "CRIF High Mark" },
];

export default function CreditScoreClient({ initialHistory }: CreditScoreClientProps) {
  const [history, setHistory] = useState(initialHistory);
  const [showForm, setShowForm] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  // Form
  const {
    register,
    handleSubmit,
    reset,
    formState: { errors },
  } = useForm({
    resolver: zodResolver(creditScoreSchema),
    defaultValues: {
      score: 750,
      bureau: "CIBIL",
      recordedOn: new Date().toISOString().split("T")[0],
    },
  });

  const latestEntry = history.length > 0 ? history[history.length - 1] : null;
  const latestScore = latestEntry?.score ?? 0;

  // Chart data formatting
  const chartData = history.map((entry) => ({
    date: format(new Date(entry.recordedOn), "dd MMM yy"),
    score: entry.score,
    bureau: entry.bureau,
  }));

  // Determine score health
  let quality = "N/A";
  let colorClass = "text-slate-500";
  let bgClass = "bg-slate-500/10 border-slate-500/20";
  if (latestScore > 0) {
    if (latestScore >= 750) {
      quality = "Excellent";
      colorClass = "text-green-400";
      bgClass = "bg-green-500/10 border-green-500/20";
    } else if (latestScore >= 700) {
      quality = "Good";
      colorClass = "text-emerald-400";
      bgClass = "bg-emerald-500/10 border-emerald-500/20";
    } else if (latestScore >= 650) {
      quality = "Fair";
      colorClass = "text-amber-400";
      bgClass = "bg-amber-500/10 border-amber-500/20";
    } else {
      quality = "Needs Attention";
      colorClass = "text-red-400";
      bgClass = "bg-red-500/10 border-red-500/20";
    }
  }

  // Action: Add Score
  const onAddScore = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await createCreditScore(data);
      if (res.success && res.data) {
        setHistory((prev) => [...prev, res.data].sort(
          (a, b) => new Date(a.recordedOn).getTime() - new Date(b.recordedOn).getTime()
        ));
        reset();
        setShowForm(false);
      } else {
        setError(res.error || "Failed to record score");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Delete Score
  const onDeleteScore = async (id: string) => {
    if (!confirm("Are you sure you want to delete this score entry?")) return;
    try {
      const res = await deleteCreditScore(id);
      if (res.success) {
        setHistory((prev) => prev.filter((s) => s.id !== id));
      } else {
        alert(res.error || "Failed to delete score");
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
          <h1 className="text-3xl font-extrabold tracking-tight text-slate-100">Credit Score Tracker</h1>
          <p className="text-slate-400 mt-1">Audit creditworthiness history across multiple bureaus.</p>
        </div>
        <button
          onClick={() => setShowForm(true)}
          className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-violet-650 hover:bg-violet-600 font-medium text-sm text-white shadow-lg shadow-violet-600/10 transition-all hover:scale-[1.01]"
        >
          <Plus className="w-4 h-4" />
          <span>Record New Score</span>
        </button>
      </div>

      {error && (
        <div className="p-4 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm flex items-start gap-3">
          <AlertCircle className="w-5 h-5 shrink-0 mt-0.5" />
          <p>{error}</p>
        </div>
      )}

      {/* Record Score Form */}
      {showForm && (
        <div className="glass-card rounded-2xl p-6 border border-violet-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <Gauge className="w-5 h-5 text-violet-400" />
              <span>Record Credit Score</span>
            </h3>
            <button onClick={() => setShowForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={handleSubmit(onAddScore)} className="grid grid-cols-1 md:grid-cols-3 gap-4">
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Bureau Name</label>
              <select
                {...register("bureau")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                {BUREAUS.map((b) => (
                  <option key={b.value} value={b.value}>
                    {b.label}
                  </option>
                ))}
              </select>
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Score Rating (300-900)</label>
              <input
                {...register("score", { valueAsNumber: true })}
                type="number"
                min={300}
                max={900}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm font-mono"
              />
              {errors.score && (
                <p className="text-xs text-red-400">{errors.score.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Date Recorded</label>
              <input
                {...register("recordedOn")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="md:col-span-3 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-violet-600 hover:bg-violet-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Recording..." : "Save Score"}
              </button>
            </div>
          </form>
        </div>
      )}

      {/* Main layout grid */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Metric display & History List */}
        <div className="space-y-6">
          <div className="glass-card rounded-2xl p-6">
            <h3 className="text-sm font-semibold text-slate-400 uppercase tracking-wider mb-4">Latest Credit Status</h3>
            {latestScore === 0 ? (
              <div className="flex flex-col items-center justify-center py-6 text-center text-slate-500 text-sm">
                <ShieldAlert className="w-10 h-10 text-slate-800 mb-2" />
                <p>No credit score historical data recorded.</p>
              </div>
            ) : (
              <div className="space-y-4">
                <div className="flex items-baseline gap-2">
                  <span className="text-5xl font-extrabold text-slate-100 font-mono">{latestScore}</span>
                  <span className="text-xs text-slate-500 font-semibold uppercase">/ 900</span>
                </div>
                <div className="space-y-1">
                  <span className={`inline-flex px-3 py-0.5 rounded-full border text-xs font-semibold ${bgClass} ${colorClass}`}>
                    {quality}
                  </span>
                  <p className="text-xs text-slate-500 mt-2">
                    Bureau: <span className="text-slate-300 font-semibold">{latestEntry?.bureau}</span>
                  </p>
                  <p className="text-[10px] text-slate-500">
                    Recorded on {format(new Date(latestEntry!.recordedOn), "dd MMM yyyy")}
                  </p>
                </div>
              </div>
            )}
          </div>

          <div className="glass-card rounded-2xl p-6 space-y-4">
            <h3 className="font-bold text-slate-200 text-base">Score History Logs</h3>
            {history.length === 0 ? (
              <p className="text-xs text-slate-500">No score history logged.</p>
            ) : (
              <div className="space-y-3 max-h-72 overflow-y-auto pr-1">
                {history.slice().reverse().map((entry) => (
                  <div key={entry.id} className="flex items-center justify-between p-3 rounded-xl bg-slate-900/40 border border-slate-900">
                    <div className="space-y-0.5">
                      <p className="text-sm font-bold text-slate-200 font-mono">{entry.score}</p>
                      <p className="text-[10px] text-slate-500">
                        {entry.bureau} • {format(new Date(entry.recordedOn), "dd MMM yyyy")}
                      </p>
                    </div>
                    <button
                      onClick={() => onDeleteScore(entry.id)}
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

        {/* Recharts chart card - spans 2 cols */}
        <div className="lg:col-span-2 glass-card rounded-2xl p-6 flex flex-col justify-between min-h-[350px]">
          <div>
            <h3 className="font-bold text-slate-200 flex items-center gap-2 mb-2">
              <LineChartIcon className="w-5 h-5 text-indigo-400" />
              <span>Credit Rating Progression</span>
            </h3>
            <p className="text-xs text-slate-400">Progression curve across record entries.</p>
          </div>

          {history.length < 2 ? (
            <div className="flex-1 flex flex-col items-center justify-center py-20 text-center text-slate-500">
              <LineChartIcon className="w-10 h-10 text-slate-800 mb-2" />
              <p className="text-sm font-medium">Insufficient graph points.</p>
              <p className="text-xs text-slate-600 mt-0.5">Please add at least two credit score entries to plot progression.</p>
            </div>
          ) : (
            <div className="h-64 w-full mt-6">
              <ResponsiveContainer width="100%" height="100%">
                <LineChart data={chartData} margin={{ top: 10, right: 10, left: -20, bottom: 0 }}>
                  <CartesianGrid strokeDasharray="3 3" stroke="rgba(255,255,255,0.03)" vertical={false} />
                  <XAxis dataKey="date" tick={{ fill: "#64748b", fontSize: 10 }} axisLine={false} tickLine={false} />
                  <YAxis domain={[300, 900]} tick={{ fill: "#64748b", fontSize: 10 }} axisLine={false} tickLine={false} />
                  <Tooltip
                    contentStyle={{ background: "#0f172a", border: "1px solid #1e293b", borderRadius: "8px" }}
                    labelStyle={{ color: "#94a3b8", fontSize: "11px" }}
                    itemStyle={{ color: "#a78bfa", fontWeight: "bold" }}
                  />
                  <Line
                    type="monotone"
                    dataKey="score"
                    stroke="#8b5cf6"
                    strokeWidth={3}
                    dot={{ fill: "#a78bfa", strokeWidth: 1, r: 4 }}
                    activeDot={{ r: 6 }}
                  />
                </LineChart>
              </ResponsiveContainer>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
