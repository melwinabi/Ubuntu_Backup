import { NextResponse } from "next/server";
import { db } from "@/lib/db";
import { recalculateNetWorthSnapshot } from "@/lib/calculations";

export async function POST(req: Request) {
  try {
    const body = await req.json();
    const {
      type, // "EXPENSE" or "INCOME"
      amount,
      paymentMethod, // "cash" | "upi" | "credit_card" | "debit_card" | "net_banking" | "other"
      categoryName, // for expenses (e.g., "Food", "Travel")
      sourceName, // for income (e.g., "Salary", "Freelance")
      accountName, // name of account (e.g., "SBI", "Cash")
      cardName, // name of credit card if credit_card
      merchant,
      notes,
    } = body;

    if (!type || !amount) {
      return NextResponse.json(
        { success: false, error: "Type and Amount are required fields." },
        { status: 400 }
      );
    }

    // 1. Get the first user as a default user
    const user = await db.user.findFirst();
    if (!user) {
      return NextResponse.json(
        { success: false, error: "No user found in the database. Please register/create a user first." },
        { status: 404 }
      );
    }
    const userId = user.id;

    if (type.toUpperCase() === "EXPENSE") {
      // Find or create category
      let categoryId = null;
      if (categoryName) {
        const catNameTrimmed = categoryName.trim();
        let category = await db.expenseCategory.findFirst({
          where: { userId, name: catNameTrimmed },
        });
        if (!category) {
          category = await db.expenseCategory.create({
            data: { userId, name: catNameTrimmed, icon: "receipt" },
          });
        }
        categoryId = category.id;
      }

      // Handle payment method mapping
      let accountId = null;
      let cardId = null;

      if (paymentMethod === "credit_card") {
        // Find Card
        let card = null;
        if (cardName) {
          card = await db.card.findFirst({
            where: { userId, cardName: { contains: cardName } },
          });
        }
        if (!card) {
          card = await db.card.findFirst({ where: { userId } });
        }

        if (!card) {
          return NextResponse.json(
            { success: false, error: "Please configure a credit card in settings first." },
            { status: 400 }
          );
        }
        cardId = card.id;

        // Increment card outstanding
        await db.card.update({
          where: { id: cardId },
          data: { currentOutstanding: { increment: parseFloat(amount) } },
        });
      } else {
        // Find Account (SBI, HDFC, Cash, etc.)
        let account = null;
        if (accountName) {
          account = await db.account.findFirst({
            where: { userId, accountName: { contains: accountName } },
          });
        }
        if (!account) {
          account = await db.account.findFirst({ where: { userId } });
        }

        if (!account) {
          return NextResponse.json(
            { success: false, error: "Please configure a bank account in settings first." },
            { status: 400 }
          );
        }
        accountId = account.id;

        // Decrement account balance
        await db.account.update({
          where: { id: accountId },
          data: { currentBalance: { decrement: parseFloat(amount) } },
        });
      }

      // Create Expense
      const expense = await db.expense.create({
        data: {
          userId,
          categoryId,
          amount: parseFloat(amount),
          spentOn: new Date(),
          paymentMethod: paymentMethod || "cash",
          accountId: paymentMethod !== "credit_card" ? accountId : null,
          cardId: paymentMethod === "credit_card" ? cardId : null,
          merchant: merchant || null,
          notes: notes || null,
          savedAmount: 0.0,
        },
      });

      await recalculateNetWorthSnapshot(userId);
      return NextResponse.json({ success: true, type: "EXPENSE", data: expense });

    } else if (type.toUpperCase() === "INCOME") {
      // Find or create IncomeSource
      const finalSourceName = sourceName || "Salary";
      let source = await db.incomeSource.findFirst({
        where: { userId, sourceName: finalSourceName.trim() },
      });
      if (!source) {
        source = await db.incomeSource.create({
          data: {
            userId,
            sourceName: finalSourceName.trim(),
            sourceType: "salary",
            frequency: "monthly",
          },
        });
      }

      // Find Account
      let account = null;
      if (accountName) {
        account = await db.account.findFirst({
          where: { userId, accountName: { contains: accountName } },
        });
      }
      if (!account) {
        account = await db.account.findFirst({ where: { userId } });
      }

      if (!account) {
        return NextResponse.json(
          { success: false, error: "Please configure a bank account in settings first." },
          { status: 400 }
        );
      }
      const accountId = account.id;

      // Create IncomeTransaction
      const transaction = await db.incomeTransaction.create({
        data: {
          incomeSourceId: source.id,
          accountId,
          amount: parseFloat(amount),
          receivedOn: new Date(),
          notes: notes || null,
        },
      });

      // Increment account balance
      await db.account.update({
        where: { id: accountId },
        data: { currentBalance: { increment: parseFloat(amount) } },
      });

      await recalculateNetWorthSnapshot(userId);
      return NextResponse.json({ success: true, type: "INCOME", data: transaction });
    }

    return NextResponse.json(
      { success: false, error: "Invalid transaction type. Must be EXPENSE or INCOME." },
      { status: 400 }
    );

  } catch (error: any) {
    return NextResponse.json(
      { success: false, error: error.message || "Failed to process transaction API" },
      { status: 500 }
    );
  }
}

export async function GET() {
  try {
    const user = await db.user.findFirst();
    if (!user) {
      return NextResponse.json({
        success: true,
        expenses: [],
        incomes: [],
        loans: [],
        cards: [],
        investments: []
      });
    }
    const userId = user.id;

    const expenses = await db.expense.findMany({
      where: { userId },
      include: { category: true, account: true, card: true },
      orderBy: { spentOn: "desc" },
    });

    const incomes = await db.incomeTransaction.findMany({
      where: { incomeSource: { userId } },
      include: { account: true, incomeSource: true },
      orderBy: { receivedOn: "desc" },
    });

    const loans = await db.loanGiven.findMany({
      where: { userId },
      include: { account: true, repayments: true },
      orderBy: { amountGivenOn: "desc" },
    });

    const cards = await db.card.findMany({
      where: { userId },
      include: { account: true },
      orderBy: { cardName: "asc" },
    });

    const investments = await db.investment.findMany({
      where: { userId },
      include: { account: true, prices: { orderBy: { priceDate: "desc" }, take: 1 } },
      orderBy: { purchaseDate: "desc" },
    });

    return NextResponse.json({
      success: true,
      expenses,
      incomes,
      loans,
      cards,
      investments,
    });
  } catch (error: any) {
    return NextResponse.json(
      { success: false, error: error.message || "Failed to fetch transactions" },
      { status: 500 }
    );
  }
}


