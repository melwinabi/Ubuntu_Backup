import 'package:flutter/material.dart';
import 'dart:async';
import 'dart:convert';
import 'package:http/http.dart' as http;
import 'dart:io' show Platform;
import 'package:flutter/foundation.dart' show kIsWeb;

String getBackendUrl(int port, String path) {
  if (kIsWeb) {
    return 'http://localhost:$port$path';
  } else if (Platform.isLinux || Platform.isMacOS || Platform.isWindows) {
    return 'http://localhost:$port$path';
  } else {
    // For mobile (Emulator & Physical phone), use local network IP of host machine
    return 'http://192.168.1.11:$port$path';
  }
}

void main() {
  runApp(const MyApp());
}

final ValueNotifier<ThemeMode> themeNotifier = ValueNotifier(ThemeMode.dark);

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return ValueListenableBuilder<ThemeMode>(
      valueListenable: themeNotifier,
      builder: (_, ThemeMode currentMode, __) {
        return MaterialApp(
          title: 'TradeBot AI',
          debugShowCheckedModeBanner: false,
          themeMode: currentMode,
          theme: ThemeData(
            colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
            useMaterial3: true,
          ),
          darkTheme: ThemeData.dark(useMaterial3: true).copyWith(
            colorScheme: ColorScheme.fromSeed(
              seedColor: Colors.deepPurple,
              brightness: Brightness.dark,
            ),
          ),
          home: const LoginPage(),
        );
      },
    );
  }
}

class LoginPage extends StatefulWidget {
  const LoginPage({super.key});

