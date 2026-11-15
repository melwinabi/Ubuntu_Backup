import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { db } from "@/lib/db";
import { getLivePreciousMetalsPrices } from "@/lib/calculations";
import InvestmentsClient from "@/components/investments/InvestmentsClient";

export const revalidate = 0; // Disable static rendering

export default async function InvestmentsPage() {
  const session = await getServerSession(authOptions);
  const userId = session!.user!.id;

  const [investments, accounts, metalsPrices] = await Promise.all([
    db.investment.findMany({
      where: { userId, isActive: true },
      include: {
        account: true,
        prices: {
          orderBy: { priceDate: "desc" },
        },
        alerts: {
          orderBy: { alertDate: "asc" },
        },
        sipSkips: true,
      },
      orderBy: { purchaseDate: "desc" },
    }),
    db.account.findMany({
      where: { userId, isActive: true },
      orderBy: { accountName: "asc" },
    }),
    getLivePreciousMetalsPrices(),
  ]);

  return (
    <InvestmentsClient
      initialInvestments={investments}
      accounts={accounts}
      metalsPrices={metalsPrices}
    />
  );
}
