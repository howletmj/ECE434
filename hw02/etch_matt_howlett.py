#!/usr/bin/env python3
import Adafruit_BBIO.GPIO as GPIO
#Buttons = ["GPIO_67","GPIO_68","GPIO_44","GPIO_26","GPIO_46","GPIO_65"]
Buttons = ["P8_27","P8_28","P8_30","P8_32","P8_34","P8_36"]
for myButton in Buttons:
	print(myButton)
	GPIO.setup(myButton, GPIO.IN)
	GPIO.add_event_detect(myButton, GPIO.FALLING)
def main():
	etchwinsize=int(input("Window Size nxn n = "))+1 #window size plus one column for row numbers
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
			if(index<etchwinsize):
				display_array[index]=index
			elif(index%etchwinsize==0):
				display_array[index]=int(index/etchwinsize)
			else:
				display_array[index]=" "
	display_array[cursorlocation] = "x"
	for index in range(0,etchwinsize*etchwinsize):
		print(" " + str(display_array[index]) + " ", end="", flush=True)#prints without a newline
		if(index%etchwinsize==etchwinsize-1):
			print("\n")
	print("\n Commands: \n")#shows the commands to run the program
	print("Button 1 to quit      Button 2 to clear      Up Button to move up\nLeft Button to move left      Down Button to move down      Right Button to move right")
	return display_array# returns the array to keep track of the where the cursor has been
main()
