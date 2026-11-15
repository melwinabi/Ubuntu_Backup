from flask import Flask, request, jsonify
import re
import requests
import os

# Flask app initialize
app = Flask(__name__)

# Try to import google.generativeai for Gemini support.
try:
    import google.generativeai as genai
    HAS_GEMINI = True
except ImportError:
    HAS_GEMINI = False

# Global state for pending transactions awaiting selections
pending_amount = None
pending_stage = None
pending_type = None
pending_category = None
pending_accounts = []

# Enable CORS manually
@app.after_request
def after_request(response):
    response.headers.add('Access-Control-Allow-Origin', '*')
    response.headers.add('Access-Control-Allow-Headers', 'Content-Type,Authorization')
    response.headers.add('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE,OPTIONS')
    return response

# Basic Home Page
@app.route('/')
def home():
    return "<h1>AI Trading Bot Server is Running locally!</h1>"

# Flattrade API Redirect URL / Callback Endpoint
@app.route('/callback')
def callback():
    request_token = request.args.get('code')
    if request_token:
        print(f"\n--- SUCCESS! Request Token received: {request_token} ---\n")
        return "<h2>Login Successful! Flattrade Token received in Terminal. You can close this tab.</h2>"
    else:
        return "<h2>Error: No token received from Flattrade.</h2>"

