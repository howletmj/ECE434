#!/usr/bin/env python3
import Adafruit_BBIO.GPIO as GPIO
import time
Buttons = ["P8_32","P8_34","P8_36","P8_38"]
LEDs = ["P8_7","P8_8","P8_9","P8_10"]
for myButton in Buttons:
	GPIO.setup(myButton,GPIO.IN)
	GPIO.add_event_detect(myButton,GPIO.FALLING)
for theLED in LEDs:
	GPIO.setup(theLED,GPIO.OUT)
while(True):
	for i in range(len(Buttons)):
		if GPIO.event_detected(Buttons[i]):
			GPIO.output(LEDs[i],GPIO.HIGH)
			time.sleep(0.5)
		GPIO.output(LEDs[i],GPIO.LOW)
