import utime
import _thread
from config import PROJECT_NAME, PROJECT_VERSION, Master_ID, Send_ok
from uart_handler import UARTHandler
from mqtt_client import MQTTHandler
from utils import get_ip_port
import log
import checkNet

# Setup logger and network check
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")
checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)

# Initialize UART and MQTT
uart_handler = UARTHandler()

# Fetch IP and Port
ip, port = get_ip_port()
if not ip or not port:
    print("Failed to get IP and Port")
    exit()

# Create MQTT client
client_ID = "CenMark_hardware_{}".format(utime.ticks_ms() % 1000)
mqtt_handler = MQTTHandler(client_ID)

# Network connection
stagecode, subcode = checknet.wait_network_connected(30)

def timer_thread():
    """ Timer thread for periodic MQTT publishing """
    while True:
        utime.sleep(0.5)
        message = "Master Info: ID: {}, Send_ok: {}".format(Master_ID, Send_ok)
        mqtt_handler.publish_master_info(message)
        utime.sleep(30)

# Start main program
if __name__ == "__main__":
    try:
        if stagecode == 3 and subcode == 1:
            print("Network connection successful!")
            
            mqtt_handler.connect()

            # Start Timer thread
            _thread.start_new_thread(timer_thread, ())

            while True:
                uart_handler.uart_write("PFOM")
                utime.sleep(1)
        else:
            print("Network connection failed! stagecode = {}, subcode = {}".format(stagecode, subcode))

    except Exception as e:
        print("Main Error:", e)
