const YF = require('yahoo-finance2').default;
const yahooFinance = new YF({ suppressNotices: ['yahooSurvey'] });

async function testMarketData() {
  try {
    console.log("Fetching Live Indian Market Data & Exact 24K Gold Rates...\n");

    const symbols = ['^NSEI', 'GC=F', 'SI=F', 'INR=X'];
    const results = await yahooFinance.quote(symbols);
    
    const prices = {};
    results.forEach(q => {
      prices[q.symbol] = q.regularMarketPrice;
    });

    const usdToInr = prices['INR=X'] || 83.50;
    const goldUsdPerOz = prices['GC=F'] || 2350;
    const silverUsdPerOz = prices['SI=F'] || 28.00;
    const nifty = prices['^NSEI'] || 0;
    
    // 1 Troy Ounce = 31.1035 grams
    const ounceToGram = 31.1035;

    // ---------------------------------------------------------
    // Indian Market Taxes (Import Duty + GST + Local Premium)
    // ---------------------------------------------------------
    // To match actual Indian 24K Gold Retail prices
    const goldTaxMultiplier = 1.16; // 16% extra (15% Duty + GST + Premium)
    const silverTaxMultiplier = 1.16; 

    // GOLD Calculation (24K - 1 Gram)
    const baseGoldInrPerGram = (goldUsdPerOz / ounceToGram) * usdToInr;
    const retailGold24k = baseGoldInrPerGram * goldTaxMultiplier;

    // SILVER Calculation (1 Gram)
    const baseSilverInrPerGram = (silverUsdPerOz / ounceToGram) * usdToInr;
    const retailSilverGram = baseSilverInrPerGram * silverTaxMultiplier;

    console.log("-----------------------------------------------------");
    console.log(`NIFTY 50                | Price: ₹ ${nifty.toFixed(2)}`);
    console.log(`24K Gold (1 Gram)       | Price: ₹ ${retailGold24k.toFixed(2)}`);
    console.log(`Silver (1 Gram)         | Price: ₹ ${retailSilverGram.toFixed(2)}`);
    console.log(`USD to INR Exchange     | Rate : ₹ ${usdToInr.toFixed(2)}`);
    console.log("-----------------------------------------------------");
    
  } catch (error) {
    console.error("Error fetching data:", error.message);
  }
}

testMarketData();
