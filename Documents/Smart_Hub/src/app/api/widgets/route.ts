import { NextRequest, NextResponse } from "next/server"
import prisma from "@/lib/prisma"
import { getServerSession } from "next-auth"
import { authOptions } from "@/app/api/auth/[...nextauth]/route"

export async function PATCH(request: NextRequest) {
  const session = await getServerSession(authOptions)
  if (!session) return NextResponse.json({ error: "Unauthorized" }, { status: 401 })

  try {
    const { id, mqttTopic } = await request.json()
    if (!id || !mqttTopic) return NextResponse.json({ error: "Missing required fields" }, { status: 400 })

    const widget = await prisma.widget.update({
      where: { id: parseInt(id) },
      data: { mqttTopic },
    })
    return NextResponse.json(widget)
  } catch (error) {
    console.error("Failed to update widget:", error)
    return NextResponse.json({ error: "Failed to update widget" }, { status: 500 })
  }
}

export async function DELETE(request: NextRequest) {
  const session = await getServerSession(authOptions)
  if (!session) return NextResponse.json({ error: "Unauthorized" }, { status: 401 })

  const { searchParams } = new URL(request.url)
  const id = searchParams.get("id")

  if (!id) return NextResponse.json({ error: "Missing widget ID" }, { status: 400 })

  try {
    await prisma.widget.delete({
      where: { id: parseInt(id) },
    })
    return NextResponse.json({ message: "Widget deleted successfully" })
  } catch (error) {
    console.error("Failed to delete widget:", error)
    return NextResponse.json({ error: "Failed to delete widget" }, { status: 500 })
  }
}
