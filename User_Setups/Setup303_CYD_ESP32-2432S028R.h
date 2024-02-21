// See SetupX_Template.h for all options available
#define USER_SETUP_ID 303

#define ILI9341_2_DRIVER

#define TFT_BL           21
#define TFT_BACKLIGHT_ON HIGH
#define TFT_MOSI         13
#define TFT_SCLK         14
#define TFT_CS           15
#define TFT_DC            2
#define TFT_RST          -1

#define SOFTSPI		  // Uncomment for software SPI
#define TOUCH_CS   33 // Chip select pin (T_CS) of touch screen
#define TOUCH_MOSI 32 // Separate Touch SoftSPI bus
#define TOUCH_MISO 39 // Separate Touch SoftSPI bus
#define TOUCH_CLK  25 // Separate Touch SoftSPI bus

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT

#define SPI_FREQUENCY       40000000

#define SPI_READ_FREQUENCY   1600000

#define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS