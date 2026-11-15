"use client";

import { useState, useEffect } from "react";
import { Target, TrendingUp, Calendar, Wallet, Sparkles, Activity, PlusCircle } from "lucide-react";

interface TargetCalculatorProps {
  currentNetWorth: number;
  monthlySipTotal: number;
  monthlyIncome: number;
  monthlyExpense: number;
}

export default function TargetNetWorthCalculator({ 
  currentNetWorth, 
  monthlySipTotal,
  monthlyIncome,
  monthlyExpense
}: TargetCalculatorProps) {
  
  // Dynamic free cash flow analysis
  const netMonthlySavings = Math.max(0, monthlyIncome - monthlyExpense);
  const effectiveMonthlyAddition = Math.max(netMonthlySavings, monthlySipTotal);

  const [targetAmount, setTargetAmount] = useState<string>("10000000"); // 1 Crore default string to allow empty input
  const [annualReturnRate, setAnnualReturnRate] = useState<number>(12); // Default 12%
  const [monthlyContribution, setMonthlyContribution] = useState<string>(effectiveMonthlyAddition > 0 ? effectiveMonthlyAddition.toString() : "10000");
  
  const [yearsToTarget, setYearsToTarget] = useState<number | null>(null);
  const [targetDate, setTargetDate] = useState<Date | null>(null);

  // Auto-update if user adds income/expense elsewhere in the app
  useEffect(() => {
    if (effectiveMonthlyAddition > 0) {
      setMonthlyContribution(effectiveMonthlyAddition.toString());
    }
  }, [effectiveMonthlyAddition]);

  // Exact 99.9% derived Mathematical Formula for Time to reach FV
  const calculateTimeToTarget = () => {
    const target = Number(targetAmount) || 0;
    const contribution = Number(monthlyContribution) || 0;

    if (target <= 0) {
      setYearsToTarget(null);
      setTargetDate(null);
      return;
    }

    if (target <= currentNetWorth && currentNetWorth > 0) {
      setYearsToTarget(0);
      setTargetDate(new Date());
      return;
    }

    const PV = currentNetWorth;
    const FV = target;
    const PMT = contribution;
    const r = annualReturnRate / 100;
    const n = 12; // Compounded monthly
    const i = r / n; // Monthly interest rate

    if (i === 0) {
      // Zero interest edge case
      if (PMT <= 0) {
        setYearsToTarget(null);
        return;
      }
      const months = (FV - PV) / PMT;
      setYearsToTarget(months / 12);
      const d = new Date();
      d.setMonth(d.getMonth() + months);
      setTargetDate(d);
      return;
    }

    // If PV already exceeds FV, we caught it above.
    // If user has no contribution and PV is 0, they can never reach it
    if (PV === 0 && PMT === 0) {
      setYearsToTarget(null);
      setTargetDate(null);
      return;
    }

    // Formula: t = ln((FV + PMT/i) / (PV + PMT/i)) / (n * ln(1 + i))
    const numerator = Math.log((FV + PMT / i) / (PV + PMT / i));
    const denominator = n * Math.log(1 + i);
    const t = numerator / denominator;

    if (isNaN(t) || t < 0 || t === Infinity) {
      setYearsToTarget(null); // Unreachable with current params
      setTargetDate(null);
    } else {
      setYearsToTarget(t);
      const d = new Date();
      d.setDate(d.getDate() + Math.round(t * 365.25));
      setTargetDate(d);
    }
  };

  useEffect(() => {
    calculateTimeToTarget();
  }, [targetAmount, annualReturnRate, monthlyContribution, currentNetWorth]);

  return (
    <div className="glass-card rounded-2xl p-6 border border-cyan-500/20 shadow-lg shadow-cyan-900/10">
      <div className="flex items-center justify-between mb-6 border-b border-slate-900 pb-4">
        <div>
          <h2 className="text-lg font-bold text-slate-100 flex items-center gap-2">
            <Target className="w-5 h-5 text-cyan-400" />
            <span>AI Target Forecaster</span>
          </h2>
          <p className="text-xs text-slate-400 mt-0.5">Dynamically analyzes your entire portfolio & cashflow to predict goal achievement</p>
        </div>
        <div className="w-10 h-10 rounded-xl bg-cyan-500/10 border border-cyan-500/20 text-cyan-400 flex items-center justify-center">
          <Sparkles className="w-5 h-5 animate-pulse" />
        </div>
      </div>

      <div className="grid grid-cols-1 lg:grid-cols-2 gap-8">
        {/* Controls */}
        <div className="space-y-6">
          
          {/* Current Status Display */}
          <div className="grid grid-cols-2 gap-3 mb-2">
            <div className="p-3 rounded-xl bg-slate-900/50 border border-slate-800">
              <div className="flex items-center gap-1.5 text-slate-400 mb-1">
                <Wallet className="w-4 h-4 text-emerald-400" />
                <span className="text-[11px] font-semibold uppercase tracking-wider">Current Net Worth</span>
              </div>
              <p className="text-lg font-black text-slate-100">
                ₹{currentNetWorth.toLocaleString('en-IN', { maximumFractionDigits: 0 })}
              </p>
            </div>
            
            <div className="p-3 rounded-xl bg-slate-900/50 border border-slate-800">
              <div className="flex items-center gap-1.5 text-slate-400 mb-1">
                <Activity className="w-4 h-4 text-cyan-400" />
                <span className="text-[11px] font-semibold uppercase tracking-wider">Analyzed Savings</span>
              </div>
              <p className="text-lg font-black text-slate-100">
                ₹{effectiveMonthlyAddition.toLocaleString('en-IN', { maximumFractionDigits: 0 })}<span className="text-xs text-slate-500 font-normal">/mo</span>
              </p>
            </div>
          </div>

          <div>
            <label className="text-xs font-semibold text-slate-300 block mb-1.5">Your Target Amount (₹)</label>
            <div className="relative">
              <span className="absolute left-3 top-1/2 -translate-y-1/2 text-slate-500 font-medium">₹</span>
              <input
                type="number"
                value={targetAmount}
                onChange={(e) => setTargetAmount(e.target.value)}
                placeholder="e.g. 10000000"
                className="w-full pl-8 pr-3 py-3 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 font-bold tracking-wide"
              />
            </div>
          </div>

          <div>
            <label className="text-xs font-semibold text-slate-300 block mb-1.5 flex justify-between">
              <span>Monthly Addition (₹)</span>
              {Number(monthlyContribution) === effectiveMonthlyAddition && (
                <span className="text-[10px] text-cyan-500 bg-cyan-900/30 px-1.5 py-0.5 rounded">Auto-Synced</span>
              )}
            </label>
            <div className="relative">
              <span className="absolute left-3 top-1/2 -translate-y-1/2 text-slate-500 font-medium">₹</span>
              <input
                type="number"
                value={monthlyContribution}
                onChange={(e) => setMonthlyContribution(e.target.value)}
                placeholder="0"
                className="w-full pl-8 pr-3 py-3 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 tracking-wide"
              />
            </div>
            <p className="text-[10px] text-slate-500 mt-1.5">
              Automatically synchronized with your active (Income - Expenses) or SIPs. If your salary increases, this adjusts automatically!
            </p>
          </div>

          <div>
            <label className="text-xs font-semibold text-slate-300 block mb-1.5 flex justify-between">
              <span>Expected Annual Portfolio Return (%)</span>
              <span className="text-cyan-400 font-bold bg-cyan-950 px-2 py-0.5 rounded-full">{annualReturnRate}%</span>
            </label>
            <input
              type="range"
              min="1"
              max="30"
              step="0.5"
              value={annualReturnRate}
              onChange={(e) => setAnnualReturnRate(Number(e.target.value))}
              className="w-full accent-cyan-500 h-2 bg-slate-800 rounded-lg appearance-none cursor-pointer"
            />
          </div>
        </div>

        {/* Results */}
        <div className="flex flex-col justify-center">
          <div className="rounded-2xl p-6 bg-gradient-to-br from-slate-900 to-slate-950 border border-slate-800 relative overflow-hidden h-full flex flex-col justify-center">
            <div className="absolute top-0 right-0 p-4 opacity-[0.03]">
              <Target className="w-48 h-48" />
            </div>

            {yearsToTarget === null ? (
              <div className="text-center py-6 relative z-10">
                <div className="w-16 h-16 rounded-full bg-slate-800 flex items-center justify-center mx-auto mb-4 border border-slate-700">
                  <TrendingUp className="w-8 h-8 text-slate-600" />
                </div>
                <h3 className="text-lg font-bold text-slate-300">Set Your Target</h3>
                <p className="text-sm text-slate-500 mt-2 max-w-[250px] mx-auto">
                  Enter a valid target amount to calculate how many days it will take to reach it.
                </p>
              </div>
            ) : yearsToTarget === 0 ? (
              <div className="text-center py-6 relative z-10">
                <div className="w-16 h-16 rounded-full bg-emerald-500/20 text-emerald-400 flex items-center justify-center mx-auto mb-3 border border-emerald-500/30">
                  <TrendingUp className="w-8 h-8" />
                </div>
                <h3 className="text-2xl font-bold text-slate-100">Target Achieved!</h3>
                <p className="text-sm text-emerald-400/80 mt-2 font-medium">Your current net worth already exceeds this goal.</p>
              </div>
            ) : (
              <div className="relative z-10 space-y-6">
                <div>
                  <p className="text-xs font-semibold uppercase tracking-widest text-slate-500 mb-2">Time Required To Reach Target</p>
                  
                  {/* Years and Months Display */}
                  <div className="flex items-baseline gap-2 mb-2">
                    <span className="text-5xl font-black text-transparent bg-clip-text bg-gradient-to-r from-cyan-400 to-blue-500">
                      {Math.floor(yearsToTarget)}
                    </span>
                    <span className="text-slate-400 font-bold text-sm tracking-wide">YRS</span>
                    
                    <span className="text-5xl font-black text-transparent bg-clip-text bg-gradient-to-r from-cyan-400 to-blue-500 ml-3">
                      {Math.round((yearsToTarget - Math.floor(yearsToTarget)) * 12)}
                    </span>
                    <span className="text-slate-400 font-bold text-sm tracking-wide">MOS</span>
                  </div>
                  
                  {/* Total Days Display */}
                  <div className="inline-block mt-1 px-3 py-1 bg-cyan-950/50 border border-cyan-900/50 rounded-lg">
                    <p className="text-sm font-semibold text-cyan-300">
                      ≈ {Math.round(yearsToTarget * 365.25).toLocaleString('en-IN')} Total Days
                    </p>
                  </div>
                </div>

                <div className="grid grid-cols-2 gap-4 border-t border-slate-800/80 pt-6 mt-6">
                  <div>
                    <div className="flex items-center gap-1.5 text-slate-500 mb-1.5">
                      <Calendar className="w-3.5 h-3.5" />
                      <span className="text-[10px] font-bold uppercase tracking-wider">Estimated Date</span>
                    </div>
                    <p className="text-base font-bold text-slate-200">
                      {targetDate?.toLocaleDateString('en-IN', { month: 'long', year: 'numeric' })}
                    </p>
                  </div>
                  <div>
                    <div className="flex items-center gap-1.5 text-slate-500 mb-1.5">
                      <PlusCircle className="w-3.5 h-3.5" />
                      <span className="text-[10px] font-bold uppercase tracking-wider">Amount Needed</span>
                    </div>
                    <p className="text-base font-bold text-emerald-400">
                      +₹{Math.max(0, Number(targetAmount) - currentNetWorth).toLocaleString('en-IN', { maximumFractionDigits: 0 })}
                    </p>
                  </div>
                </div>
              </div>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}
