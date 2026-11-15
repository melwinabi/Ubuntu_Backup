from umqtt import MQTTClient
from machine import UART
import utime
import log
import checkNet

PROJECT_NAME = "QuecPython_MQTT_example"
PROJECT_VERSION = "1.0.0"

checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)

uart = UART(UART.UART2, 115200, 8, 0, 1, 0)
# Set the log output level.
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")


state = 0

def sub_cb(topic, msg):
    global state
    uart.write("\n")
    uart.write("{}".format(msg.decode()))
    uart.write("\n")
    mqtt_log.info("Subscribe Recv: Topic={},Msg={}".format(topic.decode(), msg.decode()))
    state = 1


if __name__ == '__main__':
    stagecode, subcode = checknet.wait_network_connected(30)
    if stagecode == 3 and subcode == 1:
        mqtt_log.info('Network connection successful!')
        uart.write("Network connection successful! \n")
        # Create an MQTT example.
        c = MQTTClient("Cenmark_Valve_Control", "198.38.81.119", 1883)
        # Set the callback function of receiving messages.
        c.set_callback(sub_cb)
        # Connect to the MQTT server.
        c.connect()
        # Subscribe to a topic.
        c.subscribe(b"Cenmark_PPV")
        mqtt_log.info("Connected to Cenmark_PPV topic" )
        # Publish a message.
        c.publish(b"RX_Valve_test", b"my name is Quecpython!")
        mqtt_log.info("Publish topic: RX_Valve_test, msg: my name is Quecpython")
        uart.write("formatted_msg")
        while True:
            c.wait_msg()  # Blocking function of monitoring messages.
            if state == 1:
                break
        # Disconnects from the MQTT server.
        c.disconnect()
    else:
        mqtt_log.info('Network connection failed! stagecode = {}, subcode = {}'.format(stagecode, subcode))

