"""
UWB Unified Server - ALL IN ONE
  1. Subscribes to MQTT and saves JSON files
  2. Serves the dashboard via HTTP
  3. Streams live data to browsers via Server-Sent Events (SSE)
  4. Exposes historical data API
"""

import json
import os
import threading
import secrets
import zipfile
import io
import queue
from datetime import datetime
from functools import wraps
import paho.mqtt.client as mqtt
from flask import Flask, request, jsonify, send_file, send_from_directory, abort, Response, stream_with_context
from flask_cors import CORS

# ========== CONFIG ==========
MQTT_BROKER = "10.218.131.74"
MQTT_PORT     = 1883
MQTT_TOPIC    = "uwb/distance"
DATA_DIR      = "data"
HTTP_PORT     = 8080

APP_USERNAME  = "GowthamS"
APP_PASSWORD  = "GowthamS4516"

MQTT_USERNAME = "GowthamS"
MQTT_PASSWORD = "GowthamS4516"
# ============================

ACTIVE_TOKENS = {}
TOKEN_LIFETIME = 8 * 60 * 60

subscribers = []
subscribers_lock = threading.Lock()

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
    print(f"[{now.strftime('%H:%M:%S.%f')[:-3]}] Saved -> {filepath}  |  distance: {record['distance']} m")

    with subscribers_lock:
        for q in list(subscribers):
            try:
                q.put_nowait(record)
            except Exception:
                pass

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print(f"[MQTT] Connected to {MQTT_BROKER}:{MQTT_PORT}")
        client.subscribe(MQTT_TOPIC)
        print(f"[MQTT] Subscribed to: {MQTT_TOPIC}")
    else:
        print(f"[MQTT] Connection failed, code: {reason_code}")

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode("utf-8"))
        save_record(payload)
    except Exception as e:
        print(f"[MQTT] Error: {e}")

def on_disconnect(client, userdata, flags, reason_code, properties):
    if reason_code != 0:
        print("[MQTT] Disconnected - auto-reconnecting...")

def start_mqtt():
    client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.on_connect    = on_connect
    client.on_message    = on_message
    client.on_disconnect = on_disconnect
    client.reconnect_delay_set(min_delay=1, max_delay=10)
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
        client.loop_forever()
    except Exception as e:
        print(f"[MQTT] Fatal: {e}")

app = Flask(__name__)
CORS(app)

def require_auth(f):
    @wraps(f)
    def decorated(*args, **kwargs):
        token = request.headers.get('X-Auth-Token') or request.args.get('token')
        if not token or token not in ACTIVE_TOKENS:
            return jsonify({"error": "Unauthorized"}), 401
        if (datetime.now().timestamp() - ACTIVE_TOKENS[token]) > TOKEN_LIFETIME:
            del ACTIVE_TOKENS[token]
            return jsonify({"error": "Session expired"}), 401
        return f(*args, **kwargs)
    return decorated

@app.route('/api/login', methods=['POST'])
def login():
    data = request.get_json()
    if data.get('username') == APP_USERNAME and data.get('password') == APP_PASSWORD:
        token = secrets.token_hex(32)
        ACTIVE_TOKENS[token] = datetime.now().timestamp()
        return jsonify({"token": token})
    return jsonify({"error": "Invalid credentials"}), 401

@app.route('/api/logout', methods=['POST'])
@require_auth
def logout():
    token = request.headers.get('X-Auth-Token')
    if token in ACTIVE_TOKENS:
        del ACTIVE_TOKENS[token]
    return jsonify({"ok": True})

