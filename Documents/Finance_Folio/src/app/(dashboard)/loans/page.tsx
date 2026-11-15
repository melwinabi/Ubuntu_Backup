import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { db } from "@/lib/db";
import LoansClient from "@/components/loans/LoansClient";

export const revalidate = 0;

export default async function LoansPage() {
  const session = await getServerSession(authOptions);
  const userId = session!.user!.id;

  const [loans, accounts] = await Promise.all([
    db.loanGiven.findMany({
      where: { userId },
      include: {
        account: true,
        repayments: {
          orderBy: { paidOn: "desc" },
        },
      },
      orderBy: { amountGivenOn: "desc" },
    }),
    db.account.findMany({
      where: { userId, isActive: true },
      orderBy: { accountName: "asc" },
    }),
  ]);

  return (
    <LoansClient
      initialLoans={loans}
      accounts={accounts}
    />
  );
}
