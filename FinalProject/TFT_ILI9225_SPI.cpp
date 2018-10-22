#include "TFT_ILI9225_SPI.h"
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
//in class set default gfxFont = NULL and brightness to max
static void pabort(const char *s)
{
        perror(s);
        abort();
}
void _spiWrite(uint8_t v){
	        uint8_t rx;
			struct spi_ioc_transfer tr={
               .tx_buf = (unsigned long)v,
               .rx_buf = (unsigned long)rx,
               .len = sizeof(tx_temp),
               .delay_usecs = delay,
               .speed_hz = speed,
               .bits_per_word = bits};

        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		if (ret < 1)
                pabort("Can't send spi message");
}
void TFT_ILI9225_SPI::begin()
{
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
        ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't set bits per word");

        ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
        if (ret == -1)
                pabort("can't get bits per word");

        /*
         * max speed hz
         */
        ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't set max speed hz");

        ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
        if (ret == -1)
                pabort("can't get max speed hz");
        setBacklight(false);
		//setup spi controller here

	gpio_export(rstgpio);
	gpio_set_dir(rstgpio, const char* dir);
	gpio_export(DataCgpio);
	gpio_set_dir(DataCgpio, const char* dir);
    // Initialization Code
    
        gpio_set_value(rstgpio, 1); // Pull the reset pin high to release the ILI9225C from the reset status
        usleep(1000); 
        gpio_set_value(rstgpio, 0); // Pull the reset pin low to reset ILI9225
        usleep(10000);
        gpio_set_value(rstgpio, 1); // Pull the reset pin high to release the ILI9225C from the reset status
        usleep(50000);
    

    /* Start Initial Sequence */

    /* Set SS bit and direction output from S528 to S1 */
    _writeRegister(ILI9225_POWER_CTRL1, 0x0000); // Set SAP,DSTB,STB
    _writeRegister(ILI9225_POWER_CTRL2, 0x0000); // Set APON,PON,AON,VCI1EN,VC
    _writeRegister(ILI9225_POWER_CTRL3, 0x0000); // Set BT,DC1,DC2,DC3
    _writeRegister(ILI9225_POWER_CTRL4, 0x0000); // Set GVDD
    _writeRegister(ILI9225_POWER_CTRL5, 0x0000); // Set VCOMH/VCOML voltage
    usleep(40000); 

    // Power-on sequence
    _writeRegister(ILI9225_POWER_CTRL2, 0x0018); // Set APON,PON,AON,VCI1EN,VC
    _writeRegister(ILI9225_POWER_CTRL3, 0x6121); // Set BT,DC1,DC2,DC3
    _writeRegister(ILI9225_POWER_CTRL4, 0x006F); // Set GVDD   /*007F 0088 */
    _writeRegister(ILI9225_POWER_CTRL5, 0x495F); // Set VCOMH/VCOML voltage
    _writeRegister(ILI9225_POWER_CTRL1, 0x0800); // Set SAP,DSTB,STB
    usleep(10000);
    _writeRegister(ILI9225_POWER_CTRL2, 0x103B); // Set APON,PON,AON,VCI1EN,VC
    usleep(50000);

    _writeRegister(ILI9225_DRIVER_OUTPUT_CTRL, 0x011C); // set the display line number and display direction
    _writeRegister(ILI9225_LCD_AC_DRIVING_CTRL, 0x0100); // set 1 line inversion
    _writeRegister(ILI9225_ENTRY_MODE, 0x1038); // set GRAM write direction and BGR=1.
    _writeRegister(ILI9225_DISP_CTRL1, 0x0000); // Display off
    _writeRegister(ILI9225_BLANK_PERIOD_CTRL1, 0x0808); // set the back porch and front porch
    _writeRegister(ILI9225_FRAME_CYCLE_CTRL, 0x1100); // set the clocks number per line
    _writeRegister(ILI9225_INTERFACE_CTRL, 0x0000); // CPU interface
    _writeRegister(ILI9225_OSC_CTRL, 0x0D01); // Set Osc  /*0e01*/
    _writeRegister(ILI9225_VCI_RECYCLING, 0x0020); // Set VCI recycling
    _writeRegister(ILI9225_RAM_ADDR_SET1, 0x0000); // RAM Address
    _writeRegister(ILI9225_RAM_ADDR_SET2, 0x0000); // RAM Address

    /* Set GRAM area */
    _writeRegister(ILI9225_GATE_SCAN_CTRL, 0x0000); 
    _writeRegister(ILI9225_VERTICAL_SCROLL_CTRL1, 0x00DB); 
    _writeRegister(ILI9225_VERTICAL_SCROLL_CTRL2, 0x0000); 
    _writeRegister(ILI9225_VERTICAL_SCROLL_CTRL3, 0x0000); 
    _writeRegister(ILI9225_PARTIAL_DRIVING_POS1, 0x00DB); 
    _writeRegister(ILI9225_PARTIAL_DRIVING_POS2, 0x0000); 
    _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF); 
    _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000); 
    _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR1, 0x00DB); 
    _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR2, 0x0000); 

    /* Set GAMMA curve */
    _writeRegister(ILI9225_GAMMA_CTRL1, 0x0000); 
    _writeRegister(ILI9225_GAMMA_CTRL2, 0x0808); 
    _writeRegister(ILI9225_GAMMA_CTRL3, 0x080A); 
    _writeRegister(ILI9225_GAMMA_CTRL4, 0x000A); 
    _writeRegister(ILI9225_GAMMA_CTRL5, 0x0A08); 
    _writeRegister(ILI9225_GAMMA_CTRL6, 0x0808); 
    _writeRegister(ILI9225_GAMMA_CTRL7, 0x0000); 
    _writeRegister(ILI9225_GAMMA_CTRL8, 0x0A00); 
    _writeRegister(ILI9225_GAMMA_CTRL9, 0x0710); 
    _writeRegister(ILI9225_GAMMA_CTRL10, 0x0710); 

    _writeRegister(ILI9225_DISP_CTRL1, 0x0012); 
    usleep(50000); 
    _writeRegister(ILI9225_DISP_CTRL1, 0x1017);

    // Turn on backlight
    setBacklight(true);
    setOrientation(0);

    clear();
}



