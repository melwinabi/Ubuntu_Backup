"use client";

import { useState } from "react";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { loanGivenSchema, loanRepaymentSchema } from "@/lib/validators";
import { createLoanGiven, deleteLoan, createLoanRepayment } from "@/actions/loans";
import { 
  Plus, 
  Trash2, 
  Calendar, 
  Coins, 
  User, 
  DollarSign, 
  AlertCircle,
  X,
  PlusCircle,
  TrendingUp,
  CheckCircle2,
  Clock,
  HelpCircle
} from "lucide-react";
import { format } from "date-fns";

interface LoansClientProps {
  initialLoans: any[];
  accounts: any[];
}

export default function LoansClient({ initialLoans, accounts }: LoansClientProps) {
  const [loans, setLoans] = useState(initialLoans);
  const [showLoanForm, setShowLoanForm] = useState(false);
  const [showRepayModal, setShowRepayModal] = useState(false);
  const [selectedLoan, setSelectedLoan] = useState<any>(null);
  const [error, setError] = useState<string | null>(null);
  const [loading, setLoading] = useState(false);

  // Forms
  const loanForm = useForm({
    resolver: zodResolver(loanGivenSchema),
    defaultValues: {
      borrowerName: "",
      principalAmount: 0,
      interestRate: 0,
      amountGivenOn: new Date().toISOString().split("T")[0],
      expectedReturnDate: "",
      accountId: "",
      paymentMode: "upi" as any,
      notes: "",
    },
  });

  const [repayAmount, setRepayAmount] = useState<number>(0);
  const [repayToAccount, setRepayToAccount] = useState<string>("");
  const [repayNotes, setRepayNotes] = useState<string>("");

  // Calculations
  const processedLoans = loans.map((loan) => {
    const repaid = loan.repayments.reduce((sum: number, r: any) => sum + r.amount, 0);
    const outstanding = Math.max(0, loan.principalAmount - repaid);
    const isOverdue = loan.expectedReturnDate && new Date(loan.expectedReturnDate) < new Date() && outstanding > 0;

    return {
      ...loan,
      repaid,
      outstanding,
      isOverdue,
    };
  });

  const totalLent = processedLoans.reduce((sum, loan) => sum + loan.principalAmount, 0);
  const totalRecovered = processedLoans.reduce((sum, loan) => sum + loan.repaid, 0);
  const totalOutstanding = Math.max(0, totalLent - totalRecovered);

  // Action: Add Loan
  const onAddLoan = async (data: any) => {
    setLoading(true);
    setError(null);
    try {
      const res = await createLoanGiven(data);
      if (res.success && res.data) {
        setLoans((prev) => [{ ...res.data, repayments: [] }, ...prev]);
        loanForm.reset();
        setShowLoanForm(false);
      } else {
        setError(res.error || "Failed to create loan entry");
      }
    } catch (err: any) {
      setError(err.message || "An error occurred");
    } finally {
      setLoading(false);
    }
  };

  // Action: Delete Loan
  const onDeleteLoan = async (id: string) => {
    if (!confirm("Are you sure you want to delete this loan record?")) return;
    try {
      const res = await deleteLoan(id);
      if (res.success) {
        setLoans((prev) => prev.filter((l) => l.id !== id));
      } else {
        alert(res.error || "Failed to delete loan");
      }
    } catch (err: any) {
      alert(err.message || "An error occurred");
    }
  };

  // Action: Add Repayment
  const handleRepayment = async () => {
    if (!selectedLoan || repayAmount <= 0 || !repayToAccount) {
      alert("Please enter repayment amount and credit account.");
      return;
    }
    setLoading(true);
    setError(null);
    try {
      const res = await createLoanRepayment({
        loanId: selectedLoan.id,
        amount: repayAmount,
        repaidOn: new Date().toISOString().split("T")[0],
        accountId: repayToAccount,
        notes: repayNotes,
      });

      if (res.success && res.data) {
        // Adjust local states
        setLoans((prev) =>
          prev.map((l) => {
            if (l.id === selectedLoan.id) {
              const updatedRepayments = [res.data, ...l.repayments];
              const totalRepaid = updatedRepayments.reduce((sum, r) => sum + r.amount, 0);
              let status = "active";
              if (totalRepaid >= l.principalAmount) status = "closed";
              else if (totalRepaid > 0) status = "partially_paid";
              return { ...l, repayments: updatedRepayments, status };
            }
            return l;
          })
        );
        setShowRepayModal(false);
        setSelectedLoan(null);
        setRepayAmount(0);
        setRepayToAccount("");
        setRepayNotes("");
      } else {
        alert(res.error || "Repayment failed");
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
          <h1 className="text-3xl font-extrabold tracking-tight text-slate-100">Loans Receivable</h1>
          <p className="text-slate-400 mt-1">Audit principal lent out, calculate interest, and record repayments.</p>
        </div>
        <button
          onClick={() => setShowLoanForm(true)}
          className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-cyan-600 hover:bg-cyan-500 font-medium text-sm text-white shadow-lg shadow-cyan-600/10 transition-all hover:scale-[1.01]"
        >
          <Plus className="w-4 h-4" />
          <span>Lend Money</span>
        </button>
      </div>

      {error && (
        <div className="p-4 rounded-lg bg-red-500/10 border border-red-500/20 text-red-400 text-sm flex items-start gap-3">
          <AlertCircle className="w-5 h-5 shrink-0 mt-0.5" />
          <p>{error}</p>
        </div>
      )}

      {/* Lend Money Form */}
      {showLoanForm && (
        <div className="glass-card rounded-2xl p-6 border border-cyan-500/20">
          <div className="flex items-center justify-between mb-4">
            <h3 className="font-bold text-slate-200 flex items-center gap-2">
              <Coins className="w-5 h-5 text-cyan-400" />
              <span>Record Money Lent</span>
            </h3>
            <button onClick={() => setShowLoanForm(false)} className="text-slate-500 hover:text-slate-300">
              <X className="w-5 h-5" />
            </button>
          </div>
          <form onSubmit={loanForm.handleSubmit(onAddLoan)} className="grid grid-cols-1 md:grid-cols-2 gap-4">
            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Borrower Name</label>
              <div className="relative">
                <span className="absolute inset-y-0 left-0 pl-3 flex items-center text-slate-500">
                  <User className="w-4 h-4" />
                </span>
                <input
                  {...loanForm.register("borrowerName")}
                  placeholder="e.g. John Doe, Sister"
                  className="w-full pl-9 pr-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
                />
              </div>
              {loanForm.formState.errors.borrowerName && (
                <p className="text-xs text-red-400">{loanForm.formState.errors.borrowerName.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Principal Amount (₹)</label>
              <input
                {...loanForm.register("principalAmount")}
                type="number"
                step="0.01"
                placeholder="0.00"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
              {loanForm.formState.errors.principalAmount && (
                <p className="text-xs text-red-400">{loanForm.formState.errors.principalAmount.message}</p>
              )}
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Interest Rate (% p.a.)</label>
              <input
                {...loanForm.register("interestRate")}
                type="number"
                step="0.1"
                placeholder="0"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Date Lent</label>
              <input
                {...loanForm.register("amountGivenOn")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Payment Mode</label>
              <select
                {...loanForm.register("paymentMode")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="upi">UPI</option>
                <option value="cash">Cash</option>
                <option value="debit_card">Debit Card</option>
                <option value="credit_card">Credit Card</option>
                <option value="net_banking">Net Banking</option>
                <option value="cheque">Cheque</option>
                <option value="other">Other</option>
              </select>
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Expected Return Date (Optional)</label>
              <input
                {...loanForm.register("expectedReturnDate")}
                type="date"
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="space-y-1">
              <label className="text-xs font-semibold text-slate-300">Deduct Principal From Account</label>
              <select
                {...loanForm.register("accountId")}
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              >
                <option value="">Select account...</option>
                {accounts.map((acc) => (
                  <option key={acc.id} value={acc.id}>
                    {acc.accountName} (Balance: ₹{acc.currentBalance})
                  </option>
                ))}
              </select>
              {loanForm.formState.errors.accountId && (
                <p className="text-xs text-red-400">{loanForm.formState.errors.accountId.message}</p>
              )}
            </div>

            <div className="space-y-1 md:col-span-2">
              <label className="text-xs font-semibold text-slate-300">Notes / Agreement Details</label>
              <input
                {...loanForm.register("notes")}
                placeholder="Details of agreement or purpose..."
                className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-cyan-500 focus:outline-none text-slate-100 text-sm"
              />
            </div>

            <div className="md:col-span-2 flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => setShowLoanForm(false)}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Cancel
              </button>
              <button
                type="submit"
                disabled={loading}
                className="px-4 py-2 bg-cyan-600 hover:bg-cyan-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Recording..." : "Lend Money"}
              </button>
            </div>
          </form>
        </div>
      )}

      {/* Log Repayment Modal */}
      {showRepayModal && selectedLoan && (
        <div className="fixed inset-0 bg-slate-950/80 backdrop-blur-sm z-50 flex items-center justify-center p-4">
          <div className="w-full max-w-md bg-slate-900 border border-slate-800 rounded-2xl p-6 space-y-4">
            <div className="flex items-center justify-between border-b border-slate-800 pb-3">
              <h3 className="font-bold text-slate-200 flex items-center gap-2">
                <DollarSign className="w-5 h-5 text-green-400" />
                <span>Log Repayment Credit</span>
              </h3>
              <button onClick={() => { setShowRepayModal(false); setSelectedLoan(null); }} className="text-slate-500 hover:text-slate-300">
                <X className="w-5 h-5" />
              </button>
            </div>

            <div className="p-4 rounded-xl bg-slate-950 border border-slate-800 text-sm space-y-2">
              <div className="flex justify-between">
                <span className="text-slate-400">Borrower:</span>
                <span className="font-semibold text-slate-200">{selectedLoan.borrowerName}</span>
              </div>
              <div className="flex justify-between">
                <span className="text-slate-400">Principal Lent:</span>
                <span className="font-semibold text-slate-300">₹{selectedLoan.principalAmount}</span>
              </div>
              <div className="flex justify-between text-cyan-400 font-semibold border-t border-slate-800 pt-2">
                <span>Remaining Balance:</span>
                <span>₹{selectedLoan.outstanding.toFixed(2)}</span>
              </div>
            </div>

            <div className="space-y-4">
              <div className="space-y-1">
                <label className="text-xs font-semibold text-slate-300 font-sans">Credit Repayment To Account</label>
                <select
                  value={repayToAccount}
                  onChange={(e) => setRepayToAccount(e.target.value)}
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
                >
                  <option value="">Select Account...</option>
                  {accounts.map((acc) => (
                    <option key={acc.id} value={acc.id}>
                      {acc.accountName} (Balance: ₹{acc.currentBalance})
                    </option>
                  ))}
                </select>
              </div>

              <div className="space-y-1">
                <label className="text-xs font-semibold text-slate-300">Amount Collected (₹)</label>
                <input
                  type="number"
                  step="0.01"
                  value={repayAmount}
                  onChange={(e) => setRepayAmount(Number(e.target.value))}
                  placeholder="0.00"
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
                />
              </div>

              <div className="space-y-1">
                <label className="text-xs font-semibold text-slate-300">Notes</label>
                <input
                  type="text"
                  value={repayNotes}
                  onChange={(e) => setRepayNotes(e.target.value)}
                  placeholder="e.g. Cash repaid, Part installment"
                  className="w-full px-3 py-2 bg-slate-950 border border-slate-800 rounded-xl focus:border-green-500 focus:outline-none text-slate-100 text-sm"
                />
              </div>
            </div>

            <div className="flex justify-end gap-2 pt-2">
              <button
                type="button"
                onClick={() => { setShowRepayModal(false); setSelectedLoan(null); }}
                className="px-4 py-2 bg-slate-900 border border-slate-800 rounded-xl text-xs font-medium text-slate-300"
              >
                Close
              </button>
              <button
                type="button"
                onClick={handleRepayment}
                disabled={loading}
                className="px-4 py-2 bg-green-600 hover:bg-green-500 text-white rounded-xl text-xs font-medium"
              >
                {loading ? "Settling..." : "Log Repayment"}
              </button>
            </div>
          </div>
        </div>
      )}

      {/* KPI stats display */}
      <div className="grid grid-cols-1 sm:grid-cols-3 gap-6">
        <div className="glass-card rounded-2xl p-5">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Total Principal Out</p>
          <p className="text-2xl font-bold text-slate-200 mt-2">
            ₹{totalLent.toLocaleString("en-IN")}
          </p>
        </div>
        <div className="glass-card rounded-2xl p-5">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Total Recovered</p>
          <p className="text-2xl font-bold text-green-400 mt-2">
            ₹{totalRecovered.toLocaleString("en-IN")}
          </p>
        </div>
        <div className="glass-card rounded-2xl p-5">
          <p className="text-xs font-semibold text-slate-400 uppercase tracking-wider">Outstanding Receivables</p>
          <p className="text-2xl font-bold text-cyan-400 mt-2">
            ₹{totalOutstanding.toLocaleString("en-IN")}
          </p>
        </div>
      </div>

      {/* Loans Ledger Table */}
      <div className="space-y-4">
        <h2 className="text-xl font-bold text-slate-100 font-sans">Active & Historical Loans</h2>
        {processedLoans.length === 0 ? (
          <div className="glass-card rounded-2xl p-8 text-center text-slate-500 text-sm">
            No active money lend logs. Click &quot;Lend Money&quot; to register your first loan.
          </div>
        ) : (
          <div className="glass-card rounded-2xl overflow-hidden border border-slate-900">
            <div className="overflow-x-auto">
              <table className="w-full border-collapse text-left">
                <thead>
                  <tr className="border-b border-slate-900 bg-slate-900/20 text-xs font-semibold text-slate-400 uppercase tracking-wider">
                    <th className="p-4">Borrower Name</th>
                    <th className="p-4">Lent Date</th>
                    <th className="p-4">Expected Return</th>
                    <th className="p-4">Status</th>
                    <th className="p-4">Lent</th>
                    <th className="p-4">Outstanding</th>
                    <th className="p-4 text-right">Actions</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-slate-900 text-sm">
                  {processedLoans.map((loan) => {
                    const isClosed = loan.status === "closed";
                    const isOverdue = loan.isOverdue;

                    return (
                      <tr key={loan.id} className="hover:bg-slate-900/10 transition-colors">
                        <td className="p-4">
                          <p className="font-semibold text-slate-200">{loan.borrowerName}</p>
                          {loan.notes && <p className="text-xs text-slate-500 mt-0.5">{loan.notes}</p>}
                        </td>
                        <td className="p-4 text-slate-400 text-xs font-mono">
                          {format(new Date(loan.amountGivenOn), "dd MMM yyyy")}
                        </td>
                        <td className="p-4 font-mono text-xs">
                          {loan.expectedReturnDate ? (
                            <span className={isOverdue ? "text-red-400 font-semibold" : "text-slate-400"}>
                              {format(new Date(loan.expectedReturnDate), "dd MMM yyyy")}
                            </span>
                          ) : (
                            <span className="text-slate-500">Not Specified</span>
                          )}
                        </td>
                        <td className="p-4">
                          {isClosed ? (
                            <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full text-xs font-semibold bg-green-500/10 text-green-400">
                              <CheckCircle2 className="w-3 h-3" /> Closed
                            </span>
                          ) : loan.repaid > 0 ? (
                            <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full text-xs font-semibold bg-cyan-500/10 text-cyan-400">
                              <Clock className="w-3 h-3" /> Partial
                            </span>
                          ) : (
                            <span className="inline-flex items-center gap-1 px-2.5 py-0.5 rounded-full text-xs font-semibold bg-slate-900 text-slate-400 border border-slate-800">
                              <Clock className="w-3 h-3" /> Active
                            </span>
                          )}
                        </td>
                        <td className="p-4 text-slate-300 font-mono">
                          ₹{loan.principalAmount.toLocaleString("en-IN")}
                          {loan.interestRate > 0 && (
                            <p className="text-[10px] text-slate-500">Interest: {loan.interestRate}%</p>
                          )}
                        </td>
                        <td className="p-4 font-bold text-slate-200 font-mono">
                          <div className="space-y-0.5">
                            <p className={loan.outstanding > 0 ? "text-cyan-400" : "text-green-400"}>
                              ₹{loan.outstanding.toLocaleString("en-IN")}
                            </p>
                            {loan.repaid > 0 && (
                              <p className="text-[10px] text-slate-500">Repaid: ₹{loan.repaid}</p>
                            )}
                          </div>
                        </td>
                        <td className="p-4 text-right">
                          <div className="flex items-center justify-end gap-1.5">
                            {!isClosed && (
                              <button
                                onClick={() => {
                                  setSelectedLoan(loan);
                                  setRepayAmount(loan.outstanding);
                                  setRepayToAccount("");
                                  setRepayNotes("");
                                  setShowRepayModal(true);
                                }}
                                className="px-2.5 py-1 bg-green-600 hover:bg-green-500 text-white rounded-lg text-xs font-medium"
                              >
                                Settle
                              </button>
                            )}
                            <button
                              onClick={() => onDeleteLoan(loan.id)}
                              className="p-1.5 text-slate-500 hover:text-red-400 hover:bg-red-500/5 rounded-lg border border-transparent hover:border-red-500/10 transition-all"
                            >
                              <Trash2 className="w-4 h-4" />
                            </button>
                          </div>
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
  );
}
