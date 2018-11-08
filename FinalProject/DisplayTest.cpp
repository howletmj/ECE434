#include <stdio.h>
#include "TFT_ILI9225_SPI.h"

int main(int argc, char* argv[]){
TFT_ILI9225_SPI touchscreen(46,47,"/dev/spidev2.1",50000);
touchscreen.setBacklightBrightness(255);
while(1){

}
return 0;
}