void TFT_ILI9225_SPI::_spiWriteCommand(uint8_t c) {
    gpio_set_value(DataCgpio,0);
    _spiWrite(c);
}


void TFT_ILI9225_SPI::_spiWriteData(uint8_t c) {
    gpio_set_value(DataCgpio,1);
    _spiWrite(c);
}

void TFT_ILI9225_SPI::_orientCoordinates(uint16_t &x1, uint16_t &y1) {

    switch (_orientation) {
    case 0:  // ok
        break;
    case 1: // ok
        y1 = _maxY - y1 - 1;
        _swap(x1, y1);
        break;
    case 2: // ok
        x1 = _maxX - x1 - 1;
        y1 = _maxY - y1 - 1;
        break;
    case 3: // ok
        x1 = _maxX - x1 - 1;
        _swap(x1, y1);
        break;
    }
}

void TFT_ILI9225_SPI::_setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    _setWindow( x0, y0, x1, y1, TopDown_L2R ); // default for drawing characters
}

void TFT_ILI9225_SPI::_setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, autoIncMode_t mode) {
    
    // clip to TFT-Dimensions
    x0 = min( x0, (uint16_t) (_maxX-1) );
    x1 = min( x1, (uint16_t) (_maxX-1) );
    y0 = min( y0, (uint16_t) (_maxY-1) );
    y1 = min( y1, (uint16_t) (_maxY-1) );
    _orientCoordinates(x0, y0);
    _orientCoordinates(x1, y1);

    if (x1<x0) _swap(x0, x1);
    if (y1<y0) _swap(y0, y1);

    // autoincrement mode
    if ( _orientation > 0 ) mode = modeTab[_orientation-1][mode];
    _writeRegister(ILI9225_ENTRY_MODE, 0x1000 | ( mode<<3) );
    _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR1,x1);
    _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR2,x0);

    _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR1,y1);
    _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR2,y0);
    
    // starting position within window and increment/decrement direction
    switch ( mode>>1 ) {
      case 0:
        _writeRegister(ILI9225_RAM_ADDR_SET1,x1);
        _writeRegister(ILI9225_RAM_ADDR_SET2,y1);
        break;
      case 1:
        _writeRegister(ILI9225_RAM_ADDR_SET1,x0);
        _writeRegister(ILI9225_RAM_ADDR_SET2,y1);
        break;
      case 2:
        _writeRegister(ILI9225_RAM_ADDR_SET1,x1);
        _writeRegister(ILI9225_RAM_ADDR_SET2,y0);
        break;
      case 3:
        _writeRegister(ILI9225_RAM_ADDR_SET1,x0);
        _writeRegister(ILI9225_RAM_ADDR_SET2,y0);
        break;
    }
    _writeCommand16( ILI9225_GRAM_DATA_REG );

    //_writeRegister(ILI9225_RAM_ADDR_SET1,x0);
    //_writeRegister(ILI9225_RAM_ADDR_SET2,y0);

    //_writeCommand(0x00, 0x22);
}


