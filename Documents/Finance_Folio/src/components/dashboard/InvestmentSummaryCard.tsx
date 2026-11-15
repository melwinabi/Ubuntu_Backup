"use client";

import { Coins, LineChart } from "lucide-react";
import { convertPreciousMetalToGrams } from "@/lib/clientMath";

interface InvestmentSummaryCardProps {
  investments: {
    id: string;
    assetType: string;
    quantity: number;
    purchasePrice: number;
    prices: { pricePerUnit: number }[];
    unit?: string;
  }[];
  totalValue: number;
  metalsPrices?: {
    gold: number;
    silver: number;
  };
}

const ASSET_TYPE_LABELS: Record<string, string> = {
  stock: "Stocks & Equities",
  gold: "Gold",
  silver: "Silver",
  mutual_fund: "Mutual Funds / SIP",
  crypto: "Cryptocurrency",
  real_estate: "Real Estate",
  fixed_deposit: "Fixed Deposits",
  other: "Other Assets / Bonds",
};

const ASSET_TYPE_COLORS: Record<string, string> = {
  stock: "bg-blue-500",
  gold: "bg-amber-400",
  silver: "bg-slate-300",
  mutual_fund: "bg-indigo-500",
  crypto: "bg-violet-500",
  real_estate: "bg-emerald-500",
  fixed_deposit: "bg-rose-500",
  other: "bg-slate-500",
};

export default function InvestmentSummaryCard({ investments, totalValue, metalsPrices }: InvestmentSummaryCardProps) {
  // Aggregate by assetType
  const typeMap: Record<string, number> = {};
  investments.forEach((inv) => {
    let currentPrice = inv.prices[0]?.pricePerUnit ?? inv.purchasePrice;
    let val = inv.quantity * currentPrice;

    if (metalsPrices) {
      if (inv.assetType === "gold") {
        const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit || "g");
        val = grams * metalsPrices.gold;
      } else if (inv.assetType === "silver") {
        const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit || "g");
        val = grams * metalsPrices.silver;
      }
    }

    typeMap[inv.assetType] = (typeMap[inv.assetType] || 0) + val;
  });

  const typeData = Object.keys(typeMap).map((type) => ({
    type,
    label: ASSET_TYPE_LABELS[type] || type,
    value: typeMap[type],
  })).sort((a, b) => b.value - a.value);

  return (
    <div className="glass-card rounded-2xl p-6 flex-1 flex flex-col justify-between">
      <div>
        <div className="flex items-center justify-between border-b border-slate-900 pb-4 mb-4">
          <div>
            <h2 className="text-lg font-bold text-slate-100 flex items-center gap-2">
              <LineChart className="w-5 h-5 text-cyan-400" />
              <span>Asset Allocation</span>
            </h2>
            <p className="text-xs text-slate-400 mt-0.5">Distribution of current holdings.</p>
          </div>
        </div>

        {investments.length === 0 ? (
          <div className="flex flex-col items-center justify-center py-10 text-center text-slate-500">
            <Coins className="w-8 h-8 text-slate-800 mb-2" />
            <p className="text-xs">No active investments found.</p>
          </div>
        ) : (
          <div className="space-y-4">
            {typeData.map((item) => {
              const percent = totalValue > 0 ? (item.value / totalValue) * 100 : 0;
              const barColor = ASSET_TYPE_COLORS[item.type] || "bg-slate-500";
              return (
                <div key={item.type} className="space-y-1.5">
                  <div className="flex justify-between text-xs">
                    <span className="text-slate-300 font-medium">{item.label}</span>
                    <span className="text-slate-400 font-semibold">
                      ₹{item.value.toLocaleString("en-IN", { maximumFractionDigits: 0 })} ({percent.toFixed(0)}%)
                    </span>
                  </div>
                  {/* Progress bar */}
                  <div className="w-full bg-slate-950 h-2 rounded-full overflow-hidden border border-slate-900">
                    <div
                      className={`${barColor} h-full rounded-full transition-all duration-500`}
                      style={{ width: `${percent}%` }}
                    />
                  </div>
                </div>
              );
            })}
          </div>
        )}
      </div>
    </div>
  );
}