# Chatbot transaction logging API
@app.route('/api/chat', methods=['POST'])
def chat():
    global pending_amount, pending_stage, pending_type, pending_category, pending_accounts
    data = request.json or {}
    user_message = data.get('message', '').strip()
    
    if not user_message:
        return jsonify({"success": False, "reply": "Please enter a message."}), 400

    # 1. Stage 2: Awaiting Category selection (for Expense)
    if pending_stage == "AWAITING_CATEGORY" and pending_amount is not None:
        category_options = ["Food", "Rapido", "Snacks", "Tea", "Train", "Bus", "Other"]
        matched_cat = None
        for cat in category_options:
            if cat.lower() == user_message.lower():
                matched_cat = cat
                break
        
        if not matched_cat:
            matched_cat = user_message.capitalize()
            
        pending_category = matched_cat
        
        # Now fetch registered bank accounts from Next.js API
        try:
            accounts_res = requests.get("http://localhost:3000/api/accounts", timeout=5)
            if accounts_res.status_code == 200:
                accounts = accounts_res.json().get("accounts", [])
                
                if not accounts:
                    # Cancel state
                    pending_stage = None
                    pending_amount = None
                    pending_category = None
                    pending_type = None
                    return jsonify({
                        "success": True,
                        "reply": f"Expense type set to **'{pending_category}'**. But you have no registered bank accounts in the web application. Please add an account in Settings first."
                    })
                
                pending_accounts = accounts
                pending_stage = "AWAITING_ACCOUNT"
                
                options = [acc['accountName'] for acc in accounts]
                reply = f"Expense type set to **'{pending_category}'**.\n\nWhich bank account did you pay this from? Please select from below:"
                return jsonify({
                    "success": True,
                    "reply": reply,
                    "options": options
                })
            else:
                # Cancel state
                pending_stage = None
                pending_amount = None
                pending_category = None
                pending_type = None
                return jsonify({
                    "success": True,
                    "reply": "❌ Failed to fetch accounts list from Next.js server. Please ensure the web application is running."
                })
        except Exception as e:
            # Cancel state
            pending_stage = None
            pending_amount = None
            pending_category = None
            pending_type = None
            return jsonify({
                "success": True,
                "reply": f"❌ Error connecting to Next.js server: {e}"
            })

    # 2. Stage 3: Awaiting Account selection (for Expense)
    if pending_stage == "AWAITING_ACCOUNT" and pending_amount is not None and pending_category is not None:
        selected_account = None
        for acc in pending_accounts:
            if acc['accountName'].lower() == user_message.lower():
                selected_account = acc
                break
                
        if selected_account:
            acc_name = selected_account['accountName']
            
            # Save transaction to Next.js DB
            transaction_payload = {
                "type": "EXPENSE",
                "amount": pending_amount,
                "paymentMethod": "cash",
                "categoryName": pending_category,
                "accountName": acc_name,
                "notes": f"Paid ₹{pending_amount} for {pending_category}"
            }
            
            try:
                nextjs_url = "http://localhost:3000/api/transactions"
                res = requests.post(nextjs_url, json=transaction_payload, timeout=10)
                
                if res.status_code == 200:
                    reply = f"✅ Recorded Expense of ₹{pending_amount:.2f} under '{pending_category}' category paid from: **{acc_name}**."
                else:
                    err_msg = res.json().get("error", "Unknown database error")
                    reply = f"❌ Failed to save transaction: {err_msg}"
            except Exception as e:
                reply = f"❌ Next.js database server not reachable. Please ensure Next.js is running on port 3000. (Error: {e})"
            
            # Reset all pending state
            pending_stage = None
            pending_amount = None
            pending_category = None
            pending_type = None
            pending_accounts = []
            return jsonify({"success": True, "reply": reply})
        else:
            options = [acc['accountName'] for acc in pending_accounts]
            reply = f"Invalid account: '{user_message}'. Please select one of your registered bank accounts from below:"
            return jsonify({
                "success": True,
                "reply": reply,
                "options": options
            })

    # 3. Stage 4: Awaiting Income Source selection
    if pending_stage == "AWAITING_INCOME_SOURCE" and pending_amount is not None:
        source_options = ["Salary", "Freelance", "Investment", "Gift", "Other"]
        matched_source = None
        for src in source_options:
            if src.lower() == user_message.lower():
                matched_source = src
                break
        
        if not matched_source:
            matched_source = user_message.capitalize()
            
        pending_category = matched_source
        
        # Now fetch registered bank accounts from Next.js API
        try:
            accounts_res = requests.get("http://localhost:3000/api/accounts", timeout=5)
            if accounts_res.status_code == 200:
                accounts = accounts_res.json().get("accounts", [])
                
                if not accounts:
                    # Cancel state
                    pending_stage = None
                    pending_amount = None
                    pending_category = None
                    pending_type = None
                    return jsonify({
                        "success": True,
                        "reply": f"Income source set to **'{pending_category}'**. But you have no registered bank accounts in the web application. Please add an account in Settings first."
                    })
                
                pending_accounts = accounts
                pending_stage = "AWAITING_INCOME_ACCOUNT"
                
                options = [acc['accountName'] for acc in accounts]
                reply = f"Income source set to **'{pending_category}'**.\n\nWhich bank account did you receive this in? Please select from below:"
                return jsonify({
                    "success": True,
                    "reply": reply,
                    "options": options
                })
            else:
                # Cancel state
                pending_stage = None
                pending_amount = None
                pending_category = None
                pending_type = None
                return jsonify({
                    "success": True,
                    "reply": "❌ Failed to fetch accounts list from Next.js server. Please ensure the web application is running."
                })
        except Exception as e:
            # Cancel state
            pending_stage = None
            pending_amount = None
            pending_category = None
            pending_type = None
            return jsonify({
                "success": True,
                "reply": f"❌ Error connecting to Next.js server: {e}"
            })

    # 4. Stage 5: Awaiting Income Account selection
    if pending_stage == "AWAITING_INCOME_ACCOUNT" and pending_amount is not None and pending_category is not None:
        selected_account = None
        for acc in pending_accounts:
            if acc['accountName'].lower() == user_message.lower():
                selected_account = acc
                break
                
        if selected_account:
            acc_name = selected_account['accountName']
            
            # Save transaction to Next.js DB
            transaction_payload = {
                "type": "INCOME",
                "amount": pending_amount,
                "paymentMethod": "cash",
                "sourceName": pending_category,
                "accountName": acc_name,
                "notes": f"Received ₹{pending_amount} from {pending_category}"
            }
            
            try:
                nextjs_url = "http://localhost:3000/api/transactions"
                res = requests.post(nextjs_url, json=transaction_payload, timeout=10)
                
                if res.status_code == 200:
                    reply = f"✅ Recorded Income of ₹{pending_amount:.2f} from '{pending_category}' received in: **{acc_name}**."
                else:
                    err_msg = res.json().get("error", "Unknown database error")
                    reply = f"❌ Failed to save transaction: {err_msg}"
            except Exception as e:
                reply = f"❌ Next.js database server not reachable. Please ensure Next.js is running on port 3000. (Error: {e})"
            
            # Reset all pending state
            pending_stage = None
            pending_amount = None
            pending_category = None
            pending_type = None
            pending_accounts = []
            return jsonify({"success": True, "reply": reply})
        else:
            options = [acc['accountName'] for acc in pending_accounts]
            reply = f"Invalid account: '{user_message}'. Please select one of your registered bank accounts from below:"
            return jsonify({
                "success": True,
                "reply": reply,
                "options": options
            })

    # 5. Check if the message starts with 'P' or 'p' followed by a number (EXPENSE)
    p_match = re.match(r'^[Pp]\s*(\d+(?:\.\d{1,2})?)$', user_message)
    if p_match:
        amount = float(p_match.group(1))
        
        # Set stage to Awaiting Category
        pending_amount = amount
        pending_type = "EXPENSE"
        pending_stage = "AWAITING_CATEGORY"
        pending_category = None
        pending_accounts = []
        
        category_options = ["Food", "Rapido", "Snacks", "Tea", "Train", "Bus", "Other"]
        reply = f"I've recorded a paid amount of **₹{amount:.2f}**.\n\nPlease select the Expense Type:"
        return jsonify({
            "success": True,
            "reply": reply,
            "options": category_options
        })

    # 6. Check if the message starts with 'E' or 'e' followed by a number (INCOME)
    e_match = re.match(r'^[Ee]\s*(\d+(?:\.\d{1,2})?)$', user_message)
    if e_match:
        amount = float(e_match.group(1))
        
        # Set stage to Awaiting Income Source
        pending_amount = amount
        pending_type = "INCOME"
        pending_stage = "AWAITING_INCOME_SOURCE"
        pending_category = None
        pending_accounts = []
        
        source_options = ["Salary", "Freelance", "Investment", "Gift", "Other"]
        reply = f"I've recorded an income amount of **₹{amount:.2f}**.\n\nPlease select the Income Source:"
        return jsonify({
            "success": True,
            "reply": reply,
            "options": source_options
        })

    # 4. Try to parse using Gemini if API key is set
    gemini_key = os.environ.get("GEMINI_API_KEY")
    parsed_data = None
    ai_reply = ""

    if HAS_GEMINI and gemini_key:
        try:
            genai.configure(api_key=gemini_key)
            model = genai.GenerativeModel('gemini-pro')
            prompt = f"""
            You are a financial helper assistant. Extract transaction details from this message: "{user_message}".
            Respond ONLY with a raw JSON object and nothing else. No markdown formatting.
            The JSON object should have:
            - "type": "EXPENSE" or "INCOME" (if it looks like money was spent or earned)
            - "amount": the number value (float)
            - "paymentMethod": one of: "cash", "upi", "credit_card", "debit_card", "net_banking", "other"
            - "categoryName": the category of expense (e.g., "Food", "Travel", "Fuel", "Shopping", "Entertainment", "Medical", "Bills", "other")
            - "sourceName": the source of income (e.g., "Salary", "Freelance", "Gift")
            - "accountName": name of the bank or card if mentioned (e.g., "SBI", "HDFC", "Cash")
            - "cardName": name of the credit card if mentioned and payment method is credit_card
            - "merchant": the place or person the money was paid to
            - "notes": description of the transaction
            
            If the message is not a transaction logging request (like a normal greeting or query), return an empty JSON object {{}}.
            """
            response = model.generate_content(prompt)
            resp_text = response.text.strip()
            
            if resp_text.startswith("```"):
                resp_text = re.sub(r"^```json\s*|```$", "", resp_text, flags=re.MULTILINE).strip()
            
            import json
            parsed_data = json.loads(resp_text)
        except Exception as e:
            print(f"Gemini AI error, falling back to Rules: {e}")
            parsed_data = None

    # 5. Fallback to Rule-based NLP parsing if Gemini failed or is not configured
    if not parsed_data or not parsed_data.get("amount"):
        parsed_data = parse_message_rules(user_message)

    # 6. If it is a transaction, save it in Next.js DB
    if parsed_data and parsed_data.get("amount"):
        try:
            nextjs_url = "http://localhost:3000/api/transactions"
            res = requests.post(nextjs_url, json=parsed_data, timeout=10)
            
            if res.status_code == 200:
                t_type = parsed_data["type"]
                amt = parsed_data["amount"]
                method = parsed_data.get("paymentMethod", "cash")
                
                if t_type == "EXPENSE":
                    cat = parsed_data.get("categoryName") or "Other"
                    ai_reply = f"✅ Recorded Expense of ₹{amt:.2f} under '{cat}' category using {method}."
                else:
                    src = parsed_data.get("sourceName") or "Salary"
                    ai_reply = f"✅ Recorded Income of ₹{amt:.2f} from '{src}'."
            else:
                err_msg = res.json().get("error", "Unknown database error")
                ai_reply = f"❌ Failed to save transaction: {err_msg}"
        except Exception as e:
            ai_reply = f"❌ Next.js database server not reachable. Please ensure Next.js is running on port 3000. (Error: {e})"
    else:
        # Default greeting/conversational response
        ai_reply = (
            "Hello! I am your Finance Folio Chatbot.\n\n"
            "I can automatically log your expenses and incomes. Try typing:\n"
            "• 'Spent 450 for Food on SBI Card'\n"
            "• 'Received 12000 salary from office'\n"
            "• 'Expense of 120 for fuel with UPI'"
        )

    return jsonify({
        "success": True,
        "reply": ai_reply
    })

