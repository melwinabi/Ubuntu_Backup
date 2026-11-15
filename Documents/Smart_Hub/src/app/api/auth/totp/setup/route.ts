import { NextResponse } from "next/server"
import { generateSecret, generateURI } from 'otplib'
import QRCode from "qrcode"
import prisma from "@/lib/prisma"

export async function GET(req: Request) {
  try {
    const { searchParams } = new URL(req.url)
    const email = searchParams.get("email")

    if (!email) {
      return NextResponse.json({ error: "Email is required" }, { status: 400 })
    }

    const admin = await prisma.adminEmail.findUnique({
      where: { email }
    })

    // @ts-ignore
    return NextResponse.json({ isSet: !!admin?.totpSecret })
  } catch (error) {
    return NextResponse.json({ isSet: false })
  }
}

export async function POST(req: Request) {
  try {
    const { email } = await req.json()

    if (!email) {
      return NextResponse.json({ error: "Email is required" }, { status: 400 })
    }

    const admin = await prisma.adminEmail.findUnique({
      where: { email }
    })

    if (!admin) {
      return NextResponse.json({ error: "Admin not found" }, { status: 404 })
    }

    // Generate secret
    const secret = generateSecret()
    const otpauth = generateURI({
      secret,
      label: email,
      issuer: "Smaer-HUB"
    })
    const qrCodeUrl = await QRCode.toDataURL(otpauth)

    return NextResponse.json({ secret, qrCodeUrl })
  } catch (error) {
    console.error("TOTP Setup Error:", error)
    return NextResponse.json({ error: "Failed to setup TOTP" }, { status: 500 })
  }
}
