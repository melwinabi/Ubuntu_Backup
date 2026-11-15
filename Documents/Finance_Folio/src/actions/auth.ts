"use server";

import { db } from "@/lib/db";
import { registerSchema } from "@/lib/validators";
import bcrypt from "bcryptjs";

const DEFAULT_CATEGORIES = [
  { name: "Food & Dining", icon: "Utensils" },
  { name: "Rent & Housing", icon: "Home" },
  { name: "Utilities & Bills", icon: "FileText" },
  { name: "Transport & Fuel", icon: "Car" },
  { name: "Entertainment & Leisure", icon: "Film" },
  { name: "Shopping", icon: "ShoppingBag" },
  { name: "Medical & Health", icon: "HeartPulse" },
  { name: "Travel", icon: "Plane" },
  { name: "Investments", icon: "TrendingUp" },
  { name: "Miscellaneous", icon: "HelpCircle" },
];

async function generateCustomId(): Promise<string> {
  const count = await db.user.count();
  let nextNum = count + 1;
  while (true) {
    const formattedId = `FGAT${String(nextNum).padStart(4, "0")}`;
    const existing = await db.user.findFirst({
      where: { customId: formattedId },
    });
    if (!existing) {
      return formattedId;
    }
    nextNum++;
  }
}

export async function registerUser(data: any) {
  try {
    const validated = registerSchema.parse(data);

    const existingUser = await db.user.findUnique({
      where: { email: validated.email.toLowerCase() },
    });

    if (existingUser) {
      return { success: false, error: "Email already exists" };
    }

    const hashedPassword = await bcrypt.hash(validated.password, 10);
    const customId = await generateCustomId();

    const user = await db.user.create({
      data: {
        fullName: validated.fullName,
        email: validated.email.toLowerCase(),
        passwordHash: hashedPassword,
        customId: customId,
      },
    });

    // Create default expense categories for the user
    await db.expenseCategory.createMany({
      data: DEFAULT_CATEGORIES.map((cat) => ({
        userId: user.id,
        name: cat.name,
        icon: cat.icon,
      })),
    });

    return { success: true, customId: user.customId };
  } catch (error: any) {
    console.error("Registration error:", error);
    return { success: false, error: error.message || "Something went wrong" };
  }
}
