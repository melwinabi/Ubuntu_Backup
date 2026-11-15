def start():
    from umqtt import MQTTClient
    from machine import UART
    import utime
    import log
    import checkNet
    import sim
    import net
    import _thread
    import request
    import ujson

    PROJECT_NAME = "Cen_Networks"
    PROJECT_VERSION = "V.1.2.0"
    Master_ID = 3002850
    Publish_Master = "{}_PUB".format(Master_ID)
    Subscribe_Master = "{}_SUC".format(Master_ID)
    latitude = "00.0000000"
    longitude = "11.1111111"
    ip = ""
    port = ""
    state = 0  
    received_msg = ""
    Url_String = ""
    response = ""
    LdR = 0
    mqtt_Message = ""
    Ack_State = 0 
    j = 0

    response = request.get("https://api.cenpilot.in/api/CenPilot/Domine")
    Url_String = "".join(chunk.decode('utf-8') for chunk in response.content) 
    print(Url_String)
    parsed_data = ujson.loads(Url_String)
    ip = parsed_data['IP']
    port = parsed_data['Port']

    checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)
    log.basicConfig(level=log.INFO)
    mqtt_log = log.getLogger("MQTT")
    ms = utime.ticks_ms() % 1000
    client_ID = "CenMark_hardware_{}".format(ms)
    print(client_ID)

    class UARTHandler:
        def __init__(self, port=UART.UART2, baud_rate=115200, data_bits=8, parity=0, stop_bits=1, flow_control=0):
            self.uart = UART(port, baud_rate, data_bits, parity, stop_bits, flow_control)
            self.received_message = ""
            _thread.start_new_thread(self.uart_handler_thread, ())

        def uart_write(self, data):
            try:
                print("Sending: {}".format(data))
                self.uart.write(data)
            except Exception as e:
                print("UART Write Error:", e)

        def uart_read(self):
            nonlocal Ack_State
            if self.uart.any():
                try:
                    read_data = self.uart.read().decode('utf-8').strip()
                    print("Received from ESP32> {}".format(read_data))
                    self.received_message = read_data
                    if self.received_message.startswith("##*"):
                        segments = ["##{}".format(part.strip()) for part in self.received_message.split("##") if part.strip()]
                        for segment in segments:
                            print(segment)  
                            mqtt_handler.client.publish(Publish_Master, "{}\n".format(segment))        
                        self.received_message = ""  
                    if self.received_message == "##*AVL*3002850*010":
                        Ack_State = 1
                    elif self.received_message == "##*AVL*3002850*000":
                        Ack_State = 0
                except Exception as e:
                    print("UART Read Error:", e)
            utime.sleep(1)

        def uart_handler_thread(self):
            while True:
                try:
                    self.uart_read()
                except Exception as e:
                    print("UART Thread Error:", e)
                utime.sleep(1)

    class MQTTHandler:
        utime.sleep(0.1)
        def __init__(self):
            self.client = None

        def sub_cb(self, topic, msg):
            nonlocal state
            mqtt_log.info("Subscribe Recv: Topic={}, Msg={}".format(topic.decode(), msg.decode()))
            received_msg = msg.decode('utf-8')
            print(received_msg)
            if  received_msg == "$$*RCD*3002850" :
                uart_handler.uart.write("GSMNFO")
            elif  received_msg.startswith("@@") or received_msg.startswith("&&"):  
                uart_handler.uart.write(received_msg)   
            state = 1 
            utime.sleep(1)

        def mqtt_connect(self):
            try:
                self.client = MQTTClient(client_ID, ip, port, keepalive=300)
                self.client.set_callback(self.sub_cb)
                self.client.connect()
                self.client.subscribe(Subscribe_Master.encode())  
                mqtt_log.info("Connected to MQTT Broker and Subscribed to Topic")
                return True
            except Exception as e:
                mqtt_log.error("Failed to connect: {}".format(e))
                return False

        def mqtt_handler_thread(self):
            nonlocal state
            if self.mqtt_connect():
                self.client.publish(Publish_Master, "##Master>{}*Online".format(Master_ID))
                mqtt_log.info("Published Initial Message")
                Master_Info()
                while True:
                    try:
                        self.client.wait_msg()  
                    except OSError as e:
                        if e.args[0] == 107: 
                            mqtt_log.warn("Disconnected. Reconnecting...")
                            utime.sleep(5)
                            if self.mqtt_connect():
                                mqtt_log.info("Reconnection successful!")
                            else:
                                mqtt_log.error("Reconnection failed. Retrying...")
                                utime.sleep(5)
                    except Exception as e:
                        mqtt_log.error("MQTT Exception: {}".format(e))

                    if state == 1:
                        state = 0 
            else:
                mqtt_log.error("Initial MQTT connection failed. Exiting MQTT thread.")

        def publish_message(self, message):
            try:
                if self.client:
                    self.client.publish(Publish_Master.encode(), message.encode())
                    utime.sleep(1)
                    mqtt_log.info("Published message: {}".format(message))
                else:
                    mqtt_log.error("MQTT client not connected")
            except Exception as e:
                mqtt_log.error("Error publishing message: {}".format(e))

    def Master_Info():
        try:
            imsi = sim.getImsi()
            operator_name = net.operatorName()
            state = net.getState()
            network_state = state[0]
            iccid = sim.getIccid()

            message = "##WMfO{}{}{}{}{}{}{}{}{}{}{}{}{}*$$".format(
                Master_ID, imsi, iccid, operator_name[0], operator_name[2], operator_name[3],
                network_state[0], network_state[1], network_state[2], network_state[3],
                latitude, longitude, LdR
            ).encode()
            mqtt_handler.publish_message(message)
        except Exception as e:
            mqtt_log.warn("Master_Info Error: {}".format(e))

    def Seprator(value):
        mqtt_handler = MQTTHandler()
        if value.startswith("##*"):
            mqtt_handler.client.publish(Publish_Master,value)  
        value =""   

    uart_handler = UARTHandler()
    utime.sleep(0.1)
    stagecode, subcode = checknet.wait_network_connected(30)
    if stagecode == 3 and subcode == 1:
        mqtt_log.info("Network connection successful!")
        mqtt_handler = MQTTHandler()   
        _thread.start_new_thread(mqtt_handler.mqtt_handler_thread, ())
        utime.sleep(2)
        j = 0 
    else:
        mqtt_log.error("Network connection failed! stagecode = {}, subcode = {}".format(stagecode, subcode))

    utime.sleep(1)
    while True:
        response = request.get("https://api.cenpilot.in/api/CenPilot/ValveListONCount?Id=3002850")
        Url_String = "".join(chunk.decode('utf-8') for chunk in response.content)
        message = "int> " + Url_String
        uart_handler.uart_write(message)
        j += 1
        if j > 30:
              Master_Info()
              j = 0 
        utime.sleep(2)

if __name__ == '__main__':
    start()