def parse_message_rules(msg):
    """Fallback rule-based parser using RegEx"""
    msg_lower = msg.lower()
    
    # 1. Match amount (e.g. 500, Rs. 500, ₹500, 500 rupees)
    amt_match = re.search(r'(?:rs\.?|rupees?|₹|\$)?\s*(\d+(?:\.\d{1,2})?)\s*(?:rs|rupees)?', msg_lower)
    if not amt_match:
        return {}
    
    amount = float(amt_match.group(1))
    
    # 2. Match transaction type
    is_income = any(word in msg_lower for word in ["received", "earned", "income", "salary", "bonus", "added", "credited"])
    t_type = "INCOME" if is_income else "EXPENSE"
    
    # 3. Match payment method
    payment_method = "cash"
    if "card" in msg_lower or "credit" in msg_lower:
        payment_method = "credit_card"
    elif "upi" in msg_lower or "gpay" in msg_lower or "phonepe" in msg_lower or "paytm" in msg_lower:
        payment_method = "upi"
    elif "bank" in msg_lower or "net banking" in msg_lower:
        payment_method = "net_banking"
    
    # 4. Bank / Card Names
    account_name = None
    card_name = None
    for bank in ["sbi", "hdfc", "icici", "axis", "kotak"]:
        if bank in msg_lower:
            if payment_method == "credit_card":
                card_name = bank.upper()
            else:
                account_name = bank.upper()
                
    # 5. Extract category or source
    category_name = "Other"
    source_name = "Other"
    
    for_on_match = re.search(r'(?:on|for|from|to)\s+([a-zA-Z0-9\s]+)', msg_lower)
    if for_on_match:
        extracted = for_on_match.group(1).split()[0].capitalize()
        if extracted.lower() not in ["sbi", "hdfc", "icici", "axis", "upi", "card", "cash"]:
            if t_type == "EXPENSE":
                category_name = extracted
            else:
                source_name = extracted

    if t_type == "INCOME":
        if "salary" in msg_lower:
            source_name = "Salary"
        elif "freelance" in msg_lower:
            source_name = "Freelance"

    result = {
        "type": t_type,
        "amount": amount,
        "paymentMethod": payment_method,
        "notes": msg
    }
    
    if t_type == "EXPENSE":
        result["categoryName"] = category_name
        if payment_method == "credit_card":
            result["cardName"] = card_name
        else:
            result["accountName"] = account_name
    else:
        result["sourceName"] = source_name
        result["accountName"] = account_name
        
    return result

if __name__ == '__main__':
    print("Starting AI Trading Bot Server... Listening on port 5000.")
    app.run(host='0.0.0.0', port=5000, debug=True)