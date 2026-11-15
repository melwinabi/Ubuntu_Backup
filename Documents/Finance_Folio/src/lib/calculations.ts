import { db } from "./db";
const YF = require('yahoo-finance2').default;
const yahooFinance = new YF({ suppressNotices: ['yahooSurvey'] });
export * from "./clientMath";
import { convertPreciousMetalToGrams, calculateSipStats } from "./clientMath";

/**
 * Fetches spot gold and silver prices via Yahoo Finance and converts them to retail INR/gram.
 * Also fetches NIFTY 50 index.
 */
export async function getLivePreciousMetalsPrices() {
  try {
    const symbols = ['^NSEI', 'GC=F', 'SI=F', 'INR=X'];
    const results = (await yahooFinance.quote(symbols)) as any[];
    
    const prices: Record<string, number> = {};
    results.forEach((q: any) => {
      if (q && q.symbol && q.regularMarketPrice) {
        prices[q.symbol] = q.regularMarketPrice;
      }
    });

    const usdToInr = prices['INR=X'] || 83.50;
    const goldUSDPerOunce = prices['GC=F'] || 2350;
    const silverUSDPerOunce = prices['SI=F'] || 28.00;
    const nifty = prices['^NSEI'] || 0;
    
    const TROY_OZ_TO_GRAMS = 31.1034768;
    const goldTaxMultiplier = 1.16; // 16% Indian taxes & premiums (24K Retail)
    const silverTaxMultiplier = 1.16;

    const baseGoldInrPerGram = (goldUSDPerOunce / TROY_OZ_TO_GRAMS) * usdToInr;
    const retailGold24k = baseGoldInrPerGram * goldTaxMultiplier;

    const baseSilverInrPerGram = (silverUSDPerOunce / TROY_OZ_TO_GRAMS) * usdToInr;
    const retailSilverGram = baseSilverInrPerGram * silverTaxMultiplier;

    return {
      success: true,
      gold: retailGold24k,
      silver: retailSilverGram,
      nifty: nifty,
      goldUSDPerOunce,
      silverUSDPerOunce,
      usdToInr,
    };
  } catch (error) {
    console.error("Failed to fetch live precious metal prices:", error);
    return {
      success: false,
      gold: 7450 * 1.16, // Fallback with tax
      silver: 90 * 1.16,
      nifty: 24000,
      goldUSDPerOunce: 2350,
      silverUSDPerOunce: 29,
      usdToInr: 83.5,
    };
  }
}

/**
 * Recalculates and saves a net worth snapshot for a user.
 */
export async function recalculateNetWorthSnapshot(userId: string, dateStr?: string) {
  const targetDate = dateStr ? new Date(dateStr) : new Date();
  targetDate.setHours(0, 0, 0, 0);

  // 1. Get Cash & Bank Balances
  const accounts = await db.account.findMany({
    where: { userId, isActive: true },
    select: { currentBalance: true },
  });
  const totalCash = accounts.reduce((sum, acc) => sum + acc.currentBalance, 0);

  // 2. Get Investments Current Value
  const investments = await db.investment.findMany({
    where: { userId, isActive: true },
    include: {
      prices: {
        orderBy: { priceDate: "desc" },
        take: 1,
      },
      sipSkips: true,
    },
  });

  const metals = await getLivePreciousMetalsPrices();

  const totalInvestments = investments.reduce((sum, inv) => {
    if (inv.assetType === "gold") {
      const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit);
      return sum + grams * metals.gold;
    }
    if (inv.assetType === "silver") {
      const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit);
      return sum + grams * metals.silver;
    }

    const currentPrice = inv.prices[0]?.pricePerUnit ?? inv.purchasePrice;
    
    if (inv.isSIP && inv.sipAmount) {
      const stats = calculateSipStats(inv);
      if (inv.purchasePrice > 0) {
        const accumulatedUnits = stats.totalInvested / inv.purchasePrice;
        return sum + accumulatedUnits * currentPrice;
      }
      return sum + stats.totalInvested;
    }

    return sum + inv.quantity * currentPrice;
  }, 0);

  // 3. Get Loans Receivable Outstanding
  const loans = await db.loanGiven.findMany({
    where: {
      userId,
      status: { in: ["active", "partially_paid", "overdue"] },
    },
    include: {
      repayments: {
        select: { amount: true },
      },
    },
  });

  const totalLoansReceivable = loans.reduce((sum, loan) => {
    const totalRepaid = loan.repayments.reduce((rSum, rep) => rSum + rep.amount, 0);
    const outstanding = Math.max(0, loan.principalAmount - totalRepaid);
    return sum + outstanding;
  }, 0);

  // 4. Get Credit Card Outstanding (Liabilities)
  const cards = await db.card.findMany({
    where: { userId, cardType: "credit", isActive: true },
    select: { currentOutstanding: true },
  });
  const totalLiabilities = cards.reduce((sum, card) => sum + card.currentOutstanding, 0);

  const netWorth = totalCash + totalInvestments + totalLoansReceivable - totalLiabilities;

  // 5. Save/Upsert Snapshot
  const startOfDay = new Date(targetDate);
  startOfDay.setHours(0, 0, 0, 0);

  return await db.netWorthSnapshot.upsert({
    where: {
      userId_snapshotDate: {
        userId,
        snapshotDate: startOfDay,
      },
    },
    update: {
      totalCash,
      totalInvestments,
      totalLoansReceivable,
      totalLiabilities,
      netWorth,
    },
    create: {
      userId,
      snapshotDate: startOfDay,
      totalCash,
      totalInvestments,
      totalLoansReceivable,
      totalLiabilities,
      netWorth,
    },
  });
}
