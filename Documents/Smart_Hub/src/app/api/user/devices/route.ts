import { NextRequest, NextResponse } from "next/server"
import prisma from "@/lib/prisma"
import { getServerSession } from "next-auth"
import { authOptions } from "@/app/api/auth/[...nextauth]/route"

export async function GET(request: NextRequest) {
  const session = await getServerSession(authOptions)
  if (!session || !session.user?.id) return NextResponse.json({ error: "Unauthorized" }, { status: 401 })

  try {
    const rawId = session.user.id
    // Robustly extract digits from the ID
    const match = rawId.match(/\d+/)
    const parsedId = match ? parseInt(match[0]) : NaN
    
    if (isNaN(parsedId)) {
      console.error("Invalid userId format:", rawId)
      return NextResponse.json({ error: "Invalid user ID format" }, { status: 400 })
    }

    const userId = parsedId
    const userDevices = await (prisma as any).userDevice.findMany({
      where: { userId: Number(userId) },
      include: {
        device: {
          include: {
            widgets: true
          }
        }
      }
    })

    const devices = userDevices.map((ud: any) => ud.device)
    return NextResponse.json(devices)
  } catch (error) {
    console.error("Failed to fetch user devices:", error)
    return NextResponse.json({ error: "Failed to fetch devices" }, { status: 500 })
  }
}

export async function POST(request: NextRequest) {
  const session = await getServerSession(authOptions)
  if (!session || !session.user?.id) return NextResponse.json({ error: "Unauthorized" }, { status: 401 })

  try {
    const { deviceId } = await request.json()
    if (!deviceId) return NextResponse.json({ error: "Missing device ID" }, { status: 400 })

    const rawId = session.user.id
    const match = rawId.match(/\d+/)
    const parsedId = match ? parseInt(match[0]) : NaN
    
    if (isNaN(parsedId)) {
      return NextResponse.json({ error: "Invalid user ID format" }, { status: 400 })
    }
    const userId = parsedId
    
    // Check if link already exists
    const existing = await (prisma as any).userDevice.findUnique({
      where: {
        userId_deviceId: { 
          userId: Number(userId), 
          deviceId: Number(deviceId) 
        }
      }
    })

    if (existing) {
      return NextResponse.json({ message: "Device already added" })
    }

    const userDevice = await (prisma as any).userDevice.create({
      data: {
        userId,
        deviceId: parseInt(deviceId)
      }
    })

    console.log("Device linked successfully")
    return NextResponse.json(userDevice)
  } catch (error: any) {
    console.error("Failed to add device to user:", error)
    return NextResponse.json({ error: "Failed to add device", details: error.message }, { status: 500 })
  }
}

export async function DELETE(request: NextRequest) {
  const session = await getServerSession(authOptions)
  if (!session || !session.user?.id) return NextResponse.json({ error: "Unauthorized" }, { status: 401 })

  try {
    const { searchParams } = new URL(request.url)
    const deviceId = searchParams.get("deviceId")
    if (!deviceId) return NextResponse.json({ error: "Missing device ID" }, { status: 400 })

    const rawId = session.user.id
    const match = rawId.match(/\d+/)
    const parsedId = match ? parseInt(match[0]) : NaN
    
    if (isNaN(parsedId)) {
      return NextResponse.json({ error: "Invalid user ID format" }, { status: 400 })
    }
    const userId = parsedId
    
    try {
      await (prisma as any).userDevice.delete({
        where: {
          userId_deviceId: { 
            userId: Number(userId), 
            deviceId: Number(deviceId) 
          }
        }
      })
    } catch (dbError: any) {
      console.error("Prisma DELETE error:", dbError)
      throw dbError
    }

    return NextResponse.json({ message: "Device removed from dashboard" })
  } catch (error: any) {
    console.error("Failed to remove device from user:", error)
    return NextResponse.json({ error: "Failed to remove device", details: error.message }, { status: 500 })
  }
}
