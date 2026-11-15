from umqtt import MQTTClient
from machine import UART
import utime
import log
import checkNet
import sim
import net
import _thread
import request
import ujson
import gc

# === Project Info ===
PROJECT_NAME = "Cen_Networks"
PROJECT_VERSION = "V.1.1.0"
Master_ID = 3002850
Publish_Master = "{}_PUB".format(Master_ID)
Subscribe_Master = "{}_SUC".format(Master_ID)
latitude = "00.0000000"
longitude = "11.1111111"
LdR = 1

# === Logging ===
checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")

# === Global Flags ===
Send_ok = 0
c = None
ip = ""
port = ""
uart_handler = None

# === Get MQTT Broker Info ===
try:
    url_response = request.get("https://api.cenpilot.in/api/CenPilot/Domine")
    url_data = ujson.loads(url_response.content.decode())
    ip = url_data['IP']
    port = url_data['Port']
    mqtt_log.info("Broker IP: {}, Port: {}".format(ip, port))
except Exception as e:
    mqtt_log.error("Failed to fetch broker info: {}".format(e))

# === UART Handler ===
class UARTHandler:
    def __init__(self, no=UART.UART2, baud=115200, data_bits=8, parity=0, stop_bits=1, flow_control=0):
        self.uart = UART(no, baud, data_bits, parity, stop_bits, flow_control)
        self.received_message = ""
        _thread.start_new_thread(self.uart_handler_thread, ())

    def uart_write(self, msg):
        try:
            print("Sending: {}".format(msg))
            self.uart.write(msg.encode())
        except Exception as e:
            print("UART Write Error: {}".format(e))

    def uart_read(self):
        if self.uart.any():
            try:
                read_data = self.uart.read().decode('utf-8').strip()
                print("Received from ESP32 > {}".format(read_data))
                Seprator(read_data)
                self.received_message = read_data
            except Exception as e:
                print("UART Read Error: {}".format(e))

    def uart_handler_thread(self):
        while True:
            try:
                self.uart_read()
            except Exception as e:
                print("UART Thread Error: {}".format(e))
            utime.sleep(1)

# === Message Parsing ===
def Seprator(value):
    global Send_ok
    if value.startswith("##*"):
        if value == "##*AVL*{}*000".format(Master_ID):
            Send_ok = 0
        elif value == "##*AVL*{}*010".format(Master_ID):
            Send_ok = 1
        c.publish(Publish_Master, value)
        utime.sleep(0.5)

# === MQTT Callback ===
def sub_cb(topic, msg):
    try:
        decoded_msg = msg.decode()
        if decoded_msg.startswith("@@*") or decoded_msg.startswith("&&*"):
            reply = "##*{}*C@K*".format(Master_ID)
            c.publish(Publish_Master, reply)
            utime.sleep(0.5)
            uart_handler.uart_write(decoded_msg)
        elif decoded_msg == "$$*RCD*{}".format(Master_ID):
            uart_handler.uart_write("GSMNFO")
            utime.sleep(0.5)
    except Exception as e:
        print("MQTT Callback Error: {}".format(e))

# === Master Info Publisher ===
def Master_Info():
    try:
        imsi = sim.getImsi()
        operator_name = net.operatorName()
        state = net.getState()
        iccid = sim.getIccid()

        msg = "##*WMfO*{}*{}*{}*{}*{}*{}*{}*{}*{}*{}*{}*{}*{}*$$".format(
            Master_ID, imsi, iccid, operator_name[0], operator_name[2], operator_name[3],
            state[0], state[1], state[2], state[3], latitude, longitude, LdR
        )
        c.publish(Publish_Master, msg.encode())
        utime.sleep(0.5)
    except Exception as e:
        mqtt_log.warn("Master_Info Error: {}".format(e))

# === Periodic Timer Thread ===
def timer_thread():
    while True:
        utime.sleep(0.5)
        Master_Info()
        utime.sleep(60)

# === MQTT Connect/Reconnect ===
def mqtt_connect():
    global c
    try:
        client_id = "Cenmark_{}".format(Master_ID)
        c = MQTTClient(client_id, ip, port, keepalive=300)
        c.set_callback(sub_cb)
        c.connect()
        c.subscribe(Subscribe_Master.encode())
        mqtt_log.info("Connected to broker and subscribed to {}".format(Subscribe_Master))
        return True
    except Exception as e:
        mqtt_log.error("MQTT Connection Failed: {}".format(e))
        return False

# === Main Execution ===
if __name__ == "__main__":
    try:
        uart_handler = UARTHandler()

        stagecode, subcode = checknet.wait_network_connected(30)
        if stagecode == 3 and subcode == 1:
            mqtt_log.info("Network connection successful.")
            if mqtt_connect():
                for _ in range(2):
                    c.publish(Publish_Master, "##*Master>*{}*Online".format(Master_ID))
                    utime.sleep(0.5)

                _thread.start_new_thread(timer_thread, ())

                while True:
                    try:
                        valve_response = request.get("https://api.cenpilot.in/api/CenPilot/ValveListONCount?Id={}".format(Master_ID))
                        list_response = valve_response.content.decode()
                        print("Valve list response: {}".format(list_response))

                        message = "int> {}".format(list_response)
                        if Send_ok == 1:
                            uart_handler.uart_write(message)

                        c.check_msg()
                        utime.sleep(0.1)
                        gc.collect()
                    except Exception as e:
                        mqtt_log.warn("MQTT Loop Error: {}".format(e))
                        utime.sleep(2)
                        if mqtt_connect():
                            mqtt_log.info("Reconnected successfully.")
                        else:
                            mqtt_log.error("Reconnection failed.")
                            utime.sleep(5)
            else:
                mqtt_log.error("Initial MQTT connection failed.")
        else:
            mqtt_log.error("Network failed: stagecode={}, subcode={}".format(stagecode, subcode))
            uart_handler.uart_write("NFO")
    except Exception as e:
        print("Main Error: {}".format(e))