void TFT_ILI9225_SPI::_resetWindow() {
    _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF); 
    _writeRegister(ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000); 
    _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR1, 0x00DB); 
    _writeRegister(ILI9225_VERTICAL_WINDOW_ADDR2, 0x0000); 

}

void TFT_ILI9225_SPI::clear() {
    uint8_t old = _orientation;
    setOrientation(0);
    fillRectangle(0, 0, _maxX - 1, _maxY - 1, COLOR_BLACK);
    setOrientation(old);
    usleep(10000);
}


void TFT_ILI9225_SPI::invert(bool flag) {
    _writeCommand16(flag ? ILI9225C_INVON : ILI9225C_INVOFF);
    //_writeCommand(0x00, flag ? ILI9225C_INVON : ILI9225C_INVOFF);
}


void TFT_ILI9225_SPI::setBacklight(bool flag) {
    blState = flag;
}


void TFT_ILI9225_SPI::setBacklightBrightness(uint8_t brightness) {
    _brightness = brightness;
    setBacklight(blState);
}


void TFT_ILI9225_SPI::setDisplay(bool flag) {
    if (flag) {
        _writeRegister(0x00ff, 0x0000);
        _writeRegister(ILI9225_POWER_CTRL1, 0x0000);
        usleep(50000);
        _writeRegister(ILI9225_DISP_CTRL1, 0x1017);
        usleep(200000);
    } else {
        _writeRegister(0x00ff, 0x0000);
        _writeRegister(ILI9225_DISP_CTRL1, 0x0000);
        usleep(50000);
        _writeRegister(ILI9225_POWER_CTRL1, 0x0003);
        usleep(200000);
    }
}


void TFT_ILI9225_SPI::setOrientation(uint8_t orientation) {

    _orientation = orientation % 4;

    switch (_orientation) {
    case 0:
        _maxX = ILI9225_LCD_WIDTH;
        _maxY = ILI9225_LCD_HEIGHT;

        break;
    case 1:
        _maxX = ILI9225_LCD_HEIGHT;
        _maxY = ILI9225_LCD_WIDTH;
        break;
    case 2:
        _maxX = ILI9225_LCD_WIDTH;
        _maxY = ILI9225_LCD_HEIGHT;
        break;
    case 3:
        _maxX = ILI9225_LCD_HEIGHT;
        _maxY = ILI9225_LCD_WIDTH;
        break;
    }
}


uint8_t TFT_ILI9225_SPI::getOrientation() {
    return _orientation;
}


void TFT_ILI9225_SPI::drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    
    drawLine(x1, y1, x1, y2, color);
    drawLine(x1, y1, x2, y1, color);
    drawLine(x1, y2, x2, y2, color);
    drawLine(x2, y1, x2, y2, color);
    
}


void TFT_ILI9225_SPI::fillRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {

    _setWindow(x1, y1, x2, y2);
    for(uint16_t t=(y2 - y1 + 1) * (x2 - x1 + 1); t > 0; t--)
        _writeData16(color);
    _resetWindow();
}


