from umqtt import MQTTClient

def err_cb(err):
    print("thread err:")
    print(err)

c = MQTTClient("Cenmark_Valve_Control", "198.38.81.119", 1883)
c.error_register_cb(err_cb)

# Connect to the MQTT broker
c.connect()

# Publish the message after connection
c.publish("RX_Valve_test", "msg", retain=False, qos=0)
c.subscribe("Cenmark_PPV")
