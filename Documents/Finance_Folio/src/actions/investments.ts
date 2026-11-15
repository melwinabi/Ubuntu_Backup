"use server";

import { db } from "@/lib/db";
import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { investmentSchema, investmentPriceSchema } from "@/lib/validators";
import { revalidatePath } from "next/cache";
import { recalculateNetWorthSnapshot } from "@/lib/calculations";

async function getUserId() {
  const session = await getServerSession(authOptions);
  if (!session?.user?.id) {
    throw new Error("Unauthorized");
  }
  return session.user.id;
}

export async function getInvestments() {
  try {
    const userId = await getUserId();
    const investments = await db.investment.findMany({
      where: { userId, isActive: true },
      include: {
        account: true,
        prices: {
          orderBy: { priceDate: "desc" },
        },
        alerts: {
          orderBy: { alertDate: "asc" },
        },
        sipSkips: true,
      },
      orderBy: { purchaseDate: "desc" },
    });
    return { success: true, data: investments };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch investments" };
  }
}

export async function createInvestment(data: any) {
  try {
    const userId = await getUserId();
    const validated = investmentSchema.parse(data);

    // Validate account if specified
    if (validated.accountId) {
      const account = await db.account.findFirst({
        where: { id: validated.accountId, userId },
      });
      if (!account) {
        return { success: false, error: "Account not found or access denied" };
      }

      // Deduct cost from account
      const totalCost = (validated.isSIP ? (validated.sipAmount || 0) : (validated.purchasePrice * validated.quantity));
      if (totalCost > 0) {
        await db.account.update({
          where: { id: validated.accountId },
          data: {
            currentBalance: {
              decrement: totalCost,
            },
          },
        });
      }
    }

    const investment = await db.investment.create({
      data: {
        userId,
        assetType: validated.assetType,
        name: validated.name,
        symbol: validated.symbol || null,
        quantity: validated.quantity,
        unit: validated.unit,
        purchasePrice: validated.purchasePrice,
        purchaseDate: new Date(validated.purchaseDate),
        accountId: validated.accountId || null,
        subType: validated.subType || null,
        isSIP: validated.isSIP,
        sipFrequency: validated.sipFrequency || null,
        sipAmount: validated.sipAmount || null,
        sipNextDate: validated.sipNextDate ? new Date(validated.sipNextDate) : null,
      },
    });

    // Create an initial price history entry representing purchase price
    await db.investmentPriceHistory.create({
      data: {
        investmentId: investment.id,
        priceDate: new Date(validated.purchaseDate),
        pricePerUnit: validated.purchasePrice,
        source: "manual",
      },
    });

    // Automatically schedule alert if next SIP date is specified
    if (validated.sipNextDate) {
      await db.investmentAlert.create({
        data: {
          userId,
          investmentId: investment.id,
          title: `SIP Due: ${investment.name}`,
          alertDate: new Date(validated.sipNextDate),
          notes: `Automatic reminder for your ${validated.sipFrequency || "monthly"} SIP of ₹${validated.sipAmount || 0}.`,
        },
      });
    }

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/investments");
    revalidatePath("/networth");
    return { success: true, data: investment };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to log investment" };
  }
}

