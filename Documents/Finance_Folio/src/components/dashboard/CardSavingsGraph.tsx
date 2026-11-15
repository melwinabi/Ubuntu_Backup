"use client";

import { useMemo } from "react";
import { Gift, TrendingUp, CreditCard } from "lucide-react";

interface SavedExpense {
  amount: number;
  savedAmount: number;
  spentOn: Date;
  paymentMethod: string;
  card: { cardName: string } | null;
}

interface CardSavingsGraphProps {
  savingsData: SavedExpense[];
}

export default function CardSavingsGraph({ savingsData }: CardSavingsGraphProps) {
  // Process data for the chart
  const { totalSaved, methodBreakdown, recentSavings } = useMemo(() => {
    let total = 0;
    const methods: Record<string, number> = {};
    const recent = [...savingsData].sort((a, b) => new Date(b.spentOn).getTime() - new Date(a.spentOn).getTime()).slice(0, 4);

    savingsData.forEach((exp) => {
      total += exp.savedAmount;
      const key = exp.card?.cardName || exp.paymentMethod.replace("_", " ");
      methods[key] = (methods[key] || 0) + exp.savedAmount;
    });

    // Sort breakdown for chart
    const breakdown = Object.entries(methods)
      .sort((a, b) => b[1] - a[1])
      .map(([name, amount]) => ({ name, amount }));

    return { totalSaved: total, methodBreakdown: breakdown, recentSavings: recent };
  }, [savingsData]);

  if (totalSaved === 0) {
    return (
      <div className="glass-card rounded-2xl p-6 h-full flex flex-col items-center justify-center text-center">
        <div className="w-16 h-16 rounded-full bg-emerald-500/10 text-emerald-500 flex items-center justify-center mb-4">
          <Gift className="w-8 h-8 opacity-50" />
        </div>
        <h3 className="text-slate-300 font-bold mb-1">No Savings Recorded Yet</h3>
        <p className="text-xs text-slate-500 max-w-[250px]">
          When adding an expense via Credit or Debit Card, record your cashback/discount to see your savings grow here!
        </p>
      </div>
    );
  }

  // Find max value for the bar chart scaling
  const maxAmount = Math.max(...methodBreakdown.map(b => b.amount), 1);

  return (
    <div className="glass-card rounded-2xl p-6 h-full flex flex-col">
      <div className="flex justify-between items-start mb-6 border-b border-slate-900 pb-4">
        <div>
          <h2 className="text-lg font-bold text-slate-100 flex items-center gap-2">
            <Gift className="w-5 h-5 text-emerald-400" />
            <span>Card Rewards & Savings</span>
          </h2>
          <p className="text-xs text-slate-400 mt-0.5">Total money saved through discounts & cashbacks</p>
        </div>
        <div className="text-right">
          <p className="text-2xl font-black text-emerald-400">₹{totalSaved.toLocaleString('en-IN')}</p>
          <span className="text-[10px] uppercase font-bold text-slate-500 tracking-wider">All Time Saved</span>
        </div>
      </div>

      <div className="flex-1 flex flex-col gap-6">
        {/* Simple Bar Chart */}
        <div className="space-y-4 flex-1">
          <h3 className="text-xs font-semibold text-slate-400 uppercase tracking-widest">Savings by Source</h3>
          <div className="space-y-3">
            {methodBreakdown.map((item, idx) => {
              const widthPct = Math.max(5, (item.amount / maxAmount) * 100);
              return (
                <div key={idx} className="relative">
                  <div className="flex justify-between text-xs mb-1">
                    <span className="text-slate-300 capitalize font-medium truncate pr-2">{item.name}</span>
                    <span className="text-emerald-400 font-bold shrink-0">₹{item.amount.toLocaleString('en-IN')}</span>
                  </div>
                  <div className="h-2 w-full bg-slate-900 rounded-full overflow-hidden">
                    <div 
                      className="h-full bg-gradient-to-r from-emerald-500/50 to-emerald-400 rounded-full" 
                      style={{ width: `${widthPct}%` }}
                    />
                  </div>
                </div>
              );
            })}
          </div>
        </div>

        {/* Recent Activity */}
        <div className="mt-auto pt-4 border-t border-slate-800/60">
          <h3 className="text-[10px] font-semibold text-slate-500 uppercase tracking-widest mb-3">Recent Cashbacks</h3>
          <div className="space-y-2">
            {recentSavings.map((exp, idx) => (
              <div key={idx} className="flex justify-between items-center text-xs p-2 rounded-lg bg-slate-900/30">
                <div className="flex items-center gap-2">
                  <TrendingUp className="w-3.5 h-3.5 text-emerald-500" />
                  <span className="text-slate-300">On ₹{exp.amount.toLocaleString()} purchase</span>
                </div>
                <span className="font-bold text-emerald-400">+₹{exp.savedAmount}</span>
              </div>
            ))}
          </div>
        </div>
      </div>
    </div>
  );
}