void TFT_ILI9225_SPI::drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0-  r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
    
}


void TFT_ILI9225_SPI::fillCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color) {

    int16_t f = 1 - radius;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawLine(x0 + x, y0 + y, x0 - x, y0 + y, color); // bottom
        drawLine(x0 + x, y0 - y, x0 - x, y0 - y, color); // top
        drawLine(x0 + y, y0 - x, x0 + y, y0 + x, color); // right
        drawLine(x0 - y, y0 - x, x0 - y, y0 + x, color); // left
    }
    fillRectangle(x0-x, y0-y, x0+x, y0+y, color);
}


void TFT_ILI9225_SPI::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {

    // Classic Bresenham algorithm
    int16_t steep = abs((int16_t)(y2 - y1)) > abs((int16_t)(x2 - x1));

    int16_t dx, dy;

    if (steep) {
        _swap(x1, y1);
        _swap(x2, y2);
    }

    if (x1 > x2) {
        _swap(x1, x2);
        _swap(y1, y2);
    }

    dx = x2 - x1;
    dy = abs((int16_t)(y2 - y1));

    int16_t err = dx / 2;
    int16_t ystep;

    if (y1 < y2) ystep = 1;
    else ystep = -1;

    for (; x1<=x2; x1++) {
        if (steep) drawPixel(y1, x1, color);
        else       drawPixel(x1, y1, color);

        err -= dy;
        if (err < 0) {
            y1 += ystep;
            err += dx;
        }
    }
}


void TFT_ILI9225_SPI::drawPixel(uint16_t x1, uint16_t y1, uint16_t color) {

    if((x1 >= _maxX) || (y1 >= _maxY)) return;
/*
    _setWindow(x1, y1, x1+1, y1+1);
    _orientCoordinates(x1, y1);
    if (checkSPI) startWrite();
    //_writeData(color >> 8, color);
    _writeData16(color);
    if (checkSPI) endWrite();
*/
    _orientCoordinates(x1, y1);
    _writeRegister(ILI9225_RAM_ADDR_SET1,x1);
    _writeRegister(ILI9225_RAM_ADDR_SET2,y1);
    _writeRegister(ILI9225_GRAM_DATA_REG,color);



}


uint16_t TFT_ILI9225_SPI::maxX() {
    return _maxX;
}


uint16_t TFT_ILI9225_SPI::maxY() {
    return _maxY;
}


uint16_t TFT_ILI9225_SPI::setColor(uint8_t red8, uint8_t green8, uint8_t blue8) {
    // rgb16 = red5 green6 blue5
    return (red8 >> 3) << 11 | (green8 >> 2) << 5 | (blue8 >> 3);
}


void TFT_ILI9225_SPI::splitColor(uint16_t rgb, uint8_t &red, uint8_t &green, uint8_t &blue) {
    // rgb16 = red5 green6 blue5
    red   = (rgb & 0b1111100000000000) >> 11 << 3;
    green = (rgb & 0b0000011111100000) >>  5 << 2;
    blue  = (rgb & 0b0000000000011111)       << 3;
}


void TFT_ILI9225_SPI::_swap(uint16_t &a, uint16_t &b) {
    uint16_t w = a;
    a = b;
    b = w;
}

// Utilities
/*void TFT_ILI9225_SPI::_writeCommand16(uint16_t command) {
# ifdef HSPI_WRITE16
    SPI_DC_LOW();
    SPI_CS_LOW();
    HSPI_WRITE16(command);
    SPI_CS_HIGH();
#else 
    _spiWriteCommand(command >> 8);
    _spiWriteCommand(0x00ff & command);
#endif
}

void TFT_ILI9225_SPI::_writeData16(uint16_t data) {
# ifdef HSPI_WRITE16
    SPI_DC_HIGH();
    SPI_CS_LOW();
    HSPI_WRITE16(data);
    SPI_CS_HIGH();
#else 
    _spiWriteData(data >> 8);
    _spiWriteData(0x00ff & data);
#endif
}
*/
void TFT_ILI9225_SPI::_writeCommand16(uint16_t command) {
    gpio_set_value(DataCgpio,0);
	uint16_t rx;
	        struct spi_ioc_transfer tr={
               .tx_buf = (unsigned long)command,
               .rx_buf = (unsigned long)rx,
               .len = sizeof(tx_temp),
               .delay_usecs = delay,
               .speed_hz = speed,
               .bits_per_word = bits};

        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		if (ret < 1)
                pabort("Can't send spi message");
//16bit spi transfer
}

