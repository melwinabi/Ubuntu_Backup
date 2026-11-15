import gnss
import utime
import machine


uart = machine.UART(2, baudrate=115200, bits=8, parity=None, stop=1)


gps = gnss.GNSS(uart)


gps.open()

utime.sleep(2)

data = gps.GnssGetData()

if data:
    if data.get("fix") == 1:
        latitude = data.get("latitude", 0)
        longitude = data.get("longitude", 0)
        print("Latitude: {}, Longitude: {}".format(latitude, longitude))
    else:
        print("GNSS data is not fixed yet.")
else:
    print("No GNSS data available.")

gps.close()
