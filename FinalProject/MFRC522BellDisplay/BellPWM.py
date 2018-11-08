#!/usr/bin/env python3
import Adafruit_BBIO.PWM as PWM
import sys
import time
# Black
PWM1 = "P8_13"
PWM2 = "P8_36"
PWM3 = "P9_46"
if(int(sys.argv[1],16)==int("5C7DCD9478",16)):
	selectedPWM=PWM1
if(int(sys.argv[1],16)==int("23EB103DB",16)):
        selectedPWM=PWM2
if(int(sys.argv[1],16)==int("8F98611919",16)):
        selectedPWM=PWM3
selectedPWM=PWM1
PWM.start(selectedPWM, 10,200,1)
for i in range(3):
	PWM.start(selectedPWM,60,200,1)
	time.sleep(0.25)
	PWM.start(selectedPWM,40,200,1)        
	time.sleep(0.25)