void TFT_ILI9225_SPI::_writeData16(uint16_t data) {
    gpio_set_value(DataCgpio,1);
	uint16_t rx;
	        struct spi_ioc_transfer tr={
               .tx_buf = (unsigned long)data,
               .rx_buf = (unsigned long)rx,
               .len = sizeof(tx_temp),
               .delay_usecs = delay,
               .speed_hz = speed,
               .bits_per_word = bits};

        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		if (ret < 1)
                pabort("Can't send spi message");
}

	//8bit spi transfer
/*void TFT_ILI9225_SPI::_writeData(uint8_t HI, uint8_t LO) {
    _spiWriteData(HI);
    _spiWriteData(LO);
}

void TFT_ILI9225_SPI::_writeCommand(uint8_t HI, uint8_t LO) {
    _spiWriteCommand(HI);
    _spiWriteCommand(LO);
}*/


void TFT_ILI9225_SPI::_writeRegister(uint16_t reg, uint16_t data) {
    _writeCommand16(reg);
    _writeData16(data);
}


void TFT_ILI9225_SPI::drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color) {
    
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x3, y3, color);
    drawLine(x3, y3, x1, y1, color);
    
}


void TFT_ILI9225_SPI::fillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color) {

    uint16_t a, b, y, last;

    // Sort coordinates by Y order (y3 >= y2 >= y1)
    if (y1 > y2) {
        _swap(y1, y2); _swap(x1, x2);
    }
    if (y2 > y3) {
        _swap(y3, y2); _swap(x3, x2);
    }
    if (y1 > y2) {
        _swap(y1, y2); _swap(x1, x2);
    }

    checkSPI = false;
    if (y1 == y3) { // Handle awkward all-on-same-line case as its own thing
        a = b = x1;
        if (x2 < a)      a = x2;
        else if (x2 > b) b = x2;
        if (x3 < a)      a = x3;
        else if (x3 > b) b = x3;
            drawLine(a, y1, b, y1, color);
        return;
    }

    int16_t dx11 = x2 - x1,
            dy11 = y2 - y1,
            dx12 = x3 - x1,
            dy12 = y3 - y1,
            dx22 = x3 - x2,
            dy22 = y3 - y2;
    int32_t sa   = 0,
            sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y2=y3 (flat-bottomed triangle), the scanline y2
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y2 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y1=y2
    // (flat-topped triangle).
    if (y2 == y3) last = y2;   // Include y2 scanline
    else          last = y2 - 1; // Skip it

    for (y = y1; y <= last; y++) {
        a   = x1 + sa / dy11;
        b   = x1 + sb / dy12;
        sa += dx11;
        sb += dx12;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
        */
        if (a > b) _swap(a,b);
        drawLine(a, y, b, y, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y2=y3.
    sa = dx22 * (y - y2);
    sb = dx12 * (y - y1);
    for (; y<=y3; y++) {
        a   = x2 + sa / dy22;
        b   = x1 + sb / dy12;
        sa += dx22;
        sb += dx12;
        /* longhand:
        a = x2 + (x3 - x2) * (y - y2) / (y3 - y2);
        b = x1 + (x3 - x1) * (y - y1) / (y3 - y1);
        */
        if (a > b) _swap(a,b);
            drawLine(a, y, b, y, color);
    }
    
}

void TFT_ILI9225_SPI::setFont(uint8_t* font, bool monoSp) {

    cfont.font     = font;
    cfont.width    = readFontByte(0);
    cfont.height   = readFontByte(1);
    cfont.offset   = readFontByte(2);
    cfont.numchars = readFontByte(3);
    cfont.nbrows   = cfont.height / 8;
    cfont.monoSp   = monoSp;

    if (cfont.height % 8) cfont.nbrows++;  // Set number of bytes used by height of font in multiples of 8
}

_currentFont TFT_ILI9225_SPI::getFont() {
    return cfont;
}

uint16_t TFT_ILI9225_SPI::drawText(uint16_t x, uint16_t y, string s, uint16_t color) {
    uint16_t bg_color = color;
    drawText(x, y, s, color, bg_color);
}


uint16_t TFT_ILI9225_SPI::drawText(uint16_t x, uint16_t y, string s, uint16_t color, uint16_t bg_color) {

    uint16_t currx = x;
    uint16_t curry = y;

    // Print every character in string
    for (uint8_t k = 0; k < s.length();k++) {
        currx += drawChar(currx, curry, s[k], color, bg_color) + 1;
    }
  
    return currx;
}

uint16_t TFT_ILI9225_SPI::getTextWidth( string s ) {

    uint16_t width = 0;
    // Count every character in string ( +1 for spacing )

    for (uint8_t k = 0; k < s.length(); k++) {
        width += getCharWidth(s[k]) + 1;
    }
    return width;
}

uint16_t TFT_ILI9225_SPI::drawChar(uint16_t x, uint16_t y, uint16_t ch, uint16_t color, uint16_t bg_color) {

    uint8_t charData, charWidth;
    uint8_t h, i, j;
    uint16_t charOffset;
    bool fastMode;
    charOffset = (cfont.width * cfont.nbrows) + 1;  // bytes used by each character
    charOffset = (charOffset * (ch - cfont.offset)) + FONT_HEADER_SIZE;  // char offset (add 4 for font header)
    if ( cfont.monoSp ) charWidth = cfont.width;      // monospaced: get char width from font
    else                charWidth  = readFontByte(charOffset);  // get chracter width from 1st byte
    charOffset++;  // increment pointer to first character data byte
    
    // use autoincrement/decrement feature, if character fits completely on screen
    //fastMode = ( (x+charWidth+1) < _maxX && (y+cfont.height-1) < _maxY ) ;
    //if ( fastMode )_setWindow( x,y,x+charWidth+1, y+cfont.height-1 );  // set character Window

    for (i = 0; i <= charWidth; i++) {  // each font "column" (+1 blank column for spacing)
        h = 0;  // keep track of char height
        for (j = 0; j < cfont.nbrows; j++)     {  // each column byte
            if (i == charWidth) charData = (uint8_t)0x0; // Insert blank column
            else                charData = readFontByte(charOffset);
            charOffset++;
            
            // Process every row in font character
            for (uint8_t k = 0; k < 8; k++) {
                if (h >= cfont.height ) break;  // No need to process excess bits
                if bitRead(charData, k) drawPixel( x + i, y + (j * 8) + k, color );
                else if (color != bg_color) drawPixel( x + i, y + (j * 8) + k, bg_color );
                h++;
            }
        }
    }
    _resetWindow();
    return charWidth;
}

uint16_t TFT_ILI9225_SPI::getCharWidth(uint16_t ch) {
    uint16_t charOffset;
    charOffset = (cfont.width * cfont.nbrows) + 1;  // bytes used by each character
    charOffset = (charOffset * (ch - cfont.offset)) + FONT_HEADER_SIZE;  // char offset (add 4 for font header)

    return readFontByte(charOffset);  // get font width from 1st byte
}

// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground color (unset bits are transparent).
void TFT_ILI9225_SPI::drawBitmap(int16_t x, int16_t y,
const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    _drawBitmap( x,  y, bitmap,  w,  h, color,  0, true, true, false );
}

// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer (must be PROGMEM memory) using the specified
// foreground (for set bits) and background (for clear bits) colors.
void TFT_ILI9225_SPI::drawBitmap(int16_t x, int16_t y,
const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
    _drawBitmap( x,  y, bitmap,  w,  h, color,  bg, false, true, false );
}

// drawBitmap() variant for RAM-resident (not PROGMEM) bitmaps.
void TFT_ILI9225_SPI::drawBitmap(int16_t x, int16_t y,
uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    _drawBitmap( x,  y, bitmap,  w,  h, color,  0, true, false, false );
}

// drawBitmap() variant w/background for RAM-resident (not PROGMEM) bitmaps.
void TFT_ILI9225_SPI::drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
    _drawBitmap( x,  y, bitmap,  w,  h, color,  bg, false, false, false );
}

//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void TFT_ILI9225_SPI::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    _drawBitmap( x,  y, bitmap,  w,  h, color,  0, true, true, true );
}

