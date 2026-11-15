from umqtt import MQTTClient
from machine import UART
import utime
import log
import checkNet
import sim
import checkNet
import sys
import net

PROJECT_NAME = "Cen_Networks"
PROJECT_VERSION = "V.1.1.0"
message = "" 
received_message = ""
previous_Message = b""  
Master_ID = 3002850
checknet = checkNet.CheckNetwork(PROJECT_NAME, PROJECT_VERSION)
uart = UART(UART.UART2, 115200, 8, 0, 1, 0)
log.basicConfig(level=log.INFO)
mqtt_log = log.getLogger("MQTT")
state = 0

def sub_cb(topic, msg):
    global state
    mqtt_log.info("Subscribe Recv: Topic={}, Msg={}".format(topic.decode(), msg.decode()))
    Separator(msg)  
    state = 1  

def Separator(Data):
    global flag, previous_Message, message
    if Data.startswith(b"@@*"):
        message = "##*{}*C@K".format(Master_ID).encode()
        c.publish(b"RX_Valve_test", message)
        uart.write(Data)  
        utime.sleep(1)
    elif Data.startswith(b"##*"):
        message = Data.encode()
        c.publish(b"RX_Valve_test", message)  
        utime.sleep(1)

if __name__ == "__main__":
    sim_status = sim.getStatus()
    if sim_status != 1:
        print("SIM card is not ready. Status:", sim_status)
        sys.exit()
        utime.sleep(1)
        print("SIM card is ready.")
    stage, state = checkNet.waitNetworkReady(30)
    if stage == 3 and state == 1:
        print('Network connection successful.')
    else:
        print('Network connection failed, stage={}, state={}'.format(stage, state))
        sys.exit()

    imsi = sim.getImsi()
    print("IMSI:", imsi if type(imsi).__name__ != 'int' else "Failed to retrieve IMSI")

    iccid = sim.getIccid()
    print("ICCID:", iccid if type(iccid).__name__ != 'int' else "Failed to retrieve ICCID")

    operator_name = net.operatorName()
    print('Current Network Operator:', operator_name)

    state = net.getState()
    print('Network state:', state)    

    stagecode, subcode = checknet.wait_network_connected(30)  

    if stagecode == 3 and subcode == 1:
        mqtt_log.info("Network connection successful!")
        c = MQTTClient("Cenmark_Valve_Control", "198.38.81.119", 1883)
        c.set_callback(sub_cb)
        c.connect()
        c.subscribe(b"Cenmark_PPV")
        mqtt_log.info("Connected to Cenmark_PPV topic")

        message = ("Master > {} Online".format(Master_ID)).encode()
        c.publish(b"RX_Valve_test", message)
        utime.sleep(1)

        network_state = state[0]
        message = "##*MfO*{}*{}*{}*{}*{}*{}*{}*{}*{}*{}*$$".format(
            Master_ID, imsi, iccid, operator_name[0], operator_name[2],operator_name[3],
            network_state[0], network_state[1], network_state[2], network_state[3]
        ).encode()

        c.publish(b"RX_Valve_test", message)
        utime.sleep(1)
        uart.write("Mqtt.OK\n")
        utime.sleep(0.5)
        uart.write("GSM.OK\n")
        utime.sleep(0.5)

        while True: 
            c.wait_msg()  
            if uart.any(): 
                Read = " " 
                Read = uart.read()
                uart.write("MsRcs.OK.HS");
                print("Read: ",Read)
                if Read:
                    data2 = Read.strip()
                    if data2:
                        print("\n",data2)
                        Separator(data2.encode()) 
    else:
        mqtt_log.info("Network connection failed! stagecode = {}, subcode = {}".format(stagecode, subcode))
