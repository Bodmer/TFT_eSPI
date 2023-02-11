// Pico Explorer Base by Pimoroni (RP2040) (ST7789 on SPI bus with 240x240 TFT)
#define USER_SETUP_ID 138

#define ST7789_DRIVER     // Configure all registers

#define TFT_WIDTH  240
#define TFT_HEIGHT 240

#define CGRAM_OFFSET      // Library will add offsets required

// For Pico Explorer Base (PR2040)
#define TFT_CS 17   // Chip Select pin
#define TFT_DC 16   // Data Command control pin 
#define TFT_RST -1  // No Reset pin
#define TFT_MOSI 19
#define TFT_SCLK 18


#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
// #define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

#define SPI_FREQUENCY       40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
