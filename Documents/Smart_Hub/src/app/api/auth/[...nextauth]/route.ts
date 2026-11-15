import NextAuth from "next-auth"
import GoogleProvider from "next-auth/providers/google"
import CredentialsProvider from "next-auth/providers/credentials"
import prisma from "@/lib/prisma"
import { verify } from 'otplib'

export const authOptions = {
  providers: [
    GoogleProvider({
      clientId: process.env.GOOGLE_CLIENT_ID!,
      clientSecret: process.env.GOOGLE_CLIENT_SECRET!,
    }),
    CredentialsProvider({
      id: "user-dummy",
      name: "User Dummy",
      credentials: {
        email: { label: "Email", type: "email" },
      },
      async authorize(credentials) {
        if (credentials?.email === "asmelwin3@gmail.com") {
          const user = await (prisma as any).user.upsert({
            where: { email: credentials.email },
            update: {},
            create: { email: credentials.email },
          })
          return { id: user.id.toString(), email: user.email, role: "user", name: "Melwin User" }
        }
        return null
      }
    }),
    CredentialsProvider({
      id: "admin-otp",
      name: "Admin OTP",
      credentials: {
        email: { label: "Email", type: "email" },
        otp: { label: "OTP", type: "text" },
      },
      async authorize(credentials) {
        console.log("Authorize attempt with:", credentials)
        if (!credentials?.email || !credentials?.otp) {
          console.log("Missing credentials")
          return null
        }

        const email = credentials.email.trim().toLowerCase()
        const otp = credentials.otp.trim()

        // Dummy Login for testing
        if (email === "asmelwin3@gmail.com" && otp === "123456") {
          console.log("Bypass triggered for asmelwin3@gmail.com")
          return { id: "999", email: "asmelwin3@gmail.com", role: "admin", name: "Melwin Admin" }
        }

        // Check if admin email exists
        const admin = await prisma.adminEmail.findUnique({
          where: { email },
        })

        if (!admin) {
          console.log("Admin not found in DB")
          return null
        }

        // If TOTP is set up, verify with TOTP
        // @ts-ignore
        if (admin.totpSecret) {
          console.log("Verifying with TOTP")
          // @ts-ignore
          const isValid = verify({ token: otp, secret: admin.totpSecret })
          if (!isValid) {
            console.log("Invalid TOTP")
            return null
          }
        } else {
          // Fallback to Email OTP
          console.log("Verifying with Email OTP")
          const verification = await prisma.otpVerification.findFirst({
            where: {
              email: email,
              otp: otp,
              expiresAt: { gt: new Date() },
            },
          })

          if (!verification) {
            console.log("Invalid or expired Email OTP")
            return null
          }

          // Delete the used OTP
          await prisma.otpVerification.delete({ where: { id: verification.id } })
        }

        return { id: admin.id.toString(), email: admin.email, role: "admin" }
      },
    }),
  ],
  callbacks: {
    async signIn({ user, account, profile }: any) {
      if (account?.provider === "google" && user.email) {
        try {
          const dbUser = await (prisma as any).user.upsert({
            where: { email: user.email },
            update: {},
            create: { email: user.email },
          })
          user.id = dbUser.id.toString()
        } catch (error) {
          console.error("Failed to sync user to DB:", error)
        }
      }
      return true
    },
    async jwt({ token, user, account }: any) {
      if (user) {
        token.role = user.role || "user"
        token.userId = user.id.toString()
      }
      return token
    },
    async session({ session, token }: any) {
      if (session.user) {
        session.user.role = token.role as string
        session.user.id = token.userId as string
      }
      return session
    },
  },
  pages: {
    signIn: "/",
  },
  debug: true,
}

const handler = NextAuth(authOptions)
export { handler as GET, handler as POST }
