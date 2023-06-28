// Setup for the Microduino family with ST7789/ST7735 display
#define USER_SETUP_ID 260

#define ST7789_DRIVER     // Configure all registers
// #define ST7735_DRIVER


#if defined(ST7735_DRIVER)
#define TFT_WIDTH  80
#define TFT_HEIGHT 160
#define ST7735_GREENTAB160x80 // For 160 x 80 display (BGR, inverted, 26 offset)
#else
#define TFT_WIDTH  240
#define TFT_HEIGHT 240
// #define TFT_WIDTH  135
// #define TFT_HEIGHT 240
#endif

#define CGRAM_OFFSET      // Library will add offsets required

//#define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

#define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF


#if defined(LE501X)   //le501x

#define TFT_MISO -1
#define TFT_MOSI MOSI
#define TFT_SCLK SCK


#define TFT_CS   D5   
#define TFT_RST  -1  // TFT reset pin (could connect to MCU RST, see next line)
#define TFT_DC   D4  // Data Command control pin
#define TFT_BL	 D6

#define TFT_BACKLIGHT_ON	HIGH

#define SUPPORT_TRANSACTIONS
#define TFT_SPI_MODE SPI_MODE0
#define USE_SSI_PORT

#if defined(USE_SSI_PORT)
#define SPI_FREQUENCY   SSI_MAX_FREQ   //SPI_MAX_FREQ
#else
#define SPI_FREQUENCY   SPI_MAX_FREQ   //SPI_MAX_FREQ
#endif


#elif defined(ESP32)  //esp32
#define TFT_MISO -1
#define TFT_MOSI MOSI
#define TFT_SCLK SCK

// For ESP32 - use pin numbers
#define TFT_CS   33  
#define TFT_RST  -1  // TFT reset pin (could connect to MCU RST, see next line)
#define TFT_DC   32  // Data Command control pin
#define TFT_BL	 25

#define TFT_BACKLIGHT_ON	HIGH

// #define SUPPORT_TRANSACTIONS
//#define USE_HSPI_PORT
#define SPI_FREQUENCY 27000000

#else   //other
#define TFT_MISO -1
#define TFT_MOSI MOSI
#define TFT_SCLK SCK

#define TFT_CS   D5 
#define TFT_RST  -1  // TFT reset pin (could connect to MCU RST, see next line)
#define TFT_DC   D4  // Data Command control pin
#define TFT_BL	 D6

#define TFT_BACKLIGHT_ON	HIGH

#define SUPPORT_TRANSACTIONS
#define SPI_FREQUENCY 8000000

#endif


#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT
