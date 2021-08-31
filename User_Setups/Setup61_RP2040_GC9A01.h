//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc
//
//   See the User_Setup_Select.h file if you wish to be able to define multiple
//   setups and then easily select which setup file is used by the compiler.
//
//   If this file is edited correctly then all the library example sketches should
//   run without the need to make any more changes for a particular hardware setup!
//   Note that some sketches are designed for a particular TFT pixel width/height

#define GC9A01_DRIVER // Round TFT SPI Display 

#define TFT_WIDTH  240 // ST7789 240 x 240 and 240 x 320
#define TFT_HEIGHT 240 // GC9A01 240 x 240

#define TFT_MISO 0 // RX PIN - Not Connected
#define TFT_SCLK 2
#define TFT_MOSI 3 // TX PIN
#define TFT_CS   20  // Chip select control pin
#define TFT_DC   18  // Data Command control pin
#define TFT_RST  19  // Reset pin (could connect to Arduino RESET pin)

#define TFT_BL   17            // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT
#define PIXEL_DOUBLE // Testando

#define SPI_FREQUENCY  27000000
//#define SPI_FREQUENCY  40000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  20000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  5000000
// #define SPI_TOUCH_FREQUENCY  2500000 - Copy from Setup200_GC9A01
