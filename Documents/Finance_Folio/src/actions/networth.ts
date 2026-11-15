"use server";

import { db } from "@/lib/db";
import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { recalculateNetWorthSnapshot } from "@/lib/calculations";
import { revalidatePath } from "next/cache";

async function getUserId() {
  const session = await getServerSession(authOptions);
  if (!session?.user?.id) {
    throw new Error("Unauthorized");
  }
  return session.user.id;
}

export async function triggerRecalculateNetWorth() {
  try {
    const userId = await getUserId();
    const snapshot = await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/networth");
    return { success: true, data: snapshot };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to recalculate net worth" };
  }
}

export async function getNetWorthHistory() {
  try {
    const userId = await getUserId();
    const history = await db.netWorthSnapshot.findMany({
      where: { userId },
      orderBy: { snapshotDate: "asc" },
    });
    return { success: true, data: history };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch net worth history" };
  }
}

export async function deleteNetWorthSnapshot(id: string) {
  try {
    const userId = await getUserId();
    const existing = await db.netWorthSnapshot.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Snapshot not found" };
    }
    await db.netWorthSnapshot.delete({
      where: { id },
    });
    revalidatePath("/dashboard");
    revalidatePath("/networth");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete snapshot" };
  }
}

