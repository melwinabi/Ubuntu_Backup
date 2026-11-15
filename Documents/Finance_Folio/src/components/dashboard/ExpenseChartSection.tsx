"use client";

import { useState } from "react";
import { 
  PieChart, 
  Pie, 
  Cell, 
  ResponsiveContainer, 
  BarChart, 
  Bar, 
  XAxis, 
  YAxis, 
  Tooltip,
  Legend 
} from "recharts";
import { ArrowDownRight, Wallet, PieChart as PieIcon, BarChart3 } from "lucide-react";

interface ExpenseChartSectionProps {
  expenses: {
    id: string;
    amount: number;
    paymentMethod: string;
    category: {
      name: string;
    } | null;
  }[];
}

// Curated colors for charts
const CATEGORY_COLORS = [
  "#a78bfa", // Violet
  "#818cf8", // Indigo
  "#38bdf8", // Sky
  "#34d399", // Emerald
  "#fb7185", // Rose
  "#fbbf24", // Amber
  "#22d3ee", // Cyan
  "#f472b6", // Pink
  "#c084fc", // Purple
  "#94a3b8", // Slate
];

const METHOD_COLORS: Record<string, string> = {
  cash: "#fbbf24",       // Amber
  upi: "#34d399",        // Emerald
  credit_card: "#818cf8", // Indigo
  debit_card: "#a78bfa",  // Violet
  net_banking: "#38bdf8", // Sky
  cheque: "#f472b6",     // Pink
  other: "#94a3b8",      // Slate
};

const PAYMENT_METHOD_LABELS: Record<string, string> = {
  cash: "Cash",
  upi: "UPI / GPay",
  credit_card: "Credit Card",
  debit_card: "Debit Card",
  net_banking: "Net Banking",
  cheque: "Cheque",
  other: "Other",
};

