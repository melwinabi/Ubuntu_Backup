# boot.py - Initialize system settings and configurations before starting main.py

import machine
import log
import utime
import checkNet

# Logging configuration
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("Boot")

# Initialize network check
checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)

# Print a message to indicate the booting of the system
mqtt_log.info("Booting the EC200U-CNAA module...")

# Example of performing network check before proceeding
def check_network():
    stagecode, subcode = checknet.wait_network_connected(30)
    if stagecode == 3 and subcode == 1:
        mqtt_log.info("Network connection successful!")
    else:
        mqtt_log.error("Network connection failed! stagecode = {}, subcode = {}".format(stagecode, subcode))

# Run network check at boot
check_network()

# Optional: You could initialize additional hardware like UART, GPS, etc.
# For example, if UART is needed for serial communication:
try:
    uart = machine.UART(2, baudrate=9600, bits=8, parity=None, stop=1)
    uart.init(bits=8, parity=None, stop=1)
    mqtt_log.info("UART initialized.")
except Exception as e:
    mqtt_log.error("UART initialization failed: {}".format(e))

# Example to initialize other peripherals like LED, GPIO, etc.
try:
    led = machine.Pin(2, machine.Pin.OUT)  # Adjust the pin based on your hardware
    led.off()  # LED off at boot
    mqtt_log.info("LED initialized on pin 2.")
except Exception as e:
    mqtt_log.error("Failed to initialize LED: {}".format(e))

# This is where additional boot setup steps can be added if required (e.g., GPS initialization, sensors, etc.)

# Proceed to the main.py execution after boot.py is finished
mqtt_log.info("Boot process completed. Proceeding to main application.")
