#!/usr/bin/env python3
import Adafruit_BBIO.GPIO as GPIO
import smbus
import time
bus = smbus.SMBus(2)
matrix =0x70
bus.write_byte_data(matrix, 0x21, 0)   # Start oscillator (p10)
bus.write_byte_data(matrix, 0x81, 0)   # Disp on, blink off (p11)
bus.write_byte_data(matrix, 0xe7, 0)   # Full brightness (page 15)


Buttons = ["P8_27","P8_28","P8_30","P8_32","P8_34","P8_36"]
matrixarray = [0x00]*etchwinsize*etchwinsize*2

for myButton in Buttons:
	print(myButton)
	GPIO.setup(myButton, GPIO.IN)
	GPIO.add_event_detect(myButton, GPIO.FALLING)
def main():
	etchwinsize=int(input("Window Size nxn n = ")) #window size plus one column for row numbers
	cursorlocation = etchwinsize+2 #sets default location of cursor
	display_array = ["1"]*etchwinsize*etchwinsize #initial display array
	display_array = renderdisplay(cursorlocation,etchwinsize,display_array)#renders the text display
	while(1):
		if GPIO.event_detected(Buttons[0]):
			break
		if GPIO.event_detected(Buttons[1]):
			display_array = ["1"]*etchwinsize*etchwinsize
			cursorlocation = etchwinsize+2
			display_array = renderdisplay(cursorlocation,etchwinsize,display_array)#renders the text display
		elif GPIO.event_detected(Buttons[2]):
			if (cursorlocation/etchwinsize>2):
				cursorlocation=cursorlocation-etchwinsize
				display_array = renderdisplay(cursorlocation,etchwinsize,display_array)#renders the text display
		elif GPIO.event_detected(Buttons[3]):
			if (cursorlocation%etchwinsize>1):
				cursorlocation=cursorlocation-1
				display_array = renderdisplay(cursorlocation,etchwinsize,display_array)#renders the text display
		elif GPIO.event_detected(Buttons[4]):
			if (cursorlocation%etchwinsize<etchwinsize-1):
				cursorlocation=cursorlocation+1
				display_array = renderdisplay(cursorlocation,etchwinsize,display_array)#renders the text display
		elif GPIO.event_detected(Buttons[5]):
			if (cursorlocation/etchwinsize<etchwinsize-1):
				cursorlocation=cursorlocation+etchwinsize
				display_array = renderdisplay(cursorlocation,etchwinsize,display_array)#renders the text display

def renderdisplay(cursorlocation,etchwinsize,display_array):
	for index in range(0,etchwinsize*etchwinsize):#goes through the array and displays if row number, blank or x for where the cursor has been
		if("x" not in str(display_array[index])):
			display_array[index]=" "
	display_array[cursorlocation] = "x"
	for index in range(0,etchwinsize*etchwinsize):
		if (index==0):
			matrixarray[index] = display_array[index]
		else:
			matrixarray[index*2+1] = display_array[index]
	bus.write_i2c_block_data(matrix,0,matrixarray)
	return display_array# returns the array to keep track of the where the cursor has been
main()
