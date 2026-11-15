"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { investmentSchema, investmentPriceSchema } from "@/lib/validators";
import { 
  createInvestment, 
  deleteInvestment, 
  addInvestmentPriceUpdate,
  createInvestmentAlert,
  deleteInvestmentAlert,
  cancelInvestmentSip,
  resumeInvestmentSip,
  toggleSipSkip,
  advanceSipNextDate
} from "@/actions/investments";
import { 
  Plus, 
  Trash2, 
  Calendar, 
  TrendingUp, 
  TrendingDown, 
  Coins, 
  LineChart, 
  Edit3, 
  AlertCircle,
  X,
  PlusCircle,
  Bell,
  CheckCircle,
  Clock,
  Sparkles,
  Play,
  Pause,
  ListFilter,
  Activity
} from "lucide-react";
import { format } from "date-fns";
import { getInvestmentDetails, calculateSipStats } from "@/lib/clientMath";

interface InvestmentsClientProps {
  initialInvestments: any[];
  accounts: any[];
  metalsPrices?: {
    success: boolean;
    gold: number;
    silver: number;
    nifty?: number;
    goldUSDPerOunce: number;
    silverUSDPerOunce: number;
    usdToInr: number;
  };
}

const ASSET_TYPES = [
  { value: "stock", label: "Stocks / Equities" },
  { value: "gold", label: "Gold (24K/22K)" },
  { value: "silver", label: "Silver" },
  { value: "mutual_fund", label: "Mutual Funds / SIP" },
  { value: "crypto", label: "Cryptocurrency" },
  { value: "real_estate", label: "Real Estate" },
  { value: "fixed_deposit", label: "Fixed Deposit" },
  { value: "other", label: "Other Asset / Bonds" },
];

