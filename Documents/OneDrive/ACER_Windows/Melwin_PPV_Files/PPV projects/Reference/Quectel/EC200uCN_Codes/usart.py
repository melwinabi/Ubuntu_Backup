import utime
import log
from machine import UART

# Set up logging
log.basicConfig(level=log.INFO)
uart_log = log.getLogger("UART")

# Define the UART settings (port, baud rate, data bits, parity, stop bits, and flow control)
class SimpleUart:
    def __init__(self, port=UART.UART2, baud_rate=115200, data_bits=8, parity=0, stop_bits=1, flow_control=0):
        # Initialize UART
        self.uart = UART(port, baud_rate, data_bits, parity, stop_bits, flow_control)
        
    def uartWrite(self, msg):
        """Write message to UART"""
        uart_log.info("Writing message: {}".format(msg))
        self.uart.write(msg)
    
    def uartRead(self):
        """Read message from UART"""
        if self.uart.any():  # Check if there is data available
            msg = self.uart.read()  # Read available data
            if msg:
                utf8_msg = msg.decode()  # Decode the message as a UTF-8 string
                uart_log.info("Received message: {}".format(utf8_msg))
                return utf8_msg
        return None

    def run(self):
        """Function to run read and write loop"""
        while True:
            # Send a message to the connected device
            self.uartWrite("Hello from Quectel!\n")
            print("1")
            
            # Wait for a response
            response = self.uartRead()
            if response:
                # Print the response from UART
                uart_log.info("Response received: {}".format(response))
            
            # Delay to avoid flooding the UART with messages
            utime.sleep(1)

# Main execution
if __name__ == "__main__":
    uart_test = SimpleUart()
    uart_test.run()
