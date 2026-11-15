import request
import ujson
import checkNet
import log
import utime

# Project details
PROJECT_NAME = "Cen_Networks"
PROJECT_VERSION = "V.1.2.0"

# Logger setup
log.basicConfig(level=log.INFO)
logger = log.getLogger("HTTP")

# Initialize network check
checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)

# Wait for network connection
stagecode, subcode = checknet.wait_network_connected(30)
if stagecode == 3 and subcode == 1:
    logger.info("Network connection successful!")
else:
    logger.error("Network connection failed! stagecode = {}, subcode = {}".format(stagecode, subcode))
    # Handle network connection failure appropriately

# Function to perform HTTP GET request
def fetch_data(url):
    try:
        response = request.get(url)
        if response.status_code == 200:
            return response.content.decode()
        else:
            logger.error("Failed to fetch data: HTTP status code {}".format(response.status_code))
            return None
    except Exception as e:
        logger.error("Exception during HTTP request: {}".format(e))
        return None

# Fetch IP and port information
url = "https://api.cenpilot.in/api/CenPilot/Domine"
response_data = fetch_data(url)
if response_data:
    try:
        parsed_data = ujson.loads(response_data)
        ip = parsed_data.get('IP', '')
        port = int(parsed_data.get('Port', 0))
        logger.info("IP: {}, Port: {}".format(ip, port))
    except ValueError as e:
        logger.error("JSON parsing error: {}".format(e))
else:
    logger.error("Failed to retrieve IP and port information.")

# Fetch Valve List ON Count
valve_list_url = "https://api.cenpilot.in/api/CenPilot/ValveListONCount?Id=3002850"
valve_list_data = fetch_data(valve_list_url)
if valve_list_data:
    logger.info("Valve List ON Count: {}".format(valve_list_data))
    # Process the valve list data as needed
else:
    logger.error("Failed to retrieve Valve List ON Count.")
