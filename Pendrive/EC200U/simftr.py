# -*- coding: UTF-8 -*-
# Example
import sim
import checkNet
import sys
import net
 
if __name__ == '__main__':
    
    # 1. Query SIM card status
    sim_status = sim.getStatus()
    if sim_status != 1:
        print("SIM card is not ready. Status:", sim_status)
        sys.exit()
    
    print("SIM card is ready.")

    # 2. Wait for network registration to be successful    
    stage, state = checkNet.waitNetworkReady(30)
    if stage == 3 and state == 1:
        print('Network connection successful.')
    else:
        print('Network connection failed, stage={}, state={}'.format(stage, state))
        sys.exit()

    # 3. Query SIM card information after network registration
    imsi = sim.getImsi()
    print("IMSI:", imsi if type(imsi).__name__ != 'int' else "Failed to retrieve IMSI")

    iccid = sim.getIccid()
    print("ICCID:", iccid if type(iccid).__name__ != 'int' else "Failed to retrieve ICCID")

    
    operator_name = net.operatorName()
    print('Current Network Operator:', operator_name)

    state = net.getState()
    print('Network state:', state)