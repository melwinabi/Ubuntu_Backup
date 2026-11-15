"use server";

import { db } from "@/lib/db";
import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { cardSchema, creditCardBillSchema } from "@/lib/validators";
import { revalidatePath } from "next/cache";
import { recalculateNetWorthSnapshot } from "@/lib/calculations";

async function getUserId() {
  const session = await getServerSession(authOptions);
  if (!session?.user?.id) {
    throw new Error("Unauthorized");
  }
  return session.user.id;
}

// CARDS CRUD
export async function getCards() {
  try {
    const userId = await getUserId();
    const cards = await db.card.findMany({
      where: { userId, isActive: true },
      include: { account: true },
      orderBy: { cardName: "asc" },
    });
    return { success: true, data: cards };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch cards" };
  }
}

export async function createCard(data: any) {
  try {
    const userId = await getUserId();
    const validated = cardSchema.parse(data);

    const card = await db.card.create({
      data: {
        userId,
        accountId: validated.accountId || null,
        cardType: validated.cardType,
        cardName: validated.cardName,
        last4: validated.last4 || null,
        creditLimit: validated.creditLimit || null,
        statementDay: validated.statementDay || null,
        dueDay: validated.dueDay || null,
        currentOutstanding: validated.currentOutstanding,
      },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/credit-cards");
    return { success: true, data: card };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to create card" };
  }
}

export async function updateCard(id: string, data: any) {
  try {
    const userId = await getUserId();
    const validated = cardSchema.parse(data);

    const existing = await db.card.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Card not found or access denied" };
    }

    const card = await db.card.update({
      where: { id },
      data: {
        accountId: validated.accountId || null,
        cardType: validated.cardType,
        cardName: validated.cardName,
        last4: validated.last4 || null,
        creditLimit: validated.creditLimit || null,
        statementDay: validated.statementDay || null,
        dueDay: validated.dueDay || null,
        currentOutstanding: validated.currentOutstanding,
      },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/credit-cards");
    return { success: true, data: card };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to update card" };
  }
}

export async function deleteCard(id: string) {
  try {
    const userId = await getUserId();

    const existing = await db.card.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Card not found or access denied" };
    }

    await db.card.update({
      where: { id },
      data: { isActive: false },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/credit-cards");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete card" };
  }
}

// CREDIT CARD BILLS
export async function getCardBills() {
  try {
    const userId = await getUserId();
    const bills = await db.creditCardBill.findMany({
      where: {
        card: { userId, isActive: true },
      },
      include: {
        card: true,
      },
      orderBy: { dueDate: "asc" },
    });
    return { success: true, data: bills };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch bills" };
  }
}

export async function createCardBill(data: any) {
  try {
    const userId = await getUserId();
    const validated = creditCardBillSchema.parse(data);

    // Verify card ownership
    const card = await db.card.findFirst({
      where: { id: validated.cardId, userId },
    });
    if (!card) {
      return { success: false, error: "Card not found or access denied" };
    }

    const bill = await db.creditCardBill.create({
      data: {
        cardId: validated.cardId,
        billingStart: new Date(validated.billingStart),
        billingEnd: new Date(validated.billingEnd),
        statementDate: new Date(validated.statementDate),
        dueDate: new Date(validated.dueDate),
        totalAmount: validated.totalAmount,
        minimumDue: validated.minimumDue || null,
        paidAmount: validated.paidAmount,
        status: validated.status,
      },
    });

    // If bill created with non-zero paidAmount, adjust card outstanding
    if (validated.paidAmount > 0) {
      await db.card.update({
        where: { id: validated.cardId },
        data: {
          currentOutstanding: {
            decrement: validated.paidAmount,
          },
        },
      });
    }

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/credit-cards");
    return { success: true, data: bill };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to create card bill" };
  }
}

export async function payCardBill(billId: string, payAmount: number, fromAccountId: string) {
  try {
    const userId = await getUserId();

    const bill = await db.creditCardBill.findUnique({
      where: { id: billId },
      include: { card: true },
    });

    if (!bill || bill.card.userId !== userId) {
      return { success: false, error: "Bill not found or access denied" };
    }

    const account = await db.account.findUnique({
      where: { id: fromAccountId, userId },
    });

    if (!account) {
      return { success: false, error: "Payment account not found or access denied" };
    }

    // 1. Deduct from payment account
    await db.account.update({
      where: { id: fromAccountId },
      data: {
        currentBalance: {
          decrement: payAmount,
        },
      },
    });

    // 2. Reduce card outstanding
    await db.card.update({
      where: { id: bill.cardId },
      data: {
        currentOutstanding: {
          decrement: payAmount,
        },
      },
    });

    // 3. Update bill record
    const newPaidAmount = bill.paidAmount + payAmount;
    let newStatus = "unpaid";
    if (newPaidAmount >= bill.totalAmount) {
      newStatus = "paid";
    } else if (newPaidAmount > 0) {
      newStatus = "partially_paid";
    }

    await db.creditCardBill.update({
      where: { id: billId },
      data: {
        paidAmount: newPaidAmount,
        status: newStatus,
      },
    });

    // Log this bill payment as an expense so it appears in the cash flow, but marked as bill payment?
    // Actually, credit card purchases are already logged as expenses, so paying the credit card is a transfer.
    // We don't double count it as an expense, we just update account balance and card outstanding. This is correct!

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/credit-cards");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to process card bill payment" };
  }
}
