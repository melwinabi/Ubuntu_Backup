import _thread
from queue import Queue
from usr.serial import Serial
from usr.mqttIot import MqttIot
from usr.socketIot import SocketIot
from usr.logging import getLogger
import time

logger = getLogger(__name__)

class DTU:
    def __init__(self, config):
        self.queue = Queue()  
        self.serial = Serial(**config.get('uart_config'))

        cloud_type = config.get('system_config.cloud')
        
        if cloud_type == "mqtt":
            mqtt_config = config.get('mqtt_private_cloud_config')
            self.cloud = MqttIot(
                mqtt_config['client_id'],
                mqtt_config['server'],
                port=mqtt_config['port'],
                user=mqtt_config['user'],
                password=mqtt_config['password'],
                keepalive=mqtt_config['keepalive'],
                clean_session=mqtt_config['clean_session'],
                qos=mqtt_config['qos'],
                subscribe_topic=mqtt_config['subscribe'],
                publish_topic=mqtt_config['publish'],
                queue=self.queue,
                error_trans=True
            )
            
        elif cloud_type == "tcp":
            socket_config = config.get('socket_private_cloud_config')
            self.cloud = SocketIot(
                ip_type=socket_config['ip_type'],
                keep_alive=socket_config['keep_alive'],
                domain=socket_config['domain'],
                port=socket_config['port'],
                queue=self.queue,
                error_trans=True
            )

    def run(self):
        """ Initialize cloud and start data processing threads """
        self.cloud.init()

        # Start threads for up and down data processing
        _thread.start_new_thread(self.up_transaction_handler, ())
        _thread.start_new_thread(self.down_transaction_handler, ())

    def down_transaction_handler(self):
        """ Handles incoming data and writes it to serial """
        logger.info('Starting down transaction thread: {}'.format(_thread.get_ident()))
        
        while True:
            try:
                topic, data = self.cloud.recv()
                logger.info('Received downlink msg: {}'.format(data))
                
                # Write data to the STM32 UART or ESP32 UART
                self.serial.write(data)
                
                # Small delay to avoid busy looping
                time.sleep(0.1)
            
            except Exception as e:
                logger.error(f"Error in down_transaction_handler: {str(e)}")

    def up_transaction_handler(self):
        """ Handles outgoing data and sends it to the cloud """
        logger.info('Starting up transaction thread: {}'.format(_thread.get_ident()))
        
        while True:
            try:
                # Read from STM32 or ESP32 UART
                data = self.serial.read(1024, timeout=100)
                
                if data:
                    logger.info('Sending uplink msg: {}'.format(data))
                    self.cloud.send(data)
                
                # Small delay to prevent busy looping
                time.sleep(0.1)
            
            except Exception as e:
                logger.error(f"Error in up_transaction_handler: {str(e)}")
