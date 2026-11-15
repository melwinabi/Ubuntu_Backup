"use server";

import { db } from "@/lib/db";
import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import bcrypt from "bcryptjs";
import { revalidatePath } from "next/cache";

async function getUserId() {
  const session = await getServerSession(authOptions);
  if (!session?.user?.id) {
    throw new Error("Unauthorized");
  }
  return session.user.id;
}

export async function updateProfile(data: { fullName: string, dob?: string | null, profilePic?: string | null }) {
  try {
    const userId = await getUserId();
    
    if (!data.fullName || data.fullName.trim() === "") {
      return { success: false, error: "Name cannot be empty" };
    }

    const updatedUser = await db.user.update({
      where: { id: userId },
      data: { 
        fullName: data.fullName,
        dob: data.dob ? new Date(data.dob) : null,
        profilePic: data.profilePic || null,
      },
    });

    revalidatePath("/settings");
    return { 
      success: true, 
      data: { 
        name: updatedUser.fullName,
        dob: updatedUser.dob,
        profilePic: updatedUser.profilePic,
      } 
    };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to update profile" };
  }
}

export async function changePassword(data: { current: string, newPass: string }) {
  try {
    const userId = await getUserId();
    
    if (!data.current || !data.newPass || data.newPass.length < 6) {
      return { success: false, error: "New password must be at least 6 characters" };
    }

    const user = await db.user.findUnique({ where: { id: userId } });
    if (!user) return { success: false, error: "User not found" };

    const isPasswordValid = await bcrypt.compare(data.current, user.passwordHash);
    if (!isPasswordValid) {
      return { success: false, error: "Current password is incorrect" };
    }

    const newHashedPassword = await bcrypt.hash(data.newPass, 10);
    
    await db.user.update({
      where: { id: userId },
      data: { passwordHash: newHashedPassword },
    });

    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to change password" };
  }
}

export async function resetAllStatistics(password: string) {
  try {
    const userId = await getUserId();
    
    if (!password) {
      return { success: false, error: "Password is required to confirm reset" };
    }

    const user = await db.user.findUnique({ where: { id: userId } });
    if (!user) return { success: false, error: "User not found" };

    const isPasswordValid = await bcrypt.compare(password, user.passwordHash);
    if (!isPasswordValid) {
      return { success: false, error: "Incorrect password" };
    }
    
    // 1. Delete all transactions (Expenses, Incomes, Loan Repayments, Bills)
    await db.expense.deleteMany({ where: { userId } });
    await db.incomeTransaction.deleteMany({ where: { incomeSource: { userId } } });
    await db.loanRepayment.deleteMany({ where: { loan: { userId } } });
    await db.creditCardBill.deleteMany({ where: { card: { userId } } });
    
    // 2. Delete all records
    await db.investmentPriceHistory.deleteMany({ where: { investment: { userId } } });
    await db.investmentAlert.deleteMany({ where: { userId } });
    await db.investmentSipSkip.deleteMany({ where: { investment: { userId } } });
    await db.investment.deleteMany({ where: { userId } });
    await db.loanGiven.deleteMany({ where: { userId } });
    await db.creditScore.deleteMany({ where: { userId } });
    await db.netWorthSnapshot.deleteMany({ where: { userId } });
    
    // 3. Reset balances
    await db.account.updateMany({
      where: { userId },
      data: { currentBalance: 0, openingBalance: 0 }
    });
    
    await db.card.updateMany({
      where: { userId },
      data: { currentOutstanding: 0 }
    });

    revalidatePath("/dashboard");
    revalidatePath("/expenses");
    revalidatePath("/networth");
    revalidatePath("/investments");
    revalidatePath("/loans");
    revalidatePath("/credit-cards");
    revalidatePath("/settings");

    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to reset statistics" };
  }
}
