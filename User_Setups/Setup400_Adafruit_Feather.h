/* ST7789 135 x 240 display with no chip select line
This will work with Adafruit Feather
https://cdn-learn.adafruit.com/downloads/pdf/adafruit-esp32-s3-tft-feather.pdf
Or TS-ESP32-S3 Development Board With 1.14 Inch TFT Display 
*/
#define USER_SETUP_ID 400

#define ST7789_DRIVER     // Configure all registers

#define TFT_WIDTH  135
#define TFT_HEIGHT 240

#define CGRAM_OFFSET      // Library will add offsets required

// Generic ESP32 setup
#define TFT_MISO 37 //19
#define TFT_MOSI 35 //23
#define TFT_SCLK 36//18
#define TFT_CS    7 
#define TFT_DC    39
#define TFT_RST   40  

#define TFT_BL          45  // Display backlight control pin

#define TFT_BACKLIGHT_ON HIGH  // HIGH or LOW are options

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT


// #define SPI_FREQUENCY  27000000
#define SPI_FREQUENCY  40000000

#define SPI_READ_FREQUENCY  6000000 // 6 MHz is the maximum SPI read speed for the ST7789V
// #define SUPPORT_TRANSACTIONS