import { NextResponse } from "next/server";
import { db } from "@/lib/db";
import bcrypt from "bcryptjs";

export async function POST(req: Request) {
  try {
    const { userId, password } = await req.json();

    if (!userId || !password) {
      return NextResponse.json(
        { success: false, error: "User ID and password are required" },
        { status: 400 }
      );
    }

    const user = await db.user.findFirst({
      where: { customId: userId.trim().toUpperCase() },
    });

    if (!user) {
      return NextResponse.json(
        { success: false, error: "Invalid User ID or password" },
        { status: 401 }
      );
    }

    const isPasswordValid = await bcrypt.compare(password, user.passwordHash);
    if (!isPasswordValid) {
      return NextResponse.json(
        { success: false, error: "Invalid User ID or password" },
        { status: 401 }
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
      },
    });
  } catch (error: any) {
    return NextResponse.json(
      { success: false, error: error.message || "Authentication failed" },
      { status: 500 }
    );
  }
}
