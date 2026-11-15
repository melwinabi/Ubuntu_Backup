"""
Runnnig this routine, you need to connect the main port on the EVB to a PC by a USB-to-Serial Port Adapter, enable the main port by a UART tool on the PC and send data to this port. Then you can see the messages sent by PC.
"""
import _thread
import utime
import log
from machine import UART


'''
The following two global variables are necessary. You can modify the values of these two global variables based on your project requirements.
'''
PROJECT_NAME = "QuecPython_UART_example"
PROJECT_VERSION = "1.0.1"

'''
 * Parameter1: Port
        Note: For EC100Y-CN and EC600S-CN modules, descriptions of UARTn are as follows:
        UART0 - DEBUG PORT
        UART1 – BT PORT
        UART2 – MAIN PORT
        UART3 – USB CDC PORT
 * Parameter2：Baud rate
 * Parameter3：Data bits  (5—8)
 * Parameter4：Parity  （0：NONE  1：EVEN  2：ODD）
 * Parameter5：Stop bits (1–2)
 * Parameter6：Flow control (0: FC_NONE  1: FC_HW)
'''


# Sets the log output level
log.basicConfig(level=log.INFO)
uart_log = log.getLogger("UART")

class Example_uart(object):
    def __init__(self, no=UART.UART2, bate=115200, data_bits=8, parity=0, stop_bits=1, flow_control=0):
        self.uart = UART(no, bate, data_bits, parity, stop_bits, flow_control)
        self.uart.set_callback(self.callback)


    def callback(self, para):
        uart_log.info("call para:{}".format(para))
        if(0 == para[0]):
            self.uartRead(para[2])


    def uartWrite(self, msg):
        uart_log.info("write msg:{}".format(msg))
        self.uart.write(msg)

    def uartRead(self, len):
        msg = self.uart.read(len)
        utf8_msg = msg.decode()
        uart_log.info("UartRead msg: {}".format(utf8_msg))
        return utf8_msg

    def uartWrite_test(self):
        for i in range(10):
            write_msg = "Hello count={}".format(i)
            self.uartWrite(write_msg)
            utime.sleep(1)

if __name__ == "__main__":
    uart_test = Example_uart()
    uart_test.uartWrite_test()


# Examples of running results
'''
INFO:UART:write msg:Hello count=0
INFO:UART:write msg:Hello count=1
INFO:UART:write msg:Hello count=2
INFO:UART:write msg:Hello count=3
INFO:UART:write msg:Hello count=4
INFO:UART:write msg:Hello count=5
INFO:UART:write msg:Hello count=6
INFO:UART:write msg:Hello count=7
INFO:UART:write msg:Hello count=8
INFO:UART:write msg:Hello count=9

INFO:UART:call para:[0, 2, 15]
INFO:UART:UartRead msg: my name is XXX


'''

