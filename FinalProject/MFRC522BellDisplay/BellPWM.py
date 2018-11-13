#!/usr/bin/env python3
import Adafruit_BBIO.PWM as PWM
import sys
import time
# Black
PWM1 = "P9_16"
PWM2 = "P9_14"
PWM3 = "P8_45"
PWM4 = "P8_46"

if(int(sys.argv[1],16)==int("5C7DCD9478",16)):
	selectedPWM=PWM1
	lowPWM=9
	highPWM=14
elif(int(sys.argv[1],16)==int("23EB103DB",16)):
	selectedPWM=PWM2
	lowPWM=9
	highPWM=14
elif(int(sys.argv[1],16)==int("8F98611919",16)):
        selectedPWM=PWM3
else:
	selectedPWM=PWM4
	lowPWM=9
	highPWM=14
print(selectedPWM)
PWM.start(selectedPWM, 10.0,50,1)
for i in range(3):
	PWM.set_duty_cycle(selectedPWM,14)
	time.sleep(0.25)
		
	PWM.set_duty_cycle(selectedPWM,9)
	time.sleep(0.25)

