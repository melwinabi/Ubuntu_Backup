import { NextRequest, NextResponse } from "next/server"
import prisma from "@/lib/prisma"
import { getServerSession } from "next-auth"
import { authOptions } from "@/app/api/auth/[...nextauth]/route"

export async function GET(request: NextRequest) {
  const searchParams = request.nextUrl.searchParams
  const seriesId = searchParams.get("seriesId")
  const id = searchParams.get("id")

  try {
    if (id) {
      const device = await prisma.device.findUnique({
        where: { id: parseInt(id) },
        include: { 
          widgets: true,
          subSeries: true,
          series: true
        },
      })
      if (!device) return NextResponse.json({ error: "Device not found" }, { status: 404 })
      return NextResponse.json(device)
    }

    if (seriesId) {
      const devices = await prisma.device.findMany({
        where: { seriesId: parseInt(seriesId) },
        select: { id: true, deviceName: true },
      })
      return NextResponse.json(devices)
    }

    return NextResponse.json({ error: "Missing query parameters" }, { status: 400 })
  } catch (error) {
    console.error("Failed to fetch devices:", error)
    return NextResponse.json({ error: "Failed to fetch devices" }, { status: 500 })
  }
}

export async function POST(request: NextRequest) {
  const session = await getServerSession(authOptions)
  
  if (!session || (session.user as any)?.role !== "admin") {
    return NextResponse.json({ error: "Unauthorized" }, { status: 401 })
  }

  let body: any
  try {
    body = await request.json()
    const { 
      seriesId, 
      subSeriesId, 
      deviceId, 
      deviceName, 
      serverId, 
      mqttTopic, 
      widgets 
    } = body

    // Check if device ID already exists
    const existing = await prisma.device.findUnique({ where: { id: parseInt(deviceId) } })
    if (existing) {
      return NextResponse.json({ error: "Device ID already exists" }, { status: 400 })
    }

    const device = await prisma.device.create({
      data: {
        id: parseInt(deviceId),
        seriesId: parseInt(seriesId),
        subSeriesId: parseInt(subSeriesId),
        deviceName,
        serverId,
        mqttTopic,
        widgets: {
          create: widgets.map((w: any) => ({
            widgetName: w.name,
            mqttTopic: w.topic,
          })),
        },
      },
    })

    return NextResponse.json(device)
  } catch (error: any) {
    console.error("Failed to create device:", error)
    return NextResponse.json({ error: error.message || "Failed to create device" }, { status: 500 })
  }
}
