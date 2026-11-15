import { NextResponse } from "next/server"
import prisma from "@/lib/prisma"
import nodemailer from "nodemailer"

export async function POST(req: Request) {
  try {
    const { email } = await req.json()

    if (!email) {
      return NextResponse.json({ error: "Email is required" }, { status: 400 })
    }

    // Bypass for dummy admin
    if (email === "asmelwin3@gmail.com") {
      return NextResponse.json({ message: "OTP sent successfully (Bypass Code: 123456)" })
    }

    // Check if admin exists
    const admin = await prisma.adminEmail.findUnique({
      where: { email },
    })

    if (!admin) {
      return NextResponse.json({ error: "Unauthorized admin email" }, { status: 403 })
    }

    // Generate 6-digit OTP
    const otp = Math.floor(100000 + Math.random() * 900000).toString()
    const expiresAt = new Date(Date.now() + 5 * 60 * 1000) // 5 minutes

    // Store OTP
    await prisma.otpVerification.create({
      data: {
        email,
        otp,
        expiresAt,
      },
    })

    // Log OTP for debug (in case email fails)
    console.log(`[OTP DEBUG] OTP for ${email}: ${otp}`)

    // Send email
    try {
      const transporter = nodemailer.createTransport({
        host: process.env.SMTP_HOST,
        port: Number(process.env.SMTP_PORT),
        auth: {
          user: process.env.SMTP_USER,
          pass: process.env.SMTP_PASS,
        },
      })

      await transporter.sendMail({
        from: process.env.SMTP_FROM,
        to: email,
        subject: "Smart-HUB Admin OTP",
        text: `Your 6-digit OTP for admin login is: ${otp}. It will expire in 5 minutes.`,
        html: `
          <div style="font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; color: #333;">
            <h2 style="color: #4f46e5;">Smart-HUB</h2>
            <p>You requested an admin login OTP. Please use the code below:</p>
            <div style="background: #f3f4f6; padding: 20px; border-radius: 8px; text-align: center; font-size: 24px; font-weight: bold; letter-spacing: 4px; color: #111827;">
              ${otp}
            </div>
            <p style="font-size: 14px; color: #6b7280; margin-top: 20px;">
              This OTP will expire in 5 minutes. If you did not request this, please ignore this email.
            </p>
          </div>
        `,
      })
    } catch (mailError) {
      console.error("Mail Send Error (falling back to console log):", mailError)
      // We still return success if the OTP was generated and saved, 
      // so the user can use the console logged OTP for testing.
    }

    return NextResponse.json({ message: "OTP process completed" })
  } catch (error) {
    console.error("OTP Error:", error)
    return NextResponse.json({ error: "Failed to process OTP" }, { status: 500 })
  }
}
