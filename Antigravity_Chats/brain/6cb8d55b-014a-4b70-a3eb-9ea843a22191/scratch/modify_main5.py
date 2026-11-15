path = '/home/melwinabi/StudioProjects/chat_bot_financial_trading/lib/main.dart'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

content_lf = content.replace('\r\n', '\n')

# 1. Add platform and kIsWeb imports
old_imports = "import 'package:http/http.dart' as http;"
new_imports = """import 'package:http/http.dart' as http;
import 'dart:io' show Platform;
import 'package:flutter/foundation.dart' show kIsWeb;"""

# 2. Add getBackendUrl helper function before main()
old_main = "void main() {"
new_main = """String getBackendUrl(int port, String path) {
  if (kIsWeb) {
    return 'http://localhost:$port$path';
  } else if (Platform.isLinux || Platform.isMacOS || Platform.isWindows) {
    return 'http://localhost:$port$path';
  } else {
    // For mobile (Emulator & Physical phone), use local network IP of host machine
    return 'http://192.168.1.11:$port$path';
  }
}

void main() {"""

# 3. Replace the market API URL
old_market_url = "'http://localhost:3000/api/market-rates'"
new_market_url = "getBackendUrl(3000, '/api/market-rates')"

# 4. Replace the chatbot API URL
old_chat_url = "'http://localhost:5000/api/chat'"
new_chat_url = "getBackendUrl(5000, '/api/chat')"

changes = 0

if old_imports in content_lf:
    content_lf = content_lf.replace(old_imports, new_imports)
    changes += 1

if old_main in content_lf:
    content_lf = content_lf.replace(old_main, new_main)
    changes += 1

if old_market_url in content_lf:
    content_lf = content_lf.replace(old_market_url, new_market_url)
    changes += 1

if old_chat_url in content_lf:
    content_lf = content_lf.replace(old_chat_url, new_chat_url)
    changes += 1

if changes == 4:
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content_lf)
    print("SUCCESS: Dynamic URL helper added for desktop/emulator/mobile parallel execution!")
else:
    print(f"ERROR: Only {changes}/4 replacements succeeded.")
