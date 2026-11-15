import requests
import sqlite3

def test_nextjs_chat():
    url = "http://localhost:3000/api/chat"
    
    # Get user id from database
    try:
        conn = sqlite3.connect("prisma/dev.db")
        cursor = conn.cursor()
        cursor.execute("SELECT id FROM users LIMIT 1")
        row = cursor.fetchone()
        if row:
            user_id = row[0]
            print("Found user ID in db:", user_id)
        else:
            print("No user found in DB!")
            return
        conn.close()
    except Exception as e:
        print("DB connection error:", e)
        return

    # 1. Test Shortcut for Expense: P900
    payload = {
        "userId": user_id,
        "message": "P900"
    }
    res = requests.post(url, json=payload)
    print("Response (P900):", res.status_code, res.json())

    # 2. Test Stage 1: Select Category 'Food'
    payload = {
        "userId": user_id,
        "message": "Food"
    }
    res = requests.post(url, json=payload)
    print("Response (Awaiting Category -> select Food):", res.status_code, res.json())

    # 3. Test Stage 2: Select Account 'Cash' (or first account in options)
    options = res.json().get("options", [])
    if options:
        payload = {
            "userId": user_id,
            "message": options[0]
        }
        res = requests.post(url, json=payload)
        print("Response (Awaiting Account -> select Account):", res.status_code, res.json())
    else:
        print("No accounts available to select.")

    # 4. Test Shortcut for Income: E1500
    payload = {
        "userId": user_id,
        "message": "E1500"
    }
    res = requests.post(url, json=payload)
    print("Response (E1500):", res.status_code, res.json())

    # 5. Test Stage 3: Select Income Source 'Salary'
    payload = {
        "userId": user_id,
        "message": "Salary"
    }
    res = requests.post(url, json=payload)
    print("Response (Awaiting Income Source -> select Salary):", res.status_code, res.json())

    # 6. Test Stage 4: Select Income Account (first in options)
    options = res.json().get("options", [])
    if options:
        payload = {
            "userId": user_id,
            "message": options[0]
        }
        res = requests.post(url, json=payload)
        print("Response (Awaiting Income Account -> select Account):", res.status_code, res.json())
    else:
        print("No accounts available to select.")

if __name__ == "__main__":
    test_nextjs_chat()
