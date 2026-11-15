import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { db } from "@/lib/db";
import { getLivePreciousMetalsPrices, convertPreciousMetalToGrams } from "@/lib/calculations";
import NetWorthClient from "@/components/networth/NetWorthClient";

export const revalidate = 0;

export default async function NetWorthPage() {
  const session = await getServerSession(authOptions);
  const userId = session!.user!.id;

  // Fetch snapshots
  const snapshots = await db.netWorthSnapshot.findMany({
    where: { userId },
    orderBy: { snapshotDate: "asc" },
  });

  // Calculate current real-time assets and liabilities for preview
  const [accounts, investments, loans, cards, metalsPrices] = await Promise.all([
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
    getLivePreciousMetalsPrices(),
  ]);

  const cashAsset = accounts.reduce((sum, a) => sum + a.currentBalance, 0);
  
  const investmentAsset = investments.reduce((sum, inv) => {
    if (inv.assetType === "gold") {
      const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit || "g");
      return sum + grams * metalsPrices.gold;
    }
    if (inv.assetType === "silver") {
      const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit || "g");
      return sum + grams * metalsPrices.silver;
    }

    const price = inv.prices[0]?.pricePerUnit ?? inv.purchasePrice;
    return sum + inv.quantity * price;
  }, 0);

  const loanAsset = loans.reduce((sum, loan) => {
    if (loan.status === "closed") return sum;
    const repaid = loan.repayments.reduce((rSum, r) => rSum + r.amount, 0);
    return sum + Math.max(0, loan.principalAmount - repaid);
  }, 0);

  const cardLiability = cards.reduce((sum, c) => sum + c.currentOutstanding, 0);

  const totalAssets = cashAsset + investmentAsset + loanAsset;
  const currentCalculatedNetWorth = totalAssets - cardLiability;

  return (
    <NetWorthClient
      initialSnapshots={snapshots}
      currentPreview={{
        netWorth: currentCalculatedNetWorth,
        assets: totalAssets,
        liabilities: cardLiability,
        cash: cashAsset,
        investments: investmentAsset,
        loans: loanAsset,
      }}
    />
  );
}
