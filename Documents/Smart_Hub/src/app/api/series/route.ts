import { NextResponse } from "next/server"
import prisma from "@/lib/prisma"

export async function GET() {
  try {
    const series = await prisma.deviceSeries.findMany({
      orderBy: { seriesName: "asc" },
    })
    return NextResponse.json(series)
  } catch (error) {
    console.error("Failed to fetch series:", error)
    return NextResponse.json({ error: "Failed to fetch series" }, { status: 500 })
  }
}
