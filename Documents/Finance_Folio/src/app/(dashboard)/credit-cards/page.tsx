import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { db } from "@/lib/db";
import CardsClient from "@/components/cards/CardsClient";

export const revalidate = 0;

export default async function CreditCardsPage() {
  const session = await getServerSession(authOptions);
  const userId = session!.user!.id;

  const [cards, bills, accounts] = await Promise.all([
    db.card.findMany({
      where: { userId, isActive: true },
      include: { account: true },
      orderBy: { cardName: "asc" },
    }),
    db.creditCardBill.findMany({
      where: { card: { userId, isActive: true } },
      include: { card: true },
      orderBy: { dueDate: "asc" },
    }),
    db.account.findMany({
      where: { userId, isActive: true },
      orderBy: { accountName: "asc" },
    }),
  ]);

  return (
    <CardsClient
      initialCards={cards}
      initialBills={bills}
      accounts={accounts}
    />
  );
}
