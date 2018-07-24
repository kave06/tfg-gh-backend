import json
from datetime import datetime
import configparser

import paho.mqtt.client as mqtt
from requests import get, put, post

from ghTools.irrigation import Irrigation
from ghTools.logger import Logger
from ghTools.climate import Climate

logger = Logger().init_logger()

config = configparser.ConfigParser()
config.read('./etc/config.ini')

list_flow = []
list_date_flow = []


def on_connect(client, userdata, flags, rc):
    print('Connected with result code {}'.format(rc))


def on_message(client, userdata, msg):
    print('Received message {} on topic {} with QoS {}'
          .format((msg.payload).decode(), msg.topic, msg.qos))


def on_message_flow(client, userdata, msg):
    data = msg.payload.decode()
    if data[0] == '{':
        try:
            data_dict = json.loads(data)
            ir = Irrigation(id_relay=data_dict['sensor'])
            ir.liters = data_dict['liters']

            if data_dict['liters'] is 0 and len(list_flow) > 1:
                ir.start = list_date_flow.pop(0)
                ir.end = list_date_flow.pop()
                item_tmp = list_flow.pop()
                logger.debug(item_tmp)
                item_dict = json.loads(item_tmp)
                ir.liters = item_dict['liters']

                logger.debug('start:\t{}'.format(ir.start))
                logger.debug('end:\t{}'.format(ir.end))
                logger.debug('liters:\t{}'.format(ir.liters))

                ir.insert_liters()

                list_flow.clear()
                list_date_flow.clear()

            else:
                list_flow.append(data)
                list_date_flow.append(datetime.now())
                logger.debug(data)

        except Exception as err:
            logger.error(err)
    else:
        logger.debug(data)
        print(data)


def on_message_sensor(client, userdata, msg):
    data = msg.payload.decode()
    if data[0] == '{':
        try:
            data_dict = json.loads(data)
            logger.debug(data_dict)
            climate = Climate(sensor=data_dict['sensor'])
            climate.temp = data_dict['temp']
            climate.humi = data_dict['humi']
            climate.insert_climate()
        except Exception as err:
            logger.error(err)
    else:
        logger.debug(data)
        print(data)


client = mqtt.Client()

client.message_callback_add(config.get('topic', 'sensor1'), on_message_sensor)
client.message_callback_add(config.get('topic', 'sensor2'), on_message_sensor)
client.message_callback_add(config.get('topic', 'sensor3'), on_message_sensor)
client.message_callback_add(config.get('topic', 'sensor4'), on_message_flow)

client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(username=config.get('mqtt', 'user'), password=config.get('mqtt', 'passw'))
client.connect(host=config.get('server', 'host'), port=config.getint('server', 'port'), keepalive=60)

client.subscribe(config.get('topic', 'general'), 1)

client.loop_forever()
