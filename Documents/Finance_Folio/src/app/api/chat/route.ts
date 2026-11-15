import { NextResponse } from "next/server";
import { db } from "@/lib/db";
import { recalculateNetWorthSnapshot } from "@/lib/calculations";
import { GoogleGenerativeAI } from "@google/generative-ai";

// Simple in-memory storage for multi-user chat wizard stages
const globalForChat = global as unknown as {
  chatStates?: Map<string, {
    pendingAmount: number;
    pendingStage: "AWAITING_CATEGORY" | "AWAITING_ACCOUNT" | "AWAITING_INCOME_SOURCE" | "AWAITING_INCOME_ACCOUNT";
    pendingType: "EXPENSE" | "INCOME";
    pendingCategory?: string;
    pendingAccounts?: any[];
  }>
};

if (!globalForChat.chatStates) {
  globalForChat.chatStates = new Map();
}

const chatStates = globalForChat.chatStates;

export async function POST(req: Request) {
  try {
    const body = await req.json();
    const { message, userId: passedUserId } = body;
    const userMessage = (message || "").trim();

    if (!userMessage) {
      return NextResponse.json(
        { success: false, error: "Message is required" },
        { status: 400 }
      );
    }

    // 1. Identify User
    let userId = passedUserId;
    if (!userId) {
      const defaultUser = await db.user.findFirst();
      if (!defaultUser) {
        return NextResponse.json({
          success: true,
          reply: "❌ No registered user found in the database. Please open the web app to register first."
        });
      }
      userId = defaultUser.id;
    }

    // 2. Fetch User Accounts
    const userAccounts = await db.account.findMany({
      where: { userId, isActive: true },
      orderBy: { createdAt: "asc" },
    });

    // 3. Retrieve or Init User Chat State
    const userState = chatStates.get(userId);

    // ==========================================
    // WIZARD STAGE 1: Awaiting Expense Category
    // ==========================================
    if (userState && userState.pendingStage === "AWAITING_CATEGORY") {
      const categoryOptions = ["Food", "Rapido", "Snacks", "Tea", "Train", "Bus", "Other"];
      let matchedCat = categoryOptions.find(cat => cat.toLowerCase() === userMessage.toLowerCase());
      if (!matchedCat) {
        // Capitalize first letter of custom category
        matchedCat = userMessage.charAt(0).toUpperCase() + userMessage.slice(1);
      }

      if (userAccounts.length === 0) {
        chatStates.delete(userId);
        return NextResponse.json({
          success: true,
          reply: `Expense type set to **'${matchedCat}'**. But you have no registered bank accounts in the web application. Please add an account in Settings first.`
        });
      }

      // Update state for next step (Account Selection)
      chatStates.set(userId, {
        pendingAmount: userState.pendingAmount,
        pendingType: "EXPENSE",
        pendingStage: "AWAITING_ACCOUNT",
        pendingCategory: matchedCat,
        pendingAccounts: userAccounts,
      });

      const options = userAccounts.map(acc => acc.accountName);
      return NextResponse.json({
        success: true,
        reply: `Expense type set to **'${matchedCat}'**.\n\nWhich bank account did you pay this from? Please select from below:`,
        options
      });
    }

    // ==========================================
    // WIZARD STAGE 2: Awaiting Expense Account
    // ==========================================
    if (userState && userState.pendingStage === "AWAITING_ACCOUNT" && userState.pendingCategory) {
      const selectedAccount = userAccounts.find(acc => acc.accountName.toLowerCase() === userMessage.toLowerCase());
      
      if (selectedAccount) {
        const accName = selectedAccount.accountName;
        const amount = userState.pendingAmount;
        const categoryName = userState.pendingCategory;

        // Create Category if needed
        let categoryId = null;
        let category = await db.expenseCategory.findFirst({
          where: { userId, name: categoryName },
        });
        if (!category) {
          category = await db.expenseCategory.create({
            data: { userId, name: categoryName, icon: "receipt" },
          });
        }
        categoryId = category.id;

        // Decrement bank account balance
        await db.account.update({
          where: { id: selectedAccount.id },
          data: { currentBalance: { decrement: amount } },
        });

        // Create Expense
        await db.expense.create({
          data: {
            userId,
            categoryId,
            amount,
            spentOn: new Date(),
            paymentMethod: "cash",
            accountId: selectedAccount.id,
            notes: `Paid ₹${amount} for ${categoryName}`,
            savedAmount: 0.0,
          },
        });

        await recalculateNetWorthSnapshot(userId);
        chatStates.delete(userId); // Clear state

        return NextResponse.json({
          success: true,
          reply: `✅ Recorded Expense of ₹${amount.toFixed(2)} under '${categoryName}' category paid from: **${accName}**.`
        });
      } else {
        const options = userAccounts.map(acc => acc.accountName);
        return NextResponse.json({
          success: true,
          reply: `Invalid account: '${userMessage}'. Please select one of your registered bank accounts from below:`,
          options
        });
      }
    }

    // ==========================================
    // WIZARD STAGE 3: Awaiting Income Source
    // ==========================================
    if (userState && userState.pendingStage === "AWAITING_INCOME_SOURCE") {
      const sourceOptions = ["Salary", "Freelance", "Investment", "Gift", "Other"];
      let matchedSource = sourceOptions.find(src => src.toLowerCase() === userMessage.toLowerCase());
      if (!matchedSource) {
        matchedSource = userMessage.charAt(0).toUpperCase() + userMessage.slice(1);
      }

      if (userAccounts.length === 0) {
        chatStates.delete(userId);
        return NextResponse.json({
          success: true,
          reply: `Income source set to **'${matchedSource}'**. But you have no registered bank accounts in the web application. Please add an account in Settings first.`
        });
      }

      // Update state for next step (Income Account Selection)
      chatStates.set(userId, {
        pendingAmount: userState.pendingAmount,
        pendingType: "INCOME",
        pendingStage: "AWAITING_INCOME_ACCOUNT",
        pendingCategory: matchedSource,
        pendingAccounts: userAccounts,
      });

      const options = userAccounts.map(acc => acc.accountName);
      return NextResponse.json({
        success: true,
        reply: `Income source set to **'${matchedSource}'**.\n\nWhich bank account did you receive this in? Please select from below:`,
        options
      });
    }

    // ==========================================
    // WIZARD STAGE 4: Awaiting Income Account
    // ==========================================
    if (userState && userState.pendingStage === "AWAITING_INCOME_ACCOUNT" && userState.pendingCategory) {
      const selectedAccount = userAccounts.find(acc => acc.accountName.toLowerCase() === userMessage.toLowerCase());
      
      if (selectedAccount) {
        const accName = selectedAccount.accountName;
        const amount = userState.pendingAmount;
        const sourceName = userState.pendingCategory;

        // Create IncomeSource if needed
        let source = await db.incomeSource.findFirst({
          where: { userId, sourceName },
        });
        if (!source) {
          source = await db.incomeSource.create({
            data: {
              userId,
              sourceName,
              sourceType: "salary",
              frequency: "monthly",
            },
          });
        }

        // Create IncomeTransaction
        await db.incomeTransaction.create({
          data: {
            incomeSourceId: source.id,
            accountId: selectedAccount.id,
            amount,
            receivedOn: new Date(),
            notes: `Received ₹${amount} from ${sourceName}`,
          },
        });

        // Increment bank account balance
        await db.account.update({
          where: { id: selectedAccount.id },
          data: { currentBalance: { increment: amount } },
        });

        await recalculateNetWorthSnapshot(userId);
        chatStates.delete(userId); // Clear state

        return NextResponse.json({
          success: true,
          reply: `✅ Recorded Income of ₹${amount.toFixed(2)} from '${sourceName}' received in: **${accName}**.`
        });
      } else {
        const options = userAccounts.map(acc => acc.accountName);
        return NextResponse.json({
          success: true,
          reply: `Invalid account: '${userMessage}'. Please select one of your registered bank accounts from below:`,
          options
        });
      }
    }

    // ==========================================
    // TRIGGER 1: Check Expense Shortcut (e.g. p7000 or P4000)
    // ==========================================
    const pMatch = userMessage.match(/^[Pp]\s*(\d+(?:\.\d{1,2})?)$/);
    if (pMatch) {
      const amount = parseFloat(pMatch[1]);
      chatStates.set(userId, {
        pendingAmount: amount,
        pendingType: "EXPENSE",
        pendingStage: "AWAITING_CATEGORY",
      });

      return NextResponse.json({
        success: true,
        reply: `I've recorded a paid amount of **₹${amount.toFixed(2)}**.\n\nPlease select the Expense Type:`,
        options: ["Food", "Rapido", "Snacks", "Tea", "Train", "Bus", "Other"]
      });
    }

    // ==========================================
    // TRIGGER 2: Check Income Shortcut (e.g. e7000 or E4000)
    // ==========================================
    const eMatch = userMessage.match(/^[Ee]\s*(\d+(?:\.\d{1,2})?)$/);
    if (eMatch) {
      const amount = parseFloat(eMatch[1]);
      chatStates.set(userId, {
        pendingAmount: amount,
        pendingType: "INCOME",
        pendingStage: "AWAITING_INCOME_SOURCE",
      });

      return NextResponse.json({
        success: true,
        reply: `I've recorded an income amount of **₹${amount.toFixed(2)}**.\n\nPlease select the Income Source:`,
        options: ["Salary", "Freelance", "Investment", "Gift", "Other"]
      });
    }

    // ==========================================
    // TRIGGER 3: Gemini NLP Parsing / Local Rules
    // ==========================================
    const geminiKey = process.env.GEMINI_API_KEY;
    let parsedData: any = null;
    let replyMessage = "";

    if (geminiKey) {
      try {
        const genAI = new GoogleGenerativeAI(geminiKey);
        const model = genAI.getGenerativeModel({ model: "gemini-pro" });
        const prompt = `
          You are a financial helper assistant. Extract transaction details from this message: "${userMessage}".
          Respond ONLY with a raw JSON object and nothing else. No markdown formatting.
          The JSON object should have:
          - "type": "EXPENSE" or "INCOME" (if it looks like money was spent or earned)
          - "amount": the number value (float)
          - "paymentMethod": one of: "cash", "upi", "credit_card", "debit_card", "net_banking", "other"
          - "categoryName": the category of expense (e.g., "Food", "Travel", "Fuel", "Shopping", "Entertainment", "Medical", "Bills", "other")
          - "sourceName": the source of income (e.g., "Salary", "Freelance", "Gift")
          - "accountName": name of the bank or card if mentioned (e.g., "SBI", "HDFC", "Cash")
          - "cardName": name of the credit card if mentioned and payment method is credit_card
          - "merchant": the place or person the money was paid to
          - "notes": description of the transaction
          
          If the message is not a transaction logging request (like a normal greeting or query), return an empty JSON object {}.
        `;
        const result = await model.generateContent(prompt);
        let respText = result.response.text().trim();
        
        if (respText.startsWith("```")) {
          respText = respText.replace(/^```json\s*|```$/mg, "").trim();
        }
        
        parsedData = JSON.parse(respText);
      } catch (err) {
        console.error("Gemini AI parse error, falling back to Local Rules:", err);
      }
    }

    // Fallback parser if Gemini failed or is not available
    if (!parsedData || !parsedData.amount) {
      parsedData = parseMessageRules(userMessage);
    }

    // Save transaction if successfully parsed
    if (parsedData && parsedData.amount) {
      const type = parsedData.type;
      const amount = parsedData.amount;
      const paymentMethod = parsedData.paymentMethod || "cash";
      const notes = parsedData.notes || userMessage;

      if (type === "EXPENSE") {
        const categoryName = parsedData.categoryName || "Other";
        const accountName = parsedData.accountName;
        const cardName = parsedData.cardName;

        let categoryId = null;
        let category = await db.expenseCategory.findFirst({
          where: { userId, name: categoryName },
        });
        if (!category) {
          category = await db.expenseCategory.create({
            data: { userId, name: categoryName, icon: "receipt" },
          });
        }
        categoryId = category.id;

        let accountId = null;
        let cardId = null;

        if (paymentMethod === "credit_card") {
          let card = null;
          if (cardName) {
            card = await db.card.findFirst({
              where: { userId, cardName: { contains: cardName } },
            });
          }
          if (!card) {
            card = await db.card.findFirst({ where: { userId } });
          }

          if (card) {
            cardId = card.id;
            await db.card.update({
              where: { id: cardId },
              data: { currentOutstanding: { increment: amount } },
            });
          }
        } else {
          let account = null;
          if (accountName) {
            account = await db.account.findFirst({
              where: { userId, accountName: { contains: accountName } },
            });
          }
          if (!account) {
            account = await db.account.findFirst({ where: { userId } });
          }

          if (account) {
            accountId = account.id;
            await db.account.update({
              where: { id: accountId },
              data: { currentBalance: { decrement: amount } },
            });
          }
        }

        await db.expense.create({
          data: {
            userId,
            categoryId,
            amount,
            spentOn: new Date(),
            paymentMethod,
            accountId,
            cardId,
            merchant: parsedData.merchant || null,
            notes,
            savedAmount: 0.0,
          },
        });

        await recalculateNetWorthSnapshot(userId);
        replyMessage = `✅ Recorded Expense of ₹${amount.toFixed(2)} under '${categoryName}' category using ${paymentMethod}.`;
      } else {
        const sourceName = parsedData.sourceName || "Salary";
        const accountName = parsedData.accountName;

        let source = await db.incomeSource.findFirst({
          where: { userId, sourceName },
        });
        if (!source) {
          source = await db.incomeSource.create({
            data: {
              userId,
              sourceName,
              sourceType: "salary",
              frequency: "monthly",
            },
          });
        }

        let account = null;
        if (accountName) {
          account = await db.account.findFirst({
            where: { userId, accountName: { contains: accountName } },
          });
        }
        if (!account) {
          account = await db.account.findFirst({ where: { userId } });
        }

        if (account) {
          await db.incomeTransaction.create({
            data: {
              incomeSourceId: source.id,
              accountId: account.id,
              amount,
              receivedOn: new Date(),
              notes,
            },
          });

          await db.account.update({
            where: { id: account.id },
            data: { currentBalance: { increment: amount } },
          });
        }

        await recalculateNetWorthSnapshot(userId);
        replyMessage = `✅ Recorded Income of ₹${amount.toFixed(2)} from '${sourceName}'.`;
      }
    } else {
      replyMessage = 
        "Hello! I am your Finance Folio Chatbot.\n\n" +
        "I can automatically log your expenses and incomes. Try typing:\n" +
        "• 'Spent 450 for Food on SBI Card'\n" +
        "• 'Received 12000 salary from office'\n" +
        "• 'Expense of 120 for fuel with UPI'";
    }

    return NextResponse.json({
      success: true,
      reply: replyMessage
    });

  } catch (err: any) {
    return NextResponse.json(
      { success: false, error: err.message || "Chat processing failed" },
      { status: 500 }
    );
  }
}

function parseMessageRules(msg: string) {
  const msgLower = msg.toLowerCase();
  
  // 1. Match amount (e.g. 500, Rs. 500, ₹500, 500 rupees)
  const amtMatch = msgLower.match(/(?:rs\.?|rupees?|₹|\$)?\s*(\d+(?:\.\d{1,2})?)\s*(?:rs|rupees)?/);
  if (!amtMatch) return null;
  
  const amount = parseFloat(amtMatch[1]);
  
  // 2. Match transaction type
  const isIncome = ["received", "earned", "income", "salary", "bonus", "added", "credited"].some(word => msgLower.includes(word));
  const type = isIncome ? "INCOME" : "EXPENSE";
  
  // 3. Match payment method
  let paymentMethod = "cash";
  if (msgLower.includes("card") || msgLower.includes("credit")) {
    paymentMethod = "credit_card";
  } else if (["upi", "gpay", "phonepe", "paytm"].some(word => msgLower.includes(word))) {
    paymentMethod = "upi";
  } else if (msgLower.includes("bank") || msgLower.includes("net banking")) {
    paymentMethod = "net_banking";
  }
  
  // 4. Bank / Card Names
  let accountName = undefined;
  let cardName = undefined;
  const banks = ["sbi", "hdfc", "icici", "axis", "kotak"];
  for (const bank of banks) {
    if (msgLower.includes(bank)) {
      if (paymentMethod === "credit_card") {
        cardName = bank.toUpperCase();
      } else {
        accountName = bank.toUpperCase();
      }
    }
  }
      
  // 5. Extract category or source
  let categoryName = "Other";
  let sourceName = "Other";
  
  const forOnMatch = msgLower.match(/(?:on|for|from|to)\s+([a-zA-Z0-9\s]+)/);
  if (forOnMatch) {
    const extracted = forOnMatch[1].split(" ")[0];
    const capitalized = extracted.charAt(0).toUpperCase() + extracted.slice(1);
    if (!["sbi", "hdfc", "icici", "axis", "upi", "card", "cash"].includes(extracted.toLowerCase())) {
      if (type === "EXPENSE") {
        categoryName = capitalized;
      } else {
        sourceName = capitalized;
      }
    }
  }

  if (type === "INCOME") {
    if (msgLower.includes("salary")) {
      sourceName = "Salary";
    } else if (msgLower.includes("freelance")) {
      sourceName = "Freelance";
    }
  }

  const result: any = {
    type,
    amount,
    paymentMethod,
    notes: msg
  };
  
  if (type === "EXPENSE") {
    result.categoryName = categoryName;
    if (paymentMethod === "credit_card") {
      result.cardName = cardName;
    } else {
      result.accountName = accountName;
    }
  } else {
    result.sourceName = sourceName;
    result.accountName = accountName;
  }
      
  return result;
}
