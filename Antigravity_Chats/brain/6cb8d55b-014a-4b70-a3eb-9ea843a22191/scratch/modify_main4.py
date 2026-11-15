path = '/home/melwinabi/StudioProjects/chat_bot_financial_trading/lib/main.dart'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# Normalize
content_lf = content.replace('\r\n', '\n')

# Find the start index of class _ChatPageState
start_keyword = 'class _ChatPageState extends State<ChatPage> {'
start_idx = content_lf.find(start_keyword)

# Find the start index of class _TickerBar
end_keyword = 'class _TickerBar extends StatefulWidget {'
end_idx = content_lf.find(end_keyword)

if start_idx != -1 and end_idx != -1:
    new_chat_state_code = """class _ChatPageState extends State<ChatPage> {
  List<dynamic> _tickerData = [];
  Timer? _timer;
  bool _isFetching = false;

  // Chat message management
  final List<Map<String, dynamic>> _messages = [];
  final TextEditingController _chatController = TextEditingController();
  final ScrollController _chatScrollController = ScrollController();
  bool _isSending = false;

  @override
  void initState() {
    super.initState();
    _fetchTickerData();
    _timer = Timer.periodic(const Duration(seconds: 30), (timer) => _fetchTickerData());

    // Add Welcome message
    _messages.add({
      "text": "Hello! I am your Finance Folio Chatbot.\\n\\nI can automatically log your expenses and incomes. Try typing:\\n• 'Spent 450 for Food on SBI Card'\\n• 'Received 12000 salary from office'\\n• 'Expense of 120 for fuel with UPI'",
      "isMe": false
    });
  }

  @override
  void dispose() {
    _timer?.cancel();
    _chatController.dispose();
    _chatScrollController.dispose();
    super.dispose();
  }

  Future<void> _fetchTickerData() async {
    if (!mounted) return;
    setState(() => _isFetching = true);
    try {
      final response = await http.get(Uri.parse(
          'http://localhost:3000/api/market-rates')).timeout(const Duration(seconds: 10));

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        setState(() {
          _tickerData = [
            {
              'symbol': 'NIFTY 50',
              'regularMarketPrice': data['nifty'] ?? 0.0,
              'regularMarketChangePercent': 0.0,
            },
            {
              'symbol': 'Gold 24K (g)',
              'regularMarketPrice': data['goldPerGram'] ?? 0.0,
              'regularMarketChangePercent': 0.0,
            },
            {
              'symbol': 'Silver (g)',
              'regularMarketPrice': data['silverPerGram'] ?? 0.0,
              'regularMarketChangePercent': 0.0,
            },
            {
              'symbol': 'USD/INR',
              'regularMarketPrice': data['usdToInr'] ?? 0.0,
              'regularMarketChangePercent': 0.0,
            }
          ];
        });
      }
    } catch (e) {
      debugPrint('Error fetching market data: $e');
    } finally {
      if (mounted) setState(() => _isFetching = false);
    }
  }

  void _sendMessage(String text) async {
    if (text.trim().isEmpty) return;
    _chatController.clear();
    setState(() {
      _messages.add({"text": text, "isMe": true});
      _isSending = true;
    });
    _scrollToBottom();

    try {
      // Send to Flask server on port 5000
      final response = await http.post(
        Uri.parse('http://localhost:5000/api/chat'),
        headers: {"Content-Type": "application/json"},
        body: json.encode({"message": text}),
      ).timeout(const Duration(seconds: 15));

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        setState(() {
          _messages.add({"text": data['reply'] ?? 'No reply from server', "isMe": false});
        });
      } else {
        setState(() {
          _messages.add({"text": "Error: Flask server returned status ${response.statusCode}", "isMe": false});
        });
      }
    } catch (e) {
      setState(() {
        _messages.add({
          "text": "Error: Failed to connect to AI server. Please make sure the Python Flask server is running on port 5000.\\n(Error: $e)",
          "isMe": false
        });
      });
    } finally {
      setState(() {
        _isSending = false;
      });
      _scrollToBottom();
    }
  }

  void _scrollToBottom() {
    Future.delayed(const Duration(milliseconds: 100), () {
      if (_chatScrollController.hasClients) {
        _chatScrollController.animateTo(
          _chatScrollController.position.maxScrollExtent,
          duration: const Duration(milliseconds: 300),
          curve: Curves.easeOut,
        );
      }
    });
  }

  @override
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
      ),
      body: Column(
        children: [
          // Message List Area
          Expanded(
            child: _messages.isEmpty
                ? const Center(child: Text("No messages yet. Send a message to start!"))
                : ListView.builder(
                    controller: _chatScrollController,
                    padding: const EdgeInsets.symmetric(horizontal: 16.0, vertical: 12.0),
                    itemCount: _messages.length,
                    itemBuilder: (context, index) {
                      final msg = _messages[index];
                      final isMe = msg["isMe"] == true;
                      
                      return Align(
                        alignment: isMe ? Alignment.centerRight : Alignment.centerLeft,
                        child: Container(
                          margin: const EdgeInsets.symmetric(vertical: 4.0),
                          padding: const EdgeInsets.symmetric(horizontal: 14.0, vertical: 10.0),
                          constraints: BoxConstraints(
                            maxWidth: MediaQuery.of(context).size.width * 0.75,
                          ),
                          decoration: BoxDecoration(
                            color: isMe ? Colors.deepPurple : Colors.grey[200],
                            borderRadius: BorderRadius.only(
                              topLeft: const Radius.circular(16.0),
                              topRight: const Radius.circular(16.0),
                              bottomLeft: Radius.circular(isMe ? 16.0 : 0.0),
                              bottomRight: Radius.circular(isMe ? 0.0 : 16.0),
                            ),
                          ),
                          child: Text(
                            msg["text"] ?? '',
                            style: TextStyle(
                              color: isMe ? Colors.white : Colors.black87,
                              fontSize: 14.0,
                            ),
                          ),
                        ),
                      );
                    },
                  ),
          ),
          
          // Typing loader
          if (_isSending)
            Padding(
              padding: const EdgeInsets.symmetric(vertical: 8.0, horizontal: 16.0),
              child: Align(
                alignment: Alignment.centerLeft,
                child: Row(
                  children: [
                    const SizedBox(
                      width: 14,
                      height: 14,
                      child: CircularProgressIndicator(strokeWidth: 2, color: Colors.deepPurple),
                    ),
                    const SizedBox(width: 8),
                    Text(
                      "Bot is parsing transaction...",
                      style: TextStyle(
                        fontSize: 12, 
                        color: Colors.grey[600], 
                        fontStyle: FontStyle.italic
                      ),
                    ),
                  ],
                ),
              ),
            ),

          const Divider(height: 1.0),
          
          // Input box area
          Container(
            padding: const EdgeInsets.symmetric(horizontal: 12.0, vertical: 6.0),
            color: Colors.white,
            child: Row(
              children: [
                Expanded(
                  child: TextField(
                    controller: _chatController,
                    textInputAction: TextInputAction.send,
                    onSubmitted: _sendMessage,
                    decoration: const InputDecoration(
                      hintText: "Type transaction (e.g. Spent 500 for Dinner)...",
                      border: InputBorder.none,
                    ),
                  ),
                ),
                IconButton(
                  icon: const Icon(Icons.send, color: Colors.deepPurple),
                  onPressed: () => _sendMessage(_chatController.text),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}

"""
    new_content = content_lf[:start_idx] + new_chat_state_code + content_lf[end_idx:]
    with open(path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    print("SUCCESS: Chat UI page fully implemented!")
else:
    print("ERROR: Start or End class index not found.")
