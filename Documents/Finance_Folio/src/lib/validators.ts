import { z } from "zod";

// Authentication
export const registerSchema = z.object({
  fullName: z.string().min(2, "Name must be at least 2 characters"),
  email: z.string().email("Invalid email address"),
  password: z.string().min(6, "Password must be at least 6 characters"),
});

export const loginSchema = z.object({
  email: z.string().email("Invalid email address"),
  password: z.string().min(1, "Password is required"),
});

// Accounts
export const accountSchema = z.object({
  accountType: z.string().min(1, "Account Type is required"),
  accountName: z.string().min(2, "Name must be at least 2 characters"),
  providerName: z.string().optional().nullable(),
  accountNumberLast4: z.string().length(4, "Must be exactly 4 digits").or(z.string().length(0)).optional().nullable(),
  openingBalance: z.coerce.number().min(0, "Opening balance cannot be negative"),
});

// Cards
export const cardSchema = z.object({
  accountId: z.string().optional().nullable(),
  cardType: z.enum(["credit", "debit"]),
  cardName: z.string().min(2, "Card name is required"),
  last4: z.string().length(4, "Must be exactly 4 digits").or(z.string().length(0)).optional().nullable(),
  creditLimit: z.coerce.number().min(0).optional().nullable(),
  statementDay: z.coerce.number().min(1).max(31).optional().nullable(),
  dueDay: z.coerce.number().min(1).max(31).optional().nullable(),
  currentOutstanding: z.coerce.number().min(0).default(0),
});

// Credit Card Bills
export const creditCardBillSchema = z.object({
  cardId: z.string().min(1, "Card selection is required"),
  billingStart: z.string().min(1, "Start date is required"),
  billingEnd: z.string().min(1, "End date is required"),
  statementDate: z.string().min(1, "Statement date is required"),
  dueDate: z.string().min(1, "Due date is required"),
  totalAmount: z.coerce.number().min(0, "Amount must be positive"),
  minimumDue: z.coerce.number().min(0).optional().nullable(),
  paidAmount: z.coerce.number().min(0).default(0),
  status: z.enum(["unpaid", "partially_paid", "paid"]).default("unpaid"),
});

// Income Sources
export const incomeSourceSchema = z.object({
  sourceName: z.string().min(2, "Source name must be at least 2 characters"),
  sourceType: z.string().optional().nullable(),
  frequency: z.enum(["monthly", "weekly", "one_time", "yearly", "irregular"]).default("monthly"),
  expectedAmount: z.coerce.number().min(0).optional().nullable(),
  defaultAccountId: z.string().optional().nullable(),
});

// Income Transactions
export const incomeTransactionSchema = z.object({
  incomeSourceId: z.string().min(1, "Income source is required"),
  accountId: z.string().optional().nullable(),
  amount: z.coerce.number().min(0.01, "Amount must be greater than 0"),
  receivedOn: z.string().min(1, "Date is required"),
  notes: z.string().optional().nullable(),
});

// Expense Categories
export const expenseCategorySchema = z.object({
  name: z.string().min(1, "Category name is required"),
  icon: z.string().optional().nullable(),
});

// Daily Expenses
export const expenseSchema = z.object({
  categoryId: z.string().optional().nullable(),
  amount: z.coerce.number().min(0.01, "Amount must be greater than 0"),
  spentOn: z.string().min(1, "Date is required"),
  paymentMethod: z.enum(["cash", "upi", "credit_card", "debit_card", "net_banking", "cheque", "other"]),
  accountId: z.string().optional().nullable(),
  cardId: z.string().optional().nullable(),
  merchant: z.string().optional().nullable(),
  notes: z.string().optional().nullable(),
  savedAmount: z.coerce.number().min(0).optional().default(0),
});

// Investments
export const investmentSchema = z.object({
  assetType: z.enum(["stock", "gold", "silver", "mutual_fund", "crypto", "real_estate", "fixed_deposit", "other"]),
  name: z.string().min(1, "Investment name is required"),
  symbol: z.string().optional().nullable(),
  quantity: z.coerce.number().min(0).default(0),
  unit: z.string().default("unit"),
  purchasePrice: z.coerce.number().min(0).default(0),
  purchaseDate: z.string().min(1, "Purchase date is required"),
  accountId: z.string().optional().nullable(),
  subType: z.string().optional().nullable(),
  isSIP: z.boolean().default(false),
  sipFrequency: z.string().optional().nullable(),
  sipAmount: z.coerce.number().optional().nullable(),
  sipNextDate: z.string().optional().nullable(),
});

// Investment Price Update
export const investmentPriceSchema = z.object({
  investmentId: z.string().min(1, "Investment is required"),
  priceDate: z.string().min(1, "Date is required"),
  pricePerUnit: z.coerce.number().min(0.01, "Price must be positive"),
});

// Credit Score
export const creditScoreSchema = z.object({
  bureau: z.string().default("CIBIL"),
  score: z.coerce.number().min(300, "Score must be at least 300").max(900, "Score cannot exceed 900"),
  recordedOn: z.string().min(1, "Date is required"),
});

// Loans Given
export const loanGivenSchema = z.object({
  borrowerName: z.string().min(1, "Borrower name is required"),
  borrowerContact: z.string().optional().nullable(),
  principalAmount: z.coerce.number().min(0.01, "Principal amount must be positive"),
  amountGivenOn: z.string().min(1, "Date is required"),
  paymentMode: z.enum(["cash", "upi", "credit_card", "debit_card", "net_banking", "cheque", "other"]),
  accountId: z.string().optional().nullable(),
  interestRate: z.coerce.number().min(0).default(0),
  expectedReturnDate: z.string().optional().nullable(),
  notes: z.string().optional().nullable(),
});

// Loan Repayments
export const loanRepaymentSchema = z.object({
  loanId: z.string().min(1, "Loan selection is required"),
  amount: z.coerce.number().min(0.01, "Amount must be positive"),
  paidOn: z.string().min(1, "Date is required"),
  paymentMode: z.string().optional().nullable(),
  notes: z.string().optional().nullable(),
});
