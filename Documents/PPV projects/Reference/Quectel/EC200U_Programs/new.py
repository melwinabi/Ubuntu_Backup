import serial

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)  # Change port accordingly
ser.write(b'AT\r\n')
response = ser.read(100)
print(response.decode())