@app.route('/api/live')
def live_stream():
    """Server-Sent Events stream of live distance readings."""
    token = request.args.get('token')
    if not token or token not in ACTIVE_TOKENS:
        return "Unauthorized", 401

    def generate():
        q = queue.Queue(maxsize=100)
        with subscribers_lock:
            subscribers.append(q)
        try:
            yield f"data: {json.dumps({'hello': True})}\n\n"
            while True:
                try:
                    record = q.get(timeout=15)
                    yield f"data: {json.dumps(record)}\n\n"
                except queue.Empty:
                    yield ": keepalive\n\n"
        except GeneratorExit:
            pass
        finally:
            with subscribers_lock:
                if q in subscribers:
                    subscribers.remove(q)

    return Response(stream_with_context(generate()),
                    mimetype='text/event-stream',
                    headers={'Cache-Control': 'no-cache', 'X-Accel-Buffering': 'no'})

@app.route('/api/dates')
@require_auth
def list_dates():
    if not os.path.exists(DATA_DIR):
        return jsonify([])
    dates = []
    for d in sorted(os.listdir(DATA_DIR), reverse=True):
        path = os.path.join(DATA_DIR, d)
        if os.path.isdir(path):
            count = len([f for f in os.listdir(path) if f.endswith('.json')])
            dates.append({"date": d, "count": count})
    return jsonify(dates)

@app.route('/api/files/<date>')
@require_auth
def list_files(date):
    folder = os.path.join(DATA_DIR, date)
    if not os.path.isdir(folder):
        return jsonify([])
    files = sorted([f for f in os.listdir(folder) if f.endswith('.json')])
    return jsonify(files)

@app.route('/api/file/<date>/<filename>')
@require_auth
def get_file(date, filename):
    if '..' in date or '..' in filename or '/' in date or '/' in filename:
        abort(400)
    folder = os.path.join(DATA_DIR, date)
    filepath = os.path.join(folder, filename)
    if not os.path.isfile(filepath):
        abort(404)
    return send_from_directory(folder, filename, mimetype='application/json')

@app.route('/api/download/<date>')
@require_auth
def download_date_zip(date):
    if '..' in date or '/' in date:
        abort(400)
    folder = os.path.join(DATA_DIR, date)
    if not os.path.isdir(folder):
        abort(404)
    buf = io.BytesIO()
    with zipfile.ZipFile(buf, 'w', zipfile.ZIP_DEFLATED) as zf:
        for f in os.listdir(folder):
            if f.endswith('.json'):
                zf.write(os.path.join(folder, f), arcname=f)
    buf.seek(0)
    return send_file(buf, mimetype='application/zip',
                     as_attachment=True, download_name=f'uwb_{date}.zip')

@app.route('/api/summary/<date>')
@require_auth
def summary(date):
    if '..' in date or '/' in date:
        abort(400)
    folder = os.path.join(DATA_DIR, date)
    if not os.path.isdir(folder):
        abort(404)
    distances = []
    for f in os.listdir(folder):
        if f.endswith('.json'):
            try:
                with open(os.path.join(folder, f)) as fh:
                    d = json.load(fh)
                    if d.get('distance') is not None:
                        distances.append(d['distance'])
            except Exception:
                pass
    if not distances:
        return jsonify({"count": 0})
    return jsonify({
        "count": len(distances),
        "min": min(distances),
        "max": max(distances),
        "avg": sum(distances) / len(distances)
    })

@app.route('/')
def index():
    return send_from_directory('.', 'dashboard.html')

@app.route('/<path:path>')
def static_files(path):
    return send_from_directory('.', path)

if __name__ == "__main__":
    print("=" * 60)
    print("  UWB Unified Server")
    print("=" * 60)

    os.makedirs(DATA_DIR, exist_ok=True)

    mqtt_thread = threading.Thread(target=start_mqtt, daemon=True)
    mqtt_thread.start()

    print(f"[HTTP] Server running on http://0.0.0.0:{HTTP_PORT}")
    print(f"[HTTP] Open dashboard: http://localhost:{HTTP_PORT}")
    print(f"[DATA] Saving to: {os.path.abspath(DATA_DIR)}")
    print(f"[NGROK] Run: ngrok.exe http {HTTP_PORT}")
    print("-" * 60)

    app.run(host='0.0.0.0', port=HTTP_PORT, debug=False, use_reloader=False, threaded=True)