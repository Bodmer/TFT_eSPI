// See SetupX_Template.h for all options available

// NOTE: Backlight (TFT_BL) must be turned on to see anything on the screen
// Pin assignments and other documentation here:  https://www.lcdwiki.com/3.5inch_ESP32-32E_Display#ESP32_Pin_Assignment

#define USER_SETUP_ID 77

#define ST7796_DRIVER

#define TFT_MISO 12  // SPI bus read data signal(shared by LCD and touch screen)
#define TFT_MOSI 13  // SPI bus write data signal(shared by LCD and touch screen)
#define TFT_SCLK 14  // SPI bus clock signal(shared by LCD and touch screen)
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin

//#define TFT_RST   4  // Reset pin (could connect to RST pin)
#define TFT_RST  -1 // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

#define TOUCH_CS 33 // Chip select pin (T_CS) of touch screen

#define TFT_BL   27  // LCD backlight control signal(high level backlight on, low level backlight off)

#define TFT_WIDTH 320
#define TFT_HEIGHT 480

#define LOAD_GLCD    // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2   // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4   // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6   // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7   // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8   // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF   // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

// TFT SPI clock frequency
// #define SPI_FREQUENCY  20000000
// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000
// #define SPI_FREQUENCY  80000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  20000000

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
#define SPI_TOUCH_FREQUENCY  2500000
