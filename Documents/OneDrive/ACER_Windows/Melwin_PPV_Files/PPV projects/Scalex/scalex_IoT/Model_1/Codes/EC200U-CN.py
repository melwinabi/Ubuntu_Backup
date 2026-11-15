from machine import UART
import utime
import log
import checkNet
import sim
import net
import request

PROJECT_NAME = "Scalex_IoT"
PROJECT_VERSION = "V.1.1.0"

log.basicConfig(level=log.INFO)
uart_log = log.getLogger("UART")
net_log = log.getLogger("NET")

checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)

class SimpleUart:
    def __init__(self, port=UART.UART2, baud_rate=115200, data_bits=8, parity=0, stop_bits=1, flow_control=0):
        self.uart = UART(port, baud_rate, data_bits, parity, stop_bits, flow_control)

    def uartWrite(self, data):
        uart_log.info("Writing message: {}".format(data))
        self.uart.write(data + "\r\n")

    def uartRead(self):
        if self.uart.any():
            try:
                msg = self.uart.read().decode('utf-8').strip()
                uart_log.info("Received message: {}".format(msg))

                # Validate URL before using
                if msg.startswith("http://") or msg.startswith("https://"):
                    try:
                        response = request.get(msg, timeout=10)  # Add timeout
                        print(msg)
                        uart_log.info("GET response code: {}".format(response.status_code))
                        uart_log.info("Response content: {}".format(response.content))
                        response.close()  # Important to close socket
                    except Exception as e:
                        uart_log.error("HTTP GET failed: {}".format(e))
                else:
                    uart_log.error("Invalid URL received.")
            except Exception as e:
                uart_log.error("UART Read Error: {}".format(e))

    def run(self):
        while True:
            self.uartRead()
            utime.sleep(3)  # Increased delay to avoid rapid socket allocation


def connect_network():
    net_log.info("Waiting for network connection...")
    stagecode, subcode = checknet.wait_network_connected(30)
    if stagecode == 3 and subcode == 1:
        net_log.info("Network connected successfully!")

        try:
            imsi = sim.getImsi()
            operator_name = net.operatorName()
            state = net.getState()
            iccid = sim.getIccid()

            net_log.info("IMSI: {}".format(imsi))
            net_log.info("ICCID: {}".format(iccid))
            net_log.info("Operator Name: {}".format(operator_name))
            net_log.info("Network State: {}".format(state))
        except Exception as e:
            net_log.error("Failed to fetch SIM or network info: {}".format(e))
        return True
    else:
        net_log.error("Network connection failed! stagecode = {}, subcode = {}".format(stagecode, subcode))
        return False


# Main execution
if __name__ == "__main__":
    if connect_network():
        uart_test = SimpleUart()
        uart_test.run()
    else:
        net_log.error("Exiting program due to network failure.")
