"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { cardSchema, creditCardBillSchema } from "@/lib/validators";
import { createCard, deleteCard, createCardBill, payCardBill } from "@/actions/cards";
import { 
  Plus, 
  Trash2, 
  Calendar, 
  CreditCard as CardIcon, 
  Coins, 
  DollarSign, 
  AlertCircle,
  X,
  CreditCard,
  CheckCircle,
  Clock
} from "lucide-react";
import { format } from "date-fns";
import { getCreditCardBillingMeta } from "@/lib/clientMath";

interface CardsClientProps {
  initialCards: any[];
  initialBills: any[];
  accounts: any[];
}

export default function CardsClient({ initialCards, initialBills, accounts }: CardsClientProps) {
  const [cards, setCards] = useState(initialCards);
  const [bills, setBills] = useState(initialBills);
  const [showCardForm, setShowCardForm] = useState(false);
  const [showBillForm, setShowBillForm] = useState(false);
  const [showPayModal, setShowPayModal] = useState(false);
  const [selectedBill, setSelectedBill] = useState<any>(null);
  
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  // Forms
  const cardForm = useForm({
    resolver: zodResolver(cardSchema),
    defaultValues: {
      accountId: "",
      cardType: "credit",
      cardName: "",
      last4: "",
      creditLimit: 0,
      statementDay: 1,
      dueDay: 20,
      currentOutstanding: 0,
    },
  });

  const billForm = useForm({
    resolver: zodResolver(creditCardBillSchema),
    defaultValues: {
      cardId: "",
      billingStart: new Date(new Date().setDate(1)).toISOString().split("T")[0],
      billingEnd: new Date().toISOString().split("T")[0],
      statementDate: new Date().toISOString().split("T")[0],
      dueDate: new Date(new Date().setDate(new Date().getDate() + 20)).toISOString().split("T")[0],
      totalAmount: 0,
      minimumDue: 0,
      paidAmount: 0,
      status: "unpaid",
    },
  });

  const [payAmount, setPayAmount] = useState<number>(0);
  const [payFromAccount, setPayFromAccount] = useState<string>("");

  // Action: Add Card
  const onAddCard = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await createCard(data);
      if (res.success && res.data) {
        setCards((prev) => [...prev, res.data]);
        cardForm.reset();
        setShowCardForm(false);
      } else {
        setError(res.error || "Failed to add card");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Delete Card
  const onDeleteCard = async (id: string) => {
    if (!confirm("Are you sure you want to delete this card?")) return;
    try {
      const res = await deleteCard(id);
      if (res.success) {
        setCards((prev) => prev.filter((c) => c.id !== id));
      } else {
        alert(res.error || "Failed to delete card");
      }
    } catch (err: any) {
      alert(err.message || "An error occurred");
    }
  };

  // Action: Generate Bill
  const onAddBill = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await createCardBill(data);
      if (res.success && res.data) {
        const matchedCard = cards.find((c) => c.id === data.cardId);
        const newBill = {
          ...res.data,
          card: matchedCard || null,
        };
        setBills((prev) => [...prev, newBill]);
        
        // Adjust local card outstanding if there was a paidAmount
        if (data.paidAmount > 0) {
          setCards((prev) =>
            prev.map((c) =>
              c.id === data.cardId
                ? { ...c, currentOutstanding: c.currentOutstanding - data.paidAmount }
                : c
            )
          );
        }

        billForm.reset();
        setShowBillForm(false);
      } else {
        setError(res.error || "Failed to create bill statement");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Pay Bill Settle
  const handlePayment = async () => {
    if (!selectedBill || payAmount <= 0 || !payFromAccount) {
      alert("Please fill in all payment details.");
      return;
    }
    setLoading(true);
    setError(null);
    try {
      const res = await payCardBill(selectedBill.id, payAmount, payFromAccount);
      if (res.success) {
        // Adjust local states
        setBills((prev) =>
          prev.map((b) => {
            if (b.id === selectedBill.id) {
              const newPaid = b.paidAmount + payAmount;
              let status = "unpaid";
              if (newPaid >= b.totalAmount) status = "paid";
              else if (newPaid > 0) status = "partially_paid";
              return { ...b, paidAmount: newPaid, status };
            }
            return b;
          })
        );

        setCards((prev) =>
          prev.map((c) =>
            c.id === selectedBill.cardId
              ? { ...c, currentOutstanding: c.currentOutstanding - payAmount }
              : c
          )
        );

        setShowPayModal(false);
        setSelectedBill(null);
      } else {
        alert(res.error || "Payment failed");
      }
    } catch (err: any) {
      alert(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="space-y-8">
      {/* Header */}
      <div className="flex flex-col sm:flex-row sm:items-center justify-between gap-4">
        <div>
          <h1 className="text-3xl font-extrabold tracking-tight text-slate-100">Cards & Bills</h1>
          <p className="text-slate-400 mt-1">Manage debit/credit cards, view statements, and settle outstanding balances.</p>
        </div>
        <div className="flex items-center gap-3">
          <button
            onClick={() => {
              setShowBillForm(true);
              setShowCardForm(false);
            }}
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-violet-605 hover:bg-violet-600 font-medium text-sm text-white shadow-lg shadow-violet-600/10 transition-all hover:scale-[1.01]"
          >
            <Plus className="w-4 h-4" />
            <span>Add Bill Statement</span>
          </button>
          <button
            onClick={() => {
              setShowCardForm(true);
              setShowBillForm(false);
            }}
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-slate-900 border border-slate-800 font-medium text-sm text-slate-200 transition-all hover:scale-[1.01]"
          >
            <Plus className="w-4 h-4" />
            <span>Add Card</span>
          </button>
        </div>
      </div>

      {error && (
        <div className="p-4 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm flex items-start gap-3">
          <AlertCircle className="w-5 h-5 shrink-0 mt-0.5" />
          <p>{error}</p>
        </div>
      )}

      {/* Card Form */}
      {showCardForm && (
        <div className="glass-card rounded-2xl p-6 border border-violet-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <CardIcon className="w-5 h-5 text-violet-400" />
              <span>Add Credit/Debit Card</span>
            </h3>
            <button onClick={() => setShowCardForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={cardForm.handleSubmit(onAddCard)} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Card Name</label>
              <input
                {...cardForm.register("cardName")}
                placeholder="e.g. HDFC Regalia, ICICI Amazon Pay"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Card Type</label>
              <select
                {...cardForm.register("cardType")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="credit">Credit Card</option>
                <option value="debit">Debit Card</option>
              </select>
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Last 4 Digits</label>
              <input
                {...cardForm.register("last4")}
                maxLength={4}
                placeholder="e.g. 1234"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Credit Limit (Credit Card only)</label>
              <input
                {...cardForm.register("creditLimit")}
                type="number"
                placeholder="0.00"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Statement Day (1-31)</label>
              <input
                {...cardForm.register("statementDay")}
                type="number"
                min={1}
                max={31}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Due Day (1-31)</label>
              <input
                {...cardForm.register("dueDay")}
                type="number"
                min={1}
                max={31}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Current Outstanding (₹)</label>
              <input
                {...cardForm.register("currentOutstanding")}
                type="number"
                placeholder="0.00"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Settlement/Linked Bank Account</label>
              <select
                {...cardForm.register("accountId")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Select Linked Account...</option>
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
                onClick={() => setShowCardForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-violet-600 hover:bg-violet-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Adding..." : "Add Card"}
              </button>
            </div>
          </form>
        </div>
      )}

      {/* Bill Form */}
      {showBillForm && (
        <div className="glass-card rounded-2xl p-6 border border-violet-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <Coins className="w-5 h-5 text-violet-400" />
              <span>Log Credit Card Statement</span>
            </h3>
            <button onClick={() => setShowBillForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={billForm.handleSubmit(onAddBill)} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-1 md:col-span-2">
              <label className="text-xs font-semibold text-slate-300">Credit Card</label>
              <select
                {...billForm.register("cardId")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Select Card...</option>
                {cards
                  .filter((c) => c.cardType === "credit")
                  .map((c) => (
                    <option key={c.id} value={c.id}>
                      {c.cardName} (Limit: ₹{c.creditLimit})
                    </option>
                  ))}
              </select>
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Billing Start Date</label>
              <input
                {...billForm.register("billingStart")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Billing End Date</label>
              <input
                {...billForm.register("billingEnd")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Statement Date</label>
              <input
                {...billForm.register("statementDate")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Due Date</label>
              <input
                {...billForm.register("dueDate")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Total Amount Due (₹)</label>
              <input
                {...billForm.register("totalAmount")}
                type="number"
                step="0.01"
                placeholder="0.00"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Minimum Amount Due (₹)</label>
              <input
                {...billForm.register("minimumDue")}
                type="number"
                step="0.01"
                placeholder="0.00"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-violet-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="md:col-span-2 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowBillForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-violet-600 hover:bg-violet-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Generating..." : "Log Statement"}
              </button>
            </div>
          </form>
        </div>
      )}

      {/* Payment Settlement Modal */}
      {showPayModal && selectedBill && (
        <div className="fixed inset-0 bg-slate-950/80 backdrop-blur-sm z-50 flex items-center justify-center p-4">
          <div className="w-full max-w-md bg-slate-900 border border-slate-800 rounded-2xl p-6 space-y-4">
            <div className="flex items-center justify-between border-b border-slate-800 pb-3">
              <h3 className="font-bold text-slate-200 flex items-center gap-2">
                <DollarSign className="w-5 h-5 text-green-400" />
                <span>Settle Credit Card Bill</span>
              </h3>
              <button onClick={() => { setShowPayModal(false); setSelectedBill(null); }} className="text-slate-500 hover:text-slate-300">
                <X className="w-5 h-5" />
              </button>
            </div>

            <div className="p-4 rounded-xl bg-slate-950 border border-slate-800 text-sm space-y-2">
              <div className="flex justify-between">
                <span className="text-slate-400">Card Name:</span>
                <span className="font-semibold text-slate-200">{selectedBill.card.cardName}</span>
              </div>
              <div className="flex justify-between">
                <span className="text-slate-400">Total Bill Amount:</span>
                <span className="font-bold text-slate-100">₹{selectedBill.totalAmount}</span>
              </div>
              <div className="flex justify-between">
                <span className="text-slate-400">Already Settled:</span>
                <span className="font-semibold text-slate-400">₹{selectedBill.paidAmount}</span>
              </div>
              <div className="flex justify-between text-green-400 font-semibold border-t border-slate-800 pt-2">
                <span>Remaining Balance:</span>
                <span>₹{(selectedBill.totalAmount - selectedBill.paidAmount).toFixed(2)}</span>
              </div>
            </div>

            <div className="space-y-4">
              <div className="space-y-1">
                <label className="text-xs font-semibold text-slate-300">Settle From Account</label>
                <select
                  value={payFromAccount}
                  onChange={(e) => setPayFromAccount(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
                >
                  <option value="">Choose Settlement Bank/Wallet...</option>
                  {accounts.map((acc) => (
                    <option key={acc.id} value={acc.id}>
                      {acc.accountName} (Balance: ₹{acc.currentBalance})
                    </option>
                  ))}
                </select>
              </div>

              <div className="space-y-1">
                <label className="text-xs font-semibold text-slate-300">Settle Amount (₹)</label>
                <input
                  type="number"
                  step="0.01"
                  value={payAmount}
                  onChange={(e) => setPayAmount(Number(e.target.value))}
                  placeholder="0.00"
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
                />
              </div>
            </div>

            <div className="flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => { setShowPayModal(false); setSelectedBill(null); }}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Close
              </button>
              <button
                type="button"
                onClick={handlePayment}
                disabled={loading}
                className="px-4 py-2 bg-green-600 hover:bg-green-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Settling..." : "Authorize Settlement"}
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Content Grid */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
        {/* Cards list */}
        <div className="space-y-4 lg:col-span-1">
          <h2 className="text-xl font-bold text-slate-100">Registered Cards</h2>
          {cards.length === 0 ? (
            <div className="glass-card rounded-2xl p-6 text-center text-slate-500 text-sm">
              No cards registered yet.
            </div>
          ) : (
            <div className="space-y-4">
              {cards.map((c) => {
                const billingMeta = getCreditCardBillingMeta(c);
                const isCredit = c.cardType === "credit";
                const limitVal = c.creditLimit || 0;
                const outstandingVal = c.currentOutstanding || 0;
                const availableLimit = Math.max(0, limitVal - outstandingVal);

                return (
                  <div key={c.id} className="glass-card rounded-2xl p-5 relative overflow-hidden bg-gradient-to-br from-slate-900 via-slate-950 to-slate-900 border border-slate-900">
                    <div className="absolute top-0 right-0 w-24 h-24 bg-violet-600/5 rounded-full blur-2xl" />
                    <div className="flex items-start justify-between">
                      <div className="space-y-1">
                        <span className={`inline-flex px-2 py-0.5 rounded text-[10px] font-bold uppercase tracking-wider ${
                          isCredit ? "bg-indigo-500/10 text-indigo-400" : "bg-cyan-500/10 text-cyan-400"
                        }`}>
                          {c.cardType}
                        </span>
                        <h4 className="font-bold text-slate-200 text-sm mt-1">{c.cardName}</h4>
                        <p className="text-[10px] text-slate-500 font-mono">•••• •••• •••• {c.last4 || "XXXX"}</p>
                      </div>
                      <button
                        onClick={() => onDeleteCard(c.id)}
                        className="p-1.5 text-slate-500 hover:text-red-400 hover:bg-red-500/5 rounded-lg transition-all"
                      >
                        <Trash2 className="w-4 h-4" />
                      </button>
                    </div>

                    <div className="grid grid-cols-2 gap-4 mt-6 pt-4 border-t border-slate-900/60">
                      <div>
                        <p className="text-[10px] text-slate-500 uppercase font-semibold">Outstanding</p>
                        <p className="font-bold text-slate-200 text-sm mt-0.5">₹{outstandingVal.toLocaleString("en-IN")}</p>
                      </div>
                      {isCredit && (
                        <div>
                          <p className="text-[10px] text-slate-500 uppercase font-semibold">Available</p>
                          <p className="font-bold text-green-400 text-sm mt-0.5">₹{availableLimit.toLocaleString("en-IN")}</p>
                        </div>
                      )}
                    </div>

                    {isCredit && billingMeta && (
                      <div className="mt-4 pt-3 border-t border-slate-900/40 text-[10px] text-slate-400 flex items-center justify-between">
                        <span>Due in {billingMeta.daysRemaining} days</span>
                        <span className="font-semibold text-slate-300">
                          Due: {format(billingMeta.dueDate, "dd MMM")}
                        </span>
                      </div>
                    )}
                  </div>
                );
              })}
            </div>
          )}
        </div>

        {/* Bills list */}
        <div className="lg:col-span-2 space-y-4">
          <h2 className="text-xl font-bold text-slate-100">Credit Card Statements</h2>
          {bills.length === 0 ? (
            <div className="glass-card rounded-2xl p-8 text-center text-slate-500 text-sm">
              No statement entries. Add statements to track upcoming due dates.
            </div>
          ) : (
            <div className="glass-card rounded-2xl overflow-hidden border border-slate-900">
              <div className="overflow-x-auto">
                <table className="w-full border-collapse text-left">
                  <thead>
                    <tr className="border-b border-slate-900 bg-slate-900/20 text-xs font-semibold text-slate-400 uppercase tracking-wider">
                      <th className="p-4">Card / Cycle</th>
                      <th className="p-4">Statement Date</th>
                      <th className="p-4">Due Date</th>
                      <th className="p-4">Billing Status</th>
                      <th className="p-4">Outstanding</th>
                      <th className="p-4 text-right">Action</th>
                    </tr>
                  </thead>
                  <tbody className="divide-y divide-slate-900 text-sm">
                    {bills.map((bill) => {
                      const isPaid = bill.status === "paid";
                      const isPartiallyPaid = bill.status === "partially_paid";
                      const remaining = bill.totalAmount - bill.paidAmount;

                      return (
                        <tr key={bill.id} className="hover:bg-slate-900/10 transition-colors">
                          <td className="p-4">
                            <p className="font-semibold text-slate-200">{bill.card.cardName}</p>
                            <span className="text-[10px] text-slate-500 font-mono">
                              {format(new Date(bill.billingStart), "dd MMM")} - {format(new Date(bill.billingEnd), "dd MMM")}
                            </span>
                          </td>
                          <td className="p-4 text-slate-400 font-mono text-xs">
                            {format(new Date(bill.statementDate), "dd MMM yyyy")}
                          </td>
                          <td className="p-4 text-slate-400 font-mono text-xs">
                            {format(new Date(bill.dueDate), "dd MMM yyyy")}
                          </td>
                          <td className="p-4">
                            {isPaid ? (
                              <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full text-xs font-semibold bg-green-500/10 text-green-400">
                                <CheckCircle className="w-3 h-3" /> Paid
                              </span>
                            ) : isPartiallyPaid ? (
                              <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full text-xs font-semibold bg-amber-500/10 text-amber-400">
                                <Clock className="w-3 h-3" /> Partial
                              </span>
                            ) : (
                              <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full text-xs font-semibold bg-red-500/10 text-red-400">
                                <AlertCircle className="w-3 h-3" /> Unpaid
                              </span>
                            )}
                          </td>
                          <td className="p-4 font-bold text-slate-200 font-mono">
                            <div className="space-y-0.5">
                              <p>₹{remaining.toLocaleString("en-IN")}</p>
                              {bill.paidAmount > 0 && (
                                <p className="text-[10px] text-slate-500">Paid: ₹{bill.paidAmount}</p>
                              )}
                            </div>
                          </td>
                          <td className="p-4 text-right">
                            {!isPaid && (
                              <button
                                onClick={() => {
                                  setSelectedBill(bill);
                                  setPayAmount(remaining);
                                  setPayFromAccount("");
                                  setShowPayModal(true);
                                }}
                                className="px-3 py-1 bg-green-600 hover:bg-green-550 text-white rounded-lg text-xs font-medium"
                              >
                                Settle
                              </button>
                            )}
                          </td>
                        </tr>
                      );
                    })}
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
