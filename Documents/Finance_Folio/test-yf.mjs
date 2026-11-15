import yahooFinance from 'yahoo-finance2';
console.log(typeof yahooFinance.quote);
if (typeof yahooFinance.quote !== 'function') {
  console.log("Not a function. Default is:", yahooFinance.default);
  const YF = new yahooFinance.default();
  YF.quote('^NSEI').then(res => console.log(res.regularMarketPrice)).catch(console.error);
} else {
  yahooFinance.quote('^NSEI').then(res => console.log(res.regularMarketPrice)).catch(console.error);
}
