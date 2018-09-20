#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <signal.h>    // Defines signal-handling functions (i.e. trap Ctrl-C)
#include "beaglebone_gpio.h"

/****************************************************************
 * Global variables
 ****************************************************************/
int keepgoing = 1;    // Set to 0 when ctrl-c is pressed

/****************************************************************
 * signal_handler
 ****************************************************************/
void signal_handler(int sig);
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig)
{
        printf( "\nCtrl-C pressed, cleaning up and exiting...\n" );
        keepgoing = 0;
}
//one button is on GPIO1_29, another on GPIO2_11
//leds on GPIO2_12 and GPIO2_13
/*Rubber Ducky programing in effect:
Two Buttons on different gpio ports. I need two mmaps to reference each port and check if 0detected in the position specified.
If so set the bit in the set register otherwise set the bit in the clear register.
*/
int main(int argc, char *argv[]) {
    volatile void *gpio_addr1;
    volatile unsigned int *gpio_oe_addr1;
    volatile unsigned int *gpio_setdataout_addr1;
    volatile unsigned int *gpio_cleardataout_addr1;
    unsigned int directionreg;
    volatile void *gpio_addr2;
    volatile unsigned int *gpio_oe_addr2;
    volatile unsigned int *gpio_setdataout_addr2;
    volatile unsigned int *gpio_cleardataout_addr2;
    unsigned int directionreggpio1;
	volatile unsigned int *gpio_datain1;
	volatile unsigned int *gpio_datain2;
    
    // Set the signal callback for Ctrl-C
        signal(SIGINT, signal_handler);
    int fd = open("/dev/mem", O_RDWR);

    printf("Mapping %X - %X (size: %X)\n", GPIO0_START_ADDR, GPIO0_END_ADDR, GPIO0_SIZE);
    printf("Mapping %X - %X (size: %X)\n", GPIO2_START_ADDR, GPIO2_END_ADDR, GPIO2_SIZE);

    gpio_addr1 = mmap(0, GPIO0_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO0_START_ADDR);

    gpio_addr2 = mmap(0, GPIO2_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO2_START_ADDR);
    gpio_datain1            = gpio_addr1 + GPIO_DATAIN;
    gpio_datain2            = gpio_addr2 + GPIO_DATAIN;
    gpio_oe_addr1           = gpio_addr1 + GPIO_OE;
    gpio_setdataout_addr1   = gpio_addr1 + GPIO_SETDATAOUT;
    gpio_cleardataout_addr1 = gpio_addr1 + GPIO_CLEARDATAOUT;
    gpio_oe_addr2           = gpio_addr2 + GPIO_OE;
    gpio_setdataout_addr2   = gpio_addr2 + GPIO_SETDATAOUT;
    gpio_cleardataout_addr2 = gpio_addr2 + GPIO_CLEARDATAOUT;
	
    if(gpio_addr1 == MAP_FAILED) {
        printf("Unable to map GPIO1\n");
        exit(1);
    }
    if(gpio_addr2 == MAP_FAILED) {
     printf("Unable to map GPIO2\n");
        exit(1);
    }

    // Set led pins to be an output pin
    directionreg = *gpio_oe_addr2;
    directionreg = ( ~GPIO2_10) & (~GPIO2_11)&directionreg;
    *gpio_oe_addr2 = directionreg;
    while(keepgoing) {
        // printf("ON\n");
        if((*gpio_datain1 & GPIO0_11))
                *gpio_setdataout_addr2 = GPIO2_13;
        // printf("OFF\n");
        else
                *gpio_cleardataout_addr2 = GPIO2_13;
        if(*gpio_datain2 & GPIO2_16)
                *gpio_setdataout_addr2 = GPIO2_11;
        // printf("OFF\n");
        else
                *gpio_cleardataout_addr2 = GPIO2_11;

    }

    munmap((void *)gpio_addr1, GPIO2_SIZE);
    munmap((void *)gpio_addr2, GPIO0_SIZE);

    close(fd);
    return 0;
}
