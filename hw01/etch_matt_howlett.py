#!/usr/bin/env python3 
def main():
	etchwinsize=int(input("Window Size nxn n = "))+1 #window size plus one column for row numbers
	cursorlocation = etchwinsize+2 #sets default location of cursor
	display_array = ["1"]*etchwinsize*etchwinsize #initial display array
	while(1):
		display_array = renderdisplay(cursorlocation,etchwinsize,display_array)#renders the text display
		print("\n Commands: \n")#shows the commands to run the program
		print("q to quit      c to clear      w to move up\na to move left      s to move down      d to move right")
		inputcommand = input("\n Command:")#takes a command and then does an appropriate action		
		if(inputcommand=="q"):
			break
		if(inputcommand=="c"):
			display_array = ["1"]*etchwinsize*etchwinsize
			cursorlocation = etchwinsize+2
		elif (inputcommand=="w"):
			if (cursorlocation/etchwinsize>2):
				cursorlocation=cursorlocation-etchwinsize
		elif (inputcommand=="a"):
			if (cursorlocation%etchwinsize>1):
				cursorlocation=cursorlocation-1
		elif (inputcommand=="d"):
			if (cursorlocation%etchwinsize<etchwinsize-1):
				cursorlocation=cursorlocation+1
		elif (inputcommand=="s"):
			if (cursorlocation/etchwinsize<etchwinsize-1):
				cursorlocation=cursorlocation+etchwinsize
	
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
	return display_array# returns the array to keep track of the where the cursor has been
main()