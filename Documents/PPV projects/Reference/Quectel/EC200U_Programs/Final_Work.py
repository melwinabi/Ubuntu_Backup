from umqtt import MQTTClient
from machine import UART
import utime
import log
import checkNet
import sim
import net
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

# Fetch IP and Port from the API
response = request.get("https://api.cenpilot.in/api/CenPilot/Domine")
Url_String = response.content.decode()
parsed_data = ujson.loads(Url_String)
ip = parsed_data['IP']
port = parsed_data['Port']

class UARTHandler:
    """ Handles UART communication """
    def __init__(self, no=UART.UART2, baud=115200, data_bits=8, parity=0, stop_bits=1, flow_control=0):
        self.uart = UART(no, baud, data_bits, parity, stop_bits, flow_control)
        self.received_message = ""
        _thread.start_new_thread(self.uart_handler_thread, ())

    def uart_write(self, msg):
        """ Send data over UART """
        try:
            print("Sending: {}".format(msg))
            self.uart.write(msg.encode())
        except Exception as e:
            print("UART Write Error:", e)


    def uart_read(self):
        """ Read and process incoming UART data """
        if self.uart.any():
            try:
                read_data = self.uart.read().decode('utf-8').strip()
                print("Received from ESP32 > {}".format(read_data))
                Seprator(read_data)
                self.received_message = read_data
            except Exception as e:
                print("UART Read Error:", e)


    def uart_handler_thread(self):
        """ UART handling thread """
        while True:
            try:
                self.uart_read()
            except Exception as e:
                print("UART Thread Error:", e)
            utime.sleep(1)

def Seprator(value):
    global Send_ok
    if value.startswith("##*"):
        if value == "##*AVL*3002850*000":  # Change if needed
            Send_ok = 0
        elif value == "##*AVL*3002850*010":  # Change if needed
            Send_ok = 1
        c.publish(Publish_Master, value)
        utime.sleep(0.5)

def sub_cb(topic, msg):
    """ Handle incoming MQTT messages """
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
                if decoded_msg == "$$*RCD*3002850":  ## Change for another master
                    uart_handler.uart_write("GSMNFO")
                    utime.sleep(0.5)
    except Exception as e:
        print("MQTT Callback Error:", e)

def Master_Info():
    """ Publish master info over MQTT """
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
        mqtt_log.warn("Master_Info Error: {}".format(e))

if __name__ == "__main__":
    try:
        uart_handler = UARTHandler()

        stagecode, subcode = checknet.wait_network_connected(30)
        
        if stagecode == 3 and subcode == 1:
            mqtt_log.info('Network connection successful!')
            print("Network connection successful!")

            c = MQTTClient("Cenmark_ok", ip, port)
            c.set_callback(sub_cb)
            c.connect()
            c.subscribe(b"{}".format(Subscribe_Master))

            mqtt_log.info("Connected to Cenmark_PPV topic")

            # Publish master online status
            for _ in range(2):
                c.publish(Publish_Master, "##*Master>*{}*Online".format(Master_ID))
                utime.sleep(0.5)

            while True:
                response = request.get("https://api.cenpilot.in/api/CenPilot/ValveListONCount?Id=3002850")
                list_response = response.content.decode()
                message = "int> " + list_response
                print(message)
                
                if Send_ok == 1:
                    uart_handler.uart_write(message)

                try:                
                    c.check_msg()
                    utime.sleep(0.1)                    
                except Exception as e:
                    mqtt_log.warn("MQTT Error: {}".format(e))
                    utime.sleep(2)  # Delay before reconnection attempt
                    try:
                        c.reconnect()
                        mqtt_log.info("Reconnected to MQTT broker")
                    except Exception as e:
                        print("MQTT Reconnect Failed:", e)
                        utime.sleep(5)  # Prevent reconnection flood

        else:
            mqtt_log.info('Network connection failed! stagecode = {}, subcode = {}'.format(stagecode, subcode))
            print("Network connection failed! stagecode = {}, subcode = {}".format(stagecode, subcode))
            uart_handler.uart_write("NFO")

    except Exception as e:
        print("Main Error:", e)
