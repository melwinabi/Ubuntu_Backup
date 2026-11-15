import { NextResponse } from "next/server"
import { verify } from 'otplib'
import prisma from "@/lib/prisma"

export async function POST(req: Request) {
  try {
    const { email, secret, token } = await req.json()

    if (!email || !secret || !token) {
      return NextResponse.json({ error: "Missing required fields" }, { status: 400 })
    }

    const isValid = verify({ token, secret })

    if (!isValid) {
      return NextResponse.json({ error: "Invalid TOTP code" }, { status: 400 })
    }

    // Save secret permanently
    await prisma.adminEmail.update({
      where: { email },
      data: { 
        // @ts-ignore
        totpSecret: secret 
      }
    })

    return NextResponse.json({ message: "TOTP enabled successfully" })
  } catch (error) {
    console.error("TOTP Verification Error:", error)
    return NextResponse.json({ error: "Failed to verify TOTP" }, { status: 500 })
  }
}