export default function ExpenseChartSection({ expenses }: ExpenseChartSectionProps) {
  const [activeTab, setActiveTab] = useState<"category" | "method">("category");

  // 1. Process Category Data
  const categoryMap: Record<string, number> = {};
  expenses.forEach((exp) => {
    const catName = exp.category?.name || "Uncategorized";
    categoryMap[catName] = (categoryMap[catName] || 0) + exp.amount;
  });

  const categoryData = Object.keys(categoryMap)
    .map((name) => ({
      name,
      value: categoryMap[name],
    }))
    .sort((a, b) => b.value - a.value);

  const totalExpenses = categoryData.reduce((sum, item) => sum + item.value, 0);

  // 2. Process Method Data
  const methodMap: Record<string, number> = {};
  expenses.forEach((exp) => {
    methodMap[exp.paymentMethod] = (methodMap[exp.paymentMethod] || 0) + exp.amount;
  });

  const methodData = Object.keys(methodMap).map((method) => ({
    name: PAYMENT_METHOD_LABELS[method] || method,
    value: methodMap[method],
    rawMethod: method,
  }));

  // Custom tooltips for Recharts
  const CustomTooltip = ({ active, payload }: any) => {
    if (active && payload && payload.length) {
      return (
        <div className="bg-slate-900 border border-slate-800 p-3 rounded-lg shadow-xl">
          <p className="text-xs font-semibold text-slate-300">{payload[0].name}</p>
          <p className="text-sm font-bold text-violet-400 mt-1">
            ₹{payload[0].value.toLocaleString("en-IN", { minimumFractionDigits: 2 })}
          </p>
          {totalExpenses > 0 && (
            <p className="text-[10px] text-slate-500 mt-0.5">
              {((payload[0].value / totalExpenses) * 100).toFixed(1)}% of total
            </p>
          )}
        </div>
      );
    }
    return null;
  };

  return (
    <div className="glass-card rounded-2xl p-6 h-full flex flex-col justify-between">
      {/* Card Header */}
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 border-b border-slate-900 pb-4 mb-6">
        <div>
          <h2 className="text-lg font-bold text-slate-100 flex items-center gap-2">
            <ArrowDownRight className="w-5 h-5 text-violet-400" />
            <span>Expense Breakdown</span>
          </h2>
          <p className="text-xs text-slate-400 mt-0.5">This month&apos;s spending structure.</p>
        </div>

        {/* Tab Controls */}
        <div className="flex bg-slate-950 p-1 rounded-xl border border-slate-900 shrink-0">
          <button
            onClick={() => setActiveTab("category")}
            className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-medium transition-all ${
              activeTab === "category"
                ? "bg-violet-600 text-white shadow-md shadow-violet-600/10"
                : "text-slate-400 hover:text-slate-200"
            }`}
          >
            <PieIcon className="w-3.5 h-3.5" />
            <span>By Category</span>
          </button>
          <button
            onClick={() => setActiveTab("method")}
            className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-medium transition-all ${
              activeTab === "method"
                ? "bg-violet-600 text-white shadow-md shadow-violet-600/10"
                : "text-slate-400 hover:text-slate-200"
            }`}
          >
            <BarChart3 className="w-3.5 h-3.5" />
            <span>By Method</span>
          </button>
        </div>
      </div>

      {expenses.length === 0 ? (
        <div className="flex-1 flex flex-col items-center justify-center py-20 text-center text-slate-500">
          <Wallet className="w-10 h-10 text-slate-800 mb-2 animate-bounce" />
          <p className="text-sm font-medium">No expenses logged this month.</p>
          <p className="text-xs text-slate-600 mt-0.5">Your dashboard charts will appear here once you log an expense.</p>
        </div>
      ) : (
        <div className="grid grid-cols-1 md:grid-cols-5 gap-6 flex-1 items-center">
          {/* Chart visual - taking 3 cols on desktop */}
          <div className="md:col-span-3 h-64 w-full">
            <ResponsiveContainer width="100%" height="100%">
              {activeTab === "category" ? (
                <PieChart>
                  <Pie
                    data={categoryData}
                    cx="50%"
                    cy="50%"
                    innerRadius={60}
                    outerRadius={85}
                    paddingAngle={3}
                    dataKey="value"
                  >
                    {categoryData.map((entry, index) => (
                      <Cell key={`cell-${index}`} fill={CATEGORY_COLORS[index % CATEGORY_COLORS.length]} />
                    ))}
                  </Pie>
                  <Tooltip content={<CustomTooltip />} />
                </PieChart>
              ) : (
                <BarChart data={methodData} layout="vertical" margin={{ left: 10, right: 10 }}>
                  <XAxis type="number" hide />
                  <YAxis
                    dataKey="name"
                    type="category"
                    axisLine={false}
                    tickLine={false}
                    tick={{ fill: "#94a3b8", fontSize: 11 }}
                    width={90}
                  />
                  <Tooltip content={<CustomTooltip />} cursor={{ fill: "rgba(255,255,255,0.03)" }} />
                  <Bar dataKey="value" radius={6}>
                    {methodData.map((entry, index) => (
                      <Cell key={`cell-${index}`} fill={METHOD_COLORS[entry.rawMethod] || "#94a3b8"} />
                    ))}
                  </Bar>
                </BarChart>
              )}
            </ResponsiveContainer>
          </div>

          {/* Chart legend / details list - taking 2 cols */}
          <div className="md:col-span-2 space-y-3 max-h-60 overflow-y-auto pr-1">
            {activeTab === "category"
              ? categoryData.map((item, index) => {
                  const percent = totalExpenses > 0 ? (item.value / totalExpenses) * 100 : 0;
                  return (
                    <div key={item.name} className="flex items-center justify-between text-xs border-b border-slate-900 pb-2 last:border-0 last:pb-0">
                      <div className="flex items-center gap-2 min-w-0">
                        <span
                          className="w-2.5 h-2.5 rounded-full shrink-0"
                          style={{ backgroundColor: CATEGORY_COLORS[index % CATEGORY_COLORS.length] }}
                        />
                        <span className="text-slate-300 font-medium truncate">{item.name}</span>
                      </div>
                      <div className="text-right shrink-0">
                        <p className="font-semibold text-slate-100">₹{item.value.toLocaleString("en-IN", { maximumFractionDigits: 0 })}</p>
                        <p className="text-[10px] text-slate-500">{percent.toFixed(0)}%</p>
                      </div>
                    </div>
                  );
                })
              : methodData.map((item) => {
                  const percent = totalExpenses > 0 ? (item.value / totalExpenses) * 100 : 0;
                  return (
                    <div key={item.name} className="flex items-center justify-between text-xs border-b border-slate-900 pb-2 last:border-0 last:pb-0">
                      <div className="flex items-center gap-2 min-w-0">
                        <span
                          className="w-2.5 h-2.5 rounded-full shrink-0"
                          style={{ backgroundColor: METHOD_COLORS[item.rawMethod] || "#94a3b8" }}
                        />
                        <span className="text-slate-300 font-medium truncate">{item.name}</span>
                      </div>
                      <div className="text-right shrink-0">
                        <p className="font-semibold text-slate-100">₹{item.value.toLocaleString("en-IN", { maximumFractionDigits: 0 })}</p>
                        <p className="text-[10px] text-slate-500">{percent.toFixed(0)}%</p>
                      </div>
                    </div>
                  );
                })}
          </div>
        </div>
      )}
    </div>
  );
}
