//////////////////////////////////////////////////////////////////////////
//									//
//      SETTINGS for the ESP32-2432S028 2,8inch TFT Dev-Board		//
//									//
//////////////////////////////////////////////////////////////////////////
//
// #define TOUCH_CS 33   // commented out - using TFT_eTouch library 
// TFT_eTouch source from Achill Hasler https://github.com/achillhasler/TFT_eTouch 
// Fork with config ready to use with TFT_eSPI and ESP32-2432S028 + example Sketch Touch_calibrate https://github.com/w4b-zero/TFT_eTouch/
//
//////////////////////////////////////////////////////////////////////////

#define USER_SETUP_ID 666

#define ILI9341_2_DRIVER   // Alternative ILI9341 driver, see https://github.com/Bodmer/TFT_eSPI/issues/1172

#define TFT_WIDTH  240 // ILI9341 240 x 320
#define TFT_HEIGHT 320 // ILI9341 240 x 320

#define TFT_BL   21            // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)
#define TFT_MISO 12  //SDO
#define TFT_MOSI 13  //SDI
#define TFT_SCLK 14  //SCK
#define TFT_CS   15  //CS Chip select control pin
#define TFT_DC    2  //RS Data Command control pin
#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

#define TFT_ETOUCH_MOSI 32 // TP_DIN
#define TFT_ETOUCH_MISO 39 // TP_OUT
#define TFT_ETOUCH_SCK 25  // TP_CLK
#define TFT_ETOUCH_CS 33   // TP_CS
#define TFT_ETOUCH_PIRQ 36  // TP_IRQ

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SPI_FREQUENCY  27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
