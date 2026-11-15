import { NextResponse } from "next/server";
import { db } from "@/lib/db";

export async function GET() {
  try {
    const user = await db.user.findFirst();
    if (!user) {
      return NextResponse.json(
        { success: false, error: "No user found in the database." },
        { status: 404 }
      );
    }
    const accounts = await db.account.findMany({
      where: { userId: user.id },
      select: {
        id: true,
        accountName: true,
        accountType: true,
        currentBalance: true,
      },
      orderBy: { createdAt: "asc" },
    });
    return NextResponse.json({ success: true, accounts });
  } catch (error: any) {
    return NextResponse.json(
      { success: false, error: error.message || "Failed to fetch accounts" },
      { status: 500 }
    );
  }
}