  @override
  State<LoginPage> createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginPage> {
  final _formKey = GlobalKey<FormState>();
  final TextEditingController _userIdController = TextEditingController();
  final TextEditingController _passwordController = TextEditingController();

  bool _isPasswordVisible = false;
  bool _isLoading = false;

  void _handleLogin() async {
    if (_formKey.currentState!.validate()) {
      setState(() => _isLoading = true);
      
      try {
        final response = await http.post(
          Uri.parse(getBackendUrl(3000, '/api/auth/mobile-login')),
          headers: {"Content-Type": "application/json"},
          body: json.encode({
            "userId": _userIdController.text.trim(),
            "password": _passwordController.text,
          }),
        ).timeout(const Duration(seconds: 10));

        if (response.statusCode == 200) {
          final data = json.decode(response.body);
          if (data['success'] == true && mounted) {
            Navigator.pushReplacement(
              context,
              MaterialPageRoute(builder: (context) => ChatPage(user: data['user'])),
            );
          } else {
            _showError(data['error'] ?? 'Invalid User ID or Password!');
          }
        } else {
          final data = json.decode(response.body);
          _showError(data['error'] ?? 'Invalid User ID or Password!');
        }
      } catch (e) {
        _showError('Connection error: Please ensure Next.js is running.\n(Error: $e)');
      } finally {
        if (mounted) setState(() => _isLoading = false);
      }
    }
  }

  void _showError(String msg) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text(msg),
        backgroundColor: Colors.red,
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Center(
          child: SingleChildScrollView(
            padding: const EdgeInsets.symmetric(horizontal: 24.0),
            child: Form(
              key: _formKey,
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  const Icon(Icons.candlestick_chart, size: 72, color: Colors.deepPurple),
                  const SizedBox(height: 16),
                  Text('TradeBot AI', textAlign: TextAlign.center,
                      style: Theme.of(context).textTheme.headlineMedium?.copyWith(fontWeight: FontWeight.bold)),
                  const SizedBox(height: 36),
                  TextFormField(
                    controller: _userIdController,
                    decoration: const InputDecoration(labelText: 'User ID (e.g. FGAT0001)', border: OutlineInputBorder()),
                    validator: (value) {
                      if (value == null || value.trim().isEmpty) {
                        return 'Please enter your User ID';
                      }
                      return null;
                    },
                  ),
                  const SizedBox(height: 16),
                  TextFormField(
                    controller: _passwordController,
                    obscureText: !_isPasswordVisible,
                    validator: (value) {
                      if (value == null || value.isEmpty) {
                        return 'Please enter your password';
                      }
                      return null;
                    },
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
                  ),
                  const SizedBox(height: 24),
                  ElevatedButton(
                    onPressed: _isLoading ? null : _handleLogin,
                    style: ElevatedButton.styleFrom(backgroundColor: Colors.deepPurple, foregroundColor: Colors.white),
                    child: _isLoading ? const SizedBox(width: 20, height: 20, child: CircularProgressIndicator(color: Colors.white, strokeWidth: 2)) : const Text('Sign In'),
                  ),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class ChatPage extends StatefulWidget {
  final Map<String, dynamic> user;
  const ChatPage({super.key, required this.user});

  @override
  State<ChatPage> createState() => _ChatPageState();
}

class _ChatPageState extends State<ChatPage> {
  List<dynamic> _tickerData = [];
  Timer? _timer;
  bool _isFetching = false;

  // View state: 'chat' | 'market' | 'settings' | 'expenses'
  String _currentView = 'chat';

  // Transaction Logs state
  List<dynamic> _expenses = [];
  List<dynamic> _incomes = [];
  List<dynamic> _loans = [];
  List<dynamic> _cards = [];
  List<dynamic> _investments = [];
  bool _isLoadingTransactions = false;
  String? _transactionsError;

  // Chat message management
  final List<Map<String, dynamic>> _messages = [];
  final TextEditingController _chatController = TextEditingController();
  final ScrollController _chatScrollController = ScrollController();
  bool _isSending = false;

  // Profile management
  late Map<String, dynamic> _userProfile;
  List<dynamic> _bankAccounts = [];
  bool _isLoadingProfile = false;

  final TextEditingController _nameController = TextEditingController();
  final TextEditingController _dobController = TextEditingController();
  final TextEditingController _profilePicController = TextEditingController();

  // Mock Settings Controllers
  final TextEditingController _geminiKeyController = TextEditingController(text: "AIzaSyD-xxxxxxxxxxxx");
  final TextEditingController _flattradeClientIdController = TextEditingController(text: "FT012345");
  final TextEditingController _flattradeSecretController = TextEditingController(text: "xxxxxxxxxxxxxxxxxxxxxxxx");

  // Mock Holdings (Positions) from Flattrade
  final List<Map<String, dynamic>> _holdings = [
    {
      "symbol": "RELIANCE",
      "qty": 15,
      "avgPrice": 2450.50,
      "ltp": 2510.30,
      "pnl": 897.00
    },
    {
      "symbol": "TATASTEEL",
      "qty": 100,
      "avgPrice": 118.20,
      "ltp": 112.40,
      "pnl": -580.00
    },
    {
      "symbol": "INFY",
      "qty": 20,
      "avgPrice": 1420.00,
      "ltp": 1465.10,
      "pnl": 902.00
    },
    {
      "symbol": "SBIN",
      "qty": 50,
      "avgPrice": 580.00,
      "ltp": 605.50,
      "pnl": 1275.00
    }
  ];

  @override
  void initState() {
    super.initState();
    _userProfile = Map<String, dynamic>.from(widget.user);
    _nameController.text = _userProfile['fullName'] ?? '';
    _dobController.text = _userProfile['dob'] != null ? _userProfile['dob'].toString().split('T')[0] : '';
    _profilePicController.text = _userProfile['profilePic'] ?? '';

    _fetchTickerData();
    _timer = Timer.periodic(const Duration(seconds: 30), (timer) => _fetchTickerData());
    _fetchProfileAndAccounts();

    // Add Welcome message
    _messages.add({
      "text": "Hello! I am your Finance Folio Chatbot.\n\nI can automatically log your expenses and incomes. Try typing:\n• 'Spent 450 for Food on SBI Card'\n• 'Received 12000 salary from office'\n• 'Expense of 120 for fuel with UPI'",
      "isMe": false
    });
  }

  @override
  void dispose() {
    _timer?.cancel();
    _chatController.dispose();
    _chatScrollController.dispose();
    _geminiKeyController.dispose();
    _flattradeClientIdController.dispose();
    _flattradeSecretController.dispose();
    _nameController.dispose();
    _dobController.dispose();
    _profilePicController.dispose();
    super.dispose();
  }

  // Profile & Accounts helper methods
  Future<void> _fetchProfileAndAccounts() async {
    setState(() => _isLoadingProfile = true);
    try {
      final response = await http.get(
        Uri.parse(getBackendUrl(3000, '/api/auth/profile?userId=${_userProfile['id']}')),
      ).timeout(const Duration(seconds: 10));

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        if (data['success'] == true) {
          setState(() {
            _userProfile = data['user'];
            _bankAccounts = data['user']['accounts'] ?? [];
            _nameController.text = _userProfile['fullName'] ?? '';
            _dobController.text = _userProfile['dob'] != null ? _userProfile['dob'].toString().split('T')[0] : '';
            _profilePicController.text = _userProfile['profilePic'] ?? '';
          });
        }
      }
    } catch (e) {
      debugPrint("Error fetching profile: $e");
    } finally {
      setState(() => _isLoadingProfile = false);
    }
  }

  Future<bool> _updateProfileOnBackend(Map<String, dynamic> payload) async {
    try {
      final response = await http.post(
        Uri.parse(getBackendUrl(3000, '/api/auth/profile')),
        headers: {"Content-Type": "application/json"},
        body: json.encode({
          "userId": _userProfile['id'],
          ...payload,
        }),
      ).timeout(const Duration(seconds: 10));

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        if (data['success'] == true) {
          await _fetchProfileAndAccounts();
          return true;
        } else {
          _showError(data['error'] ?? "Action failed");
        }
      } else {
        final data = json.decode(response.body);
        _showError(data['error'] ?? "Server error");
      }
    } catch (e) {
      _showError("Connection error: $e");
    }
    return false;
  }

  void _showError(String message) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(content: Text(message), backgroundColor: Colors.red),
    );
  }

  void _showSuccess(String message) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(content: Text(message), backgroundColor: Colors.green),
    );
  }

  Widget _buildProfileImage(String? picStr) {
    if (picStr == null || picStr.trim().isEmpty) {
      return const CircleAvatar(
        radius: 45,
        backgroundColor: Colors.deepPurple,
        child: Icon(Icons.person, size: 50, color: Colors.white),
      );
    }
    
    try {
      if (picStr.startsWith("http://") || picStr.startsWith("https://")) {
        return CircleAvatar(
          radius: 45,
          backgroundImage: NetworkImage(picStr),
        );
      } else if (picStr.contains(",") && picStr.split(",").length > 1) {
        final base64Str = picStr.split(",")[1];
        return CircleAvatar(
          radius: 45,
          backgroundImage: MemoryImage(base64Decode(base64Str)),
        );
      } else {
        return CircleAvatar(
          radius: 45,
          backgroundImage: MemoryImage(base64Decode(picStr)),
        );
      }
    } catch (e) {
      return const CircleAvatar(
        radius: 45,
        backgroundColor: Colors.deepPurple,
        child: Icon(Icons.person, size: 50, color: Colors.white),
      );
    }
  }

  Future<void> _selectDate(BuildContext context) async {
    final DateTime? picked = await showDatePicker(
      context: context,
      initialDate: DateTime.tryParse(_dobController.text) ?? DateTime(2000, 1, 1),
      firstDate: DateTime(1900),
      lastDate: DateTime.now(),
    );
    if (picked != null) {
      setState(() {
        _dobController.text = picked.toIso8601String().split('T')[0];
      });
    }
  }

  void _showChangePasswordDialog() {
    final currentPassController = TextEditingController();
    final newPassController = TextEditingController();
    final confirmPassController = TextEditingController();
    
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: const Text("Change Password"),
          content: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              TextField(
                controller: currentPassController,
                obscureText: true,
                decoration: const InputDecoration(labelText: "Current Password"),
              ),
              TextField(
                controller: newPassController,
                obscureText: true,
                decoration: const InputDecoration(labelText: "New Password"),
              ),
              TextField(
                controller: confirmPassController,
                obscureText: true,
                decoration: const InputDecoration(labelText: "Confirm New Password"),
              ),
            ],
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context),
              child: const Text("Cancel"),
            ),
            ElevatedButton(
              onPressed: () async {
                if (newPassController.text != confirmPassController.text) {
                  _showError("Passwords do not match!");
                  return;
                }
                if (newPassController.text.length < 6) {
                  _showError("New password must be at least 6 characters!");
                  return;
                }
                Navigator.pop(context);
                final success = await _updateProfileOnBackend({
                  "action": "change_password",
                  "currentPassword": currentPassController.text,
                  "newPassword": newPassController.text,
                });
                if (success) {
                  _showSuccess("Password changed successfully!");
                }
              },
              child: const Text("Update"),
            ),
          ],
        );
      },
    );
  }

  void _showFactoryResetDialog() {
    final passwordController = TextEditingController();
    
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: const Text("Enter Password"),
          content: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Text(
                "Please enter your login password to authorize the factory reset:",
                style: TextStyle(fontSize: 13),
              ),
              const SizedBox(height: 12),
              TextField(
                controller: passwordController,
                obscureText: true,
                decoration: const InputDecoration(
                  border: OutlineInputBorder(),
                  labelText: "Password",
                  contentPadding: EdgeInsets.symmetric(horizontal: 10, vertical: 8),
                ),
              ),
            ],
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context),
              child: const Text("Cancel"),
            ),
            ElevatedButton(
              style: ElevatedButton.styleFrom(backgroundColor: Colors.red, foregroundColor: Colors.white),
              onPressed: () {
                final password = passwordController.text;
                if (password.isEmpty) {
                  _showError("Password cannot be empty!");
                  return;
                }
                Navigator.pop(context);
                
                // Show second confirmation dialog
                _showFactoryResetConfirmDialog(password);
              },
              child: const Text("Next"),
            ),
          ],
        );
      },
    );
  }

  void _showFactoryResetConfirmDialog(String password) {
    final confirmController = TextEditingController();
    
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: const Text("Confirm Reset"),
          content: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Text(
                "WARNING: This action is permanent and cannot be undone! All expenses, incomes, investments, and history will be deleted. Account balances will reset to 0.\n\nType 'CONFIRM' below to wipe all data:",
                style: TextStyle(fontSize: 13, color: Colors.red),
              ),
              const SizedBox(height: 12),
              TextField(
                controller: confirmController,
                decoration: const InputDecoration(
                  border: OutlineInputBorder(),
                  labelText: "Type 'CONFIRM'",
                  contentPadding: EdgeInsets.symmetric(horizontal: 10, vertical: 8),
                ),
              ),
            ],
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context),
              child: const Text("Cancel"),
            ),
            ElevatedButton(
              style: ElevatedButton.styleFrom(backgroundColor: Colors.red, foregroundColor: Colors.white),
              onPressed: () async {
                final confirmText = confirmController.text.trim();
                if (confirmText != 'CONFIRM' && confirmText != 'confirm') {
                  _showError("Confirmation text did not match!");
                  return;
                }
                Navigator.pop(context);
                
                setState(() {
                  _isLoadingProfile = true;
                });
                
                final success = await _updateProfileOnBackend({
                  "action": "reset_statistics",
                  "password": password,
                });
                
                setState(() {
                  _isLoadingProfile = false;
                });
                
                if (success) {
                  _showSuccess("All statistics have been reset successfully!");
                  _fetchTransactions();
                }
              },
              child: const Text("Wipe All Data"),
            ),
          ],
        );
      },
    );
  }

  void _showAccountDialog([Map<String, dynamic>? account]) {
    final isEdit = account != null;
    final nameController = TextEditingController(text: isEdit ? account['accountName'] : '');
    final providerController = TextEditingController(text: isEdit ? account['providerName'] : '');
    final last4Controller = TextEditingController(text: isEdit ? account['accountNumberLast4'] : '');
    final balanceController = TextEditingController(text: isEdit ? (account['currentBalance'] ?? 0.0).toString() : '0.0');
    String selectedType = isEdit ? (account['accountType'] ?? 'bank') : 'bank';
    
    showDialog(
      context: context,
      builder: (context) {
        return StatefulBuilder(
          builder: (context, setDialogState) {
            return AlertDialog(
              title: Text(isEdit ? "Edit Bank Account" : "Add Bank Account"),
              content: SingleChildScrollView(
                child: Column(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    TextField(
                      controller: nameController,
                      decoration: const InputDecoration(labelText: "Account Name (e.g. HDFC Salary)"),
                    ),
                    const SizedBox(height: 12),
                    DropdownButtonFormField<String>(
                      value: selectedType,
                      decoration: const InputDecoration(labelText: "Account Type"),
                      items: const [
                        DropdownMenuItem(value: "bank", child: Text("Bank Account")),
                        DropdownMenuItem(value: "wallet", child: Text("Wallet")),
                        DropdownMenuItem(value: "cash", child: Text("Cash")),
                        DropdownMenuItem(value: "other", child: Text("Other")),
                      ],
                      onChanged: (val) {
                        if (val != null) {
                          setDialogState(() {
                            selectedType = val;
                          });
                        }
                      },
                    ),
                    TextField(
                      controller: providerController,
                      decoration: const InputDecoration(labelText: "Provider Name (e.g. HDFC Bank)"),
                    ),
                    TextField(
                      controller: last4Controller,
                      maxLength: 4,
                      keyboardType: TextInputType.number,
                      decoration: const InputDecoration(labelText: "Last 4 Digits"),
                    ),
                    TextField(
                      controller: balanceController,
                      keyboardType: const TextInputType.numberWithOptions(decimal: true),
                      decoration: const InputDecoration(labelText: "Current Balance"),
                    ),
                  ],
                ),
              ),
              actions: [
                TextButton(
                  onPressed: () => Navigator.pop(context),
                  child: const Text("Cancel"),
                ),
                ElevatedButton(
                  onPressed: () async {
                    if (nameController.text.trim().isEmpty) {
                      _showError("Account name is required!");
                      return;
                    }
                    Navigator.pop(context);
                    final success = await _updateProfileOnBackend({
                      "action": "save_account",
                      "accountId": isEdit ? account['id'] : null,
                      "accountName": nameController.text.trim(),
                      "accountType": selectedType,
                      "providerName": providerController.text.trim(),
                      "accountNumberLast4": last4Controller.text.trim(),
                      "currentBalance": balanceController.text.trim(),
                    });
                    if (success) {
                      _showSuccess(isEdit ? "Account updated successfully!" : "Account created successfully!");
                    }
                  },
                  child: const Text("Save"),
                ),
              ],
            );
          },
        );
      },
    );
  }

  void _confirmDeleteAccount(String accountId) {
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: const Text("Confirm Delete"),
          content: const Text("Are you sure you want to delete this bank account? This action cannot be undone."),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context),
              child: const Text("Cancel"),
            ),
            ElevatedButton(
              onPressed: () async {
                Navigator.pop(context);
                final success = await _updateProfileOnBackend({
                  "action": "delete_account",
                  "accountId": accountId,
                });
                if (success) {
                  _showSuccess("Account deleted successfully!");
                }
              },
              style: ElevatedButton.styleFrom(backgroundColor: Colors.red, foregroundColor: Colors.white),
              child: const Text("Delete"),
            ),
          ],
        );
      },
    );
  }

  Future<void> _fetchTickerData() async {
    if (!mounted) return;
    setState(() => _isFetching = true);
    try {
      final response = await http.get(Uri.parse(
          getBackendUrl(3000, '/api/market-rates'))).timeout(const Duration(seconds: 10));

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

  String formatDate(String? isoString) {
    if (isoString == null) return '';
    try {
      final dateTime = DateTime.parse(isoString).toLocal();
      return "${dateTime.day.toString().padLeft(2, '0')}/${dateTime.month.toString().padLeft(2, '0')}/${dateTime.year} ${dateTime.hour.toString().padLeft(2, '0')}:${dateTime.minute.toString().padLeft(2, '0')}";
    } catch (_) {
      return isoString.split('T')[0];
    }
  }

  Future<void> _fetchTransactions() async {
    if (!mounted) return;
    setState(() {
      _isLoadingTransactions = true;
      _transactionsError = null;
    });
    try {
      final response = await http.get(Uri.parse(
          getBackendUrl(3000, '/api/transactions'))).timeout(const Duration(seconds: 10));

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        if (data['success'] == true) {
          setState(() {
            _expenses = data['expenses'] ?? [];
            _incomes = data['incomes'] ?? [];
            _loans = data['loans'] ?? [];
            _cards = data['cards'] ?? [];
            _investments = data['investments'] ?? [];
            _isLoadingTransactions = false;
          });
        } else {
          setState(() {
            _transactionsError = data['error'] ?? 'Failed to load transactions';
            _isLoadingTransactions = false;
          });
        }
      } else {
        setState(() {
          _transactionsError = 'Server returned code: ${response.statusCode}';
          _isLoadingTransactions = false;
        });
      }
    } catch (e) {
      setState(() {
        _transactionsError = 'Connection error: $e';
        _isLoadingTransactions = false;
      });
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
      // Send to Next.js server on port 3000
      final response = await http.post(
        Uri.parse(getBackendUrl(3000, '/api/chat')),
        headers: {"Content-Type": "application/json"},
        body: json.encode({
          "message": text,
          "userId": _userProfile['id']
        }),
      ).timeout(const Duration(seconds: 15));

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        setState(() {
          _messages.add({
            "text": data['reply'] ?? 'No reply from server', 
            "isMe": false,
            "options": data['options']
          });
        });
      } else {
        setState(() {
          _messages.add({"text": "Error: Flask server returned status ${response.statusCode}", "isMe": false});
        });
      }
    } catch (e) {
      setState(() {
        _messages.add({
          "text": "Error: Failed to connect to AI server. Please make sure the Python Flask server is running on port 5000.\n(Error: $e)",
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

  Widget _buildChatView() {
    return Column(
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
                    final isDark = Theme.of(context).brightness == Brightness.dark;
                    final options = msg["options"] as List<dynamic>?;
                    final isLast = index == _messages.length - 1;
                    
                    return Column(
                      crossAxisAlignment: isMe ? CrossAxisAlignment.end : CrossAxisAlignment.start,
                      children: [
                        Align(
                          alignment: isMe ? Alignment.centerRight : Alignment.centerLeft,
                          child: Container(
                            margin: const EdgeInsets.symmetric(vertical: 4.0),
                            padding: const EdgeInsets.symmetric(horizontal: 14.0, vertical: 10.0),
                            constraints: BoxConstraints(
                              maxWidth: MediaQuery.of(context).size.width * 0.75,
                            ),
                            decoration: BoxDecoration(
                              color: isMe 
                                  ? Colors.deepPurple 
                                  : (isDark ? Colors.grey[850] : Colors.grey[200]),
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
                                color: isMe ? Colors.white : (isDark ? Colors.white70 : Colors.black87),
                                fontSize: 14.0,
                              ),
                            ),
                          ),
                        ),
                        if (!isMe && options != null && options.isNotEmpty && isLast)
                          Padding(
                            padding: const EdgeInsets.only(left: 16.0, top: 4.0, bottom: 8.0),
                            child: Wrap(
                              spacing: 8.0,
                              runSpacing: 4.0,
                              crossAxisAlignment: WrapCrossAlignment.center,
                              children: options.map((opt) {
                                if (opt.toString().toLowerCase() == 'other') {
                                  return _OtherOptionInput(
                                    isDark: isDark,
                                    onSubmit: (val) {
                                      setState(() {
                                        msg.remove("options");
                                      });
                                      _sendMessage(val);
                                    },
                                  );
                                }
                                return ActionChip(
                                  label: Text(opt.toString()),
                                  backgroundColor: isDark ? Colors.deepPurple[900] : Colors.deepPurple[50],
                                  labelStyle: TextStyle(
                                    color: isDark ? Colors.white : Colors.deepPurple,
                                    fontWeight: FontWeight.bold,
                                    fontSize: 13,
                                  ),
                                  onPressed: () {
                                    // Remove the options so they don't click again
                                    setState(() {
                                      msg.remove("options");
                                    });
                                    _sendMessage(opt.toString());
                                  },
                                );
                              }).toList(),
                            ),
                          ),
                      ],
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
          color: Theme.of(context).cardColor,
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
    );
  }

  Widget _buildMarketView() {
    final isDark = Theme.of(context).brightness == Brightness.dark;
    
    return RefreshIndicator(
      onRefresh: _fetchTickerData,
      child: ListView(
        padding: const EdgeInsets.all(16.0),
        children: [
          // 1. Title
          Text(
            "Live Market Indicators",
            style: Theme.of(context).textTheme.titleLarge?.copyWith(fontWeight: FontWeight.bold),
          ),
          const SizedBox(height: 12),

          // Grid for Ticker Cards
          GridView.builder(
            shrinkWrap: true,
            physics: const NeverScrollableScrollPhysics(),
            gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
              crossAxisCount: 2,
              crossAxisSpacing: 12,
              mainAxisSpacing: 12,
              childAspectRatio: 1.6,
            ),
            itemCount: _tickerData.length,
            itemBuilder: (context, index) {
              final item = _tickerData[index];
              final priceVal = item['regularMarketPrice'] ?? 0.0;
              final priceStr = priceVal is num ? priceVal.toStringAsFixed(2) : priceVal.toString();
              
              return Card(
                elevation: 2,
                shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                child: Padding(
                  padding: const EdgeInsets.all(12.0),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Text(
                        item['symbol'] ?? '',
                        style: const TextStyle(fontWeight: FontWeight.bold, fontSize: 13, color: Colors.grey),
                      ),
                      const SizedBox(height: 8),
                      Text(
                        priceStr,
                        style: const TextStyle(fontWeight: FontWeight.bold, fontSize: 18),
                      ),
                    ],
                  ),
                ),
              );
            },
          ),
          const SizedBox(height: 28),

          // 2. Positions & Holdings Title
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(
                "Flattrade Active Holdings",
                style: Theme.of(context).textTheme.titleLarge?.copyWith(fontWeight: FontWeight.bold),
              ),
              const Icon(Icons.shield_outlined, color: Colors.green, size: 20),
            ],
          ),
          const SizedBox(height: 12),

          // Holdings Table
          Container(
            decoration: BoxDecoration(
              color: isDark ? Colors.grey[900] : Colors.white,
              borderRadius: BorderRadius.circular(12),
              border: Border.all(color: isDark ? Colors.grey[800]! : Colors.grey[300]!),
            ),
            clipBehavior: Clip.antiAlias,
            child: Table(
              columnWidths: const {
                0: FlexColumnWidth(2.5), // Symbol
                1: FlexColumnWidth(1.2), // Qty
                2: FlexColumnWidth(2.0), // Buy Avg
                3: FlexColumnWidth(2.0), // LTP
                4: FlexColumnWidth(2.0), // P&L
              },
              children: [
                // Header Row
                TableRow(
                  decoration: BoxDecoration(
                    color: isDark ? Colors.grey[850] : Colors.grey[100],
                  ),
                  children: const [
                    Padding(padding: EdgeInsets.all(10.0), child: Text("Symbol", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 11))),
                    Padding(padding: EdgeInsets.all(10.0), child: Text("Qty", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 11))),
                    Padding(padding: EdgeInsets.all(10.0), child: Text("Avg", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 11))),
                    Padding(padding: EdgeInsets.all(10.0), child: Text("LTP", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 11))),
                    Padding(padding: EdgeInsets.all(10.0), child: Text("P&L", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 11))),
                  ],
                ),
                // Data Rows
                ..._holdings.map((h) {
                  final double pnlVal = h["pnl"];
                  final Color pnlColor = pnlVal >= 0 ? Colors.green : Colors.red;
                  final String pnlSign = pnlVal >= 0 ? "+" : "";
                  
                  return TableRow(
                    children: [
                      Padding(padding: const EdgeInsets.all(10.0), child: Text(h["symbol"], style: const TextStyle(fontWeight: FontWeight.w600, fontSize: 12))),
                      Padding(padding: const EdgeInsets.all(10.0), child: Text(h["qty"].toString(), style: const TextStyle(fontSize: 12))),
                      Padding(padding: const EdgeInsets.all(10.0), child: Text(h["avgPrice"].toStringAsFixed(2), style: const TextStyle(fontSize: 12))),
                      Padding(padding: const EdgeInsets.all(10.0), child: Text(h["ltp"].toStringAsFixed(2), style: const TextStyle(fontSize: 12))),
                      Padding(
                        padding: const EdgeInsets.all(10.0), 
                        child: Text(
                          "$pnlSign${pnlVal.toStringAsFixed(2)}", 
                          style: TextStyle(color: pnlColor, fontWeight: FontWeight.bold, fontSize: 12)
                        ),
                      ),
                    ],
                  );
                }).toList(),
              ],
            ),
          ),
          const SizedBox(height: 20),
          const Center(
            child: Text(
              "Positions fetched via Flattrade SDK callback",
              style: TextStyle(fontSize: 11, color: Colors.grey, fontStyle: FontStyle.italic),
            ),
          )
        ],
      ),
    );
  }

  Widget _buildSettingsView() {
    final isDark = Theme.of(context).brightness == Brightness.dark;
    final String customId = _userProfile['customId'] ?? 'FGAT0001';
    final String email = _userProfile['email'] ?? '';
    
    return _isLoadingProfile
        ? const Center(child: CircularProgressIndicator())
        : ListView(
            padding: const EdgeInsets.all(16.0),
            children: [
              // 1. Profile Display Header
              Card(
                elevation: 3,
                shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
                child: Padding(
                  padding: const EdgeInsets.all(20.0),
                  child: Column(
                    children: [
                      _buildProfileImage(_userProfile['profilePic']),
                      const SizedBox(height: 16),
                      Text(
                        _userProfile['fullName'] ?? 'User Profile',
                        style: const TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
                      ),
                      const SizedBox(height: 4),
                      Text(
                        email,
                        style: TextStyle(color: Colors.grey[600], fontSize: 13),
                      ),
                      const SizedBox(height: 10),
                      Container(
                        padding: const EdgeInsets.symmetric(horizontal: 14, vertical: 6),
                        decoration: BoxDecoration(
                          color: Colors.deepPurple.withOpacity(0.15),
                          borderRadius: BorderRadius.circular(20),
                        ),
                        child: Text(
                          "User ID: $customId",
                          style: const TextStyle(
                            color: Colors.deepPurple,
                            fontWeight: FontWeight.bold,
                            fontSize: 13,
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(height: 16),

              // 2. Profile Details Edit Card
              Card(
                elevation: 2,
                shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                child: Padding(
                  padding: const EdgeInsets.all(16.0),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      const Row(
                        children: [
                          Icon(Icons.edit_note, color: Colors.deepPurple),
                          SizedBox(width: 8),
                          Text("Edit Profile Details", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 15)),
                        ],
                      ),
                      const Divider(height: 20),
                      
                      // Full Name
                      const Text("Display Name", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 12, color: Colors.grey)),
                      const SizedBox(height: 6),
                      TextField(
                        controller: _nameController,
                        decoration: const InputDecoration(
                          border: OutlineInputBorder(),
                          contentPadding: EdgeInsets.symmetric(horizontal: 10, vertical: 8),
                        ),
                      ),
                      const SizedBox(height: 16),

                      // DOB
                      const Text("Date of Birth", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 12, color: Colors.grey)),
                      const SizedBox(height: 6),
                      Row(
                        children: [
                          Expanded(
                            child: TextField(
                              controller: _dobController,
                              readOnly: true,
                              decoration: const InputDecoration(
                                border: OutlineInputBorder(),
                                hintText: "Select DOB",
                                contentPadding: EdgeInsets.symmetric(horizontal: 10, vertical: 8),
                              ),
                            ),
                          ),
                          const SizedBox(width: 8),
                          IconButton(
                            icon: const Icon(Icons.calendar_month, color: Colors.deepPurple),
                            onPressed: () => _selectDate(context),
                          ),
                        ],
                      ),
                      const SizedBox(height: 16),

                      // Profile Pic URL
                      const Text("Profile Photo URL / Base64", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 12, color: Colors.grey)),
                      const SizedBox(height: 6),
                      TextField(
                        controller: _profilePicController,
                        decoration: const InputDecoration(
                          border: OutlineInputBorder(),
                          hintText: "Enter Image URL or raw base64 string",
                          contentPadding: EdgeInsets.symmetric(horizontal: 10, vertical: 8),
                        ),
                      ),
                      const SizedBox(height: 16),

                      SizedBox(
                        width: double.infinity,
                        child: ElevatedButton(
                          onPressed: () async {
                            final success = await _updateProfileOnBackend({
                              "action": "update_profile",
                              "fullName": _nameController.text,
                              "dob": _dobController.text.isNotEmpty ? _dobController.text : null,
                              "profilePic": _profilePicController.text,
                            });
                            if (success) {
                              _showSuccess("Profile details updated successfully!");
                            }
                          },
                          style: ElevatedButton.styleFrom(backgroundColor: Colors.deepPurple, foregroundColor: Colors.white),
                          child: const Text("Save Details"),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(height: 16),

              // 3. Security & Actions Card
              Card(
                elevation: 2,
                shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                child: Padding(
                  padding: const EdgeInsets.all(16.0),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      const Row(
                        children: [
                          Icon(Icons.security, color: Colors.deepPurple),
                          SizedBox(width: 8),
                          Text("Security & Theme", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 15)),
                        ],
                      ),
                      const Divider(height: 20),
                      
                      Row(
                        mainAxisAlignment: MainAxisAlignment.spaceBetween,
                        children: [
                          const Text("Dark Mode Theme", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 13)),
                          Switch(
                            value: isDark,
                            activeColor: Colors.deepPurple,
                            onChanged: (val) {
                              themeNotifier.value = val ? ThemeMode.dark : ThemeMode.light;
                            },
                          )
                        ],
                      ),
                      const SizedBox(height: 12),
                      
                      SizedBox(
                        width: double.infinity,
                        child: OutlinedButton.icon(
                          onPressed: _showChangePasswordDialog,
                          icon: const Icon(Icons.lock_open),
                          label: const Text("Change Password"),
                          style: OutlinedButton.styleFrom(foregroundColor: Colors.deepPurple),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
              const SizedBox(height: 16),

              // 4. Bank Accounts Section
              Card(
                elevation: 2,
                shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                child: Padding(
                  padding: const EdgeInsets.all(16.0),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Row(
                        mainAxisAlignment: MainAxisAlignment.spaceBetween,
                        children: [
                          const Row(
                            children: [
                              Icon(Icons.account_balance, color: Colors.deepPurple),
                              SizedBox(width: 8),
                              Text("Bank Accounts", style: TextStyle(fontWeight: FontWeight.bold, fontSize: 15)),
                            ],
                          ),
                          IconButton(
                            icon: const Icon(Icons.add_circle, color: Colors.green),
                            onPressed: () => _showAccountDialog(),
                          ),
                        ],
                      ),
                      const Divider(height: 20),
                      
                      _bankAccounts.isEmpty
                          ? const Padding(
                              padding: EdgeInsets.symmetric(vertical: 12.0),
                              child: Text(
                                "No registered bank accounts found. Use the + icon to add a new account.",
                                style: TextStyle(fontStyle: FontStyle.italic, fontSize: 12, color: Colors.grey),
                              ),
                            )
                          : ListView.builder(
                              shrinkWrap: true,
                              physics: const NeverScrollableScrollPhysics(),
                              itemCount: _bankAccounts.length,
                              itemBuilder: (context, index) {
                                final acc = _bankAccounts[index];
                                final name = acc['accountName'] ?? 'Account';
                                final type = acc['accountType'] ?? 'bank';
                                final bal = (acc['currentBalance'] ?? 0.0) as num;
                                final provider = acc['providerName'] ?? 'General';
                                final last4 = acc['accountNumberLast4'] ?? '';
                                
                                return Container(
                                  margin: const EdgeInsets.symmetric(vertical: 4),
                                  padding: const EdgeInsets.all(10),
                                  decoration: BoxDecoration(
                                    color: isDark ? Colors.grey[850] : Colors.grey[100],
                                    borderRadius: BorderRadius.circular(8),
                                  ),
                                  child: Row(
                                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                                    children: [
                                      Expanded(
                                        child: Column(
                                          crossAxisAlignment: CrossAxisAlignment.start,
                                          children: [
                                            Text(
                                              "$name (${type.toString().toUpperCase()})",
                                              style: const TextStyle(fontWeight: FontWeight.bold, fontSize: 13),
                                            ),
                                            const SizedBox(height: 2),
                                            Text(
                                              "$provider${last4.isNotEmpty ? ' •••• $last4' : ''}",
                                              style: TextStyle(color: Colors.grey[600], fontSize: 11),
                                            ),
                                          ],
                                        ),
                                      ),
                                      Row(
                                        children: [
                                          Text(
                                            "₹${bal.toStringAsFixed(2)}",
                                            style: const TextStyle(fontWeight: FontWeight.bold, color: Colors.deepPurple, fontSize: 13),
                                          ),
                                          const SizedBox(width: 4),
                                          IconButton(
                                            icon: const Icon(Icons.edit, size: 16, color: Colors.blue),
                                            padding: EdgeInsets.zero,
                                            constraints: const BoxConstraints(),
                                            onPressed: () => _showAccountDialog(acc),
                                          ),
                                          const SizedBox(width: 8),
                                          IconButton(
                                            icon: const Icon(Icons.delete, size: 16, color: Colors.red),
                                            padding: EdgeInsets.zero,
                                            constraints: const BoxConstraints(),
                                            onPressed: () => _confirmDeleteAccount(acc['id']),
                                          ),
                                        ],
                                      )
                                    ],
                                  ),
                                );
                              },
                            ),
                    ],
                  ),
                ),
              ),
              const SizedBox(height: 16),
              
              // 5. Danger Zone Card (Factory Reset)
              Card(
                elevation: 2,
                color: Colors.red.withOpacity(isDark ? 0.05 : 0.02),
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(12),
                  side: BorderSide(color: Colors.red.withOpacity(0.3), width: 1),
                ),
                child: Padding(
                  padding: const EdgeInsets.all(16.0),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Row(
                        children: [
                          const Icon(Icons.warning_amber_rounded, color: Colors.red),
                          const SizedBox(width: 8),
                          Text(
                            "Danger Zone",
                            style: TextStyle(
                              fontWeight: FontWeight.bold,
                              fontSize: 15,
                              color: isDark ? Colors.red[300] : Colors.red[700],
                            ),
                          ),
                        ],
                      ),
                      const Divider(height: 20),
                      const Text(
                        "Reset Statistics",
                        style: TextStyle(fontWeight: FontWeight.bold, fontSize: 13, color: Colors.red),
                      ),
                      const SizedBox(height: 4),
                      Text(
                        "Permanently delete all expenses, incomes, investments, and history. Your account balances will be reset to 0. This cannot be undone.",
                        style: TextStyle(color: Colors.grey[600], fontSize: 11),
                      ),
                      const SizedBox(height: 12),
                      SizedBox(
                        width: double.infinity,
                        child: ElevatedButton(
                          onPressed: _showFactoryResetDialog,
                          style: ElevatedButton.styleFrom(
                            backgroundColor: Colors.red[700],
                            foregroundColor: Colors.white,
                          ),
                          child: const Text("Factory Reset Statistics"),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
            ],
          );
  }

  Widget _buildExpensesView() {
    final double totalExpenses = _expenses.fold<double>(0.0, (sum, item) => sum + (item['amount'] as num).toDouble());
    final double totalIncome = _incomes.fold<double>(0.0, (sum, item) => sum + (item['amount'] as num).toDouble());
    final double netBalance = totalIncome - totalExpenses;
    final isDark = Theme.of(context).brightness == Brightness.dark;

    return DefaultTabController(
      length: 5,
      child: Column(
        children: [
          Padding(
            padding: const EdgeInsets.all(12.0),
            child: Row(
              children: [
                Expanded(
                  child: Card(
                    elevation: 2,
                    shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                    child: Padding(
                      padding: const EdgeInsets.symmetric(vertical: 12.0, horizontal: 8.0),
                      child: Column(
                        children: [
                          const Text("Total Income", style: TextStyle(fontSize: 11, color: Colors.grey)),
                          const SizedBox(height: 4),
                          Text(
                            "₹${totalIncome.toStringAsFixed(2)}",
                            style: const TextStyle(fontSize: 14, fontWeight: FontWeight.bold, color: Colors.green),
                            overflow: TextOverflow.ellipsis,
                          ),
                        ],
                      ),
                    ),
                  ),
                ),
                Expanded(
                  child: Card(
                    elevation: 2,
                    shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                    child: Padding(
                      padding: const EdgeInsets.symmetric(vertical: 12.0, horizontal: 8.0),
                      child: Column(
                        children: [
                          const Text("Total Expense", style: TextStyle(fontSize: 11, color: Colors.grey)),
                          const SizedBox(height: 4),
                          Text(
                            "₹${totalExpenses.toStringAsFixed(2)}",
                            style: const TextStyle(fontSize: 14, fontWeight: FontWeight.bold, color: Colors.red),
                            overflow: TextOverflow.ellipsis,
                          ),
                        ],
                      ),
                    ),
                  ),
                ),
                Expanded(
                  child: Card(
                    elevation: 2,
                    shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                    child: Padding(
                      padding: const EdgeInsets.symmetric(vertical: 12.0, horizontal: 8.0),
                      child: Column(
                        children: [
                          const Text("Net Balance", style: TextStyle(fontSize: 11, color: Colors.grey)),
                          const SizedBox(height: 4),
                          Text(
                            "₹${netBalance.toStringAsFixed(2)}",
                            style: TextStyle(
                              fontSize: 14,
                              fontWeight: FontWeight.bold,
                              color: netBalance >= 0 ? Colors.greenAccent : Colors.redAccent,
                            ),
                            overflow: TextOverflow.ellipsis,
                          ),
                        ],
                      ),
                    ),
                  ),
                ),
              ],
            ),
          ),
          TabBar(
            isScrollable: true,
            labelColor: Colors.deepPurple,
            unselectedLabelColor: isDark ? Colors.white70 : Colors.black87,
            indicatorColor: Colors.deepPurple,
            tabs: const [
              Tab(text: "Expenses", icon: Icon(Icons.trending_down)),
              Tab(text: "Incomes", icon: Icon(Icons.trending_up)),
              Tab(text: "Card Spends", icon: Icon(Icons.credit_card)),
              Tab(text: "Trades P&L", icon: Icon(Icons.show_chart)),
              Tab(text: "Loans", icon: Icon(Icons.handshake)),
            ],
          ),
          Expanded(
            child: RefreshIndicator(
              onRefresh: _fetchTransactions,
              child: _isLoadingTransactions
                  ? const Center(child: CircularProgressIndicator())
                  : _transactionsError != null
                      ? ListView(
                          children: [
                            SizedBox(height: MediaQuery.of(context).size.height * 0.25),
                            const Icon(Icons.cloud_off, size: 64, color: Colors.grey),
                            const SizedBox(height: 16),
                            Center(
                              child: Padding(
                                padding: const EdgeInsets.symmetric(horizontal: 24.0),
                                child: Text(
                                  _transactionsError!,
                                  textAlign: TextAlign.center,
                                  style: const TextStyle(color: Colors.red),
                                ),
                              ),
                            ),
                          ],
                        )
                      : TabBarView(
                          children: [
                            // Expenses Tab
                            _expenses.isEmpty
                                ? const Center(child: Text("No expenses recorded yet."))
                                : ListView.builder(
                                    itemCount: _expenses.length,
                                    itemBuilder: (context, index) {
                                      final item = _expenses[index];
                                      final double amount = (item['amount'] as num).toDouble();
                                      final categoryName = item['category']?['name'] ?? 'Other';
                                      final paymentMethod = item['paymentMethod'] ?? 'cash';
                                      final accountName = item['account']?['accountName'] ?? item['card']?['cardName'] ?? 'Cash';
                                      final spentOnStr = formatDate(item['spentOn']);
                                      final notes = item['notes'] ?? '';

                                      IconData catIcon = Icons.shopping_bag;
                                      Color iconColor = Colors.orange;

                                      switch (categoryName.toLowerCase()) {
                                        case 'food':
                                        case 'snacks':
                                        case 'tea':
                                          catIcon = Icons.fastfood;
                                          iconColor = Colors.amber;
                                          break;
                                        case 'rapido':
                                        case 'bus':
                                        case 'train':
                                          catIcon = Icons.directions_transit;
                                          iconColor = Colors.blue;
                                          break;
                                      }

                                      return Card(
                                        margin: const EdgeInsets.symmetric(horizontal: 12.0, vertical: 6.0),
                                        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
                                        child: ListTile(
                                          leading: CircleAvatar(
                                            backgroundColor: iconColor.withOpacity(0.2),
                                            child: Icon(catIcon, color: iconColor),
                                          ),
                                          title: Text(categoryName, style: const TextStyle(fontWeight: FontWeight.bold)),
                                          subtitle: Column(
                                            crossAxisAlignment: CrossAxisAlignment.start,
                                            children: [
                                              Text("via ${paymentMethod.toUpperCase()} ($accountName)"),
                                              if (notes.isNotEmpty) Text(notes, style: const TextStyle(fontStyle: FontStyle.italic, fontSize: 11)),
                                              const SizedBox(height: 2),
                                              Text(spentOnStr, style: const TextStyle(fontSize: 10, color: Colors.grey)),
                                            ],
                                          ),
                                          trailing: Text(
                                            "- ₹${amount.toStringAsFixed(2)}",
                                            style: const TextStyle(color: Colors.red, fontWeight: FontWeight.bold, fontSize: 14),
                                          ),
                                        ),
                                      );
                                    },
                                  ),

                            // Incomes Tab
                            _incomes.isEmpty
                                ? const Center(child: Text("No income recorded yet."))
                                : ListView.builder(
                                    itemCount: _incomes.length,
                                    itemBuilder: (context, index) {
                                      final item = _incomes[index];
                                      final double amount = (item['amount'] as num).toDouble();
                                      final sourceName = item['incomeSource']?['sourceName'] ?? 'Salary';
                                      final accountName = item['account']?['accountName'] ?? 'Cash';
                                      final receivedOnStr = formatDate(item['receivedOn']);
                                      final notes = item['notes'] ?? '';

                                      return Card(
                                        margin: const EdgeInsets.symmetric(horizontal: 12.0, vertical: 6.0),
                                        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
                                        child: ListTile(
                                          leading: CircleAvatar(
                                            backgroundColor: Colors.green.withOpacity(0.2),
                                            child: const Icon(Icons.attach_money, color: Colors.green),
                                          ),
                                          title: Text(sourceName, style: const TextStyle(fontWeight: FontWeight.bold)),
                                          subtitle: Column(
                                            crossAxisAlignment: CrossAxisAlignment.start,
                                            children: [
                                              Text("Received in: $accountName"),
                                              if (notes.isNotEmpty) Text(notes, style: const TextStyle(fontStyle: FontStyle.italic, fontSize: 11)),
                                              const SizedBox(height: 2),
                                              Text(receivedOnStr, style: const TextStyle(fontSize: 10, color: Colors.grey)),
                                            ],
                                          ),
                                          trailing: Text(
                                            "+ ₹${amount.toStringAsFixed(2)}",
                                            style: const TextStyle(color: Colors.green, fontWeight: FontWeight.bold, fontSize: 14),
                                          ),
                                        ),
                                      );
                                    },
                                  ),

                            // Card Spends Tab
                            _cards.isEmpty
                                ? const Center(child: Text("No active credit cards."))
                                : ListView.builder(
                                    itemCount: _cards.length,
                                    itemBuilder: (context, index) {
                                      final item = _cards[index];
                                      final double limit = (item['creditLimit'] as num?)?.toDouble() ?? 0.0;
                                      final double outstanding = (item['currentOutstanding'] as num).toDouble();
                                      final double available = limit - outstanding;
                                      final cardName = item['cardName'] ?? 'Credit Card';
                                      final last4 = item['last4'] ?? 'xxxx';

                                      return Card(
                                        margin: const EdgeInsets.symmetric(horizontal: 12.0, vertical: 6.0),
                                        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
                                        child: Padding(
                                          padding: const EdgeInsets.all(12.0),
                                          child: Column(
                                            crossAxisAlignment: CrossAxisAlignment.start,
                                            children: [
                                              Row(
                                                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                                                children: [
                                                  Row(
                                                    children: [
                                                      const Icon(Icons.credit_card, color: Colors.deepPurple),
                                                      const SizedBox(width: 8),
                                                      Text(
                                                        "$cardName (***$last4)",
                                                        style: const TextStyle(fontWeight: FontWeight.bold, fontSize: 15),
                                                      ),
                                                    ],
                                                  ),
                                                  Text(
                                                    "₹${outstanding.toStringAsFixed(2)} Spent",
                                                    style: const TextStyle(color: Colors.red, fontWeight: FontWeight.bold),
                                                  ),
                                                ],
                                              ),
                                              const SizedBox(height: 10),
                                              LinearProgressIndicator(
                                                value: limit > 0 ? (outstanding / limit) : 0,
                                                backgroundColor: Colors.grey[300],
                                                color: Colors.deepPurple,
                                              ),
                                              const SizedBox(height: 8),
                                              Row(
                                                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                                                children: [
                                                  Text("Limit: ₹${limit.toStringAsFixed(2)}", style: const TextStyle(fontSize: 12, color: Colors.grey)),
                                                  Text("Available: ₹${available.toStringAsFixed(2)}", style: const TextStyle(fontSize: 12, color: Colors.green)),
                                                ],
                                              ),
                                            ],
                                          ),
                                        ),
                                      );
                                    },
                                  ),

                            // Trades P&L Tab
                            _investments.isEmpty
                                ? const Center(child: Text("No active trade investments."))
                                : ListView.builder(
                                    itemCount: _investments.length,
                                    itemBuilder: (context, index) {
                                      final item = _investments[index];
                                      final double quantity = (item['quantity'] as num).toDouble();
                                      final double buyPrice = (item['purchasePrice'] as num).toDouble();
                                      final double ltp = (item['prices'] != null && (item['prices'] as List).isNotEmpty)
                                          ? (item['prices'][0]['pricePerUnit'] as num).toDouble()
                                          : buyPrice;
                                      
                                      final double totalCost = quantity * buyPrice;
                                      final double currentValue = quantity * ltp;
                                      final double pnl = currentValue - totalCost;
                                      final double pnlPercent = totalCost > 0 ? (pnl / totalCost) * 100 : 0.0;

                                      final symbol = item['symbol'] ?? item['name'] ?? 'Asset';
                                      final pnlColor = pnl >= 0 ? Colors.green : Colors.red;

                                      return Card(
                                        margin: const EdgeInsets.symmetric(horizontal: 12.0, vertical: 6.0),
                                        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
                                        child: ListTile(
                                          leading: CircleAvatar(
                                            backgroundColor: pnlColor.withOpacity(0.2),
                                            child: Icon(pnl >= 0 ? Icons.trending_up : Icons.trending_down, color: pnlColor),
                                          ),
                                          title: Text(symbol, style: const TextStyle(fontWeight: FontWeight.bold)),
                                          subtitle: Text("Qty: $quantity | Buy: ₹${buyPrice.toStringAsFixed(2)} | LTP: ₹${ltp.toStringAsFixed(2)}"),
                                          trailing: Column(
                                            mainAxisAlignment: MainAxisAlignment.center,
                                            crossAxisAlignment: CrossAxisAlignment.end,
                                            children: [
                                              Text(
                                                "${pnl >= 0 ? '+' : ''}₹${pnl.toStringAsFixed(2)}",
                                                style: TextStyle(color: pnlColor, fontWeight: FontWeight.bold, fontSize: 14),
                                              ),
                                              Text(
                                                "${pnl >= 0 ? '+' : ''}${pnlPercent.toStringAsFixed(2)}%",
                                                style: TextStyle(color: pnlColor, fontSize: 11),
                                              ),
                                            ],
                                          ),
                                        ),
                                      );
                                    },
                                  ),

                            // Loans Tab
                            _loans.isEmpty
                                ? const Center(child: Text("No loans recorded yet."))
                                : ListView.builder(
                                    itemCount: _loans.length,
                                    itemBuilder: (context, index) {
                                      final item = _loans[index];
                                      final double principal = (item['principalAmount'] as num).toDouble();
                                      
                                      final double totalRepaid = (item['repayments'] as List?)?.fold<double>(0.0, (sum, r) => sum + ((r as Map)['amount'] as num).toDouble()) ?? 0.0;
                                      final double outstanding = principal - totalRepaid;
                                      
                                      final borrower = item['borrowerName'] ?? 'Borrower';
                                      final givenOnStr = formatDate(item['amountGivenOn']);
                                      final status = item['status'] ?? 'active';

                                      return Card(
                                        margin: const EdgeInsets.symmetric(horizontal: 12.0, vertical: 6.0),
                                        shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
                                        child: Padding(
                                          padding: const EdgeInsets.all(12.0),
                                          child: Column(
                                            crossAxisAlignment: CrossAxisAlignment.start,
                                            children: [
                                              Row(
                                                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                                                children: [
                                                  Text(borrower, style: const TextStyle(fontWeight: FontWeight.bold, fontSize: 15)),
                                                  Container(
                                                    padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
                                                    decoration: BoxDecoration(
                                                      color: status == 'closed' ? Colors.green.withOpacity(0.2) : Colors.orange.withOpacity(0.2),
                                                      borderRadius: BorderRadius.circular(6),
                                                    ),
                                                    child: Text(
                                                      status.toUpperCase(),
                                                      style: TextStyle(
                                                        color: status == 'closed' ? Colors.green : Colors.orange,
                                                        fontSize: 10,
                                                        fontWeight: FontWeight.bold,
                                                      ),
                                                    ),
                                                  ),
                                                ],
                                              ),
                                              const SizedBox(height: 8),
                                              Text("Principal: ₹${principal.toStringAsFixed(2)}", style: const TextStyle(fontSize: 12)),
                                              Text("Repaid: ₹${totalRepaid.toStringAsFixed(2)}", style: const TextStyle(fontSize: 12, color: Colors.green)),
                                              const Divider(height: 16),
                                              Row(
                                                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                                                children: [
                                                  Text(
                                                    "Outstanding: ₹${outstanding.toStringAsFixed(2)}",
                                                    style: const TextStyle(fontWeight: FontWeight.bold, color: Colors.red),
                                                  ),
                                                  Text("Given on: ${givenOnStr.split(' ')[0]}", style: const TextStyle(fontSize: 10, color: Colors.grey)),
                                                ],
                                              ),
                                            ],
                                          ),
                                        ),
                                      );
                                    },
                                  ),
                          ],
                        ),
            ),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    // Dynamically choose appBar title
    String titleText = "TradeBot AI Chat";
    if (_currentView == 'market') {
      titleText = "Live Market & Holdings";
    } else if (_currentView == 'settings') {
      titleText = "Settings";
    } else if (_currentView == 'expenses') {
      titleText = "Log";
    }

    return Scaffold(
      appBar: AppBar(
        title: Text(titleText),
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
              selected: _currentView == 'chat',
              selectedColor: Colors.deepPurple,
              onTap: () {
                Navigator.pop(context);
                setState(() {
                  _currentView = 'chat';
                });
              },
            ),
            ListTile(
              leading: const Icon(Icons.trending_up),
              title: const Text('Live Markets'),
              selected: _currentView == 'market',
              selectedColor: Colors.deepPurple,
              onTap: () {
                Navigator.pop(context);
                setState(() {
                  _currentView = 'market';
                });
              },
            ),
            ListTile(
              leading: const Icon(Icons.receipt_long),
              title: const Text('Log'),
              selected: _currentView == 'expenses',
              selectedColor: Colors.deepPurple,
              onTap: () {
                Navigator.pop(context);
                setState(() {
                  _currentView = 'expenses';
                });
                _fetchTransactions();
              },
            ),
            ListTile(
              leading: const Icon(Icons.settings),
              title: const Text('Settings'),
              selected: _currentView == 'settings',
              selectedColor: Colors.deepPurple,
              onTap: () {
                Navigator.pop(context);
                setState(() {
                  _currentView = 'settings';
                });
              },
            ),
            const Divider(),
            ListTile(
              leading: const Icon(Icons.logout, color: Colors.red),
              title: const Text('Logout', style: TextStyle(color: Colors.red)),
              onTap: () {
                Navigator.pop(context); // Close drawer
                showDialog(
                  context: context,
                  builder: (BuildContext context) {
                    return AlertDialog(
                      title: const Text('Confirm Logout'),
                      content: const Text('Are you sure you want to log out?'),
                      actions: [
                        TextButton(
                          onPressed: () => Navigator.pop(context), // Close dialog
                          child: const Text('Cancel'),
                        ),
                        ElevatedButton(
                          onPressed: () {
                            Navigator.pop(context); // Close dialog
                            Navigator.pushReplacement(
                              context,
                              MaterialPageRoute(builder: (context) => const LoginPage()),
                            );
                          },
                          style: ElevatedButton.styleFrom(
                            backgroundColor: Colors.red,
                            foregroundColor: Colors.white,
                          ),
                          child: const Text('Logout'),
                        ),
                      ],
                    );
                  },
                );
              },
            ),
          ],
        ),
      ),
      body: _currentView == 'chat'
          ? _buildChatView()
          : _currentView == 'market'
              ? _buildMarketView()
              : _currentView == 'settings'
                  ? _buildSettingsView()
                  : _buildExpensesView(),
    );
  }
}

class _TickerBar extends StatefulWidget {
  final List<dynamic> data;
  const _TickerBar({required this.data});

  @override
  State<_TickerBar> createState() => _TickerBarState();
}

class _TickerBarState extends State<_TickerBar> {
  final ScrollController _scrollController = ScrollController();

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addPostFrameCallback((_) => _startScrolling());
  }

  void _startScrolling() async {
    while (_scrollController.hasClients) {
      await Future.delayed(const Duration(milliseconds: 50));
      if (_scrollController.hasClients) {
        double maxScroll = _scrollController.position.maxScrollExtent;
        double currentScroll = _scrollController.offset;
        if (currentScroll >= maxScroll) {
          _scrollController.jumpTo(0);
        } else {
          _scrollController.animateTo(currentScroll + 1,
              duration: const Duration(milliseconds: 50), curve: Curves.linear);
        }
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    if (widget.data.isEmpty) return Container(height: 30, color: Colors.black, child: const Center(child: Text('Fetching Markets...', style: TextStyle(color: Colors.white, fontSize: 10))));

    return Container(
      height: 30,
      color: Colors.black,
      child: ListView.builder(
        controller: _scrollController,
        scrollDirection: Axis.horizontal,
        itemCount: 100,
        itemBuilder: (context, index) {
          final item = widget.data[index % widget.data.length];
          final symbol = item['symbol'] ?? 'Ticker';
          final priceVal = item['regularMarketPrice'] ?? 0.0;
          final priceStr = priceVal is num ? priceVal.toStringAsFixed(2) : priceVal.toString();
          final change = item['regularMarketChangePercent'] ?? 0.0;
          final color = (change >= 0) ? Colors.greenAccent : Colors.redAccent;

          return Padding(
            padding: const EdgeInsets.symmetric(horizontal: 15),
            child: Center(
              child: Text('$symbol: $priceStr (${change.toStringAsFixed(2)}%)',
                  style: TextStyle(color: color, fontSize: 12, fontWeight: FontWeight.bold)),
            ),
          );
        },
      ),
    );
  }
}

class _OtherOptionInput extends StatefulWidget {
  final bool isDark;
  final Function(String) onSubmit;

  const _OtherOptionInput({
    Key? key,
    required this.isDark,
    required this.onSubmit,
  }) : super(key: key);

  @override
  __OtherOptionInputState createState() => __OtherOptionInputState();
}

class __OtherOptionInputState extends State<_OtherOptionInput> {
  final TextEditingController _controller = TextEditingController();

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 140,
      height: 38,
      padding: const EdgeInsets.symmetric(horizontal: 10.0),
      decoration: BoxDecoration(
        color: widget.isDark ? Colors.deepPurple[900] : Colors.deepPurple[50],
        borderRadius: BorderRadius.circular(18),
        border: Border.all(color: Colors.deepPurple.withOpacity(0.5)),
      ),
      child: Row(
        children: [
          Expanded(
            child: TextField(
              controller: _controller,
              style: TextStyle(
                color: widget.isDark ? Colors.white : Colors.deepPurple,
                fontSize: 12,
                fontWeight: FontWeight.bold,
              ),
              decoration: InputDecoration(
                hintText: "Other...",
                hintStyle: TextStyle(
                  color: (widget.isDark ? Colors.white70 : Colors.deepPurple).withOpacity(0.5),
                  fontSize: 12,
                  fontWeight: FontWeight.normal,
                ),
                border: InputBorder.none,
                isDense: true,
                contentPadding: EdgeInsets.zero,
              ),
              onSubmitted: (val) {
                if (val.trim().isNotEmpty) {
                  widget.onSubmit(val.trim());
                }
              },
            ),
          ),
          const SizedBox(width: 4),
          GestureDetector(
            child: Icon(Icons.send, size: 14, color: widget.isDark ? Colors.white70 : Colors.deepPurple),
            onTap: () {
              if (_controller.text.trim().isNotEmpty) {
                widget.onSubmit(_controller.text.trim());
              }
            },
          ),
        ],
      ),
    );
  }
}