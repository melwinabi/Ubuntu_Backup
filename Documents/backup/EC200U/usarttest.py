import _thread
import utime
from machine import UART

class ExampleUART:
    def __init__(self, no=UART.UART2, baud=115200, data_bits=8, parity=0, stop_bits=1, flow_control=0):
        self.uart = UART(no, baud, data_bits, parity, stop_bits, flow_control)

        # Start a separate thread for handling incoming data
        _thread.start_new_thread(self.handler_thread, ())

    def uart_write(self, msg):
        """ Send data over UART """
        print("Sending: {msg}")
        self.uart.write(msg)

    def uart_read(self):
        """ Read and print incoming data """
        if self.uart.any():
            msg = self.uart.read().decode('utf-8')
            print(msg)
            return msg
        return ""

    def uart_write_test(self):
        """ Send test messages repeatedly """
        for i in range(100):
            n = i
            msg = "Hello from Quectel: {}\n".format(i)
            self.uart_write(msg)
            utime.sleep(1)
            

    def handler_thread(self):
        """ Thread to handle incoming data """
        while True:
            self.uart_read()
            utime.sleep(0.1)  # Add a small delay to prevent CPU overload

if __name__ == "__main__":
    uart_test = ExampleUART()
    
    # Start sending test messages
    uart_test.uart_write_test()  