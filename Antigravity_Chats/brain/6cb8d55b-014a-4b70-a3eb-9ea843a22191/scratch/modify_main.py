path = '/home/melwinabi/StudioProjects/chat_bot_financial_trading/lib/main.dart'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Change back to localhost for Linux Desktop application support
old_str = "'http://10.0.2.2:3000/api/market-rates'"
new_str = "'http://localhost:3000/api/market-rates'"

if old_str in content:
    content = content.replace(old_str, new_str)
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("SUCCESS: Changed URL back to localhost for Linux Desktop!")
else:
    # Double quotes check
    old_str_double = '"http://10.0.2.2:3000/api/market-rates"'
    new_str_double = '"http://localhost:3000/api/market-rates"'
    if old_str_double in content:
        content = content.replace(old_str_double, new_str_double)
        with open(path, 'w', encoding='utf-8') as f:
            f.write(content)
        print("SUCCESS: Changed URL back to localhost (double quotes)!")
    else:
        print("ERROR: 10.0.2.2 URL not found in main.dart")
