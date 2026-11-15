import machine
import time
import dht  # For DHT11/DHT22 sensors
  

# Initialize the DHT sensor
dht_sensor = dht.DHT22(machine.Pin(4))  # Replace with your pin

# Initialize RTC (for ESP32)
rtc = machine.RTC()
rtc.datetime((2025, 1, 7, 0, 12, 0, 0, 0))  # Set date/time (Year, Month, Day, Weekday, Hour, Minute, Second, Millisecond)

# Log file

log_file = "temperature_log.txt"

# Function to log temperature
def log_temperature():
    try:
        dht_sensor.measure()
        temp = dht_sensor.temperature()
        timestamp = rtc.datetime()
        log_entry = f"{timestamp[0]}-{timestamp[1]:02d}-{timestamp[2]:02d} {timestamp[4]:02d}:{timestamp[5]:02d}:{timestamp[6]:02d} - {temp:.2f}°C\n"
        print("Logging:", log_entry.strip())
        
        # Save to file
        with open(log_file, "a") as f:
            f.write(log_entry)
    except Exception as e:
        print("Error reading sensor:", e)

# Main loop
while True:
    log_temperature()
    time.sleep(10)
