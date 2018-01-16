import logging
import re
import paho.mqtt.client as mqtt
import serial
import time

ser = serial.Serial(
    port='/dev/ttyS1',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)

MAIN_TOPIC = 'nixie'

# will create a dictionary consisting of methods annotated with @action to call when a specific
# topic event is received, example 9xie/button will call do_button
actions = {}
action = lambda f: actions.setdefault(MAIN_TOPIC + "/" + f.__name__[3:], f)

mqttc = mqtt.Client()

responseCache = []


@action
def do_direct_serial(msg):
    logging.info("Got serial payload " + str(msg.payload))
    ser.write(str(msg.payload) + '\n')


@action
def do_nixie(msg):
    logging.info("Got nixie payload" + str(msg.payload))
    ser.write('nixie' + str(msg.payload) + '\n')


@action
def do_pixel(msg):
    logging.info("Got pixel payload" + str(msg.payload))
    ser.write('pixel' + str(msg.payload) + '\n')


@action
def do_button(msg):
    logging.info("Got button payload" + str(msg.payload))
    ser.write('button' + str(msg.payload) + '\n')


@action
def do_out(msg):
    responseCache.append(msg.payload)


@action
def do_test(msg):
    commands = {
        'nixie': [
            "V=ON",
            "V=OFF",
            "Interval=100",
            "Run=COUNTER",
            "Run=T_COUNTER",
            "Run=DISPLAY",
            "Run=SCROLL",
            "Run=PULSE",
            "Run=WHIPE",
            "Write=123456",
            "Write=123456"
        ],
        'pixel': [
            "V=ON",
            "V=OFF",
            "Interval=100",
            "Duration=100",
            "Brightness=100",
            "Brightness=100",
            "Run=NONE",
            "Run=RAINBOW_CYCLE",
            "Run=THEATER_CHASE",
            "Run=COLOR_WIPE",
            "Run=SCANNER",
            "Run=FADE",
            "Run=PULSE",
            "Color1=255,125,64",
            "Color2=255,125,64",
            "Mask=NONE",
            "Mask=001001001"
        ]
    }

    last_response = 'NONE'
    command = 'NONE'

    try:
        for channel, channel_commands in commands:
            for command in channel_commands:
                mqttc.publish(channel, command)
                logging.info("Sending command %s to %s" % (command, channel))
                time.sleep(1)
                last_response = responseCache.pop(0)  # handle indexerror
                assert last_response == 'success'

    except (AssertionError, IndexError) as err:
        logging.exception("Test failed on command %s with response %s" % (command, last_response))


def setup_logging():
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
                        datefmt='%m-%d %H:%M',
                        filename='/tmp/myapp.log',
                        filemode='w')
    # define a Handler which writes INFO messages or higher to the sys.stderr
    console = logging.StreamHandler()
    console.setLevel(logging.INFO)
    # set a format which is simpler for console use
    formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
    # tell the handler to use this format
    console.setFormatter(formatter)
    # add the handler to the root logger
    logging.getLogger('').addHandler(console)


def __main__():
    setup_logging()

    def on_connect(client, userdata, flags, rc):
        logging.info("Connected with result code " + str(rc))
        mqttc.subscribe(topic=MAIN_TOPIC + '/#', qos=0)

    def on_subscribe(client, userdata, mid, granted_qos):
        logging.info("Subscribed: " + str(mid) + " " + str(granted_qos))

    def on_message(client, userdata, msg):
        logging.debug("Received '" + str(msg.payload) + "' on topic '" + msg.topic + "' with QoS " + str(msg.qos))

        for regex, act in actions.items():
            m = re.match(regex, msg.topic)
            if m:
                act(msg)
                break

    def on_disconnect(client, userdata, rc):
        logging.info("Disconnect from server")

    # Assign the callbacks
    mqttc.on_message = on_message
    mqttc.on_connect = on_connect
    mqttc.on_subscribe = on_subscribe
    mqttc.on_disconnect = on_disconnect

    mqttc.connect('127.0.0.1')
    mqttc.loop_forever()


if __name__ == '__main__':
    __main__()
