import { NextRequest, NextResponse } from "next/server"
import prisma from "@/lib/prisma"

export async function GET(request: NextRequest) {
  const searchParams = request.nextUrl.searchParams
  const seriesId = searchParams.get("seriesId")

  if (!seriesId) {
    return NextResponse.json({ error: "seriesId is required" }, { status: 400 })
  }

  try {
    const subSeries = await prisma.deviceSubSeries.findMany({
      where: { seriesId: parseInt(seriesId) },
      orderBy: { subSeriesName: "asc" },
    })
    return NextResponse.json(subSeries)
  } catch (error) {
    console.error("Failed to fetch sub-series:", error)
    return NextResponse.json({ error: "Failed to fetch sub-series" }, { status: 500 })
  }
}
