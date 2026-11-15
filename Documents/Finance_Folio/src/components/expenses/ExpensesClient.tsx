"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { expenseSchema, expenseCategorySchema } from "@/lib/validators";
import { createExpense, deleteExpense, createExpenseCategory } from "@/actions/expenses";
import { 
  Plus, 
  Trash2, 
  Calendar, 
  CreditCard, 
  Wallet, 
  Tag, 
  Store,
  AlertCircle,
  X,
  Search,
  Filter
} from "lucide-react";
import { format } from "date-fns";

interface ExpensesClientProps {
  initialCategories: any[];
  initialExpenses: any[];
  accounts: any[];
  cards: any[];
}

const PAYMENT_METHODS = [
  { value: "cash", label: "Cash" },
  { value: "upi", label: "UPI / GPay / PhonePe" },
  { value: "credit_card", label: "Credit Card" },
  { value: "debit_card", label: "Debit Card" },
  { value: "net_banking", label: "Net Banking" },
  { value: "cheque", label: "Cheque" },
  { value: "other", label: "Other" },
];

export default function ExpensesClient({ 
  initialCategories, 
  initialExpenses, 
  accounts, 
  cards 
}: ExpensesClientProps) {
  const [categories, setCategories] = useState(initialCategories);
  const [expenses, setExpenses] = useState(initialExpenses);
  const [showExpenseForm, setShowExpenseForm] = useState(false);
  const [showCategoryForm, setShowCategoryForm] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);
  
  // Search & Filter State
  const [searchTerm, setSearchTerm] = useState("");
  const [selectedCategory, setSelectedCategory] = useState("all");

  // Forms
  const expenseForm = useForm({
    resolver: zodResolver(expenseSchema),
    defaultValues: {
      categoryId: "",
      amount: 0,
      spentOn: new Date().toISOString().split("T")[0],
      paymentMethod: "upi",
      accountId: "",
      cardId: "",
      merchant: "",
      notes: "",
      savedAmount: 0,
    },
  });

  const categoryForm = useForm({
    resolver: zodResolver(expenseCategorySchema),
    defaultValues: {
      name: "",
      icon: "Tag",
    },
  });

  // Watch payment method to dynamically display Account vs Card selection
  const watchedPaymentMethod = expenseForm.watch("paymentMethod");

  // Action: Add Expense
  const onAddExpense = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await createExpense(data);
      if (res.success && res.data) {
        // Hydrate data locally
        const matchedCategory = categories.find((c) => c.id === data.categoryId);
        const matchedAccount = accounts.find((a) => a.id === data.accountId);
        const matchedCard = cards.find((c) => c.id === data.cardId);

        const newExp = {
          ...res.data,
          category: matchedCategory || null,
          account: matchedAccount || null,
          card: matchedCard || null,
        };

        setExpenses((prev) => [newExp, ...prev]);
        expenseForm.reset({
          categoryId: "",
          amount: 0,
          spentOn: new Date().toISOString().split("T")[0],
          paymentMethod: "upi",
          accountId: "",
          cardId: "",
          merchant: "",
          notes: "",
          savedAmount: 0,
        });
        setShowExpenseForm(false);
      } else {
        setError(res.error || "Failed to log expense");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Delete Expense
  const onDeleteExpense = async (id: string) => {
    if (!confirm("Are you sure you want to delete this expense record?")) return;
    try {
      const res = await deleteExpense(id);
      if (res.success) {
        setExpenses((prev) => prev.filter((e) => e.id !== id));
      } else {
        alert(res.error || "Failed to delete expense");
      }
    } catch (err: any) {
      alert(err.message || "An error occurred");
    }
  };

  // Action: Add Category
  const onAddCategory = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await createExpenseCategory(data);
      if (res.success && res.data) {
        setCategories((prev) => [...prev, res.data]);
        categoryForm.reset();
        setShowCategoryForm(false);
      } else {
        setError(res.error || "Failed to create category");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Filtered Expenses list
  const filteredExpenses = expenses.filter((exp) => {
    const matchesSearch = 
      (exp.merchant || "").toLowerCase().includes(searchTerm.toLowerCase()) ||
      (exp.notes || "").toLowerCase().includes(searchTerm.toLowerCase());
    
    const matchesCategory = 
      selectedCategory === "all" || exp.categoryId === selectedCategory;

    return matchesSearch && matchesCategory;
  });

  return (
    <div className="space-y-8">
      {/* Header */}
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4">
        <div>
          <h1 className="text-3xl font-extrabold tracking-tight text-slate-100">Expenses Tracker</h1>
          <p className="text-slate-400 mt-1">Log your daily spends, categorize payments, and audit costs.</p>
        </div>
        <div className="flex items-center gap-3">
          <button
            onClick={() => {
              setShowExpenseForm(true);
              setShowCategoryForm(false);
            }}
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-violet-650 hover:bg-violet-600 font-medium text-sm text-white shadow-lg shadow-violet-600/10 transition-all hover:scale-[1.01]"
          >
            <Plus className="w-4 h-4" />
            <span>Log Expense</span>
          </button>
          <button
            onClick={() => {
              setShowCategoryForm(true);
              setShowExpenseForm(false);
            }}
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-slate-900 border border-slate-800 font-medium text-sm text-slate-200 transition-all hover:scale-[1.01]"
          >
            <Plus className="w-4 h-4" />
            <span>Add Category</span>
          </button>
        </div>
      </div>

      {error && (
        <div className="p-4 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm flex items-start gap-3">
          <AlertCircle className="w-5 h-5 shrink-0 mt-0.5" />
          <p>{error}</p>
        </div>
      )}

      {/* Forms Drawer/Popup */}
      {showCategoryForm && (
        <div className="glass-card rounded-2xl p-6 border border-violet-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <Tag className="w-5 h-5 text-violet-400" />
              <span>Add Custom Category</span>
            </h3>
            <button onClick={() => setShowCategoryForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={categoryForm.handleSubmit(onAddCategory)} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Category Name</label>
              <input
                {...categoryForm.register("name")}
                placeholder="e.g. Subscriptions, Subscriptions"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
              {categoryForm.formState.errors.name && (
                <p className="text-xs text-red-400">{categoryForm.formState.errors.name.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Icon Representative</label>
              <select
                {...categoryForm.register("icon")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="Tag">Standard Tag</option>
                <option value="ShoppingBag">Shopping Bag</option>
                <option value="Utensils">Food / Dining</option>
                <option value="Home">Home / Rent</option>
                <option value="Car">Transport / Car</option>
                <option value="HeartPulse">Medical / Health</option>
                <option value="FileText">Bill / Document</option>
              </select>
            </div>

            <div className="md:col-span-2 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowCategoryForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-violet-600 hover:bg-violet-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Adding..." : "Add Category"}
              </button>
            </div>
          </form>
        </div>
      )}

      {showExpenseForm && (
        <div className="glass-card rounded-2xl p-6 border border-violet-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <CreditCard className="w-5 h-5 text-violet-400" />
              <span>Log Daily Expense</span>
            </h3>
            <button onClick={() => setShowExpenseForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={expenseForm.handleSubmit(onAddExpense)} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Expense Category</label>
              <select
                {...expenseForm.register("categoryId")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Select Category...</option>
                {categories.map((c) => (
                  <option key={c.id} value={c.id}>
                    {c.name}
                  </option>
                ))}
              </select>
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Amount Spent (₹)</label>
              <input
                {...expenseForm.register("amount")}
                type="number"
                step="0.01"
                placeholder="0.00"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
              {expenseForm.formState.errors.amount && (
                <p className="text-xs text-red-400">{expenseForm.formState.errors.amount.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Payment Method</label>
              <select
                {...expenseForm.register("paymentMethod")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                {PAYMENT_METHODS.map((method) => (
                  <option key={method.value} value={method.value}>
                    {method.label}
                  </option>
                ))}
              </select>
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Date of Expense</label>
              <input
                {...expenseForm.register("spentOn")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            {/* Dynamic Account / Credit Card Selector */}
            {watchedPaymentMethod === "credit_card" ? (
              <div className="space-y-1 md:col-span-2">
                <label className="text-xs font-semibold text-slate-300">Credit Card Used</label>
                <select
                  {...expenseForm.register("cardId")}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
                >
                  <option value="">Select Credit Card...</option>
                  {cards
                    .filter((c) => c.cardType === "credit")
                    .map((card) => (
                      <option key={card.id} value={card.id}>
                        {card.cardName} (Outstanding: ₹{card.currentOutstanding})
                      </option>
                    ))}
                </select>
                {expenseForm.formState.errors.cardId && (
                  <p className="text-xs text-red-400">{expenseForm.formState.errors.cardId.message}</p>
                )}
              </div>
            ) : (
              <div className="space-y-1 md:col-span-2">
                <label className="text-xs font-semibold text-slate-300">Settlement Account (Cash/Wallet/Bank)</label>
                <select
                  {...expenseForm.register("accountId")}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
                >
                  <option value="">Select Account...</option>
                  {accounts.map((acc) => (
                    <option key={acc.id} value={acc.id}>
                      {acc.accountName} (Balance: ₹{acc.currentBalance})
                    </option>
                  ))}
                </select>
                {expenseForm.formState.errors.accountId && (
                  <p className="text-xs text-red-400">{expenseForm.formState.errors.accountId.message}</p>
                )}
              </div>
            )}

            {/* Discount / Cashback Field (Visible only for Cards/UPI) */}
            {(watchedPaymentMethod === "credit_card" || watchedPaymentMethod === "debit_card" || watchedPaymentMethod === "upi") && (
              <div className="space-y-1">
                <label className="text-xs font-semibold text-emerald-400 flex justify-between">
                  <span>Discount / Cashback Received (₹)</span>
                  <span className="text-[9px] uppercase tracking-wider opacity-70">Optional</span>
                </label>
                <input
                  {...expenseForm.register("savedAmount")}
                  type="number"
                  step="0.01"
                  placeholder="0.00"
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-emerald-500 focus:outline-none text-emerald-400 font-medium text-sm"
                />
                <p className="text-[10px] text-slate-500 mt-1">Amount saved using this card/payment method.</p>
              </div>
            )}

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Merchant / Vendor</label>
              <input
                {...expenseForm.register("merchant")}
                placeholder="e.g. Amazon, Uber, Starbucks"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Notes / Details</label>
              <input
                {...expenseForm.register("notes")}
                placeholder="Additional details..."
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="md:col-span-2 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowExpenseForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-violet-600 hover:bg-violet-505 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Logging..." : "Log Expense"}
              </button>
            </div>
          </form>
        </div>
      )}

      {/* Filter and Search Bar */}
      <div className="flex flex-col sm:flex-row gap-4 items-center justify-between bg-slate-900/10 p-4 rounded-2xl border border-slate-900/50">
        <div className="relative w-full sm:w-80">
          <span className="absolute inset-y-0 left-0 pl-3 flex items-center text-slate-500">
            <Search className="w-4 h-4" />
          </span>
          <input
            type="text"
            placeholder="Search merchant, notes..."
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
            className="w-full pl-9 pr-4 py-2 bg-slate-950 border border-slate-800 rounded-xl text-sm focus:outline-none focus:border-violet-500 text-slate-200"
          />
        </div>

        <div className="flex items-center gap-2 w-full sm:w-auto justify-end">
          <Filter className="w-4 h-4 text-slate-500" />
          <select
            value={selectedCategory}
            onChange={(e) => setSelectedCategory(e.target.value)}
            className="px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl text-sm focus:outline-none focus:border-violet-500 text-slate-200"
          >
            <option value="all">All Categories</option>
            {categories.map((c) => (
              <option key={c.id} value={c.id}>
                {c.name}
              </option>
            ))}
          </select>
        </div>
      </div>

      {/* Expenses History List */}
      <div className="space-y-4">
        <h2 className="text-xl font-bold text-slate-100">Expenses Log</h2>
        {filteredExpenses.length === 0 ? (
          <div className="glass-card rounded-2xl p-8 text-center text-slate-500 text-sm">
            No expenses found. Adjust filters or log your first expense!
          </div>
        ) : (
          <div className="glass-card rounded-2xl overflow-hidden border border-slate-900">
            <div className="overflow-x-auto">
              <table className="w-full border-collapse text-left">
                <thead>
                  <tr className="border-b border-slate-900 bg-slate-900/20 text-xs font-semibold text-slate-400 uppercase tracking-wider">
                    <th className="p-4">Merchant / Notes</th>
                    <th className="p-4">Category</th>
                    <th className="p-4">Method / Source</th>
                    <th className="p-4">Date</th>
                    <th className="p-4">Amount</th>
                    <th className="p-4 text-right">Action</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-slate-900 text-sm">
                  {filteredExpenses.map((exp) => (
                    <tr key={exp.id} className="hover:bg-slate-900/10 transition-colors">
                      <td className="p-4">
                        <div className="flex items-center gap-2">
                          <span className="font-semibold text-slate-200">
                            {exp.merchant || "Direct Expense"}
                          </span>
                        </div>
                        {exp.notes && <p className="text-xs text-slate-500 mt-0.5">{exp.notes}</p>}
                      </td>
                      <td className="p-4">
                        <span className="inline-flex items-center px-2.5 py-0.5 rounded-full text-xs font-semibold bg-violet-500/10 text-violet-400 border border-violet-500/10">
                          {exp.category?.name || "Uncategorized"}
                        </span>
                      </td>
                      <td className="p-4 text-slate-400 text-xs">
                        <div className="space-y-0.5">
                          <span className="capitalize font-semibold text-slate-300">
                            {PAYMENT_METHOD_LABELS[exp.paymentMethod] || exp.paymentMethod}
                          </span>
                          {exp.paymentMethod === "credit_card" ? (
                            <p className="text-[10px] text-indigo-400 flex items-center gap-1">
                              <CreditCard className="w-3 h-3" />
                              {exp.card?.cardName || "Card"}
                            </p>
                          ) : (
                            <p className="text-[10px] text-cyan-400 flex items-center gap-1">
                              <Wallet className="w-3 h-3" />
                              {exp.account?.accountName || "Account"}
                            </p>
                          )}
                        </div>
                      </td>
                      <td className="p-4 text-slate-400">
                        <span className="inline-flex items-center gap-1 text-xs">
                          <Calendar className="w-3.5 h-3.5" />
                          {format(new Date(exp.spentOn), "dd MMM yyyy")}
                        </span>
                      </td>
                      <td className="p-4">
                        <p className="font-bold text-slate-200">
                          ₹{exp.amount.toLocaleString("en-IN", { minimumFractionDigits: 2 })}
                        </p>
                        {(exp.savedAmount && exp.savedAmount > 0) ? (
                          <p className="text-xs font-semibold text-emerald-400 mt-0.5">
                            + ₹{exp.savedAmount.toLocaleString("en-IN")} Saved
                          </p>
                        ) : null}
                      </td>
                      <td className="p-4 text-right">
                        <button
                          onClick={() => onDeleteExpense(exp.id)}
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
  );
}

const PAYMENT_METHOD_LABELS: Record<string, string> = {
  cash: "Cash",
  upi: "UPI / Wallet",
  credit_card: "Credit Card",
  debit_card: "Debit Card",
  net_banking: "Net Banking",
  cheque: "Cheque",
  other: "Other",
};
