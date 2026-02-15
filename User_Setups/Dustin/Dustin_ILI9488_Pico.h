//#define RP2040_PIO_SPI   // Note: TFT_eSPI touch functions not available when PIO used.

#define ILI9488_DRIVER
//#define ILI9341_DRIVER

#define RP2040_PIO_SPI

//#define TFT_MISO  0
#define TFT_MOSI  3
#define TFT_SCLK  2
#define TFT_CS   20  // Chip select control pin
#define TFT_DC   22  // Data Command control pin
#define TFT_RST  21  // Reset pin (could connect to Arduino RESET pin)

//#define TOUCH_CS 14

//#define TFT_BL   14            // Actually touch CS, so just a way to bias high
//#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)


#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

#define SPI_FREQUENCY  20000000
