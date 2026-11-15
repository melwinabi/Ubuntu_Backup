from machine import UART
import utime

uart = UART(UART.UART2, 115200, 8, 0, 1, 0)  # Initialize UART2
while True:
    uart.write("Hello from Quectel!\n")  # Send data over UART2
    utime.sleep(1)  # Wait 1 second before sending next message
