from umqtt import MQTTClient
from machine import UART
import utime
import log
import checkNet

PROJECT_NAME = "QuecPython_MQTT_example"
PROJECT_VERSION = "1.0.0"

checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)

uart = UART(UART.UART2, 115200, 8, 0, 1, 0)
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")

state = 0  # Ensure 'state' is initialized

def sub_cb(topic, msg):
    global state
    uart.write("\n")
    uart.write("{}".format(msg.decode()))
    uart.write("\n")
    mqtt_log.info("Subscribe Recv: Topic={}, Msg={}".format(topic.decode(), msg.decode()))
    state = 1  # Set state to indicate a message was received

def mqtt_connect():
    """Connects and subscribes to MQTT broker with error handling."""
    global c
    try:
        c = MQTTClient("Cen_Master54564654564", "mqtt.ppvgroups.in", 1883, keepalive=300)
        c.set_callback(sub_cb)
        c.connect()
        c.subscribe(b"3002850_SUC")
        mqtt_log.info("Connected to Cenmark_PPV topic")
        uart.write("Connected to Cenmark_PPV topic\n")
        return True
    except Exception as e:
        mqtt_log.error("Failed to connect: {}".format(e))
        uart.write("Failed to connect\n")
        return False

if __name__ == '__main__':
    stagecode, subcode = checknet.wait_network_connected(30)

    if stagecode == 3 and subcode == 1:
        mqtt_log.info('Network connection successful!')
        uart.write("Network connection successful!\n")

        if mqtt_connect():
            c.publish("3002850_PUB","my name is Quecpython!")
            mqtt_log.info("Publish topic: 3002850_PUB, msg: my name is Quecpython")

            while True:
                try:
                    c.wait_msg()  # Blocking call to wait for messages
                except OSError as e:
                    if e.args[0] == 107:  # ENOTCONN
                        mqtt_log.warn("Disconnected. Trying to reconnect...")
                        uart.write("Reconnecting...\n")
                        utime.sleep(5)
                        if mqtt_connect():
                            mqtt_log.info("Reconnection successful!")
                            uart.write("Reconnection successful!\n")
                        else:
                            mqtt_log.error("Reconnection failed.")
                            utime.sleep(5)
                except Exception as e:
                    mqtt_log.error("Exception: {}".format(e))

                # Instead of breaking, reset `state` and continue listening
                if state == 1:
                    state = 0  # Reset state instead of disconnecting

        else:
            mqtt_log.error("Initial connection failed. Exiting.")
    else:
        mqtt_log.info('Network connection failed! stagecode = {}, subcode = {}'.format(stagecode, subcode))
