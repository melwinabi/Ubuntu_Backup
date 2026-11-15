import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { db } from "@/lib/db";
import Link from "next/link";
import { 
  TrendingUp, 
  TrendingDown, 
  PiggyBank, 
  Gauge, 
  ArrowUpRight, 
  ArrowDownRight, 
  CreditCard, 
  Coins, 
  Calendar,
  AlertCircle,
  ChevronRight,
  PlusCircle
} from "lucide-react";
import { format } from "date-fns";
import ExpenseChartSection from "@/components/dashboard/ExpenseChartSection";
import InvestmentSummaryCard from "@/components/dashboard/InvestmentSummaryCard";
import CreditScoreGaugeCard from "@/components/dashboard/CreditScoreGaugeCard";
import LiveMarketRatesCard from "@/components/dashboard/LiveMarketRatesCard";
import TargetNetWorthCalculator from "@/components/dashboard/TargetNetWorthCalculator";
import CardSavingsGraph from "@/components/dashboard/CardSavingsGraph";
import { getLivePreciousMetalsPrices, convertPreciousMetalToGrams } from "@/lib/calculations";

export const revalidate = 0; // Disable static rendering for dashboard to ensure live data

export default async function DashboardPage() {
  const session = await getServerSession(authOptions);
  const userId = session!.user!.id;

  // 1. Fetch current month's dates
  const startOfMonth = new Date();
  startOfMonth.setDate(1);
  startOfMonth.setHours(0, 0, 0, 0);

  // 2. Fetch all raw data in parallel
  const [
    accounts,
    investments,
    loans,
    cards,
    unpaidBills,
    expensesThisMonth,
    incomeThisMonth,
    latestCreditScore,
    snapshots,
    savingsData
  ] = await Promise.all([
    db.account.findMany({ where: { userId, isActive: true } }),
    db.investment.findMany({
      where: { userId, isActive: true },
      include: { prices: { orderBy: { priceDate: "desc" }, take: 1 } },
    }),
    db.loanGiven.findMany({
      where: { userId },
      include: { repayments: { select: { amount: true } } },
    }),
    db.card.findMany({ where: { userId, isActive: true } }),
    db.creditCardBill.findMany({
      where: { card: { userId, isActive: true }, status: { in: ["unpaid", "partially_paid"] } },
      include: { card: true },
      orderBy: { dueDate: "asc" },
    }),
    db.expense.findMany({
      where: { userId, spentOn: { gte: startOfMonth } },
      include: { category: true },
    }),
    db.incomeTransaction.findMany({
      where: { incomeSource: { userId }, receivedOn: { gte: startOfMonth } },
      include: { incomeSource: true },
    }),
    db.creditScore.findFirst({
      where: { userId },
      orderBy: { recordedOn: "desc" },
    }),
    db.netWorthSnapshot.findMany({
      where: { userId },
      orderBy: { snapshotDate: "desc" },
      take: 2,
    }),
    db.expense.findMany({
      where: { userId, savedAmount: { gt: 0 } },
      select: { amount: true, savedAmount: true, spentOn: true, paymentMethod: true, card: { select: { cardName: true } } },
      orderBy: { spentOn: "desc" }
    })
  ]);

  // 3. Compute Net Worth
  const metalsPrices = await getLivePreciousMetalsPrices();
  const totalCash = accounts.reduce((sum, acc) => sum + acc.currentBalance, 0);
  
  const totalInvestments = investments.reduce((sum, inv) => {
    if (inv.assetType === "gold") {
      const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit || "g");
      return sum + grams * metalsPrices.gold;
    }
    if (inv.assetType === "silver") {
      const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit || "g");
      return sum + grams * metalsPrices.silver;
    }

    const currentPrice = inv.prices[0]?.pricePerUnit ?? inv.purchasePrice;
    return sum + inv.quantity * currentPrice;
  }, 0);

  const totalLoansReceivable = loans.reduce((sum, loan) => {
    if (loan.status === "closed") return sum;
    const repaid = loan.repayments.reduce((rSum, r) => rSum + r.amount, 0);
    return sum + Math.max(0, loan.principalAmount - repaid);
  }, 0);

  const totalLiabilities = cards.reduce((sum, card) => sum + card.currentOutstanding, 0);

  const netWorth = totalCash + totalInvestments + totalLoansReceivable - totalLiabilities;

  // Net worth change comparison
  let netWorthChange = 0;
  let netWorthChangePercent = 0;
  if (snapshots.length >= 2) {
    const prevNetWorth = snapshots[1].netWorth;
    netWorthChange = netWorth - prevNetWorth;
    netWorthChangePercent = prevNetWorth > 0 ? (netWorthChange / prevNetWorth) * 100 : 0;
  } else if (snapshots.length === 1) {
    const prevNetWorth = snapshots[0].netWorth;
    netWorthChange = netWorth - prevNetWorth;
    netWorthChangePercent = prevNetWorth > 0 ? (netWorthChange / prevNetWorth) * 100 : 0;
  }

  // 4. Compute Income & Expenses This Month
  const totalIncomeThisMonth = incomeThisMonth.reduce((sum, tx) => sum + tx.amount, 0);
  const totalExpensesThisMonth = expensesThisMonth.reduce((sum, tx) => sum + tx.amount, 0);
  const remainingBalance = totalIncomeThisMonth - totalExpensesThisMonth;
  const activeIncomeSources = new Set(incomeThisMonth.map(tx => tx.incomeSourceId)).size;

  // 5. Prepare Investments details
  const totalInvestedValue = investments.reduce((sum, inv) => sum + inv.quantity * inv.purchasePrice, 0);
  const investmentGainLoss = totalInvestments - totalInvestedValue;
  const investmentGainLossPercent = totalInvestedValue > 0 ? (investmentGainLoss / totalInvestedValue) * 100 : 0;

  // 6. Calculate total active monthly SIPs for target forecaster
  const monthlySipTotal = investments
    .filter(inv => inv.isActive && inv.isSIP && inv.sipStatus === "active" && inv.sipAmount)
    .reduce((sum, inv) => {
      // Normalize to monthly equivalent
      let amount = inv.sipAmount || 0;
      if (inv.sipFrequency === "weekly") amount *= 4.33;
      if (inv.sipFrequency === "quarterly") amount /= 3;
      if (inv.sipFrequency === "yearly") amount /= 12;
      return sum + amount;
    }, 0);

  return (
    <div className="space-y-8">
      {/* Page Header */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4">
        <div>
          <h1 className="text-3xl font-extrabold tracking-tight bg-gradient-to-r from-slate-100 via-slate-300 to-slate-100 bg-clip-text text-transparent">
            Financial Dashboard
          </h1>
          <p className="text-slate-400 mt-1">
            Real-time overview of your net worth, investments, cash flow, and debts.
          </p>
        </div>
        <div className="flex items-center gap-3">
          <Link
            href="/expenses"
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-violet-600 hover:bg-violet-500 font-medium text-sm text-white shadow-lg shadow-violet-600/10 hover:scale-[1.01] active:scale-[0.99] transition-all"
          >
            <PlusCircle className="w-4 h-4" />
            <span>Add Expense</span>
          </Link>
          <Link
            href="/income"
            className="flex items-center gap-2 px-4 py-2.5 rounded-xl bg-slate-900 hover:bg-slate-850 border border-slate-800 font-medium text-sm text-slate-200 hover:scale-[1.01] active:scale-[0.99] transition-all"
          >
            <PlusCircle className="w-4 h-4" />
            <span>Add Income</span>
          </Link>
        </div>
      </div>

      {/* Row 1: KPI Summary Cards */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
        {/* Card 1: Net Worth */}
        <Link href="/networth" className="glass-card rounded-2xl p-6 flex flex-col justify-between">
          <div className="flex items-center justify-between mb-4">
            <span className="text-sm font-semibold text-slate-400 uppercase tracking-wider">Net Worth</span>
            <div className="w-10 h-10 rounded-xl bg-violet-500/10 border border-violet-500/20 text-violet-400 flex items-center justify-center">
              <PiggyBank className="w-5 h-5" />
            </div>
          </div>
          <div>
            <span className="text-3xl font-bold tracking-tight text-slate-100">
              ₹{netWorth.toLocaleString("en-IN", { minimumFractionDigits: 2, maximumFractionDigits: 2 })}
            </span>
            <div className="flex items-center gap-1.5 mt-2 text-xs">
              {netWorthChange >= 0 ? (
                <span className="flex items-center gap-0.5 text-green-400 font-medium bg-green-500/10 px-2 py-0.5 rounded-full">
                  <TrendingUp className="w-3.5 h-3.5" />
                  +{netWorthChangePercent.toFixed(1)}%
                </span>
              ) : (
                <span className="flex items-center gap-0.5 text-red-400 font-medium bg-red-500/10 px-2 py-0.5 rounded-full">
                  <TrendingDown className="w-3.5 h-3.5" />
                  {netWorthChangePercent.toFixed(1)}%
                </span>
              )}
              <span className="text-slate-500">vs last month</span>
            </div>
          </div>
        </Link>

        {/* Card 2: Income This Month */}
        <Link href="/income" className="glass-card rounded-2xl p-6 flex flex-col justify-between">
          <div className="flex items-center justify-between mb-4">
            <span className="text-sm font-semibold text-slate-400 uppercase tracking-wider">Income This Month</span>
            <div className="w-10 h-10 rounded-xl bg-green-500/10 border border-green-500/20 text-green-400 flex items-center justify-center">
              <ArrowUpRight className="w-5 h-5" />
            </div>
          </div>
          <div>
            <span className="text-3xl font-bold tracking-tight text-slate-100">
              ₹{totalIncomeThisMonth.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
            </span>
            <div className="flex items-center gap-1.5 mt-2 text-xs text-slate-400">
              <span className="font-medium text-green-400 bg-green-500/10 px-2 py-0.5 rounded-full">
                {activeIncomeSources} active
              </span>
              <span>sources</span>
            </div>
          </div>
        </Link>

        {/* Card 3: Expenses & Cash Flow */}
        <Link href="/expenses" className="glass-card rounded-2xl p-6 flex flex-col justify-between">
          <div className="flex items-center justify-between mb-4">
            <span className="text-sm font-semibold text-slate-400 uppercase tracking-wider">Expenses / Cash Flow</span>
            <div className="w-10 h-10 rounded-xl bg-red-500/10 border border-violet-500/0 text-red-400 flex items-center justify-center">
              <ArrowDownRight className="w-5 h-5" />
            </div>
          </div>
          <div>
            <span className="text-3xl font-bold tracking-tight text-slate-100">
              ₹{totalExpensesThisMonth.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
            </span>
            <div className="flex items-center gap-1.5 mt-2 text-xs">
              {remainingBalance >= 0 ? (
                <span className="flex items-center gap-0.5 text-green-400 font-medium bg-green-500/10 px-2 py-0.5 rounded-full">
                  +₹{remainingBalance.toLocaleString("en-IN", { maximumFractionDigits: 0 })} left
                </span>
              ) : (
                <span className="flex items-center gap-0.5 text-red-400 font-medium bg-red-500/10 px-2 py-0.5 rounded-full">
                  -₹{Math.abs(remainingBalance).toLocaleString("en-IN", { maximumFractionDigits: 0 })} deficit
                </span>
              )}
              <span className="text-slate-500">remaining balance</span>
            </div>
          </div>
        </Link>

        {/* Card 4: Investments Overall */}
        <Link href="/investments" className="glass-card rounded-2xl p-6 flex flex-col justify-between">
          <div className="flex items-center justify-between mb-4">
            <span className="text-sm font-semibold text-slate-400 uppercase tracking-wider">Investment Value</span>
            <div className="w-10 h-10 rounded-xl bg-cyan-500/10 border border-cyan-500/20 text-cyan-400 flex items-center justify-center">
              <TrendingUp className="w-5 h-5" />
            </div>
          </div>
          <div>
            <span className="text-3xl font-bold tracking-tight text-slate-100">
              ₹{totalInvestments.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
            </span>
            <div className="flex items-center gap-1.5 mt-2 text-xs">
              {investmentGainLoss >= 0 ? (
                <span className="flex items-center gap-0.5 text-green-400 font-medium bg-green-500/10 px-2 py-0.5 rounded-full">
                  +{investmentGainLossPercent.toFixed(1)}%
                </span>
              ) : (
                <span className="flex items-center gap-0.5 text-red-400 font-medium bg-red-500/10 px-2 py-0.5 rounded-full">
                  {investmentGainLossPercent.toFixed(1)}%
                </span>
              )}
              <span className="text-slate-500">overall return</span>
            </div>
          </div>
        </Link>
      </div>

      {/* Row 2: Charts and Gauges */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Expense Charts Section - spans 2 cols on lg screens */}
        <div className="lg:col-span-2">
          <ExpenseChartSection expenses={expensesThisMonth} />
        </div>

        {/* Credit Score Gauge & Investments Breakdown */}
        <div className="flex flex-col gap-6">
          <CreditScoreGaugeCard latestScore={latestCreditScore} />
          <InvestmentSummaryCard investments={investments} totalValue={totalInvestments} metalsPrices={metalsPrices} />
        </div>
      </div>

      {/* Row 3: Liabilities, Outstanding Receivables & Market Rates */}
      <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
        {/* Upcoming Credit Card Bills */}
        <div className="glass-card rounded-2xl p-6">
          <div className="flex items-center justify-between mb-6 border-b border-slate-900 pb-4">
            <div>
              <h2 className="text-lg font-bold text-slate-100 flex items-center gap-2">
                <CreditCard className="w-5 h-5 text-indigo-400" />
                <span>Upcoming Credit Card Bills</span>
              </h2>
              <p className="text-xs text-slate-400 mt-0.5">Bills generated and awaiting settlement.</p>
            </div>
            <Link href="/credit-cards" className="text-xs text-violet-400 hover:text-violet-300 font-medium flex items-center gap-0.5 transition-colors">
              <span>View Cards</span>
              <ChevronRight className="w-3.5 h-3.5" />
            </Link>
          </div>

          {unpaidBills.length === 0 ? (
            <div className="flex flex-col items-center justify-center py-10 text-center text-slate-500">
              <CheckCircleComponent className="w-8 h-8 text-green-500/20 mb-2" />
              <p className="text-sm font-medium">All credit card bills paid!</p>
              <p className="text-xs text-slate-600 mt-0.5">No outstanding statements due.</p>
            </div>
          ) : (
            <div className="space-y-4">
              {unpaidBills.map((bill) => {
                const remaining = Math.max(0, bill.totalAmount - bill.paidAmount);
                const today = new Date();
                const dueDate = new Date(bill.dueDate);
                const diffTime = dueDate.getTime() - today.getTime();
                const diffDays = Math.ceil(diffTime / (1000 * 60 * 60 * 24));
                const isOverdue = diffDays <= 0;

                return (
                  <div key={bill.id} className="flex items-center justify-between p-4 rounded-xl bg-slate-900/40 border border-slate-900">
                    <div className="space-y-1">
                      <p className="text-sm font-semibold text-slate-200">{bill.card.cardName}</p>
                      <div className="flex items-center gap-2 text-xs text-slate-500">
                        <span>Due: {format(dueDate, "dd MMM yyyy")}</span>
                        <span>•</span>
                        {isOverdue ? (
                          <span className="text-red-400 font-semibold flex items-center gap-0.5">
                            <AlertCircle className="w-3 h-3" /> Overdue
                          </span>
                        ) : (
                          <span className={`${diffDays <= 5 ? "text-amber-400 font-medium" : "text-slate-400"}`}>
                            {diffDays} days remaining
                          </span>
                        )}
                      </div>
                    </div>
                    <div className="text-right">
                      <p className="text-sm font-bold text-slate-100">
                        ₹{remaining.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
                      </p>
                      <span className="text-[10px] text-slate-500">Total: ₹{bill.totalAmount}</span>
                    </div>
                  </div>
                );
              })}
            </div>
          )}
        </div>

        {/* Loans Given Summary */}
        <div className="glass-card rounded-2xl p-6">
          <div className="flex items-center justify-between mb-6 border-b border-slate-900 pb-4">
            <div>
              <h2 className="text-lg font-bold text-slate-100 flex items-center gap-2">
                <Coins className="w-5 h-5 text-cyan-400" />
                <span>Loans Given (Outstanding)</span>
              </h2>
              <p className="text-xs text-slate-400 mt-0.5">Money lent to friends/family with outstanding balance.</p>
            </div>
            <Link href="/loans" className="text-xs text-violet-400 hover:text-violet-300 font-medium flex items-center gap-0.5 transition-colors">
              <span>Manage Loans</span>
              <ChevronRight className="w-3.5 h-3.5" />
            </Link>
          </div>

          {totalLoansReceivable === 0 ? (
            <div className="flex flex-col items-center justify-center py-10 text-center text-slate-500">
              <Coins className="w-8 h-8 text-slate-700 mb-2" />
              <p className="text-sm font-medium">No active loans given.</p>
              <p className="text-xs text-slate-600 mt-0.5">You have collected all lent money.</p>
            </div>
          ) : (
            <div className="space-y-4">
              {loans
                .filter((loan) => loan.status !== "closed")
                .map((loan) => {
                  const repaid = loan.repayments.reduce((sum, r) => sum + r.amount, 0);
                  const outstanding = Math.max(0, loan.principalAmount - repaid);
                  const isOverdue = loan.expectedReturnDate && new Date(loan.expectedReturnDate) < new Date();

                  return (
                    <div key={loan.id} className="flex items-center justify-between p-4 rounded-xl bg-slate-900/40 border border-slate-900">
                      <div className="space-y-1">
                        <p className="text-sm font-semibold text-slate-200">{loan.borrowerName}</p>
                        <div className="flex items-center gap-2 text-xs text-slate-500">
                          {loan.expectedReturnDate ? (
                            <span className={isOverdue ? "text-red-400 font-medium" : ""}>
                              Return by: {format(new Date(loan.expectedReturnDate), "dd MMM yyyy")}
                            </span>
                          ) : (
                            <span>No return date set</span>
                          )}
                          {isOverdue && (
                            <span className="text-red-400 font-semibold bg-red-500/10 px-1.5 py-0.5 rounded text-[10px]">
                              OVERDUE
                            </span>
                          )}
                        </div>
                      </div>
                      <div className="text-right">
                        <p className="text-sm font-bold text-slate-100">
                          ₹{outstanding.toLocaleString("en-IN", { maximumFractionDigits: 2 })}
                        </p>
                        <span className="text-[10px] text-slate-500">
                          Lent: ₹{loan.principalAmount.toLocaleString("en-IN", { maximumFractionDigits: 0 })}
                        </span>
                      </div>
                    </div>
                  );
                })}
            </div>
          )}
        </div>

        {/* Live Market Rates */}
        <LiveMarketRatesCard metalsPrices={metalsPrices} />
        
        {/* Target Net Worth Forecaster */}
        <TargetNetWorthCalculator 
          currentNetWorth={netWorth} 
          monthlySipTotal={monthlySipTotal} 
          monthlyIncome={totalIncomeThisMonth}
          monthlyExpense={totalExpensesThisMonth}
        />

        {/* Card Rewards and Savings */}
        <div className="md:col-span-2">
          <CardSavingsGraph savingsData={savingsData as any} />
        </div>
      </div>
    </div>
  );
}

function CheckCircleComponent({ className }: { className?: string }) {
  return (
    <svg
      xmlns="http://www.w3.org/2000/svg"
      fill="none"
      viewBox="0 0 24 24"
      strokeWidth={1.5}
      stroke="currentColor"
      className={className}
    >
      <path strokeLinecap="round" strokeLinejoin="round" d="M9 12.75L11.25 15 15 9.75M21 12a9 9 0 11-18 0 9 9 0 0118 0z" />
    </svg>
  );
}