export async function addInvestmentPriceUpdate(data: any) {
  try {
    const userId = await getUserId();
    const validated = investmentPriceSchema.parse(data);

    const investment = await db.investment.findFirst({
      where: { id: validated.investmentId, userId },
    });
    if (!investment) {
      return { success: false, error: "Investment not found or access denied" };
    }

    const priceDate = new Date(validated.priceDate);
    priceDate.setHours(0, 0, 0, 0);

    const priceUpdate = await db.investmentPriceHistory.upsert({
      where: {
        investmentId_priceDate: {
          investmentId: validated.investmentId,
          priceDate,
        },
      },
      update: {
        pricePerUnit: validated.pricePerUnit,
      },
      create: {
        investmentId: validated.investmentId,
        priceDate,
        pricePerUnit: validated.pricePerUnit,
        source: "manual",
      },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath(`/investments`);
    revalidatePath(`/investments/${validated.investmentId}`);
    revalidatePath("/networth");
    return { success: true, data: priceUpdate };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to update investment price" };
  }
}

export async function deleteInvestment(id: string) {
  try {
    const userId = await getUserId();

    const existing = await db.investment.findFirst({
      where: { id, userId },
    });
    if (!existing) {
      return { success: false, error: "Investment not found or access denied" };
    }

    await db.investment.update({
      where: { id },
      data: { isActive: false },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/investments");
    revalidatePath("/networth");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete investment" };
  }
}

export async function createInvestmentAlert(data: {
  investmentId: string;
  title: string;
  alertDate: string;
  notes?: string;
}) {
  try {
    const userId = await getUserId();
    const alert = await db.investmentAlert.create({
      data: {
        userId,
        investmentId: data.investmentId,
        title: data.title,
        alertDate: new Date(data.alertDate),
        notes: data.notes || null,
      },
    });
    revalidatePath("/investments");
    return { success: true, data: alert };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to create alert" };
  }
}

export async function deleteInvestmentAlert(id: string) {
  try {
    const userId = await getUserId();
    const alert = await db.investmentAlert.findFirst({
      where: { id, userId },
    });
    if (!alert) {
      return { success: false, error: "Alert not found or access denied" };
    }
    await db.investmentAlert.delete({
        where: { id },
      });
      revalidatePath("/investments");
      return { success: true };
    } catch (error: any) {
      return { success: false, error: error.message || "Failed to delete alert" };
    }
  }

  export async function cancelInvestmentSip(id: string) {
    try {
      const userId = await getUserId();
      const existing = await db.investment.findFirst({
        where: { id, userId },
      });
      if (!existing) {
        return { success: false, error: "Investment not found or access denied" };
      }

      await db.investment.update({
        where: { id },
        data: {
          sipStatus: "cancelled",
          sipCancelDate: new Date(),
        },
      });

      await recalculateNetWorthSnapshot(userId);
      revalidatePath("/dashboard");
      revalidatePath("/investments");
      revalidatePath("/networth");
      return { success: true };
    } catch (error: any) {
      return { success: false, error: error.message || "Failed to cancel SIP" };
    }
  }

  export async function resumeInvestmentSip(id: string) {
    try {
      const userId = await getUserId();
      const existing = await db.investment.findFirst({
        where: { id, userId },
      });
      if (!existing) {
        return { success: false, error: "Investment not found or access denied" };
      }

      await db.investment.update({
        where: { id },
        data: {
          sipStatus: "active",
          sipCancelDate: null,
        },
      });

      await recalculateNetWorthSnapshot(userId);
      revalidatePath("/dashboard");
      revalidatePath("/investments");
      revalidatePath("/networth");
      return { success: true };
    } catch (error: any) {
      return { success: false, error: error.message || "Failed to resume SIP" };
    }
  }

  export async function toggleSipSkip(investmentId: string, skipDateStr: string) {
    try {
      const userId = await getUserId();
      const existing = await db.investment.findFirst({
        where: { id: investmentId, userId },
      });
      if (!existing) {
        return { success: false, error: "Investment not found or access denied" };
      }

      const skipDate = new Date(skipDateStr);
      skipDate.setHours(0, 0, 0, 0);

      const match = await db.investmentSipSkip.findFirst({
        where: { investmentId, skipDate },
      });

      if (match) {
        await db.investmentSipSkip.delete({
          where: { id: match.id },
        });
      } else {
        await db.investmentSipSkip.create({
          data: {
            investmentId,
            skipDate,
          },
        });
      }

      await recalculateNetWorthSnapshot(userId);
      revalidatePath("/dashboard");
      revalidatePath("/investments");
      revalidatePath("/networth");
      return { success: true };
    } catch (error: any) {
      return { success: false, error: error.message || "Failed to toggle SIP skip date" };
    }
  }

  export async function advanceSipNextDate(id: string) {
    try {
      const userId = await getUserId();
      const existing = await db.investment.findFirst({
        where: { id, userId },
      });
      if (!existing || !existing.sipNextDate || !existing.sipFrequency) {
        return { success: false, error: "Investment not found or not a SIP with next date" };
      }

      const nextDate = new Date(existing.sipNextDate);
      if (existing.sipFrequency === "weekly") {
        nextDate.setDate(nextDate.getDate() + 7);
      } else if (existing.sipFrequency === "monthly") {
        nextDate.setMonth(nextDate.getMonth() + 1);
      } else if (existing.sipFrequency === "half_yearly") {
        nextDate.setMonth(nextDate.getMonth() + 6);
      } else if (existing.sipFrequency === "yearly") {
        nextDate.setFullYear(nextDate.getFullYear() + 1);
      }

      await db.investment.update({
        where: { id },
        data: {
          sipNextDate: nextDate,
        },
      });

      revalidatePath("/investments");
      return { success: true };
    } catch (error: any) {
      return { success: false, error: error.message || "Failed to advance SIP next date" };
    }
  }

