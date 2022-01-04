// Setup for the TTGO T-Display S2
// Note SPI DMA with ESP32 S2 is not currently supported

// See SetupX_Template.h for all options available

#define TFT_WIDTH  135
#define TFT_HEIGHT 240
#define ST7789_DRIVER     // Configure all registers

#define USE_HSPI_PORT

#define TFT_MISO   -1
#define TFT_MOSI   35
#define TFT_SCLK   36
#define TFT_CS     34
#define TFT_DC     37
#define TFT_RST    38
#define TFT_BL     33

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT

#define SPI_FREQUENCY        27000000
#define SPI_TOUCH_FREQUENCY  2500000
