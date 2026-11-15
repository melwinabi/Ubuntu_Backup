from umqtt import MQTTClient
from machine import UART
import utime
import log
import checkNet
import sim
import sys
import net
import _thread

PROJECT_NAME = "Cen_Networks"
PROJECT_VERSION = "V.1.1.0"
Master_ID = 3002850
Publish_Master = "3002850_PUB"
Subscribe_Master = "3002850_SUC"
latitude = "00.0000000"
longitude = "11.1111111"
LdR = 1

checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")















if __name__ == "__main__":
    try:

            while True:
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



    except Exception as e:
        print("Main Error:", e)
