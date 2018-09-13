1. The Buttons_to_LEDs.py lets you control 4 leds via 4 buttons. The buttons are on pins P8_32,P8_34,P8_36,P8_38. The LEDS are on pins P8_7-P8_10.
2. The Etch_matt_howlett.py is my adapted etch-a-sketch that lets the user control motion, clear and quit via gpios tied to buttons.
Quit = P8_27, clear = P8_28, up = P8_30, left = P8_32,right = P8_34, down = P8_36. It works!!
3. The gpiotoggle.py allows for pin toggle testing using python. Follow onscreen prompts to set pin as in PX_X and time in seconds
4. The pintoggle program biult from the c and h files present allows for pin toggle testing using c. First parameter is the gpio parameter and second is duration of ontime in us. GPIO number example: for P8_10 the parameter would be 68 corresponding to GPIO_68. 
