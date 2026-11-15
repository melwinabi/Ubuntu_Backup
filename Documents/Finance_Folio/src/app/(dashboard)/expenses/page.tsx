import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { db } from "@/lib/db";
import ExpensesClient from "@/components/expenses/ExpensesClient";

export const revalidate = 0; // Disable static rendering

export default async function ExpensesPage() {
  const session = await getServerSession(authOptions);
  const userId = session!.user!.id;

  const [categories, expenses, accounts, cards] = await Promise.all([
    db.expenseCategory.findMany({
      where: { userId },
      orderBy: { name: "asc" },
    }),
    db.expense.findMany({
      where: { userId },
      include: { category: true, account: true, card: true },
      orderBy: { spentOn: "desc" },
    }),
    db.account.findMany({
      where: { userId, isActive: true },
      orderBy: { accountName: "asc" },
    }),
    db.card.findMany({
      where: { userId, isActive: true },
      orderBy: { cardName: "asc" },
    }),
  ]);

  return (
    <ExpensesClient
      initialCategories={categories}
      initialExpenses={expenses}
      accounts={accounts}
      cards={cards}
    />
  );
}
