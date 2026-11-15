import { NextResponse } from "next/server";
import { db } from "@/lib/db";
import bcrypt from "bcryptjs";

// GET profile and accounts
export async function GET(req: Request) {
  try {
    const { searchParams } = new URL(req.url);
    const userId = searchParams.get("userId");

    if (!userId) {
      return NextResponse.json(
        { success: false, error: "User ID is required" },
        { status: 400 }
      );
    }

    const user = await db.user.findUnique({
      where: { id: userId },
      include: {
        accounts: {
          orderBy: { createdAt: "asc" },
        },
      },
    });

    if (!user) {
      return NextResponse.json(
        { success: false, error: "User not found" },
        { status: 404 }
      );
    }

    return NextResponse.json({
      success: true,
      user: {
        id: user.id,
        fullName: user.fullName,
        email: user.email,
        customId: user.customId,
        dob: user.dob,
        profilePic: user.profilePic,
        accounts: user.accounts,
      },
    });
  } catch (error: any) {
    return NextResponse.json(
      { success: false, error: error.message || "Failed to fetch profile" },
      { status: 500 }
    );
  }
}

// POST updates to profile or bank accounts
export async function POST(req: Request) {
  try {
    const body = await req.json();
    const { userId, action } = body;

    if (!userId) {
      return NextResponse.json(
        { success: false, error: "User ID is required" },
        { status: 400 }
      );
    }

    const user = await db.user.findUnique({ where: { id: userId } });
    if (!user) {
      return NextResponse.json(
        { success: false, error: "User not found" },
        { status: 404 }
      );
    }

    // Handle Profile updates
    if (action === "update_profile") {
      const { fullName, dob, profilePic } = body;
      const updateData: any = {};
      if (fullName) updateData.fullName = fullName;
      if (dob) updateData.dob = new Date(dob);
      if (profilePic !== undefined) updateData.profilePic = profilePic;

      const updated = await db.user.update({
        where: { id: userId },
        data: updateData,
      });

      return NextResponse.json({
        success: true,
        message: "Profile updated successfully",
        user: updated,
      });
    }

    // Handle Password change
    if (action === "change_password") {
      const { currentPassword, newPassword } = body;
      if (!currentPassword || !newPassword) {
        return NextResponse.json(
          { success: false, error: "Current and new password are required" },
          { status: 400 }
        );
      }

      const isPasswordValid = await bcrypt.compare(currentPassword, user.passwordHash);
      if (!isPasswordValid) {
        return NextResponse.json(
          { success: false, error: "Current password is incorrect" },
          { status: 400 }
        );
      }

      const hashedNew = await bcrypt.hash(newPassword, 10);
      await db.user.update({
        where: { id: userId },
        data: { passwordHash: hashedNew },
      });

      return NextResponse.json({
        success: true,
        message: "Password changed successfully",
      });
    }

    // Handle Bank Account Save (Create / Update)
    if (action === "save_account") {
      const { accountId, accountName, accountType, currentBalance, providerName, accountNumberLast4 } = body;
      if (!accountName || !accountType) {
        return NextResponse.json(
          { success: false, error: "Account Name and Type are required" },
          { status: 400 }
        );
      }

      if (accountId) {
        // Update
        const updatedAcc = await db.account.update({
          where: { id: accountId },
          data: {
            accountName,
            accountType,
            currentBalance: currentBalance ? parseFloat(currentBalance) : 0.0,
            providerName,
            accountNumberLast4,
          },
        });
        return NextResponse.json({ success: true, message: "Account updated", account: updatedAcc });
      } else {
        // Create
        const newAcc = await db.account.create({
          data: {
            userId,
            accountName,
            accountType,
            currentBalance: currentBalance ? parseFloat(currentBalance) : 0.0,
            providerName,
            accountNumberLast4,
            openingBalance: currentBalance ? parseFloat(currentBalance) : 0.0,
          },
        });
        return NextResponse.json({ success: true, message: "Account created", account: newAcc });
      }
    }

    // Handle Bank Account Delete
    if (action === "delete_account") {
      const { accountId } = body;
      if (!accountId) {
        return NextResponse.json(
          { success: false, error: "Account ID is required" },
          { status: 400 }
        );
      }

      await db.account.delete({
        where: { id: accountId },
      });

      return NextResponse.json({
        success: true,
        message: "Account deleted successfully",
      });
    }

    // Handle Reset Statistics (Hard Reset)
    if (action === "reset_statistics") {
      const { password } = body;
      if (!password) {
        return NextResponse.json(
          { success: false, error: "Password is required to reset statistics" },
          { status: 400 }
        );
      }

      const isPasswordValid = await bcrypt.compare(password, user.passwordHash);
      if (!isPasswordValid) {
        return NextResponse.json(
          { success: false, error: "Incorrect password" },
          { status: 400 }
        );
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

      return NextResponse.json({
        success: true,
        message: "All statistics have been reset successfully",
      });
    }

    return NextResponse.json(
      { success: false, error: "Invalid action" },
      { status: 400 }
    );
  } catch (error: any) {
    return NextResponse.json(
      { success: false, error: error.message || "Failed to update profile" },
      { status: 500 }
    );
  }
}