void TFT_ILI9225_SPI::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
    _drawBitmap( x,  y, bitmap,  w,  h, color,  bg, false, true, true );
}


// internal function for drawing bitmaps with/without transparent bg, or from ram or progmem
void TFT_ILI9225_SPI::_drawBitmap(int16_t x, int16_t y,
const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg, bool transparent, bool progmem,bool Xbit) {
    bool noAutoInc = false;     // Flag set when transparent pixel was 'written'
    int16_t i, j, byteWidth = (w + 7) / 8;
    int16_t wx0,wy0,wx1,wy1,wh,ww;  // Window-position and size
    uint8_t byte, maskBit;
    maskBit = Xbit? 0x01:0x80;
    // adjust window hight/width to displaydimensions
    wx0 = x<0?0:x;
    wy0 = y<0?0:y;
    wx1 = (x+w>_maxX?_maxX:x+w)-1;
    wy1 = (y+h>_maxY?_maxY:y+h)-1;
    wh  = wy1-wy0 +1;
    ww  = wx1-wx0 +1;
    _setWindow( wx0,wy0,wx1,wy1,L2R_TopDown);
    for (j = y>=0?0:-y; j < (y>=0?0:-y)+wh; j++) {
        for (i = 0; i < w; i++ ) {
            if (i & 7) { if ( Xbit ) byte >>=1; else byte <<= 1; }
            else {  if ( progmem ) byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
                    else      byte   = bitmap[j * byteWidth + i / 8];
            }
            if ( x+i >= wx0 && x+i <= wx1 ) {
                // write only if pixel is within window
                if (byte & maskBit) {
                    if (noAutoInc) {
                        //there was a transparent area, set pixelkoordinates again
                        drawPixel(x + i, y + j, color);
                        noAutoInc = false;
                    }
                    else  { 
                        _writeData16(color);
                    }
                }
                else  {
                    if (transparent) noAutoInc = true; // no autoincrement in transparent area!
                    else _writeData16( bg);
                }
            }
        }
    }
    
}



