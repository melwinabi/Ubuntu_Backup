"use server";

import { db } from "@/lib/db";
import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { incomeSourceSchema, incomeTransactionSchema } from "@/lib/validators";
import { revalidatePath } from "next/cache";
import { recalculateNetWorthSnapshot } from "@/lib/calculations";

async function getUserId() {
  const session = await getServerSession(authOptions);
  if (!session?.user?.id) {
    throw new Error("Unauthorized");
  }
  return session.user.id;
}

// INCOME SOURCES CRUD
export async function getIncomeSources() {
  try {
    const userId = await getUserId();
    const sources = await db.incomeSource.findMany({
      where: { userId, isActive: true },
      include: { transactions: true },
      orderBy: { sourceName: "asc" },
    });
    return { success: true, data: sources };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch income sources" };
  }
}

export async function createIncomeSource(data: any) {
  try {
    const userId = await getUserId();
    const validated = incomeSourceSchema.parse(data);

    const source = await db.incomeSource.create({
      data: {
        userId,
        sourceName: validated.sourceName,
        sourceType: validated.sourceType || null,
        frequency: validated.frequency,
        expectedAmount: validated.expectedAmount || null,
        defaultAccountId: validated.defaultAccountId || null,
      },
    });

    revalidatePath("/dashboard");
    revalidatePath("/income");
    return { success: true, data: source };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to create income source" };
  }
}

export async function updateIncomeSource(id: string, data: any) {
  try {
    const userId = await getUserId();
    const validated = incomeSourceSchema.parse(data);

    const existing = await db.incomeSource.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Income source not found or access denied" };
    }

    const source = await db.incomeSource.update({
      where: { id },
      data: {
        sourceName: validated.sourceName,
        sourceType: validated.sourceType || null,
        frequency: validated.frequency,
        expectedAmount: validated.expectedAmount || null,
        defaultAccountId: validated.defaultAccountId || null,
      },
    });

    revalidatePath("/dashboard");
    revalidatePath("/income");
    return { success: true, data: source };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to update income source" };
  }
}

export async function deleteIncomeSource(id: string) {
  try {
    const userId = await getUserId();

    const existing = await db.incomeSource.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Income source not found or access denied" };
    }

    await db.incomeSource.update({
      where: { id },
      data: { isActive: false },
    });

    revalidatePath("/dashboard");
    revalidatePath("/income");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete income source" };
  }
}

// INCOME TRANSACTIONS CRUD
export async function getIncomeTransactions() {
  try {
    const userId = await getUserId();
    const transactions = await db.incomeTransaction.findMany({
      where: {
        incomeSource: { userId, isActive: true },
      },
      include: {
        incomeSource: true,
        account: true,
      },
      orderBy: { receivedOn: "desc" },
    });
    return { success: true, data: transactions };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch income transactions" };
  }
}

export async function createIncomeTransaction(data: any) {
  try {
    const userId = await getUserId();
    const validated = incomeTransactionSchema.parse(data);

    // Verify source ownership
    const source = await db.incomeSource.findFirst({
      where: { id: validated.incomeSourceId, userId },
    });
    if (!source) {
      return { success: false, error: "Income source not found or access denied" };
    }

    // Verify account ownership if provided
    if (validated.accountId) {
      const account = await db.account.findFirst({
        where: { id: validated.accountId, userId },
      });
      if (!account) {
        return { success: false, error: "Account not found or access denied" };
      }
    }

    const transaction = await db.incomeTransaction.create({
      data: {
        incomeSourceId: validated.incomeSourceId,
        accountId: validated.accountId || null,
        amount: validated.amount,
        receivedOn: new Date(validated.receivedOn),
        notes: validated.notes || null,
      },
    });

    // Increment account balance
    if (validated.accountId) {
      await db.account.update({
        where: { id: validated.accountId },
        data: {
          currentBalance: {
            increment: validated.amount,
          },
        },
      });
    }

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/income");
    revalidatePath("/networth");
    return { success: true, data: transaction };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to log income transaction" };
  }
}

export async function deleteIncomeTransaction(id: string) {
  try {
    const userId = await getUserId();

    const transaction = await db.incomeTransaction.findUnique({
      where: { id },
      include: {
        incomeSource: true,
      },
    });

    if (!transaction || transaction.incomeSource.userId !== userId) {
      return { success: false, error: "Transaction not found or access denied" };
    }

    // Decrement account balance if it was credited
    if (transaction.accountId) {
      await db.account.update({
        where: { id: transaction.accountId },
        data: {
          currentBalance: {
            decrement: transaction.amount,
          },
        },
      });
    }

    await db.incomeTransaction.delete({
      where: { id },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/income");
    revalidatePath("/networth");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete income transaction" };
  }
}
