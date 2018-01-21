#!/user/bin/env python

import socket
import paho.mqtt.client as mqtt
import time
import scrollphathd
from scrollphathd.fonts import font5x5
from scrollphathd.fonts import font5x7

msg_queue = list()

def internet_connected(host="8.8.8.8", port=53):
    """
    Host: 8.8.8.8 (google-public-dns-a.google.com)
    OpenPort: 53/tcp
    Service: domain (DNS/TCP)
    """
    try:
        socket.setdefaulttimeout(1)
        socket.socket(socket.AF_INET, socket.SOCK_STREAM).connect((host, port))
        return True
    except Exception as ex:
        pass

    return False

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected!")
    else:
        print("Connection failed!")

def on_message(client, userData, message):
    decoded_message = str(message.payload.decode("utf-8"))
    print("MSG: " + str(message.topic.decode("utf-8")) + " " + decoded_message)
    current_message = "New Msg: " + decoded_message + "      "
    msg_queue.append(current_message)

connected = False

while True:
    if internet_connected():
        # If previously internet was disconnected, then print message
        # and set BlinkStick to green
        if not connected:
            print("Internet is up!")
            connected = True

        # Wait for 1 second before checking for internet connectivity
        break
    else:
        # If previously internet connected, then print message
        if connected:
            print("Internet is down...")
            connected = False



DISPLAY_BAR = True

BRIGHTNESS = 0.3
scrollphathd.rotate(180)
scrollphathd.set_brightness(0.1)

BROKER_ADDRESS = "192.168.1.20"
client = mqtt.Client("rpi_1")

#client.username_pw_set("username","password")

client.on_message=on_message
client.on_connect=on_connect

print ("Connecting to broker...")
client.connect(BROKER_ADDRESS)
client.subscribe("test/rpi_1")

try:
    while True:
        if len(msg_queue) > 0 :

            scrollphathd.clear()
            THIS_MESSAGE = msg_queue.pop(0)
            BUFFER = scrollphathd.write_string(THIS_MESSAGE, x=17, y=0, font=font5x7)

            for x in range(BUFFER * 2):
                scrollphathd.show()
                scrollphathd.scroll(1)
                time.sleep(0.02)

        else:
            scrollphathd.clear()
            float_sec = (time.time() % 60) / 59.0
            seconds_progress = float_sec *15

            if DISPLAY_BAR:
                for y in range(15):
                    current_pixel = min(seconds_progress,1)
                    scrollphathd.set_pixel(y+1,6,current_pixel)
                    seconds_progress -=1

                    if seconds_progress <- 0:
                        break
            else:
                scrollphathd.set_pixel(int(seconds_progress),6, 0.3)

            scrollphathd.write_string(
                time.strftime("%H:%M"),
                x=0,
                y=0,
                font=font5x5
            )

            if int(time.time()) %2 ==0:
                scrollphathd.clear_rect(8,0,1,5)

            scrollphathd.show()

            client.loop()

    #scrollphathd.show()
    time.sleep(0.1)



except KeyboardInterrupt:
    print("Exiting...")
    client.disconnect()
    client.loop_stop()
