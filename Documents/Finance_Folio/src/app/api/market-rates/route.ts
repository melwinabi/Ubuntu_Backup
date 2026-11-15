import { NextResponse } from "next/server";
import { getLivePreciousMetalsPrices } from "@/lib/calculations";

export const revalidate = 60; // Cache for 1 minute to prevent hitting rate limits

export async function GET() {
  try {
    const data = await getLivePreciousMetalsPrices();
    
    if (!data.success) {
      return NextResponse.json(
        { success: false, error: "Failed to fetch live prices" },
        { status: 500 }
      );
    }

    return NextResponse.json({
      success: true,
      nifty: data.nifty,
      goldPerGram: data.gold,
      silverPerGram: data.silver,
      usdToInr: data.usdToInr,
      goldUSDPerOunce: data.goldUSDPerOunce,
      silverUSDPerOunce: data.silverUSDPerOunce,
      lastUpdated: new Date().toISOString()
    });
  } catch (error: any) {
    return NextResponse.json(
      { success: false, error: error.message || "Internal server error" },
      { status: 500 }
    );
  }
}
