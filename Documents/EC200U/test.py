from machine import UART
from umqtt import MQTTClient
import utime

# Initialize UART
uart = UART(UART.UART1, baudrate=115200, bits=8, parity=None, stop=1)

# MQTT message callback
def mqtt_callback(topic, msg):
    # Format the message
    formatted_msg = f"@{msg.decode()}\r\n"
    # Send to Arduino via UART
    uart.write(formatted_msg)

# Initialize MQTT client
client = MQTTClient(client_id="Cenmark_Valve_Control", server="198.38.81.119", port=1883)
client.set_callback(mqtt_callback)
client.connect()
client.subscribe("Cenmark_PPV")

# Main loop
while True:
    client.wait_msg()  # Wait for MQTT messages
    utime.sleep(1)