//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
/*void TFT_ILI9225_SPI::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

    int16_t i, j, byteWidth = (w + 7) / 8;
    uint8_t byte;

    startWrite();
    checkSPI = false;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++ ) {
            if (i & 7) byte >>= 1;
            else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
            if (byte & 0x01) drawPixel(x + i, y + j, color);
        }
    }
    checkSPI = true;
    endWrite();
}
*/

//High speed color bitmap
void TFT_ILI9225_SPI::drawBitmap(uint16_t x1, uint16_t y1, 
const uint16_t** bitmap, int16_t w, int16_t h) {
    _setWindow(x1, y1, x1+w, y1+h);
    gpio_set_value(DataCgpio,1);
    for (uint16_t x = 0; x < w; x++) {
        for (uint16_t y = 0; y < h; y++) {
            uint16_t col = bitmap[x][y];
            _spiWrite(col>>8);
            _spiWrite(col);
        }
    }
}

//High speed color bitmap
void TFT_ILI9225_SPI::drawBitmap(uint16_t x1, uint16_t y1, 
uint16_t** bitmap, int16_t w, int16_t h) {
    _setWindow(x1, y1, x1+w, y1+h);
    gpio_set_value(DataCgpio,1);
    for (uint16_t x = 0; x < w; x++) {
        for (uint16_t y = 0; y < h; y++) {
            uint16_t col = bitmap[x][y];
            _spiWrite(col>>8);
            _spiWrite(col);
        }
    }
}

