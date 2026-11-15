from umqtt import MQTTClient
from machine import UART
import utime
import log
import checkNet

PROJECT_NAME = "Cen_Networks"
PROJECT_VERSION = "V.1.1.0"
message = "" 
received_message = ""
Master_ID = 3002850
checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)

uart = UART(UART.UART2, 115200, 8, 0, 1, 0)
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")

state = 0

def sub_cb(topic, msg):
    global state
    mqtt_log.info("Subscribe Recv: Topic={}, Msg={}".format(topic.decode(), msg.decode()))
    Separator(msg)
    state = 1

def Separator(Data):
    if Data.startswith("Tx."):
        uart.write(Data.decode())  # Directly decode and write to UART
        print(Data.decode())
        print("done")

if __name__ == "__main__":
    stagecode, subcode = checknet.wait_network_connected(30)
    
    if stagecode == 3 and subcode == 1:
        mqtt_log.info("Network connection successful!")
        uart.write("Network connection successful! \n")
        
        c = MQTTClient("Cenmark_Valve_Control", "198.38.81.119", 1883)
        c.set_callback(sub_cb)
        c.connect()
        c.subscribe(b"Cenmark_PPV")
        mqtt_log.info("Connected to Cenmark_PPV topic")

        message = ("Master > {} Online".format(Master_ID)).encode()
        c.publish(b"RX_Valve_test", message)

        while True:     
            c.wait_msg()  # This blocks until a message is received.
            if uart.any():  
                message = uart.read()
    
    else:
        mqtt_log.info("Network connection failed! stagecode = {}, subcode = {}".format(stagecode, subcode))
