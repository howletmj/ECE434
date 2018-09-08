#!/usr/bin/env python3
import Adafruit_BBIO.GPIO as GPIO
import time
speed =float(input("Time between Toggling in s:"))
pin = input("Pin to Toggle:")
GPIO.setup(pin,GPIO.OUT)
while(True):
	GPIO.output(pin,GPIO.HIGH)
	time.sleep(speed)
	GPIO.output(pin,GPIO.LOW)
	time.sleep(speed)