export default function InvestmentsClient({ 
  initialInvestments, 
  accounts,
  metalsPrices = {
    success: false,
    gold: 7450,
    silver: 90,
    nifty: 24000,
    goldUSDPerOunce: 2350,
    silverUSDPerOunce: 29,
    usdToInr: 83.5
  }
}: InvestmentsClientProps) {
  const [investments, setInvestments] = useState(initialInvestments);
  const [showAddForm, setShowAddForm] = useState(false);
  const [showPriceForm, setShowPriceForm] = useState(false);
  const [showAlertForm, setShowAlertForm] = useState(false);
  const [selectedInvestmentForPrice, setSelectedInvestmentForPrice] = useState<string>("");
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  // Alert form state
  const [alertInvestmentId, setAlertInvestmentId] = useState("");
  const [alertTitle, setAlertTitle] = useState("");
  const [alertDate, setAlertDate] = useState("");
  const [alertNotes, setAlertNotes] = useState("");

  // Custom Confirm Modal state
  const [confirmModal, setConfirmModal] = useState<{
    isOpen: boolean;
    title: string;
    description: string;
    onConfirm: () => void;
  }>({
    isOpen: false,
    title: "",
    description: "",
    onConfirm: () => {},
  });

  const [selectedSipForManage, setSelectedSipForManage] = useState<any | null>(null);

  // Forms
  const addForm = useForm({
    resolver: zodResolver(investmentSchema),
    defaultValues: {
      assetType: "stock",
      name: "",
      symbol: "",
      quantity: 0,
      unit: "shares",
      purchasePrice: 0,
      purchaseDate: new Date().toISOString().split("T")[0],
      accountId: "",
      subType: "",
      isSIP: false,
      sipFrequency: "monthly",
      sipAmount: 0,
      sipNextDate: "",
    },
  });

  const priceForm = useForm({
    resolver: zodResolver(investmentPriceSchema),
    defaultValues: {
      investmentId: "",
      priceDate: new Date().toISOString().split("T")[0],
      pricePerUnit: 0,
    },
  });

  const watchAssetType = addForm.watch("assetType");
  const watchSubType = addForm.watch("subType");
  const watchIsSIP = addForm.watch("isSIP");

  // Calculations
  const processedInvestments = investments.map((inv) => {
    const details = getInvestmentDetails(inv, metalsPrices);
    const sipStats = inv.isSIP ? calculateSipStats(inv) : null;
    return {
      ...inv,
      ...details,
      sipStats,
    };
  });

  const totalPurchaseValue = processedInvestments.reduce((sum, inv) => sum + inv.purchaseValue, 0);
  const totalCurrentValue = processedInvestments.reduce((sum, inv) => sum + inv.currentValue, 0);
  const totalGainLoss = totalCurrentValue - totalPurchaseValue;
  const overallGainLossPercent = totalPurchaseValue > 0 ? (totalGainLoss / totalPurchaseValue) * 100 : 0;

  // Aggregate all alerts across all investments
  const todayDate = new Date();
  todayDate.setHours(0, 0, 0, 0);

  const dynamicSipAlerts = processedInvestments.flatMap((inv) => {
    if (!inv.isSIP || inv.sipStatus === "cancelled" || !inv.sipNextDate) return [];

    const nextDate = new Date(inv.sipNextDate);
    nextDate.setHours(0, 0, 0, 0);

    // Check if this next installment month is already skipped
    const isSkipped = (inv.sipSkips || []).some((skip: any) => {
      const d = new Date(skip.skipDate);
      return d.getFullYear() === nextDate.getFullYear() && d.getMonth() === nextDate.getMonth();
    });

    if (isSkipped) return [];

    const alerts = [];
    const oneDay = 24 * 60 * 60 * 1000;
    const diffDays = Math.ceil((nextDate.getTime() - todayDate.getTime()) / oneDay);

    // If 1 day before due date
    if (diffDays === 1) {
      alerts.push({
        id: `sip-due-tomorrow-${inv.id}`,
        investmentId: inv.id,
        investmentName: inv.name,
        assetType: inv.assetType,
        title: `SIP Due Tomorrow: ${inv.name}`,
        alertDate: new Date(todayDate).toISOString(),
        targetDueDate: nextDate.toISOString(),
        isTriggered: false,
        notes: `Your ${inv.sipFrequency} SIP installment of ₹${inv.sipAmount} is due tomorrow.`,
        isDynamic: true,
      });
    }
    // If today is the due date
    else if (diffDays === 0) {
      alerts.push({
        id: `sip-due-today-${inv.id}`,
        investmentId: inv.id,
        investmentName: inv.name,
        assetType: inv.assetType,
        title: `SIP Due Today: ${inv.name}`,
        alertDate: new Date(todayDate).toISOString(),
        targetDueDate: nextDate.toISOString(),
        isTriggered: false,
        notes: `Your ${inv.sipFrequency} SIP installment of ₹${inv.sipAmount} is due today. Please make sure the auto-debit succeeds.`,
        isDynamic: true,
      });
    }
    // If due date has crossed (overdue)
    else if (diffDays < 0) {
      alerts.push({
        id: `sip-overdue-${inv.id}`,
        investmentId: inv.id,
        investmentName: inv.name,
        assetType: inv.assetType,
        title: `SIP Overdue (Date Crossed): ${inv.name}`,
        alertDate: new Date(todayDate).toISOString(),
        targetDueDate: nextDate.toISOString(),
        isTriggered: true,
        notes: `The due date (${format(nextDate, "dd MMM yyyy")}) for your ₹${inv.sipAmount} SIP has crossed. Check if the auto-debit succeeded or log it as skipped.`,
        isDynamic: true,
        isOverdue: true,
      });
    }

    return alerts;
  });

  const activeAlerts = [
    ...processedInvestments.flatMap(inv => 
      (inv.alerts || []).map((alert: any) => ({
        ...alert,
        investmentName: inv.name,
        assetType: inv.assetType,
        isDynamic: false,
      }))
    ),
    ...dynamicSipAlerts
  ].sort((a, b) => new Date(a.alertDate).getTime() - new Date(b.alertDate).getTime());

  // Action: Add Investment
  const onAddInvestment = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await createInvestment(data);
      if (res.success && res.data) {
        // Hydrate locally
        const newInv = {
          ...res.data,
          prices: [{ pricePerUnit: res.data.purchasePrice, priceDate: res.data.purchaseDate }],
          alerts: res.data.sipNextDate ? [{
            id: Math.random().toString(),
            title: `SIP Due: ${res.data.name}`,
            alertDate: res.data.sipNextDate,
            isTriggered: false,
            notes: `Automatic reminder for your ${res.data.sipFrequency} SIP of ₹${res.data.sipAmount}.`
          }] : [],
        };
        setInvestments((prev) => [newInv, ...prev]);
        addForm.reset({
          assetType: "stock",
          name: "",
          symbol: "",
          quantity: 0,
          unit: "shares",
          purchasePrice: 0,
          purchaseDate: new Date().toISOString().split("T")[0],
          accountId: "",
          subType: "",
          isSIP: false,
          sipFrequency: "monthly",
          sipAmount: 0,
          sipNextDate: "",
        });
        setShowAddForm(false);
      } else {
        setError(res.error || "Failed to log investment");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Price Update
  const onAddPriceUpdate = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await addInvestmentPriceUpdate(data);
      if (res.success && res.data) {
        setInvestments((prev) =>
          prev.map((inv) => {
            if (inv.id === data.investmentId) {
              const updatedPrices = [...(inv.prices || [])];
              const matchIndex = updatedPrices.findIndex(
                (p) => format(new Date(p.priceDate), "yyyy-MM-dd") === data.priceDate
              );
              if (matchIndex > -1) {
                updatedPrices[matchIndex].pricePerUnit = data.pricePerUnit;
              } else {
                updatedPrices.unshift({
                  priceDate: new Date(data.priceDate),
                  pricePerUnit: data.pricePerUnit,
                });
              }
              return { ...inv, prices: updatedPrices };
            }
            return inv;
          })
        );
        priceForm.reset({
          investmentId: "",
          priceDate: new Date().toISOString().split("T")[0],
          pricePerUnit: 0,
        });
        setShowPriceForm(false);
      } else {
        setError(res.error || "Failed to update price");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Delete Investment
  const onDeleteInvestment = (id: string) => {
    setConfirmModal({
      isOpen: true,
      title: "Delete Investment Asset",
      description: "Are you sure you want to delete this investment holding? This will remove the asset from your holdings list and update your net worth valuation.",
      onConfirm: async () => {
        setConfirmModal((prev) => ({ ...prev, isOpen: false }));
        try {
          const res = await deleteInvestment(id);
          if (res.success) {
            setInvestments((prev) => prev.filter((i) => i.id !== id));
          } else {
            alert(res.error || "Failed to delete investment");
          }
        } catch (err: any) {
          alert(err.message || "An error occurred");
        }
      }
    });
  };

  // Action: Create Alert
  const handleCreateAlert = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!alertInvestmentId || !alertTitle || !alertDate) {
      alert("Please fill in all required alert fields.");
      return;
    }
    setLoading(true);
    try {
      const res = await createInvestmentAlert({
        investmentId: alertInvestmentId,
        title: alertTitle,
        alertDate,
        notes: alertNotes,
      });
      if (res.success && res.data) {
        // Hydrate locally
        setInvestments((prev) =>
          prev.map((inv) => {
            if (inv.id === alertInvestmentId) {
              return {
                ...inv,
                alerts: [...(inv.alerts || []), res.data].sort(
                  (a, b) => new Date(a.alertDate).getTime() - new Date(b.alertDate).getTime()
                ),
              };
            }
            return inv;
          })
        );
        // Reset alert inputs
        setAlertInvestmentId("");
        setAlertTitle("");
        setAlertDate("");
        setAlertNotes("");
        setShowAlertForm(false);
      } else {
        alert(res.error || "Failed to create alert");
      }
    } catch (err: any) {
      alert(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Delete Alert
  const handleDeleteAlert = (alertId: string, investmentId: string) => {
    setConfirmModal({
      isOpen: true,
      title: "Dismiss Reminder Alert",
      description: "Are you sure you want to dismiss this reminder alert? You will no longer receive notifications or calendar updates for this event.",
      onConfirm: async () => {
        setConfirmModal((prev) => ({ ...prev, isOpen: false }));
        try {
          const res = await deleteInvestmentAlert(alertId);
          if (res.success) {
            setInvestments((prev) =>
              prev.map((inv) => {
                if (inv.id === investmentId) {
                  return {
                    ...inv,
                    alerts: (inv.alerts || []).filter((a: any) => a.id !== alertId),
                  };
                }
                return inv;
              })
            );
          } else {
            alert(res.error || "Failed to remove alert");
          }
        } catch (err: any) {
          alert(err.message || "An error occurred");
        }
      }
    });
  };

  // Action: Cancel SIP
  const handleCancelSip = async (id: string) => {
    setConfirmModal({
      isOpen: true,
      title: "Cancel SIP Schedule",
      description: "Are you sure you want to cancel this SIP? Automatic deposits will stop compiling from today. All historical payments will be preserved.",
      onConfirm: async () => {
        setConfirmModal((prev) => ({ ...prev, isOpen: false }));
        try {
          const res = await cancelInvestmentSip(id);
          if (res.success) {
            setInvestments((prev) =>
              prev.map((inv) => {
                if (inv.id === id) {
                  return {
                    ...inv,
                    sipStatus: "cancelled",
                    sipCancelDate: new Date().toISOString(),
                  };
                }
                return inv;
              })
            );
            // Refresh managed modal details if open
            setSelectedSipForManage((prev: any) => {
              if (prev && prev.id === id) {
                return {
                  ...prev,
                  sipStatus: "cancelled",
                  sipCancelDate: new Date().toISOString(),
                };
              }
              return prev;
            });
          } else {
            alert(res.error || "Failed to cancel SIP");
          }
        } catch (err: any) {
          alert(err.message || "An error occurred");
        }
      }
    });
  };

  // Action: Resume SIP
  const handleResumeSip = async (id: string) => {
    try {
      const res = await resumeInvestmentSip(id);
      if (res.success) {
        setInvestments((prev) =>
          prev.map((inv) => {
            if (inv.id === id) {
              return {
                ...inv,
                sipStatus: "active",
                sipCancelDate: null,
              };
            }
            return inv;
          })
        );
        // Refresh managed modal details if open
        setSelectedSipForManage((prev: any) => {
          if (prev && prev.id === id) {
            return {
              ...prev,
              sipStatus: "active",
              sipCancelDate: null,
            };
          }
          return prev;
        });
      } else {
        alert(res.error || "Failed to resume SIP");
      }
    } catch (err: any) {
      alert(err.message || "An error occurred");
    }
  };

  // Action: Toggle Skip Date
  const handleToggleSkipDate = async (investmentId: string, dateStr: string) => {
    try {
      const res = await toggleSipSkip(investmentId, dateStr);
      if (res.success) {
        setInvestments((prev) =>
          prev.map((inv) => {
            if (inv.id === investmentId) {
              const currentSkips = [...(inv.sipSkips || [])];
              const skipDate = new Date(dateStr);
              skipDate.setHours(0, 0, 0, 0);

              const matchIndex = currentSkips.findIndex(
                (skip) => {
                  const d = new Date(skip.skipDate);
                  d.setHours(0,0,0,0);
                  return d.getTime() === skipDate.getTime();
                }
              );

              if (matchIndex > -1) {
                currentSkips.splice(matchIndex, 1);
              } else {
                currentSkips.push({ skipDate });
              }

              const updatedInv = { ...inv, sipSkips: currentSkips };
              // Refresh managed modal details if open
              setSelectedSipForManage((prev: any) => {
                if (prev && prev.id === investmentId) {
                  return updatedInv;
                }
                return prev;
              });

              return updatedInv;
            }
            return inv;
          })
        );
      } else {
        alert(res.error || "Failed to toggle skip date");
      }
    } catch (err: any) {
      alert(err.message || "An error occurred");
    }
  };

  // Action: Advance SIP Next Date (Rollover)
  const handleAdvanceSipNextDate = async (id: string) => {
    try {
      const res = await advanceSipNextDate(id);
      if (res.success) {
        setInvestments((prev) =>
          prev.map((inv) => {
            if (inv.id === id) {
              const currentNext = new Date(inv.sipNextDate || new Date());
              if (inv.sipFrequency === "weekly") {
                currentNext.setDate(currentNext.getDate() + 7);
              } else if (inv.sipFrequency === "monthly") {
                currentNext.setMonth(currentNext.getMonth() + 1);
              } else if (inv.sipFrequency === "half_yearly") {
                currentNext.setMonth(currentNext.getMonth() + 6);
              } else if (inv.sipFrequency === "yearly") {
                currentNext.setFullYear(currentNext.getFullYear() + 1);
              }
              const updated = {
                ...inv,
                sipNextDate: currentNext.toISOString(),
              };
              // Refresh managed modal details if open
              setSelectedSipForManage((prevModal: any) => {
                if (prevModal && prevModal.id === id) {
                  return updated;
                }
                return prevModal;
              });
              return updated;
            }
            return inv;
          })
        );
      } else {
        alert(res.error || "Failed to advance Next Date");
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
          <h1 className="text-3xl font-extrabold tracking-tight text-slate-100 font-sans">Portfolio Investments</h1>
          <p className="text-slate-400 mt-1">Track asset performance, yields, live metals spot prices, and SIP plans.</p>
        </div>
        <div className="flex items-center gap-3">
          <button
            onClick={() => {
              setShowAlertForm(true);
              setShowAddForm(false);
              setShowPriceForm(false);
            }}
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-slate-900 border border-slate-800 font-medium text-sm text-slate-200 transition-all hover:scale-[1.01]"
          >
            <Bell className="w-4 h-4 text-amber-400 animate-pulse" />
            <span>Set Reminder</span>
          </button>
          <button
            onClick={() => {
              setShowPriceForm(true);
              setShowAddForm(false);
              setShowAlertForm(false);
              priceForm.setValue("investmentId", selectedInvestmentForPrice);
            }}
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-slate-900 border border-slate-800 font-medium text-sm text-slate-200 transition-all hover:scale-[1.01]"
          >
            <Edit3 className="w-4 h-4 text-cyan-400" />
            <span>Update Price</span>
          </button>
          <button
            onClick={() => {
              setShowAddForm(true);
              setShowPriceForm(false);
              setShowAlertForm(false);
            }}
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-cyan-600 hover:bg-cyan-500 font-medium text-sm text-white shadow-lg shadow-cyan-600/10 transition-all hover:scale-[1.01]"
          >
            <Plus className="w-4 h-4" />
            <span>Add Asset</span>
          </button>
        </div>
      </div>

      {/* Live Metal Spot Prices Bar */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
        {/* NIFTY 50 */}
        <div className="glass-card rounded-2xl p-4 border border-emerald-500/20 flex items-center justify-between">
          <div className="flex items-center gap-3">
            <TrendingUp className="w-8 h-8 text-emerald-400" />
            <div>
              <p className="text-xs font-semibold text-slate-400 uppercase">NIFTY 50</p>
              <p className="text-lg font-bold text-slate-200">
                ₹{metalsPrices.nifty?.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
              </p>
            </div>
          </div>
          <div className="text-right">
            <span className="text-[10px] text-green-400 flex items-center justify-end gap-1">
              <span className="w-1.5 h-1.5 rounded-full bg-green-500 animate-pulse"></span>
              Live
            </span>
          </div>
        </div>

        {/* Live Gold */}
        <div className="glass-card rounded-2xl p-4 border border-amber-500/20 flex items-center justify-between">
          <div className="flex items-center gap-3">
            <Coins className="w-8 h-8 text-amber-400" />
            <div>
              <p className="text-xs font-semibold text-slate-400 uppercase">Live Gold (24K)</p>
              <p className="text-lg font-bold text-slate-200">
                ₹{metalsPrices.gold.toLocaleString("en-IN", { maximumFractionDigits: 2 })} <span className="text-xs text-slate-500">/ gram</span>
              </p>
            </div>
          </div>
          <div className="text-right">
            <p className="text-[10px] text-slate-500">Spot Rate (London PM)</p>
            <p className="text-xs font-semibold text-amber-500/90 font-mono">
              ${metalsPrices.goldUSDPerOunce.toLocaleString("en-US", { maximumFractionDigits: 2 })} / oz
            </p>
          </div>
        </div>

        {/* Live Silver */}
        <div className="glass-card rounded-2xl p-4 border border-slate-300/20 flex items-center justify-between">
          <div className="flex items-center gap-3">
            <Coins className="w-8 h-8 text-slate-300" />
            <div>
              <p className="text-xs font-semibold text-slate-400 uppercase">Live Silver</p>
              <p className="text-lg font-bold text-slate-200">
                ₹{metalsPrices.silver.toLocaleString("en-IN", { maximumFractionDigits: 2 })} <span className="text-xs text-slate-500">/ gram</span>
              </p>
            </div>
          </div>
          <div className="text-right">
            <p className="text-[10px] text-slate-500">Spot Rate (London PM)</p>
            <p className="text-xs font-semibold text-slate-300/90 font-mono">
              ${metalsPrices.silverUSDPerOunce.toLocaleString("en-US", { maximumFractionDigits: 2 })} / oz
            </p>
          </div>
        </div>

        {/* USD to INR */}
        <div className="glass-card rounded-2xl p-4 border border-indigo-500/20 flex items-center justify-between">
          <div className="flex items-center gap-3">
            <Activity className="w-8 h-8 text-indigo-400" />
            <div>
              <p className="text-xs font-semibold text-slate-400 uppercase">USD to INR</p>
              <p className="text-lg font-bold text-slate-200">
                ₹{metalsPrices.usdToInr.toLocaleString("en-IN", { maximumFractionDigits: 2 })} <span className="text-xs text-slate-500">/ $1</span>
              </p>
            </div>
          </div>
          <div className="text-right">
            <span className="text-[10px] text-green-400 flex items-center justify-end gap-1">
              <span className="w-1.5 h-1.5 rounded-full bg-green-500 animate-pulse"></span>
              Live
            </span>
          </div>
        </div>
      </div>

      {error && (
        <div className="p-4 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm flex items-start gap-3">
          <AlertCircle className="w-5 h-5 shrink-0 mt-0.5" />
          <p>{error}</p>
        </div>
      )}

      {/* Forms Popup/Drawer */}
      {showAddForm && (
        <div className="glass-card rounded-2xl p-6 border border-cyan-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <PlusCircle className="w-5 h-5 text-cyan-400" />
              <span>Add Investment Asset</span>
            </h3>
            <button onClick={() => setShowAddForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={addForm.handleSubmit(onAddInvestment)} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            
            {/* Category */}
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Asset Category</label>
              <select
                {...addForm.register("assetType")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              >
                {ASSET_TYPES.map((type) => (
                  <option key={type.value} value={type.value}>
                    {type.label}
                  </option>
                ))}
              </select>
            </div>

            {/* Name */}
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Asset Name</label>
              <input
                {...addForm.register("name")}
                placeholder={
                  watchAssetType === "gold" || watchAssetType === "silver"
                    ? "e.g. Joyalukkas Gold Coin, HDFC Digital Gold"
                    : watchAssetType === "mutual_fund"
                    ? "e.g. Parag Parikh Flexi Cap Fund"
                    : "e.g. Reliance Industries, Government Bond 2030"
                }
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
              {addForm.formState.errors.name && (
                <p className="text-xs text-red-400">{addForm.formState.errors.name.message}</p>
              )}
            </div>

            {/* Dynamic Gold/Silver sub-questions */}
            {(watchAssetType === "gold" || watchAssetType === "silver") && (
              <>
                <div className="space-y-1">
                  <label className="text-xs font-semibold text-slate-300">Precious Metal Type</label>
                  <select
                    {...addForm.register("subType")}
                    onChange={(e) => {
                      const val = e.target.value;
                      if (val === "physical") {
                        addForm.setValue("unit", "g");
                      } else {
                        addForm.setValue("unit", "unit");
                      }
                    }}
                    className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                  >
                    <option value="">Choose Sub-type...</option>
                    <option value="physical">Physical Metal (Coins, Bars, Jewelry)</option>
                    <option value="digital">Digital Metal</option>
                    <option value="sgb">Sovereign Gold Bond (SGB) / Metal ETF</option>
                  </select>
                </div>

                {watchSubType === "physical" ? (
                  <div className="space-y-1">
                    <label className="text-xs font-semibold text-slate-300">Weight Unit</label>
                    <select
                      {...addForm.register("unit")}
                      className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                    >
                      <option value="g">Grams (g)</option>
                      <option value="kg">Kilograms (kg)</option>
                      <option value="sovereign">Sovereign / Pavan (8 grams)</option>
                      <option value="oz">Troy Ounces (oz)</option>
                    </select>
                  </div>
                ) : (
                  <div className="space-y-1">
                    <label className="text-xs font-semibold text-slate-300">Holding Unit</label>
                    <input
                      {...addForm.register("unit")}
                      placeholder="e.g. units, grams"
                      className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                    />
                  </div>
                )}
              </>
            )}

            {/* Stocks Symbol field */}
            {watchAssetType === "stock" && (
              <div className="space-y-1">
                <label className="text-xs font-semibold text-slate-300">Ticker Symbol / Exchange Ticker</label>
                <input
                  {...addForm.register("symbol")}
                  placeholder="e.g. RELIANCE.NS, TCS.NS"
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                />
              </div>
            )}

            {/* Quantity purchased (except SIP where we log recurring money details) */}
            {!watchIsSIP && (
              <div className="space-y-1">
                <label className="text-xs font-semibold text-slate-300">
                  {watchAssetType === "gold" || watchAssetType === "silver" ? "Weight / Quantity" : "Quantity Purchased"}
                </label>
                <input
                  {...addForm.register("quantity")}
                  type="number"
                  step="0.0001"
                  placeholder="0.00"
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                />
                {addForm.formState.errors.quantity && (
                  <p className="text-xs text-red-400">{addForm.formState.errors.quantity.message}</p>
                )}
              </div>
            )}

            {/* Purchase Price (except SIP where we specify recurring amount) */}
            {!watchIsSIP && (
              <div className="space-y-1">
                <label className="text-xs font-semibold text-slate-300">
                  {watchAssetType === "gold" || watchAssetType === "silver" 
                    ? "Purchase Price Per Unit / Gram (₹)" 
                    : "Purchase Price Per Unit (₹)"}
                </label>
                <input
                  {...addForm.register("purchasePrice")}
                  type="number"
                  step="0.01"
                  placeholder="0.00"
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                />
                {addForm.formState.errors.purchasePrice && (
                  <p className="text-xs text-red-400">{addForm.formState.errors.purchasePrice.message}</p>
                )}
              </div>
            )}

            {/* Systematic Investment Plan (SIP) Trigger */}
            {(watchAssetType === "mutual_fund" || watchAssetType === "stock" || watchAssetType === "other") && (
              <div className="md:col-span-2 flex items-center gap-2 py-2">
                <input
                  {...addForm.register("isSIP")}
                  type="checkbox"
                  id="isSIPCheckbox"
                  onChange={(e) => {
                    addForm.setValue("isSIP", e.target.checked);
                    if (e.target.checked) {
                      addForm.setValue("quantity", 1);
                      addForm.setValue("purchasePrice", 0);
                    }
                  }}
                  className="w-4 h-4 rounded text-cyan-600 bg-slate-950 border-slate-800 focus:ring-cyan-500"
                />
                <label htmlFor="isSIPCheckbox" className="text-sm font-semibold text-slate-300 cursor-pointer">
                  Is this a Systematic Investment Plan (SIP)?
                </label>
              </div>
            )}

            {/* SIP Details Panel */}
            {watchIsSIP && (
              <div className="md:col-span-2 border border-slate-800/80 bg-slate-900/10 rounded-xl p-4 grid grid-cols-1 md:grid-cols-2 gap-4">
                <div className="space-y-1">
                  <label className="text-xs font-semibold text-slate-300">SIP Asset Division</label>
                  <select
                    {...addForm.register("subType")}
                    className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                  >
                    <option value="large_cap">Large Cap Equity (Stable Blue-Chip)</option>
                    <option value="mid_cap">Mid Cap Equity (Medium Companies)</option>
                    <option value="small_cap">Small Cap Equity (High Growth)</option>
                    <option value="multi_cap">Multi Cap / Flexi Cap Fund</option>
                  </select>
                </div>

                <div className="space-y-1">
                  <label className="text-xs font-semibold text-slate-300">Frequency</label>
                  <select
                    {...addForm.register("sipFrequency")}
                    className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                  >
                    <option value="weekly">Weekly</option>
                    <option value="monthly">Monthly</option>
                    <option value="half_yearly">6 Months Once (Semi-Annually)</option>
                    <option value="yearly">Yearly</option>
                  </select>
                </div>

                <div className="space-y-1">
                  <label className="text-xs font-semibold text-slate-300">SIP Installment Amount (₹)</label>
                  <input
                    {...addForm.register("sipAmount")}
                    type="number"
                    placeholder="e.g. 5000"
                    className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                  />
                </div>

                <div className="space-y-1">
                  <label className="text-xs font-semibold text-slate-300">Next SIP Payment Date</label>
                  <input
                    {...addForm.register("sipNextDate")}
                    type="date"
                    className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                  />
                </div>
              </div>
            )}

            {/* General Fields */}
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Purchase / Start Date</label>
              <input
                {...addForm.register("purchaseDate")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Funding Source Account</label>
              <select
                {...addForm.register("accountId")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Select Account (No deduction)...</option>
                {accounts.map((acc) => (
                  <option key={acc.id} value={acc.id}>
                    {acc.accountName} (Balance: ₹{acc.currentBalance})
                  </option>
                ))}
              </select>
            </div>

            <div className="md:col-span-2 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowAddForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-cyan-600 hover:bg-cyan-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Logging..." : "Log Holding"}
              </button>
            </div>
          </form>
        </div>
      )}

      {/* Alert Creation Form */}
      {showAlertForm && (
        <div className="glass-card rounded-2xl p-6 border border-amber-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <Bell className="w-5 h-5 text-amber-400" />
              <span>Set Investment Due Alert</span>
            </h3>
            <button onClick={() => setShowAlertForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={handleCreateAlert} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-1 md:col-span-2">
              <label className="text-xs font-semibold text-slate-300">Select Portfolio Holding</label>
              <select
                value={alertInvestmentId}
                onChange={(e) => setAlertInvestmentId(e.target.value)}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Choose Asset...</option>
                {investments.map((inv) => (
                  <option key={inv.id} value={inv.id}>
                    {inv.name} ({inv.assetType})
                  </option>
                ))}
              </select>
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Alert Title / Event</label>
              <input
                value={alertTitle}
                onChange={(e) => setAlertTitle(e.target.value)}
                placeholder="e.g. Monthly Gold Purchase, SIP Renewal"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Trigger Date</label>
              <input
                value={alertDate}
                onChange={(e) => setAlertDate(e.target.value)}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1 md:col-span-2">
              <label className="text-xs font-semibold text-slate-300">Reminding Notes (Optional)</label>
              <textarea
                value={alertNotes}
                onChange={(e) => setAlertNotes(e.target.value)}
                placeholder="e.g. Pay ₹5,000 via HDFC bank transfer before maturity."
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm h-20 resize-none"
              />
            </div>

            <div className="md:col-span-2 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowAlertForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-cyan-600 hover:bg-cyan-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Scheduling..." : "Create Reminder Alert"}
              </button>
            </div>
          </form>
        </div>
      )}

      {showPriceForm && (
        <div className="glass-card rounded-2xl p-6 border border-cyan-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <LineChart className="w-5 h-5 text-cyan-400" />
              <span>Update Asset Price</span>
            </h3>
            <button onClick={() => setShowPriceForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={priceForm.handleSubmit(onAddPriceUpdate)} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-1 md:col-span-2">
              <label className="text-xs font-semibold text-slate-300">Select Asset Holding</label>
              <select
                {...priceForm.register("investmentId")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Choose Asset...</option>
                {investments.map((inv) => (
                  <option key={inv.id} value={inv.id}>
                    {inv.name} ({inv.symbol || "No Symbol"})
                  </option>
                ))}
              </select>
              {priceForm.formState.errors.investmentId && (
                <p className="text-xs text-red-400">{priceForm.formState.errors.investmentId.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Updated Price Per Unit (₹)</label>
              <input
                {...priceForm.register("pricePerUnit")}
                type="number"
                step="0.01"
                placeholder="0.00"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
              {priceForm.formState.errors.pricePerUnit && (
                <p className="text-xs text-red-400">{priceForm.formState.errors.pricePerUnit.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Price As Of Date</label>
              <input
                {...priceForm.register("priceDate")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="md:col-span-2 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowPriceForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-cyan-600 hover:bg-cyan-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Updating..." : "Update Valuation"}
              </button>
            </div>
          </form>
        </div>
      )}

      {/* KPI Stats Grid */}
      <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-4 gap-6">
        <div className="glass-card rounded-2xl p-5">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Purchase Cost</p>
          <p className="text-2xl font-bold text-slate-200 mt-2">
            ₹{totalPurchaseValue.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
          </p>
        </div>
        <div className="glass-card rounded-2xl p-5">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Current Value</p>
          <p className="text-2xl font-bold text-slate-200 mt-2">
            ₹{totalCurrentValue.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
          </p>
        </div>
        <div className="glass-card rounded-2xl p-5">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Net Return</p>
          <div className="flex items-center gap-2 mt-2">
            <span className={`text-2xl font-bold ${totalGainLoss >= 0 ? "text-green-400" : "text-red-400"}`}>
              ₹{totalGainLoss.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
            </span>
          </div>
        </div>
        <div className="glass-card rounded-2xl p-5">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">ROI Percent</p>
          <div className="flex items-center gap-2 mt-2">
            {totalGainLoss >= 0 ? (
              <span className="text-2xl font-bold text-green-400 flex items-center gap-1">
                <TrendingUp className="w-5 h-5" />
                +{overallGainLossPercent.toFixed(2)}%
              </span>
            ) : (
              <span className="text-2xl font-bold text-red-400 flex items-center gap-1">
                <TrendingDown className="w-5 h-5" />
                {overallGainLossPercent.toFixed(2)}%
              </span>
            )}
          </div>
        </div>
      </div>

      {/* Investments Holdings List */}
      <div className="space-y-4">
        <h2 className="text-xl font-bold text-slate-100">Holdings Ledger</h2>
        {processedInvestments.length === 0 ? (
          <div className="glass-card rounded-2xl p-8 text-center text-slate-500 text-sm">
            No assets found in your portfolio. Click &quot;Add Asset&quot; to log your first investment!
          </div>
        ) : (
          <div className="glass-card rounded-2xl overflow-hidden border border-slate-900">
            <div className="overflow-x-auto">
              <table className="w-full border-collapse text-left">
                <thead>
                  <tr className="border-b border-slate-900 bg-slate-900/20 text-xs font-semibold text-slate-400 uppercase tracking-wider">
                    <th className="p-4">Asset Name</th>
                    <th className="p-4">Qty / Unit</th>
                    <th className="p-4">Avg Purchase</th>
                    <th className="p-4">Current Price</th>
                    <th className="p-4">Current Value</th>
                    <th className="p-4">ROI</th>
                    <th className="p-4 text-right">Actions</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-slate-900 text-sm">
                  {processedInvestments.map((inv) => (
                    <tr key={inv.id} className="hover:bg-slate-900/10 transition-colors">
                      <td className="p-4">
                        <div>
                          <p className="font-semibold text-slate-200 flex items-center gap-1.5">
                            {inv.name}
                            {inv.isSIP && (
                              <span className="text-[9px] bg-cyan-500/15 border border-cyan-500/20 text-cyan-400 px-1.5 py-0.5 rounded-full flex items-center gap-0.5 uppercase">
                                <Sparkles className="w-2.5 h-2.5" /> SIP
                              </span>
                            )}
                          </p>
                          <div className="flex items-center gap-2 mt-0.5 text-xs text-slate-500 uppercase">
                            <span>{inv.assetType}</span>
                            {inv.subType && (
                              <>
                                <span>•</span>
                                <span>{inv.subType.replace("_", " ")}</span>
                              </>
                            )}
                            {inv.isSIP && inv.sipFrequency && (
                              <>
                                <span>•</span>
                                <span className="text-cyan-500/90 font-semibold">{inv.sipFrequency} (₹{inv.sipAmount})</span>
                              </>
                            )}
                            {inv.symbol && (
                              <>
                                <span>•</span>
                                <span className="font-mono text-[10px] bg-slate-900 px-1 py-0.2 rounded text-slate-400">
                                  {inv.symbol}
                                </span>
                              </>
                            )}
                          </div>
                        </div>
                      </td>
                      <td className="p-4 text-slate-300 font-mono">
                        {inv.isSIP && inv.sipStats ? (
                          <div>
                            {inv.purchasePrice > 0 ? (
                              <>
                                <p>{(inv.sipStats.totalInvested / inv.purchasePrice).toLocaleString("en-IN", { maximumFractionDigits: 4 })} <span className="text-xs text-slate-500">{inv.unit}</span></p>
                                <p className="text-[10px] text-slate-500 font-sans mt-0.5">{inv.sipStats.installmentsCompleted} deposits</p>
                              </>
                            ) : (
                              <>
                                <p>{inv.sipStats.installmentsCompleted} <span className="text-xs text-slate-500">payments</span></p>
                                <p className="text-[10px] text-slate-500 font-sans mt-0.5">{inv.sipStats.skipCount} skipped</p>
                              </>
                            )}
                          </div>
                        ) : (
                          <>
                            {inv.quantity} <span className="text-xs text-slate-500">{inv.unit}</span>
                          </>
                        )}
                      </td>
                      <td className="p-4 text-slate-300 font-mono">
                        {inv.isSIP ? (
                          <div>
                            <p>₹{inv.sipAmount?.toLocaleString("en-IN")}</p>
                            <p className="text-[10px] text-slate-500 font-sans mt-0.5">per {inv.sipFrequency?.replace("half_yearly", "6 months")}</p>
                          </div>
                        ) : (
                          `₹${inv.purchasePrice.toLocaleString("en-IN")}`
                        )}
                      </td>
                      <td className="p-4 text-slate-300 font-mono">
                        {inv.isSIP && inv.purchasePrice === 0 ? (
                          <span className="text-xs text-slate-500 italic">Accumulating</span>
                        ) : (
                          `₹${inv.currentPrice.toLocaleString("en-IN", { maximumFractionDigits: 2 })}`
                        )}
                      </td>
                      <td className="p-4 font-bold text-slate-200 font-mono">
                        ₹{inv.currentValue.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
                      </td>
                      <td className="p-4 font-semibold font-mono">
                        <div className="space-y-0.5">
                          <p className={inv.gainLoss >= 0 ? "text-green-400" : "text-red-400"}>
                            {inv.gainLoss >= 0 ? "+" : ""}
                            ₹{inv.gainLoss.toLocaleString("en-IN", { maximumFractionDigits: 0 })}
                          </p>
                          <p className={`text-[10px] ${inv.gainLoss >= 0 ? "text-green-500/80" : "text-red-500/80"}`}>
                            {inv.gainLoss >= 0 ? "+" : ""}
                            {inv.percentageChange.toFixed(2)}%
                          </p>
                        </div>
                      </td>
                      <td className="p-4 text-right">
                        <div className="flex items-center justify-end gap-1">
                          {inv.isSIP && (
                            <button
                              onClick={() => setSelectedSipForManage(inv)}
                              className="p-1.5 text-cyan-400 hover:text-cyan-300 hover:bg-cyan-500/5 rounded-lg border border-transparent hover:border-cyan-500/10 transition-all animate-pulse"
                              title="Manage SIP Skips & Status"
                            >
                              <ListFilter className="w-4 h-4" />
                            </button>
                          )}
                          <button
                            onClick={() => {
                              setSelectedInvestmentForPrice(inv.id);
                              setShowPriceForm(true);
                              priceForm.setValue("investmentId", inv.id);
                              priceForm.setValue("pricePerUnit", inv.currentPrice);
                            }}
                            className="p-1.5 text-slate-400 hover:text-cyan-400 hover:bg-cyan-500/5 rounded-lg border border-transparent hover:border-cyan-500/10 transition-all"
                            title="Update Price"
                          >
                            <Edit3 className="w-4 h-4" />
                          </button>
                          <button
                            onClick={() => onDeleteInvestment(inv.id)}
                            className="p-1.5 text-slate-500 hover:text-red-400 hover:bg-red-500/5 rounded-lg border border-transparent hover:border-red-500/10 transition-all"
                            title="Delete Asset"
                          >
                            <Trash2 className="w-4 h-4" />
                          </button>
                        </div>
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        )}
      </div>

      {/* Reminders & Alerts center */}
      <div className="space-y-4">
        <div className="flex items-center justify-between">
          <h2 className="text-xl font-bold text-slate-100 flex items-center gap-2">
            <Bell className="w-5 h-5 text-amber-400" />
            <span>Investment Reminder Alerts</span>
          </h2>
          <button
            onClick={() => setShowAlertForm(true)}
            className="text-xs font-semibold text-cyan-400 hover:underline flex items-center gap-1"
          >
            <Plus className="w-3.5 h-3.5" />
            <span>Schedule Alert</span>
          </button>
        </div>

        {activeAlerts.length === 0 ? (
          <div className="glass-card rounded-2xl p-6 text-center text-slate-500 text-sm border border-slate-900 bg-slate-950/20">
            No due alerts scheduled. Alerts will trigger automated reminders for your SIPs and maturity dates.
          </div>
        ) : (
          <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
            {activeAlerts.map((alert: any) => {
              const targetDate = alert.isDynamic ? new Date(alert.targetDueDate) : new Date(alert.alertDate);
              const daysLeft = Math.ceil((targetDate.getTime() - new Date().getTime()) / (1000 * 3600 * 24));
              const isPastDue = daysLeft < 0;

              return (
                <div 
                  key={alert.id} 
                  className={`glass-card rounded-2xl p-4 border flex items-start justify-between gap-4 transition-all ${
                    isPastDue ? "border-red-500/20 bg-red-950/5" : "border-slate-800/80 hover:border-slate-700/80"
                  }`}
                >
                  <div className="flex items-start gap-3">
                    <div className={`p-2 rounded-xl mt-0.5 ${isPastDue ? "bg-red-500/10 text-red-400" : "bg-amber-500/10 text-amber-400"}`}>
                      <Clock className="w-4 h-4" />
                    </div>
                    <div>
                      <div className="flex items-center gap-1.5 flex-wrap">
                        <h4 className="font-semibold text-slate-200 text-sm">{alert.title}</h4>
                        {alert.isDynamic && (
                          <span className="text-[8px] bg-cyan-500/15 border border-cyan-500/20 text-cyan-400 px-1 py-0.2 rounded-full uppercase tracking-widest font-mono">
                            Auto
                          </span>
                        )}
                      </div>
                      <p className="text-xs text-slate-400 font-sans mt-0.5">{alert.investmentName}</p>
                      {alert.notes && <p className="text-[11px] text-slate-500 mt-1 font-serif italic">{alert.notes}</p>}
                      <div className="flex items-center gap-2 mt-2">
                        <span className="text-[10px] bg-slate-950 px-2 py-0.5 rounded text-slate-400 font-mono">
                          Due: {format(targetDate, "dd MMM yyyy")}
                        </span>
                        <span className={`text-[10px] font-semibold ${
                          isPastDue ? "text-red-400 animate-pulse" : "text-amber-400"
                        }`}>
                          {isPastDue ? "Overdue (Date Crossed)" : daysLeft === 0 ? "Today" : `${daysLeft} days left`}
                        </span>
                      </div>
                    </div>
                  </div>
                  {alert.isDynamic ? (
                    <button
                      onClick={() => {
                        const inv = investments.find(i => i.id === alert.investmentId);
                        if (inv) setSelectedSipForManage(inv);
                      }}
                      className="p-1.5 text-cyan-400 hover:text-cyan-300 hover:bg-cyan-500/5 rounded-lg border border-transparent hover:border-cyan-500/10 transition-all"
                      title="Manage SIP Skips & Rollover"
                    >
                      <ListFilter className="w-3.5 h-3.5" />
                    </button>
                  ) : (
                    <button
                      onClick={() => handleDeleteAlert(alert.id, alert.investmentId)}
                      className="p-1.5 text-slate-500 hover:text-red-400 hover:bg-red-500/5 rounded-lg transition-all"
                      title="Dismiss reminder"
                    >
                      <Trash2 className="w-3.5 h-3.5" />
                    </button>
                  )}
                </div>
              );
            })}
          </div>
        )}
      {/* Custom Confirmation Modal */}
      {confirmModal.isOpen && (
        <div className="fixed inset-0 z-50 flex items-center justify-center p-4 bg-slate-950/85 backdrop-blur-md transition-all duration-300 animate-in fade-in">
          <div className="glass-card rounded-2xl p-6 max-w-md w-full border border-red-500/20 shadow-2xl shadow-red-500/5 animate-in fade-in zoom-in-95 duration-200">
            <div className="flex items-center gap-3 text-red-400 mb-4">
              <div className="p-2 bg-red-500/10 rounded-xl">
                <AlertCircle className="w-6 h-6 animate-pulse" />
              </div>
              <h3 className="font-bold text-lg text-slate-100">{confirmModal.title}</h3>
            </div>
            
            <p className="text-sm text-slate-300 leading-relaxed mb-6">
              {confirmModal.description}
            </p>

            <div className="flex justify-end gap-3">
              <button
                type="button"
                onClick={() => setConfirmModal((prev) => ({ ...prev, isOpen: false }))}
                className="px-4 py-2 rounded-xl bg-slate-900 border border-slate-800 text-xs font-semibold text-slate-300 hover:bg-slate-850 hover:text-slate-100 transition-all"
              >
                Cancel
              </button>
              <button
                type="button"
                onClick={confirmModal.onConfirm}
                className="px-4 py-2 rounded-xl bg-red-600 hover:bg-red-500 text-xs font-semibold text-white shadow-lg shadow-red-600/10 transition-all hover:scale-[1.01]"
              >
                Confirm Delete
              </button>
            </div>
          </div>
        </div>
      )}
      {/* Manage SIP Modal */}
      {selectedSipForManage && (() => {
        const stats = calculateSipStats(selectedSipForManage);
        const isActive = selectedSipForManage.sipStatus !== "cancelled";

        return (
          <div className="fixed inset-0 z-40 flex items-center justify-center p-4 bg-slate-950/80 backdrop-blur-sm transition-all duration-300">
            <div className="glass-card rounded-3xl p-6 max-w-2xl w-full border border-slate-800 shadow-2xl flex flex-col max-h-[90vh] animate-in fade-in zoom-in-95 duration-200">
              {/* Header */}
              <div className="flex items-center justify-between border-b border-slate-900 pb-4 mb-4">
                <div>
                  <h3 className="font-extrabold text-xl text-slate-100 flex items-center gap-2">
                    <Sparkles className="w-5 h-5 text-cyan-400 font-sans" />
                    <span>Manage SIP Plan</span>
                  </h3>
                  <p className="text-xs text-slate-400 mt-1">{selectedSipForManage.name}</p>
                </div>
                <button
                  onClick={() => setSelectedSipForManage(null)}
                  className="p-1.5 bg-slate-900 border border-slate-800 hover:border-slate-700 text-slate-400 hover:text-slate-200 rounded-xl transition-all"
                >
                  <X className="w-4 h-4" />
                </button>
              </div>

              {/* Scrollable Content */}
              <div className="flex-1 overflow-y-auto space-y-6 pr-1">
                {/* Status Section */}
                <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 p-4 bg-slate-950 rounded-2xl border border-slate-900">
                  <div className="flex items-center gap-3">
                    <div className={`w-3 h-3 rounded-full ${isActive ? "bg-green-500 animate-pulse" : "bg-red-500"}`} />
                    <div>
                      <p className="text-sm font-bold text-slate-200">
                        {isActive ? "Active Systematic Plan" : "Plan Suspended / Cancelled"}
                      </p>
                      <p className="text-xs text-slate-400 mt-0.5">
                        {isActive 
                          ? `Bank auto-debits ₹${selectedSipForManage.sipAmount} ${selectedSipForManage.sipFrequency}`
                          : `Stopped calculating automatic additions after ${format(new Date(selectedSipForManage.sipCancelDate), "dd MMM yyyy")}`
                        }
                      </p>
                    </div>
                  </div>
                  <div>
                    {isActive ? (
                      <button
                        type="button"
                        onClick={() => handleCancelSip(selectedSipForManage.id)}
                        className="flex items-center gap-1.5 px-4 py-2 bg-red-950/20 border border-red-500/20 hover:border-red-500/40 text-xs font-semibold text-red-400 rounded-xl transition-all"
                      >
                        <Pause className="w-3.5 h-3.5" /> Stop Plan
                      </button>
                    ) : (
                      <button
                        type="button"
                        onClick={() => handleResumeSip(selectedSipForManage.id)}
                        className="flex items-center gap-1.5 px-4 py-2 bg-green-950/20 border border-green-500/20 hover:border-green-500/40 text-xs font-semibold text-green-400 rounded-xl transition-all"
                      >
                        <Play className="w-3.5 h-3.5" /> Resume Plan
                      </button>
                    )}
                  </div>
                </div>

                {/* Next Cycle Rollover */}
                {isActive && selectedSipForManage.sipNextDate && (
                  <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4 p-4 bg-slate-900/10 rounded-2xl border border-cyan-500/20">
                    <div className="flex items-center gap-3">
                      <div className="p-2 bg-cyan-500/10 rounded-xl text-cyan-400">
                        <Clock className="w-5 h-5" />
                      </div>
                      <div>
                        <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Next Auto-Debit Date</p>
                        <p className="text-sm font-bold text-slate-200 mt-0.5">
                          {format(new Date(selectedSipForManage.sipNextDate), "dd MMMM yyyy")}
                        </p>
                      </div>
                    </div>
                    <div>
                      <button
                        type="button"
                        onClick={() => handleAdvanceSipNextDate(selectedSipForManage.id)}
                        className="flex items-center gap-1.5 px-4 py-2 bg-cyan-600 hover:bg-cyan-500 text-xs font-semibold text-white rounded-xl transition-all"
                      >
                        <CheckCircle className="w-3.5 h-3.5" /> Confirm Payment & Advance Month
                      </button>
                    </div>
                  </div>
                )}

                {/* Stats Grid */}
                <div className="grid grid-cols-2 sm:grid-cols-4 gap-4">
                  <div className="p-3 bg-slate-900/30 border border-slate-900 rounded-xl">
                    <p className="text-[10px] font-semibold text-slate-500 uppercase tracking-wider">Start Date</p>
                    <p className="text-sm font-bold text-slate-200 mt-1">
                      {format(new Date(selectedSipForManage.purchaseDate), "dd MMM yyyy")}
                    </p>
                  </div>
                  <div className="p-3 bg-slate-900/30 border border-slate-900 rounded-xl">
                    <p className="text-[10px] font-semibold text-slate-500 uppercase tracking-wider">Deposits Paid</p>
                    <p className="text-sm font-bold text-slate-200 mt-1">{stats.installmentsCompleted} / {stats.installmentDates.length}</p>
                  </div>
                  <div className="p-3 bg-slate-900/30 border border-slate-900 rounded-xl">
                    <p className="text-[10px] font-semibold text-slate-500 uppercase tracking-wider">Skipped Payments</p>
                    <p className="text-sm font-bold text-slate-200 mt-1">{stats.skipCount}</p>
                  </div>
                  <div className="p-3 bg-slate-900/30 border border-slate-900 rounded-xl">
                    <p className="text-[10px] font-semibold text-slate-500 uppercase tracking-wider">Invested Capital</p>
                    <p className="text-sm font-bold text-cyan-400 mt-1">₹{stats.totalInvested.toLocaleString("en-IN")}</p>
                  </div>
                </div>

                {/* Installments Checklist */}
                <div className="space-y-3">
                  <h4 className="text-sm font-bold text-slate-200 flex items-center gap-1.5">
                    <Calendar className="w-4 h-4 text-cyan-400" />
                    <span>Installment Payments History</span>
                  </h4>
                  <p className="text-xs text-slate-400 leading-normal">
                    Toggle checkboxes to mark individual monthly auto-debits as **skipped** if you skipped the bank transfer for that month.
                  </p>

                  <div className="space-y-2 border border-slate-900 rounded-2xl p-4 bg-slate-950/50 max-h-60 overflow-y-auto">
                    {stats.installmentDates.map((instDate, idx) => {
                      const dateStr = instDate.toISOString();
                      
                      // Check if skipped
                      const isSkipped = (selectedSipForManage.sipSkips || []).some((skip: any) => {
                        const d = new Date(skip.skipDate);
                        return d.getFullYear() === instDate.getFullYear() && d.getMonth() === instDate.getMonth();
                      });

                      return (
                        <div 
                          key={idx} 
                          onClick={() => handleToggleSkipDate(selectedSipForManage.id, dateStr)}
                          className="flex items-center justify-between p-3 rounded-xl bg-slate-950 border border-slate-900 hover:border-slate-800/80 cursor-pointer select-none transition-all"
                        >
                          <div className="flex items-center gap-3">
                            <input
                              type="checkbox"
                              checked={isSkipped}
                              onChange={() => {}} // toggled by parent div click
                              className="w-4 h-4 rounded border-slate-800 text-cyan-600 bg-slate-950 focus:ring-cyan-500"
                            />
                            <div>
                              <p className="text-sm font-semibold text-slate-200 font-sans">
                                {format(instDate, "dd MMMM yyyy")}
                              </p>
                              <p className="text-[10px] text-slate-500 mt-0.5">Installment #{idx + 1}</p>
                            </div>
                          </div>
                          <div>
                            {isSkipped ? (
                              <span className="text-[10px] bg-red-500/10 border border-red-500/20 text-red-400 px-2 py-0.5 rounded-full font-semibold">
                                Skipped
                              </span>
                            ) : (
                              <span className="text-[10px] bg-green-500/10 border border-green-500/20 text-green-400 px-2 py-0.5 rounded-full font-semibold">
                                Deposited (₹{selectedSipForManage.sipAmount})
                              </span>
                            )}
                          </div>
                        </div>
                      );
                    })}
                  </div>
                </div>
              </div>

              {/* Footer */}
              <div className="border-t border-slate-900 pt-4 mt-4 flex justify-end">
                <button
                  type="button"
                  onClick={() => setSelectedSipForManage(null)}
                  className="px-4 py-2 bg-slate-900 border border-slate-800 hover:bg-slate-850 text-slate-200 rounded-xl text-xs font-semibold"
                >
                  Done
                </button>
              </div>
            </div>
          </div>
        );
      })()}

    </div>
    </div>
  );
}
