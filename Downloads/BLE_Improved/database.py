"""
UWB Distance Data Logger
Subscribes to MQTT on port 1883 and saves each reading as a JSON file in real time.

Two layers of authentication:
  1. User login prompt (controls who can run the logger)
  2. MQTT broker credentials (sent to Mosquitto to authorize connection)

Install dependency:
  pip install paho-mqtt
"""

import json
import os
import getpass
from datetime import datetime
import paho.mqtt.client as mqtt

# ========== CONFIG ==========
MQTT_BROKER  = "192.168.220.6"
MQTT_PORT    = 1883
MQTT_TOPIC   = "uwb/distance"
DATA_DIR     = "data"

# --- User login credentials (who can run this logger) ---
# REPLACE with your chosen username and password
APP_USERNAME = "GowthamS"
APP_PASSWORD = "GowthamS4516"

# --- MQTT broker credentials (must match Mosquitto passwd file) ---
# REPLACE with the credentials you created with mosquitto_passwd
MQTT_USERNAME = "GowthamS"
MQTT_PASSWORD = "GowthamS4516"
# ============================

def get_save_path():
    today = datetime.now().strftime("%Y-%m-%d")
    folder = os.path.join(DATA_DIR, today)
    os.makedirs(folder, exist_ok=True)
    return folder

def save_record(payload: dict):
    now = datetime.now()
    timestamp = now.strftime("%H-%M-%S-%f")[:-3]

    record = {
        "timestamp": now.isoformat(),
        "distance":  payload.get("distance", payload.get("dist", None)),
        "raw":       payload
    }

    folder = get_save_path()
    filepath = os.path.join(folder, f"{timestamp}.json")

    with open(filepath, "w") as f:
        json.dump(record, f, indent=2)

    print(f"[{now.strftime('%H:%M:%S.%f')[:-3]}] Saved → {filepath}  |  distance: {record['distance']} m")

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print(f"Connected to MQTT broker {MQTT_BROKER}:{MQTT_PORT}")
        client.subscribe(MQTT_TOPIC)
        print(f"Subscribed to: {MQTT_TOPIC}")
        print(f"Saving to:     {os.path.abspath(DATA_DIR)}")
        print("-" * 60)
    elif reason_code == 4 or reason_code == 5:
        print("MQTT authentication failed — wrong broker username or password.")
        client.disconnect()
    else:
        print(f"Connection failed, code: {reason_code}")

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode("utf-8"))
        save_record(payload)
    except json.JSONDecodeError as e:
        print(f"JSON parse error: {e} | raw: {msg.payload}")
    except Exception as e:
        print(f"Error saving record: {e}")

def on_disconnect(client, userdata, flags, reason_code, properties):
    if reason_code != 0:
        print("Unexpected disconnect — will auto-reconnect...")

def prompt_login():
    """Ask user to enter credentials at runtime."""
    print("Enter credentials to access the database:")
    username = input("Username: ").strip()
    password = getpass.getpass("Password: ").strip()
    return username, password

if __name__ == "__main__":
    print("=" * 60)
    print("  UWB Distance Data Logger")
    print("=" * 60)

    # Step 1: App-level login
    username, password = prompt_login()
    if username != APP_USERNAME or password != APP_PASSWORD:
        print("Access denied — incorrect username or password.")
        exit(1)

    print("Access granted.")
    print("-" * 60)

    os.makedirs(DATA_DIR, exist_ok=True)

    # Step 2: Connect to Mosquitto with broker credentials
    client = mqtt.Client(
        callback_api_version=mqtt.CallbackAPIVersion.VERSION2
    )
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)

    client.on_connect    = on_connect
    client.on_message    = on_message
    client.on_disconnect = on_disconnect

    client.reconnect_delay_set(min_delay=1, max_delay=10)

    try:
        print(f"Connecting to {MQTT_BROKER}:{MQTT_PORT} ...")
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
        client.loop_forever()
    except KeyboardInterrupt:
        print("\nStopped by user.")
        client.disconnect()
    except Exception as e:
        print(f"Fatal error: {e}")