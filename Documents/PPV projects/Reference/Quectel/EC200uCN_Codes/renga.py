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

PROJECT_NAME = "Cen_Networks"
PROJECT_VERSION = "V.1.1.0"
Master_ID = 3002850
Publish_Master = "3002850_PUB"
Subscribe_Master = "3002850_SUC"
latitude = "00.0000000"
longitude = "11.1111111"
LdR = 1
response = ""
Url_String = ""
valve_String = ""
ip = ""
port = ""
Send_ok = 0
checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")
list_response = ""

response = request.get("https://api.cenpilot.in/api/CenPilot/Domine")
content_gen = response.content
Url_String = ''.join([x for x in content_gen])
parsed_data = ujson.loads(Url_String)
ip = parsed_data['IP']
port = parsed_data['Port']

client_id = "Cenmark_{}".format(utime.ticks_ms() % 10000)

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
            print("UART Write Error:", e)

    def uart_read(self):
        if self.uart.any():
            try:
                read_data = self.uart.read().decode('utf-8').strip()
                print("Received from ESP32>{}".format(read_data))
                Seprator(read_data)
                self.received_message = read_data
            except Exception as e:
                print("UART Read Error:", e)

    def uart_handler_thread(self):
        while True:
            try:
                self.uart_read()
            except Exception as e:
                print("UART Thread Error:", e)
            utime.sleep(1)


def Seprator(value):
    global Send_ok
    if value.startswith("##*"):
        if value == "##*AVL*3002850*000":
            Send_ok = 0
        elif value == "##*AVL*3002850*010":
            Send_ok = 1
        try:
            c.publish(Publish_Master, value)
        except Exception as e:
            print("MQTT Publish Error in Seprator:", e)
        utime.sleep(0.5)


def sub_cb(topic, msg):
    try:
        if msg and len(msg) > 0:
            decoded_msg = msg.decode()
            if decoded_msg.startswith("@@*") or decoded_msg.startswith("&&*"):
                imdres = "##*{}*C@K*".format(Master_ID)
                c.publish(Publish_Master, imdres)
                utime.sleep(0.5)
                uart_handler.uart_write(decoded_msg)
            elif decoded_msg.startswith("$$*"):
                print(decoded_msg)
                if decoded_msg == "$$*RCD*3002850":
                    uart_handler.uart_write("GSMNFO")
                    utime.sleep(0.5)
    except Exception as e:
        print("MQTT Callback Error:", e)


def Master_Info():
    try:
        imsi = sim.getImsi()
        operator_name = net.operatorName()
        state = net.getState()
        network_state = state[0]
        iccid = sim.getIccid()

        message = "##*WMfO*{}*{}*{}*{}*{}*{}*{}*{}*{}*{}*{}*{}*{}*$$".format(
            Master_ID, imsi, iccid, operator_name[0], operator_name[2], operator_name[3],
            network_state[0], network_state[1], network_state[2], network_state[3],
            latitude, longitude, LdR
        ).encode()

        c.publish(Publish_Master, message)
        utime.sleep(0.5)
    except Exception as e:
        mqtt_log.warning("Master_Info Error: {}".format(e))


def mqtt_connect():
    global c
    try:
        c = MQTTClient(
            client_id=client_id,
            server=ip,
            port=port,
            keepalive=60
        )
        c.set_callback(sub_cb)
        c.connect()
        c.subscribe(bytes(Subscribe_Master, 'utf-8'))
        mqtt_log.info("MQTT connected and subscribed to {} with client_id {}".format(Subscribe_Master, client_id))

        for _ in range(2):
            c.publish(Publish_Master, "##*Master>*{}*Online".format(Master_ID))
            utime.sleep(0.5)
    except Exception as e:
        mqtt_log.warning("MQTT connect error: {}".format(e))
        utime.sleep(5)
        mqtt_connect()


def timer_thread():
    while True:
        utime.sleep(0.5)
        Master_Info()
        utime.sleep(60)


def keep_alive_thread():
    while True:
        try:
            c.ping()
        except Exception as e:
            mqtt_log.warning("Keep-alive ping failed: {}".format(e))
        utime.sleep(30)


def publish_thread():
    while True:
        try:
            response = request.get("https://api.cenpilot.in/api/CenPilot/ValveListONCount?Id={}".format(Master_ID))
            raw_valve_content = ''.join([x for x in response.content])
            print("Raw response:", raw_valve_content)
            message = "int> " + raw_valve_content

            if Send_ok == 1:
                uart_handler.uart_write(message)
        except Exception as e:
            mqtt_log.warning("Publish thread error: {}".format(e))
        utime.sleep(5)


def subscribe_thread():
    while True:
        try:
            c.check_msg()
        except OSError as e:
            if e.args[0] != 11:
                mqtt_log.warning("MQTT check_msg Error: {}".format(e))
                try:
                    c.disconnect()
                except:
                    pass
                mqtt_connect()
        except Exception as e:
            mqtt_log.warning("Subscribe thread error: {}".format(e))
        utime.sleep(1)


if __name__ == "__main__":
    try:
        uart_handler = UARTHandler()
        stagecode, subcode = checknet.wait_network_connected(30)

        if stagecode == 3 and subcode == 1:
            mqtt_log.info('Network connection successful!')
            mqtt_connect()
            _thread.start_new_thread(timer_thread, ())
            _thread.start_new_thread(keep_alive_thread, ())
            _thread.start_new_thread(publish_thread, ())
            _thread.start_new_thread(subscribe_thread, ())

            while True:
                utime.sleep(10)
        else:
            mqtt_log.info('Network connection failed! stagecode = {}, subcode = {}'.format(stagecode, subcode))
            uart_handler.uart_write("NFO")
    except Exception as e:
        print("Main Error:", e)