void TFT_ILI9225_SPI::vertScroll(int16_t top, int16_t scrollines, int16_t offset)
{
    int16_t bfa = _maxY - top - scrollines;  // bottom fixed area
    int16_t vsp;
    int16_t sea = top;
    vsp = top + offset; // vertical start position
    if (offset < 0)
        vsp += scrollines;          //keep in unsigned range
    sea = top + scrollines - 1;
    _writeRegister(ILI9225_VERTICAL_SCROLL_CTRL1, sea);       //SEA
    _writeRegister(ILI9225_VERTICAL_SCROLL_CTRL2, top);       //SSA
    _writeRegister(ILI9225_VERTICAL_SCROLL_CTRL3, vsp - top);       //SST
}



// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

void TFT_ILI9225_SPI::setGFXFont(const GFXfont *f) {
    gfxFont = (GFXfont *)f;
}


// Draw a string
void TFT_ILI9225_SPI::drawGFXText(int16_t x, int16_t y, STRING s, uint16_t color) {

    int16_t currx = x;

    if(gfxFont) {
        // Print every character in string
        #ifdef USE_STRING_CLASS
        for (uint8_t k = 0; k < s.length(); k++) {
            currx += drawGFXChar(currx, y, s.charAt(k), color) + 1;
        }
        #else
        for (uint8_t k = 0; k < strlen(s); k++) {
            currx += drawGFXChar(currx, y, s[k], color) + 1;
        }
        #endif
    }
}
uint16_t TFT_ILI9225_SPI::Read16(void){
	//16 spi transfer of 0x0000
	uint16_t rx;
	struct spi_ioc_transfer tr={
               .tx_buf = (unsigned long)0x0000,
               .rx_buf = (unsigned long)rx,
               .len = sizeof(tx_temp),
               .delay_usecs = delay,
               .speed_hz = speed,
               .bits_per_word = bits};

        /*Send the data*/
        ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		if (ret < 1)
                pabort("Can't send spi message");
		return rx
}
uint16_t TFT_ILI9225_SPI::ReadAxis(bool Axis)
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


// Draw a character
uint16_t TFT_ILI9225_SPI::drawGFXChar(int16_t x, int16_t y, unsigned char c, uint16_t color) {

    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
    uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t  w  = pgm_read_byte(&glyph->width),
             h  = pgm_read_byte(&glyph->height),
             xa = pgm_read_byte(&glyph->xAdvance);
    int8_t   xo = pgm_read_byte(&glyph->xOffset),
             yo = pgm_read_byte(&glyph->yOffset);
    uint8_t  xx, yy, bits = 0, bit = 0;

    // Add character clipping here one day

    
    for(yy=0; yy<h; yy++) {
        for(xx=0; xx<w; xx++) {
            if(!(bit++ & 7)) {
                bits = pgm_read_byte(&bitmap[bo++]);
            }
            if(bits & 0x80) {
                drawPixel(x+xo+xx, y+yo+yy, color);
            }
            bits <<= 1;
        }
    }

    return (uint16_t)xa;
}


void TFT_ILI9225_SPI::getGFXCharExtent(uint8_t c, int16_t *gw, int16_t *gh, int16_t *xa) {
    uint8_t first = pgm_read_byte(&gfxFont->first),
            last  = pgm_read_byte(&gfxFont->last);
    // Char present in this font?
    if((c >= first) && (c <= last)) {
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c - first]);
        *gw = pgm_read_byte(&glyph->width);
        *gh = pgm_read_byte(&glyph->height);
        *xa = pgm_read_byte(&glyph->xAdvance);
        // int8_t  xo = pgm_read_byte(&glyph->xOffset),
        //         yo = pgm_read_byte(&glyph->yOffset);
    }
}


void TFT_ILI9225_SPI::getGFXTextExtent(string str, int16_t x, int16_t y, int16_t *w, int16_t *h) {
    *w  = *h = 0;

    for (uint8_t k = 0; k < str.length();k++) {
        uint8_t c = str[k];
        int16_t gw, gh, xa;
        getGFXCharExtent(c, &gw, &gh, &xa);
        if(gh > *h) {
            *h = gh;
        }
        *w += xa;
    }
}

