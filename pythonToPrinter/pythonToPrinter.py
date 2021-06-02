import serial
import time
from time import ctime

#print(ctime())
#print de lokale tijd


arduino = serial.Serial('COM5',9600)
#sluit string af met een '/'
l = "15.45,nederland,17:45,20-6-2021,123333337\0"
h = ""


def stuur_character_HIGH():
    arduino.write(l.encode())
    print("sending", l)
    time.sleep(1)


time.sleep(2)
stuur_character_HIGH()


while(True):
    if(arduino.in_waiting >= 0):
        data = arduino.readline()[:-2]
        decoded_data = str(data,'utf-8')
        print("on arduino com:", decoded_data)
    