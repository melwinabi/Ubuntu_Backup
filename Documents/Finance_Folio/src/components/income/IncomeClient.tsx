"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { incomeSourceSchema, incomeTransactionSchema } from "@/lib/validators";
import { 
  createIncomeSource, 
  deleteIncomeSource, 
  createIncomeTransaction, 
  deleteIncomeTransaction 
} from "@/actions/income";
import { 
  Plus, 
  ArrowUpRight, 
  Trash2, 
  Calendar, 
  DollarSign, 
  Wallet, 
  Briefcase,
  AlertCircle,
  X
} from "lucide-react";
import { format } from "date-fns";

interface IncomeClientProps {
  initialSources: any[];
  initialTransactions: any[];
  accounts: any[];
}

export default function IncomeClient({ initialSources, initialTransactions, accounts }: IncomeClientProps) {
  const [sources, setSources] = useState(initialSources);
  const [transactions, setTransactions] = useState(initialTransactions);
  const [showSourceForm, setShowSourceForm] = useState(false);
  const [showTxForm, setShowTxForm] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  // Forms
  const sourceForm = useForm({
    resolver: zodResolver(incomeSourceSchema),
    defaultValues: {
      sourceName: "",
      sourceType: "salary",
      frequency: "monthly",
      expectedAmount: 0,
      defaultAccountId: "",
    },
  });

  const txForm = useForm({
    resolver: zodResolver(incomeTransactionSchema),
    defaultValues: {
      incomeSourceId: "",
      accountId: "",
      amount: 0,
      receivedOn: new Date().toISOString().split("T")[0],
      notes: "",
    },
  });

  // Action: Create Source
  const onAddSource = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await createIncomeSource(data);
      if (res.success && res.data) {
        setSources((prev) => [...prev, { ...res.data, transactions: [] }]);
        sourceForm.reset();
        setShowSourceForm(false);
      } else {
        setError(res.error || "Failed to create source");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Delete Source
  const onDeleteSource = async (id: string) => {
    if (!confirm("Are you sure you want to archive this income source?")) return;
    try {
      const res = await deleteIncomeSource(id);
      if (res.success) {
        setSources((prev) => prev.filter((s) => s.id !== id));
      } else {
        alert(res.error || "Failed to delete source");
      }
    } catch (err: any) {
      alert(err.message || "An error occurred");
    }
  };

  // Action: Log Deposit
  const onAddTx = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await createIncomeTransaction(data);
      if (res.success && res.data) {
        // Find source & account for instant state injection
        const matchedSource = sources.find((s) => s.id === data.incomeSourceId);
        const matchedAccount = accounts.find((a) => a.id === data.accountId);
        
        const newTx = {
          ...res.data,
          incomeSource: matchedSource ? { sourceName: matchedSource.sourceName } : null,
          account: matchedAccount ? { accountName: matchedAccount.accountName } : null,
        };

        setTransactions((prev) => [newTx, ...prev]);
        txForm.reset();
        setShowTxForm(false);
      } else {
        setError(res.error || "Failed to log deposit");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Delete Transaction
  const onDeleteTx = async (id: string) => {
    if (!confirm("Are you sure you want to delete this deposit entry?")) return;
    try {
      const res = await deleteIncomeTransaction(id);
      if (res.success) {
        setTransactions((prev) => prev.filter((t) => t.id !== id));
      } else {
        alert(res.error || "Failed to delete deposit");
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
          <h1 className="text-3xl font-extrabold tracking-tight text-slate-100">Income Management</h1>
          <p className="text-slate-400 mt-1">Configure your income streams and log deposits.</p>
        </div>
        <div className="flex items-center gap-3">
          <button
            onClick={() => {
              setShowTxForm(true);
              setShowSourceForm(false);
            }}
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-green-600 hover:bg-green-500 font-medium text-sm text-white shadow-lg shadow-green-600/10 transition-all hover:scale-[1.01]"
          >
            <Plus className="w-4 h-4" />
            <span>Log Deposit</span>
          </button>
          <button
            onClick={() => {
              setShowSourceForm(true);
              setShowTxForm(false);
            }}
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-slate-900 border border-slate-800 font-medium text-sm text-slate-200 transition-all hover:scale-[1.01]"
          >
            <Plus className="w-4 h-4" />
            <span>Add Source</span>
          </button>
        </div>
      </div>

      {error && (
        <div className="p-4 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm flex items-start gap-3">
          <AlertCircle className="w-5 h-5 shrink-0 mt-0.5" />
          <p>{error}</p>
        </div>
      )}

      {/* Pop-up Modals/Forms */}
      {showSourceForm && (
        <div className="glass-card rounded-2xl p-6 border border-violet-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <Briefcase className="w-5 h-5 text-violet-400" />
              <span>Add Income Source</span>
            </h3>
            <button onClick={() => setShowSourceForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={sourceForm.handleSubmit(onAddSource)} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Source Name</label>
              <input
                {...sourceForm.register("sourceName")}
                placeholder="e.g. Primary Salary, Consulting"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
              {sourceForm.formState.errors.sourceName && (
                <p className="text-xs text-red-400">{sourceForm.formState.errors.sourceName.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Type</label>
              <select
                {...sourceForm.register("sourceType")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="salary">Salary</option>
                <option value="freelance">Freelance / Consulting</option>
                <option value="business">Business Revenue</option>
                <option value="rental">Rental Income</option>
                <option value="investment">Investment Yield</option>
                <option value="other">Other</option>
              </select>
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Expected Monthly Amount</label>
              <input
                {...sourceForm.register("expectedAmount")}
                type="number"
                placeholder="0.00"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Frequency</label>
              <select
                {...sourceForm.register("frequency")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="monthly">Monthly</option>
                <option value="weekly">Weekly</option>
                <option value="one_time">One-time</option>
                <option value="yearly">Yearly</option>
                <option value="irregular">Irregular</option>
              </select>
            </div>

            <div className="space-y-1 md:col-span-2">
              <label className="text-xs font-semibold text-slate-300">Default Account Credited</label>
              <select
                {...sourceForm.register("defaultAccountId")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Select Account...</option>
                {accounts.map((acc) => (
                  <option key={acc.id} value={acc.id}>
                    {acc.accountName} (₹{acc.currentBalance})
                  </option>
                ))}
              </select>
            </div>

            <div className="md:col-span-2 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowSourceForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-violet-600 hover:bg-violet-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Adding..." : "Add Source"}
              </button>
            </div>
          </form>
        </div>
      )}

      {showTxForm && (
        <div className="glass-card rounded-2xl p-6 border border-green-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <DollarSign className="w-5 h-5 text-green-400" />
              <span>Log Income Deposit</span>
            </h3>
            <button onClick={() => setShowTxForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={txForm.handleSubmit(onAddTx)} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Income Source</label>
              <select
                {...txForm.register("incomeSourceId")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Select Source...</option>
                {sources.map((s) => (
                  <option key={s.id} value={s.id}>
                    {s.sourceName}
                  </option>
                ))}
              </select>
              {txForm.formState.errors.incomeSourceId && (
                <p className="text-xs text-red-400">{txForm.formState.errors.incomeSourceId.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Amount Received (₹)</label>
              <input
                {...txForm.register("amount")}
                type="number"
                step="0.01"
                placeholder="0.00"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
              />
              {txForm.formState.errors.amount && (
                <p className="text-xs text-red-400">{txForm.formState.errors.amount.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Deposit Account</label>
              <select
                {...txForm.register("accountId")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Select Credited Account...</option>
                {accounts.map((acc) => (
                  <option key={acc.id} value={acc.id}>
                    {acc.accountName} (Balance: ₹{acc.currentBalance})
                  </option>
                ))}
              </select>
              {txForm.formState.errors.accountId && (
                <p className="text-xs text-red-400">{txForm.formState.errors.accountId.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Date Received</label>
              <input
                {...txForm.register("receivedOn")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1 md:col-span-2">
              <label className="text-xs font-semibold text-slate-300">Notes / Details</label>
              <input
                {...txForm.register("notes")}
                placeholder="e.g. Salary credited for May"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="md:col-span-2 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowTxForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-green-600 hover:bg-green-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Logging..." : "Log Deposit"}
              </button>
            </div>
          </form>
        </div>
      )}

      {/* Grid of sources and transactions */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Sources side */}
        <div className="space-y-4">
          <h2 className="text-xl font-bold text-slate-100">Income Streams</h2>
          {sources.length === 0 ? (
            <div className="glass-card rounded-2xl p-6 text-center text-slate-500 text-sm">
              No income streams configured. Add your first salary or freelance stream!
            </div>
          ) : (
            <div className="space-y-3">
              {sources.map((s) => (
                <div key={s.id} className="glass-card rounded-2xl p-4 flex items-center justify-between">
                  <div>
                    <h4 className="font-semibold text-slate-200 text-sm">{s.sourceName}</h4>
                    <div className="flex items-center gap-2 text-xs text-slate-500 mt-1 capitalize">
                      <span>{s.sourceType}</span>
                      <span>•</span>
                      <span>{s.frequency}</span>
                    </div>
                  </div>
                  <div className="flex items-center gap-4">
                    <div className="text-right">
                      <p className="font-bold text-slate-100 text-sm">
                        ₹{s.expectedAmount?.toLocaleString("en-IN") || "0"}
                      </p>
                      <span className="text-[10px] text-slate-500">Expected</span>
                    </div>
                    <button
                      onClick={() => onDeleteSource(s.id)}
                      className="p-2 text-slate-500 hover:text-red-400 hover:bg-red-500/5 rounded-lg border border-transparent hover:border-red-500/10 transition-all"
                    >
                      <Trash2 className="w-4 h-4" />
                    </button>
                  </div>
                </div>
              ))}
            </div>
          )}
        </div>

        {/* Transactions log table - spans 2 cols */}
        <div className="lg:col-span-2 space-y-4">
          <h2 className="text-xl font-bold text-slate-100">Deposit Logs</h2>
          {transactions.length === 0 ? (
            <div className="glass-card rounded-2xl p-8 text-center text-slate-500 text-sm">
              No deposits recorded yet. Click &quot;Log Deposit&quot; to credit money to your bank accounts!
            </div>
          ) : (
            <div className="glass-card rounded-2xl overflow-hidden border border-slate-900">
              <div className="overflow-x-auto">
                <table className="w-full border-collapse text-left">
                  <thead>
                    <tr className="border-b border-slate-900 bg-slate-900/20 text-xs font-semibold text-slate-400 uppercase tracking-wider">
                      <th className="p-4">Source</th>
                      <th className="p-4">Account</th>
                      <th className="p-4">Date</th>
                      <th className="p-4">Amount</th>
                      <th className="p-4 text-right">Action</th>
                    </tr>
                  </thead>
                  <tbody className="divide-y divide-slate-900 text-sm">
                    {transactions.map((tx) => (
                      <tr key={tx.id} className="hover:bg-slate-900/10 transition-colors">
                        <td className="p-4">
                          <p className="font-medium text-slate-200">{tx.incomeSource?.sourceName || "Unknown"}</p>
                          {tx.notes && <span className="text-xs text-slate-500">{tx.notes}</span>}
                        </td>
                        <td className="p-4 text-slate-400">
                          <span className="inline-flex items-center gap-1">
                            <Wallet className="w-3.5 h-3.5" />
                            {tx.account?.accountName || "Cash/Other"}
                          </span>
                        </td>
                        <td className="p-4 text-slate-400">
                          <span className="inline-flex items-center gap-1">
                            <Calendar className="w-3.5 h-3.5" />
                            {format(new Date(tx.receivedOn), "dd MMM yyyy")}
                          </span>
                        </td>
                        <td className="p-4 font-bold text-green-400">
                          ₹{tx.amount.toLocaleString("en-IN", { minimumFractionDigits: 2 })}
                        </td>
                        <td className="p-4 text-right">
                          <button
                            onClick={() => onDeleteTx(tx.id)}
                            className="p-1.5 text-slate-500 hover:text-red-400 hover:bg-red-500/5 rounded-lg border border-transparent hover:border-red-500/10 transition-all"
                          >
                            <Trash2 className="w-4 h-4" />
                          </button>
                        </td>
                      </tr>
                    ))}
                  </tbody>
                </table>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
