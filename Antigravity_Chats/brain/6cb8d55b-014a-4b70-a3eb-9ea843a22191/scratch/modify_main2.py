path = '/home/melwinabi/StudioProjects/chat_bot_financial_trading/lib/main.dart'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# 1. Update Timer to 30 seconds for auto-refresh without manual intervention
old_timer = "Timer.periodic(const Duration(minutes: 5)"
new_timer = "Timer.periodic(const Duration(seconds: 30)"

# 2. Update price representation to only 2 decimal places
old_price_block = """          final price = item['regularMarketPrice'] ?? '0.0';
          final change = item['regularMarketChangePercent'] ?? 0.0;
          final color = (change >= 0) ? Colors.greenAccent : Colors.redAccent;

          return Padding(
            padding: const EdgeInsets.symmetric(horizontal: 15),
            child: Center(
              child: Text('$symbol: $price (${change.toStringAsFixed(2)}%)',"""

new_price_block = """          final priceVal = item['regularMarketPrice'] ?? 0.0;
          final priceStr = priceVal is num ? priceVal.toStringAsFixed(2) : priceVal.toString();
          final change = item['regularMarketChangePercent'] ?? 0.0;
          final color = (change >= 0) ? Colors.greenAccent : Colors.redAccent;

          return Padding(
            padding: const EdgeInsets.symmetric(horizontal: 15),
            child: Center(
              child: Text('$symbol: $priceStr (${change.toStringAsFixed(2)}%)',"""

# Execute changes
content_lf = content.replace('\r\n', '\n')
old_price_block_lf = old_price_block.replace('\r\n', '\n')
new_price_block_lf = new_price_block.replace('\r\n', '\n')

changes_made = False

if old_timer in content_lf:
    content_lf = content_lf.replace(old_timer, new_timer)
    changes_made = True

if old_price_block_lf in content_lf:
    content_lf = content_lf.replace(old_price_block_lf, new_price_block_lf)
    changes_made = True

if changes_made:
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content_lf)
    print("SUCCESS: Timer updated to 30s and price display formatted to 2 decimals!")
else:
    print("ERROR: Could not find target patterns to replace in main.dart")
