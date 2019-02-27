

#define ST7789_DSTIKE_DRIVER


#define TFT_WIDTH  240
#define TFT_HEIGHT 240

#define SPI_FREQUENCY  40000000

#define TFT_DC    23
#define TFT_RST   32
#define TFT_MOSI  26
#define TFT_SCLK  27

#define TFT_BGR 0   // Colour order Blue-Green-Red
#define TFT_RGB 1   // Colour order Red-Green-Blue


// Load the right driver definition - do not tinker here !
#include "TFT_Drivers/ST7789_DSTIKE_Defines.h"
#define TFT_DRIVER 0x7789


#define SMOOTH_FONT


// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000
// #define SPI_FREQUENCY  80000000

#define SPI_READ_FREQUENCY  20000000

#define SPI_TOUCH_FREQUENCY  2500000
