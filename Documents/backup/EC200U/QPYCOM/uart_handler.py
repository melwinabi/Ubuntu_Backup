from machine import UART
import _thread
import utime

class UARTHandler:
    """ Handles UART communication """
    def __init__(self, no=UART.UART2, baud=115200, data_bits=8, parity=0, stop_bits=1, flow_control=0):
        self.uart = UART(no, baud, data_bits, parity, stop_bits, flow_control)
        self.received_message = ""
        _thread.start_new_thread(self.uart_handler_thread, ())

    def uart_write(self, msg):
        """ Send data over UART """
        try:
            print("Sending: {}".format(msg))
            self.uart.write(msg.encode())
        except Exception as e:
            print("UART Write Error:", e)

    def uart_read(self):
        """ Read and process incoming UART data """
        if self.uart.any():
            try:
                read_data = self.uart.read().decode('utf-8').strip()
                print("Received from ESP32>{}".format(read_data))
                self.received_message = read_data
            except Exception as e:
                print("UART Read Error:", e)

    def uart_handler_thread(self):
        """ UART handling thread """
        while True:
            try:
                self.uart_read()
            except Exception as e:
                print("UART Thread Error:", e)
            utime.sleep(1)
