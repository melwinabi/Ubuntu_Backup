import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { db } from "@/lib/db";
import IncomeClient from "@/components/income/IncomeClient";

export const revalidate = 0; // Disable static rendering

export default async function IncomePage() {
  const session = await getServerSession(authOptions);
  const userId = session!.user!.id;

  const [sources, transactions, accounts] = await Promise.all([
    db.incomeSource.findMany({
      where: { userId, isActive: true },
      include: { transactions: true },
      orderBy: { sourceName: "asc" },
    }),
    db.incomeTransaction.findMany({
      where: { incomeSource: { userId, isActive: true } },
      include: { incomeSource: true, account: true },
      orderBy: { receivedOn: "desc" },
    }),
    db.account.findMany({
      where: { userId, isActive: true },
      orderBy: { accountName: "asc" },
    }),
  ]);

  return (
    <IncomeClient
      initialSources={sources}
      initialTransactions={transactions}
      accounts={accounts}
    />
  );
}
