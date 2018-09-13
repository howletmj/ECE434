#!/usr/bin/env python3
import Adafruit_BBIO.GPIO as GPIO
import smbus
import time
from Adafruit_BBIO.Encoder import RotaryEncoder, eQEP2,eQEP1
myEncoder = RotaryEncoder(eQEP1)
myEncoder2 = RotaryEncoder(eQEP2)
myEncoder.setAbsolute()
myEncoder.enable()
myEncoder.zero()
myEncoder2.setAbsolute()
myEncoder2.enable()
myEncoder2.zero()
bus = smbus.SMBus(2)
matrix =0x70
bus.write_byte_data(matrix, 0x21, 0)   # Start oscillator (p10)
bus.write_byte_data(matrix, 0x81, 0)   # Disp on, blink off (p11)
bus.write_byte_data(matrix, 0xe7, 0)   # Full brightness (page 15)


Buttons = ["P8_27","P8_28"]
matrixarray=[0x00]
for myButton in Buttons:
	print(myButton)
	GPIO.setup(myButton, GPIO.IN)
	GPIO.add_event_detect(myButton, GPIO.FALLING)
def main():
	etchwinsize=int(input("Window Size nxn n = ")) #window size plus one column for row numbers
	global matrixarray
	matrixarray = [0x00]*etchwinsize*2
	cursorlocation = etchwinsize+2 #sets default location of cursor
	display_array = ["1"]*etchwinsize*etchwinsize #initial display array
	previouslocation =cursorlocation
	display_array = renderdisplay(previouslocation,cursorlocation,etchwinsize,display_array)#renders the text display
	while(1):
		if GPIO.event_detected(Buttons[0]):
			break
		if GPIO.event_detected(Buttons[1]):
			display_array = ["1"]*etchwinsize*etchwinsize
			cursorlocation = etchwinsize+2
			display_array = renderdisplay(previouslocation,cursorlocation,etchwinsize,display_array)#renders the text display
		elif (myEncoder.position<0):#up
			if (cursorlocation/etchwinsize>1):
				previouslocation=cursorlocation
				cursorlocation=cursorlocation-etchwinsize
				display_array = renderdisplay(previouslocation,cursorlocation,etchwinsize,display_array)#renders the text display
			myEncoder.zero()
		elif (myEncoder2.position<0):#left
			if (cursorlocation%etchwinsize>0):
				previouslocation = cursorlocation
				cursorlocation=cursorlocation-1
				display_array = renderdisplay(previouslocation,cursorlocation,etchwinsize,display_array)#renders the text display
			myEncoder2.zero()
		elif (myEncoder2.position>0):
			if (cursorlocation%etchwinsize<etchwinsize-1):
				previouslocation = cursorlocation
				cursorlocation=cursorlocation+1
				display_array = renderdisplay(previouslocation,cursorlocation,etchwinsize,display_array)#renders the text display
			myEncoder2.zero()
		elif (myEncoder.position>0):
			if (cursorlocation<etchwinsize*(etchwinsize-1)):
				previouslocation=cursorlocation
				cursorlocation=cursorlocation+etchwinsize
				display_array = renderdisplay(previouslocation,cursorlocation,etchwinsize,display_array)#renders the text display
			myEncoder.zero()

def renderdisplay(previouslocation,cursorlocation,etchwinsize,display_array):
	display_array[cursorlocation] = "x"
	global matrixarray
	for index in range(etchwinsize*etchwinsize):
                print(" " + str(display_array[index]) + " ", end="", flush=True)#prints without a newline
                if(index%etchwinsize==etchwinsize-1):
                        print("\n")

	prevxloc = int(previouslocation/etchwinsize)
	prevyloc = previouslocation%etchwinsize
	xloc = int(cursorlocation/etchwinsize)
	yloc = cursorlocation%etchwinsize
	matrixarray[prevxloc*2+1]=0x00
	matrixarray[prevxloc*2]=matrixarray[prevxloc*2]|(2**yloc)
	matrixarray[xloc*2+1]=matrixarray[xloc*2+1]|(2**yloc)
	print(cursorlocation)
	bus.write_i2c_block_data(matrix,0,matrixarray)
	return display_array# returns the array to keep track of the where the cursor has been
main()
