/**
 * Normalizes physical gold/silver weight units into grams.
 */
export function convertPreciousMetalToGrams(quantity: number, unit: string) {
  const normalized = unit.toLowerCase().trim();
  if (normalized === "kg" || normalized === "kilogram") {
    return quantity * 1000;
  }
  if (normalized === "sovereign" || normalized === "pavan") {
    return quantity * 8;
  }
  if (normalized === "oz" || normalized === "ounce") {
    return quantity * 31.1034768;
  }
  return quantity; // Default unit: grams (g)
}

/**
 * Calculates the number of completed installments and total invested amount for a SIP.
 */
export function calculateSipStats(inv: {
  isSIP: boolean;
  purchaseDate: Date | string;
  sipFrequency?: string | null;
  sipAmount?: number | null;
  sipStatus?: string | null;
  sipCancelDate?: Date | string | null;
  sipSkips?: { skipDate: Date | string }[];
}) {
  if (!inv.isSIP || !inv.sipAmount || !inv.sipFrequency) {
    return {
      installmentsCompleted: 1,
      totalInvested: inv.sipAmount || 0,
      skipCount: 0,
      installmentDates: [new Date(inv.purchaseDate)],
    };
  }

  const startDate = new Date(inv.purchaseDate);
  const endDate = inv.sipStatus === "cancelled" && inv.sipCancelDate 
    ? new Date(inv.sipCancelDate) 
    : new Date();

  // If start date is in the future, 0 installments
  if (startDate > endDate) {
    return {
      installmentsCompleted: 0,
      totalInvested: 0,
      skipCount: 0,
      installmentDates: [],
    };
  }

  let installmentDates: Date[] = [];
  let currentDate = new Date(startDate);

  while (currentDate <= endDate) {
    installmentDates.push(new Date(currentDate));
    if (inv.sipFrequency === "weekly") {
      currentDate.setDate(currentDate.getDate() + 7);
    } else if (inv.sipFrequency === "monthly") {
      currentDate.setMonth(currentDate.getMonth() + 1);
    } else if (inv.sipFrequency === "half_yearly") {
      currentDate.setMonth(currentDate.getMonth() + 6);
    } else if (inv.sipFrequency === "yearly") {
      currentDate.setFullYear(currentDate.getFullYear() + 1);
    } else {
      break; 
    }
  }

  const skippedSet = new Set(
    (inv.sipSkips || []).map((skip) => {
      const d = new Date(skip.skipDate);
      return `${d.getFullYear()}-${d.getMonth()}-${d.getDate()}`;
    })
  );

  let skipCount = 0;
  let installmentsCompleted = 0;

  installmentDates.forEach((instDate) => {
    const dateStr = `${instDate.getFullYear()}-${instDate.getMonth()}-${instDate.getDate()}`;
    let isSkipped = skippedSet.has(dateStr);
    
    if (!isSkipped && (inv.sipFrequency === "monthly" || inv.sipFrequency === "half_yearly" || inv.sipFrequency === "yearly")) {
      isSkipped = (inv.sipSkips || []).some((skip) => {
        const d = new Date(skip.skipDate);
        return d.getFullYear() === instDate.getFullYear() && d.getMonth() === instDate.getMonth();
      });
    }

    if (isSkipped) {
      skipCount++;
    } else {
      installmentsCompleted++;
    }
  });

  const totalInvested = installmentsCompleted * inv.sipAmount;

  return {
    installmentsCompleted,
    totalInvested,
    skipCount,
    installmentDates,
  };
}

/**
 * Calculates percentage change and current details for an investment.
 */
export function getInvestmentDetails(inv: any, metalsPrices?: { gold: number; silver: number }) {
  let currentPrice = inv.prices?.[0]?.pricePerUnit ?? inv.purchasePrice;
  let purchaseValue = inv.quantity * inv.purchasePrice;
  let currentValue = inv.quantity * currentPrice;

  if (inv.isSIP && inv.sipAmount) {
    const stats = calculateSipStats(inv);
    purchaseValue = stats.totalInvested;
    if (inv.purchasePrice > 0) {
      const accumulatedUnits = stats.totalInvested / inv.purchasePrice;
      currentValue = accumulatedUnits * currentPrice;
    } else {
      currentValue = stats.totalInvested;
    }
  }

  if (metalsPrices) {
    if (inv.assetType === "gold") {
      const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit);
      currentPrice = metalsPrices.gold;
      currentValue = grams * metalsPrices.gold;
    } else if (inv.assetType === "silver") {
      const grams = convertPreciousMetalToGrams(inv.quantity, inv.unit);
      currentPrice = metalsPrices.silver;
      currentValue = grams * metalsPrices.silver;
    }
  }

  const gainLoss = currentValue - purchaseValue;
  const percentageChange = purchaseValue > 0 ? (gainLoss / purchaseValue) * 100 : 0;

  return {
    currentPrice,
    purchaseValue,
    currentValue,
    gainLoss,
    percentageChange,
  };
}

/**
 * Calculates credit card billing cycle metadata.
 */
export function getCreditCardBillingMeta(card: {
  statementDay: number | null;
  dueDay: number | null;
}) {
  if (!card.statementDay || !card.dueDay) return null;

  const today = new Date();
  const currentYear = today.getFullYear();
  const currentMonth = today.getMonth(); 

  let statementDate = new Date(currentYear, currentMonth, card.statementDay);
  if (today.getDate() < card.statementDay) {
    statementDate = new Date(currentYear, currentMonth - 1, card.statementDay);
  }

  let dueDate = new Date(statementDate.getFullYear(), statementDate.getMonth(), card.dueDay);
  if (card.dueDay <= card.statementDay) {
    dueDate = new Date(statementDate.getFullYear(), statementDate.getMonth() + 1, card.dueDay);
  }

  const daysRemaining = Math.ceil((dueDate.getTime() - today.getTime()) / (1000 * 60 * 60 * 24));

  return {
    statementDate,
    dueDate,
    daysRemaining,
  };
}
