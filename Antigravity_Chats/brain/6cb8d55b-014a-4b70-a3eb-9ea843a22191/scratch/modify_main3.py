path = '/home/melwinabi/StudioProjects/chat_bot_financial_trading/lib/main.dart'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# 1. Target the password TextFormField
old_password_field = """                  TextFormField(
                    controller: _passwordController,
                    obscureText: !_isPasswordVisible,
                    decoration: const InputDecoration(labelText: 'Password', border: OutlineInputBorder()),
                  ),"""

new_password_field = """                  TextFormField(
                    controller: _passwordController,
                    obscureText: !_isPasswordVisible,
                    decoration: InputDecoration(
                      labelText: 'Password',
                      border: const OutlineInputBorder(),
                      suffixIcon: IconButton(
                        icon: Icon(
                          _isPasswordVisible ? Icons.visibility : Icons.visibility_off,
                        ),
                        onPressed: () {
                          setState(() {
                            _isPasswordVisible = !_isPasswordVisible;
                          });
                        },
                      ),
                    ),
                  ),"""

# 2. Target the ChatPage Scaffold build method to add the Drawer
old_scaffold_start = """  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('TradeBot AI Chat'),
        backgroundColor: Colors.deepPurple,
        foregroundColor: Colors.white,
        bottom: PreferredSize(
          preferredSize: const Size.fromHeight(30),
          child: _TickerBar(data: _tickerData),
        ),
      ),"""

new_scaffold_start = """  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('TradeBot AI Chat'),
        backgroundColor: Colors.deepPurple,
        foregroundColor: Colors.white,
        bottom: PreferredSize(
          preferredSize: const Size.fromHeight(30),
          child: _TickerBar(data: _tickerData),
        ),
      ),
      drawer: Drawer(
        child: ListView(
          padding: EdgeInsets.zero,
          children: [
            const DrawerHeader(
              decoration: BoxDecoration(
                color: Colors.deepPurple,
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Icon(Icons.candlestick_chart, color: Colors.white, size: 48),
                  SizedBox(height: 8),
                  Text(
                    'Finance Folio Bot',
                    style: TextStyle(color: Colors.white, fontSize: 20, fontWeight: FontWeight.bold),
                  ),
                  Text(
                    'Auto Trading & Management',
                    style: TextStyle(color: Colors.white70, fontSize: 12),
                  ),
                ],
              ),
            ),
            ListTile(
              leading: const Icon(Icons.chat_bubble),
              title: const Text('AI Chat Room'),
              onTap: () {
                Navigator.pop(context);
              },
            ),
            ListTile(
              leading: const Icon(Icons.trending_up),
              title: const Text('Live Markets'),
              onTap: () {
                Navigator.pop(context);
              },
            ),
            ListTile(
              leading: const Icon(Icons.settings),
              title: const Text('Settings'),
              onTap: () {
                Navigator.pop(context);
              },
            ),
            const Divider(),
            ListTile(
              leading: const Icon(Icons.logout, color: Colors.red),
              title: const Text('Logout', style: TextStyle(color: Colors.red)),
              onTap: () {
                Navigator.pop(context); // Close drawer
                Navigator.pushReplacement(
                  context,
                  MaterialPageRoute(builder: (context) => const LoginPage()),
                );
              },
            ),
          ],
        ),
      ),"""

# Normalize and replace
content_lf = content.replace('\r\n', '\n')
old_password_field_lf = old_password_field.replace('\r\n', '\n')
new_password_field_lf = new_password_field.replace('\r\n', '\n')
old_scaffold_start_lf = old_scaffold_start.replace('\r\n', '\n')
new_scaffold_start_lf = new_scaffold_start.replace('\r\n', '\n')

changes_made = 0

if old_password_field_lf in content_lf:
    content_lf = content_lf.replace(old_password_field_lf, new_password_field_lf)
    changes_made += 1

if old_scaffold_start_lf in content_lf:
    content_lf = content_lf.replace(old_scaffold_start_lf, new_scaffold_start_lf)
    changes_made += 1

if changes_made == 2:
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content_lf)
    print("SUCCESS: Password eye visibility toggle and Sidebar Menu Drawer added!")
else:
    print(f"ERROR: Found only {changes_made}/2 blocks to replace in main.dart")
