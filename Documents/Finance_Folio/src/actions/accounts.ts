"use server";

import { db } from "@/lib/db";
import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { accountSchema } from "@/lib/validators";
import { revalidatePath } from "next/cache";
import { recalculateNetWorthSnapshot } from "@/lib/calculations";

async function getUserId() {
  const session = await getServerSession(authOptions);
  if (!session?.user?.id) {
    throw new Error("Unauthorized");
  }
  return session.user.id;
}

export async function getAccounts() {
  try {
    const userId = await getUserId();
    const accounts = await db.account.findMany({
      where: { userId, isActive: true },
      orderBy: { accountName: "asc" },
    });
    return { success: true, data: accounts };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch accounts" };
  }
}

export async function createAccount(data: any) {
  try {
    const userId = await getUserId();
    const validated = accountSchema.parse(data);

    const account = await db.account.create({
      data: {
        userId,
        accountType: validated.accountType,
        accountName: validated.accountName,
        providerName: validated.providerName || null,
        accountNumberLast4: validated.accountNumberLast4 || null,
        openingBalance: validated.openingBalance,
        currentBalance: validated.openingBalance, // Initial current balance is opening balance
      },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/networth");
    return { success: true, data: account };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to create account" };
  }
}

export async function updateAccount(id: string, data: any) {
  try {
    const userId = await getUserId();
    const validated = accountSchema.parse(data);

    // Verify ownership
    const existing = await db.account.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Account not found or access denied" };
    }

    // When updating account, adjust current balance by the difference in opening balance
    const openingBalanceDiff = validated.openingBalance - existing.openingBalance;
    const newCurrentBalance = existing.currentBalance + openingBalanceDiff;

    const account = await db.account.update({
      where: { id },
      data: {
        accountType: validated.accountType,
        accountName: validated.accountName,
        providerName: validated.providerName || null,
        accountNumberLast4: validated.accountNumberLast4 || null,
        openingBalance: validated.openingBalance,
        currentBalance: newCurrentBalance,
      },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/networth");
    return { success: true, data: account };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to update account" };
  }
}

export async function deleteAccount(id: string) {
  try {
    const userId = await getUserId();

    const existing = await db.account.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Account not found or access denied" };
    }

    // Soft delete to preserve financial history, or hard delete? Let's soft delete by setting isActive=false
    await db.account.update({
      where: { id },
      data: { isActive: false },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/networth");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete account" };
  }
}

export async function adjustAccountBalance(id: string, newBalance: number) {
  try {
    const userId = await getUserId();
    const existing = await db.account.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Account not found or access denied" };
    }
    const account = await db.account.update({
      where: { id },
      data: {
        currentBalance: newBalance,
      },
    });
    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/networth");
    revalidatePath("/settings");
    return { success: true, data: account };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to adjust balance" };
  }
}

