from umqtt import MQTTClient
import utime
import log
from config import Master_ID, Publish_Master, Subscribe_Master, ip, port

log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")

class MQTTHandler:
    """ Handles MQTT operations """
    
    def __init__(self, client_id):
        self.client_id = client_id
        self.client = None

    def connect(self):
        """ Connect to MQTT Broker """
        try:
            self.client = MQTTClient(self.client_id, ip, port, keepalive=300)
            self.client.set_callback(self.sub_cb)
            self.client.connect()
            self.client.subscribe(b"{}".format(Subscribe_Master))
            mqtt_log.info("Connected to Cenmark_PPV topic")
        except Exception as e:
            mqtt_log.warn("MQTT Connect Error: {}".format(e))

    def sub_cb(self, topic, msg):
        """ Handle incoming MQTT messages """
        try:
            if msg and len(msg) > 0:
                decoded_msg = msg.decode()
                print("Received MQTT message:", decoded_msg)

                # Handle message logic
                if decoded_msg.startswith("@@*") or decoded_msg.startswith("&&*"):
                    imdres = "##*{}*C@K*".format(Master_ID)
                    self.client.publish(Publish_Master, imdres)
                    utime.sleep(0.5)

        except Exception as e:
            print("MQTT Callback Error:", e)

    def publish_master_info(self, message):
        """ Publish master info """
        try:
            self.client.publish(Publish_Master, message)
            utime.sleep(0.5)
        except Exception as e:
            mqtt_log.warn("Publish Error: {}".format(e))
