from machine import UART
import utime

uart = UART(UART.UART2, 115200, 8, 0, 1, 0)
print("Ready To Receive")

while True:
    if uart.any():  
        msg = uart.read()
        if msg:  
            msg = msg.decode().strip()  
            print("Received:", msg)  

            if msg == "hai":
                uart.write("\n")  
                uart.write("hello")  
            else:
                uart.write("\n")  
                uart.write("bye")  

    utime.sleep(0.5)  
