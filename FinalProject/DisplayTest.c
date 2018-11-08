#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "gpio-utils.h"

#include <stdint.h>
#include <inttypes.h>
#include <string.h>

static int DataCgpio;
static int rstgpio;
static const char* device;
static long speed;
static int bits;
static int fd;
static uint8_t mode;
static void pabort(const char *s)
{
        perror(s);
        abort();
}

void _spiWrite(uint8_t v){
                uint8_t rx;
                signed char ret;
printf("I write");
                        struct spi_ioc_transfer tr={
               .tx_buf= (unsigned long)v,
               .rx_buf= (unsigned long)rx,
               .len = sizeof(v),
               .delay_usecs = 0,
               .speed_hz = 100000,
               .bits_per_word = 8};

        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
                if (ret < 1)
                pabort("Can't send spi message");
}

void _spiWriteCommand(uint8_t c) {
    
    _spiWrite(c);
}
void TFT_ILI9225_SPI(unsigned int newrstgpio,unsigned int newDataCgpio,const char* newdevice,uint16_t newspeed){
rstgpio = newrstgpio;
DataCgpio = newDataCgpio;
device=newdevice;
bits=8;
speed=newspeed;

}


void _spiWriteData(uint8_t c) {
    _spiWrite(c);
}

uint16_t Read16(void){
        //16 spi transfer of 0x0000
        uint16_t rx;
printf("I read");
        unsigned char ret;
        struct spi_ioc_transfer tr={
               .tx_buf = (unsigned long)0x0000,
               .rx_buf = (unsigned long)rx,
               .len = sizeof(rx),
               .delay_usecs = 0,
               .speed_hz = 100000,
               .bits_per_word = 8};

        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
                if (ret < 1)
                pabort("Can't send spi message");
                return rx;
}
uint16_t ReadAxis(int Axis)
{//needs a fix
  int Data;

  if (Axis)
  {
    _spiWriteCommand(0x90);   
  }else
  {
    _spiWriteCommand(0xD0);
  }
  Data = Read16()>>3; //read 16
  
  return Data;
}

int main(int argc, char* argv[]){
TFT_ILI9225_SPI(26,23,"/dev/spidev2.1",50000);

                signed char ret;
                fd = open(device, O_RDWR);
                        if (fd < 0)
                pabort("can't open device");

        /*
         * spi mode
         */
        ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
        if (ret == -1)
                pabort("can't set spi mode");

        ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
        if (ret == -1)
                pabort("can't get spi mode");

        /*
         * bits per word
         */
        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't get bits per word");
       ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't set bits per word");

        /*
         * max speed hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't set max speed hz");

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't get max speed hz");
        
                //setup spi controller here

    // Initialization Code
    
     while(1){
int xvalue;
int yvalue;
xvalue = ReadAxis(0);
yvalue = ReadAxis(1);
printf("Xvalue: %d\n",xvalue);
printf("Yvalue: %d\n",yvalue);
usleep(100000);
}
return 0;
}
