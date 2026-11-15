"use server";

import { db } from "@/lib/db";
import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { creditScoreSchema } from "@/lib/validators";
import { revalidatePath } from "next/cache";

async function getUserId() {
  const session = await getServerSession(authOptions);
  if (!session?.user?.id) {
    throw new Error("Unauthorized");
  }
  return session.user.id;
}

export async function getCreditScores() {
  try {
    const userId = await getUserId();
    const scores = await db.creditScore.findMany({
      where: { userId },
      orderBy: { recordedOn: "desc" },
    });
    return { success: true, data: scores };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch credit scores" };
  }
}

export async function createCreditScore(data: any) {
  try {
    const userId = await getUserId();
    const validated = creditScoreSchema.parse(data);

    const score = await db.creditScore.create({
      data: {
        userId,
        bureau: validated.bureau,
        score: validated.score,
        recordedOn: new Date(validated.recordedOn),
      },
    });

    revalidatePath("/dashboard");
    revalidatePath("/credit-score");
    return { success: true, data: score };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to log credit score" };
  }
}

export async function deleteCreditScore(id: string) {
  try {
    const userId = await getUserId();

    const existing = await db.creditScore.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Score record not found or access denied" };
    }

    await db.creditScore.delete({
      where: { id },
    });

    revalidatePath("/dashboard");
    revalidatePath("/credit-score");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete credit score" };
  }
}
