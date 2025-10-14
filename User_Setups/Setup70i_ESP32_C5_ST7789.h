// Setup for the ESP32 C5 with ST7789 display

#define USER_SETUP_ID 70
// See SetupX_Template.h for all options available

#define ST7789_DRIVER

#define TFT_RGB_ORDER 0
#define TFT_WIDTH 170
#define TFT_HEIGHT 320

#define TFT_BACKLIGHT_ON 1
#define TFT_BL 25
#define TFT_RST -1 // connected to ESP32C5 RST pin
#define TFT_DC 24
#define TFT_MISO 2 // set to share SPI with other devices
#define TFT_MOSI 7
#define TFT_SCLK 6
#define TFT_CS 23
#define TOUCH_CS -1 // Optional for touch screen


#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

#define SPI_FREQUENCY 20000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000
