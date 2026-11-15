import utime

while True:
    current_time = utime.localtime()  
    print("Current time:", current_time)  
    utime.sleep(1)  

    