"use server";

import { db } from "@/lib/db";
import { getServerSession } from "next-auth";
import { authOptions } from "@/lib/auth";
import { loanGivenSchema, loanRepaymentSchema } from "@/lib/validators";
import { revalidatePath } from "next/cache";
import { recalculateNetWorthSnapshot } from "@/lib/calculations";

async function getUserId() {
  const session = await getServerSession(authOptions);
  if (!session?.user?.id) {
    throw new Error("Unauthorized");
  }
  return session.user.id;
}

// LOANS GIVEN CRUD
export async function getLoans() {
  try {
    const userId = await getUserId();
    const loans = await db.loanGiven.findMany({
      where: { userId },
      include: {
        account: true,
        repayments: true,
      },
      orderBy: { amountGivenOn: "desc" },
    });
    return { success: true, data: loans };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch loans" };
  }
}

export async function createLoanGiven(data: any) {
  try {
    const userId = await getUserId();
    const validated = loanGivenSchema.parse(data);

    // Validate account if specified
    if (validated.accountId) {
      const account = await db.account.findFirst({
        where: { id: validated.accountId, userId },
      });
      if (!account) {
        return { success: false, error: "Account not found or access denied" };
      }

      // Deduct principal from account (since user is giving this cash)
      await db.account.update({
        where: { id: validated.accountId },
        data: {
          currentBalance: {
            decrement: validated.principalAmount,
          },
        },
      });
    }

    const loan = await db.loanGiven.create({
      data: {
        userId,
        borrowerName: validated.borrowerName,
        borrowerContact: validated.borrowerContact || null,
        principalAmount: validated.principalAmount,
        amountGivenOn: new Date(validated.amountGivenOn),
        paymentMode: validated.paymentMode,
        accountId: validated.accountId || null,
        interestRate: validated.interestRate,
        expectedReturnDate: validated.expectedReturnDate ? new Date(validated.expectedReturnDate) : null,
        status: "active",
        notes: validated.notes || null,
      },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/loans");
    revalidatePath("/networth");
    return { success: true, data: loan };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to log loan" };
  }
}

export async function deleteLoan(id: string) {
  try {
    const userId = await getUserId();

    const loan = await db.loanGiven.findFirst({
      where: { id, userId },
    });
    if (!loan) {
      return { success: false, error: "Loan not found or access denied" };
    }

    // Optional: Return principal back to account on deletion?
    // Usually deletion is just clean up, but let's reverse the balance shift if the user wants.
    // However, to keep it simple, we just delete the loan.
    await db.loanGiven.delete({
      where: { id },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/loans");
    revalidatePath("/networth");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete loan" };
  }
}

// REPAYMENTS CRUD
export async function getLoanRepayments(loanId: string) {
  try {
    const userId = await getUserId();
    const loan = await db.loanGiven.findFirst({
      where: { id: loanId, userId },
    });
    if (!loan) {
      return { success: false, error: "Loan not found or access denied" };
    }

    const repayments = await db.loanRepayment.findMany({
      where: { loanId },
      orderBy: { paidOn: "desc" },
    });
    return { success: true, data: repayments };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to fetch repayments" };
  }
}

export async function createLoanRepayment(data: any) {
  try {
    const userId = await getUserId();
    const validated = loanRepaymentSchema.parse(data);

    const loan = await db.loanGiven.findFirst({
      where: { id: validated.loanId, userId },
      include: { repayments: true },
    });
    if (!loan) {
      return { success: false, error: "Loan not found or access denied" };
    }

    const repayment = await db.loanRepayment.create({
      data: {
        loanId: validated.loanId,
        amount: validated.amount,
        paidOn: new Date(validated.paidOn),
        paymentMode: validated.paymentMode || null,
        notes: validated.notes || null,
      },
    });

    // Credit original settlement account if exists
    if (loan.accountId) {
      await db.account.update({
        where: { id: loan.accountId },
        data: {
          currentBalance: {
            increment: validated.amount,
          },
        },
      });
    }

    // Calculate new status
    const totalRepayments = loan.repayments.reduce((sum, r) => sum + r.amount, 0) + validated.amount;
    let newStatus = "active";
    if (totalRepayments >= loan.principalAmount) {
      newStatus = "closed";
    } else if (totalRepayments > 0) {
      newStatus = "partially_paid";
    }

    await db.loanGiven.update({
      where: { id: validated.loanId },
      data: { status: newStatus },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/loans");
    revalidatePath("/networth");
    return { success: true, data: repayment };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to log repayment" };
  }
}

export async function deleteLoanRepayment(id: string) {
  try {
    const userId = await getUserId();

    const repayment = await db.loanRepayment.findUnique({
      where: { id },
      include: {
        loan: {
          include: {
            repayments: true,
          },
        },
      },
    });

    if (!repayment || repayment.loan.userId !== userId) {
      return { success: false, error: "Repayment not found or access denied" };
    }

    // Deduct repayment amount from account
    if (repayment.loan.accountId) {
      await db.account.update({
        where: { id: repayment.loan.accountId },
        data: {
          currentBalance: {
            decrement: repayment.amount,
          },
        },
      });
    }

    await db.loanRepayment.delete({
      where: { id },
    });

    // Recalculate loan status
    const remainingRepayments = repayment.loan.repayments.filter((r) => r.id !== id);
    const totalRepayments = remainingRepayments.reduce((sum, r) => sum + r.amount, 0);
    let newStatus = "active";
    if (totalRepayments >= repayment.loan.principalAmount) {
      newStatus = "closed";
    } else if (totalRepayments > 0) {
      newStatus = "partially_paid";
    }

    await db.loanGiven.update({
      where: { id: repayment.loanId },
      data: { status: newStatus },
    });

    await recalculateNetWorthSnapshot(userId);
    revalidatePath("/dashboard");
    revalidatePath("/loans");
    revalidatePath("/networth");
    return { success: true };
  } catch (error: any) {
    return { success: false, error: error.message || "Failed to delete repayment" };
  }
}
