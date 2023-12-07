// See SetupX_Template.h for all options available
#define USER_SETUP_ID 16

#define TFT_PARALLEL_8_BIT

#define ILI9488_DRIVER

// ESP32 S3 pins used for the parallel interface TFT
#define TFT_CS   13
#define TFT_DC   12  // Data Command control pin - must use a GPIO in the range 0-31
#define TFT_RST  9

#define TFT_WR   11  // Write strobe control pin - must use a GPIO in the range 0-31
#define TFT_RD   10

#define TFT_D0   3  // Must use GPIO in the range 0-31 for the data bus
#define TFT_D1   8  // so a single register write sets/clears all bits
#define TFT_D2   16
#define TFT_D3   15
#define TFT_D4   7
#define TFT_D5   6
#define TFT_D6   5
#define TFT_D7   4

#define TFT_BL   46
#define TFT_BACKLIGHT_ON 1

// DMA Config
#define TFT_DMA_FREQUENCY 25000000
#define TFT_DMA_MAX_TX_SIZE 65536
#define TFT_DMA_FAST_TRANSFER

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT
