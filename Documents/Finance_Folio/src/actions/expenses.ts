"use server";

import { db } from "@/lib/db";
import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { expenseSchema, expenseCategorySchema } from "@/lib/validators";
import { revalidatePath } from "next/cache";
import { recalculateNetWorthSnapshot } from "@/lib/calculations";

async function getUserId() {
  const session = await getServerSession(authOptions);
  if (!session?.user?.id) {
    throw new Error("Unauthorized");
  }
  return session.user.id;
}

// CATEGORIES CRUD
export async function getExpenseCategories() {
  try {
    const userId = await getUserId();
    const categories = await db.expenseCategory.findMany({
      where: { userId },
      orderBy: { name: "asc" },
    });
    return { success: true, data: categories };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch categories" };
  }
}

export async function createExpenseCategory(data: any) {
  try {
    const userId = await getUserId();
    const validated = expenseCategorySchema.parse(data);

    const category = await db.expenseCategory.create({
      data: {
        userId,
        name: validated.name,
        icon: validated.icon || "HelpCircle",
      },
    });

    revalidatePath("/expenses");
    return { success: true, data: category };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to create category" };
  }
}

// EXPENSES CRUD
export async function getExpenses() {
  try {
    const userId = await getUserId();
    const expenses = await db.expense.findMany({
      where: { userId },
      include: {
        category: true,
        account: true,
        card: true,
      },
      orderBy: { spentOn: "desc" },
    });
    return { success: true, data: expenses };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch expenses" };
  }
}

export async function createExpense(data: any) {
  try {
    const userId = await getUserId();
    const validated = expenseSchema.parse(data);

    // Validate relations ownership
    if (validated.categoryId) {
      const cat = await db.expenseCategory.findFirst({
        where: { id: validated.categoryId, userId },
      });
      if (!cat) return { success: false, error: "Category not found or access denied" };
    }

    if (validated.paymentMethod === "credit_card") {
      if (!validated.cardId) {
        return { success: false, error: "Credit card selection is required for credit card payment method" };
      }
      const card = await db.card.findFirst({
        where: { id: validated.cardId, userId, cardType: "credit" },
      });
      if (!card) return { success: false, error: "Credit card not found or access denied" };

      // Increment card outstanding
      await db.card.update({
        where: { id: validated.cardId },
        data: {
          currentOutstanding: {
            increment: validated.amount,
          },
        },
      });
    } else {
      if (!validated.accountId) {
        return { success: false, error: "Account selection is required for this payment method" };
      }
      const account = await db.account.findFirst({
        where: { id: validated.accountId, userId },
      });
      if (!account) return { success: false, error: "Account not found or access denied" };

      // Decrement account balance
      await db.account.update({
        where: { id: validated.accountId },
        data: {
          currentBalance: {
            decrement: validated.amount,
          },
        },
      });
    }

    const expense = await db.expense.create({
      data: {
        userId,
        categoryId: validated.categoryId || null,
        amount: validated.amount,
        spentOn: new Date(validated.spentOn),
        paymentMethod: validated.paymentMethod,
        accountId: validated.paymentMethod !== "credit_card" ? validated.accountId : null,
        cardId: validated.paymentMethod === "credit_card" ? validated.cardId : null,
        merchant: validated.merchant || null,
        notes: validated.notes || null,
        savedAmount: validated.savedAmount || 0,
      },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/expenses");
    revalidatePath("/networth");
    return { success: true, data: expense };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to record expense" };
  }
}

export async function deleteExpense(id: string) {
  try {
    const userId = await getUserId();

    const expense = await db.expense.findFirst({
      where: { id, userId },
    });

    if (!expense) {
      return { success: false, error: "Expense not found or access denied" };
    }

    // Reverse the financial transaction
    if (expense.paymentMethod === "credit_card" && expense.cardId) {
      // Decrement card outstanding
      await db.card.update({
        where: { id: expense.cardId },
        data: {
          currentOutstanding: {
            decrement: expense.amount,
          },
        },
      });
    } else if (expense.accountId) {
      // Increment account balance
      await db.account.update({
        where: { id: expense.accountId },
        data: {
          currentBalance: {
            increment: expense.amount,
          },
        },
      });
    }

    await db.expense.delete({
      where: { id },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/expenses");
    revalidatePath("/networth");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete expense" };
  }
}
