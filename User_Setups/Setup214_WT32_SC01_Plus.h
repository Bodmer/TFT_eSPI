// This setup is for the WT32-SC01 Plus Module
// See SetupX_Template.h for all options available
#define USER_SETUP_ID 214

////////////////////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////////////////////
#define ESP32_PARALLEL
#define WT32_SC01_PLUS
////////////////////////////////////////////////////////////////////////////////////////////
// Display driver type
////////////////////////////////////////////////////////////////////////////////////////////
#define ST7796_DRIVER

////////////////////////////////////////////////////////////////////////////////////////////
// WT32-SC01 Plus
////////////////////////////////////////////////////////////////////////////////////////////

#define TFT_INVERSION_ON
#define TFT_ROTATION_ON

//#define TFT_CS   -1  // Do not define, chip select control pin permanently connected to 0V

// These pins can be moved and are controlled directly by the library software
#define TFT_DC   0    // Data Command control pin
#define TFT_RST   4    // Reset pin
#define TFT_BL  45   //Backlight

#define TFT_RD   -1  // Do not define, read pin permanently connected to 3V3

// Note: All the following pins are PIO hardware configured and driven
#define TFT_WR   47

  // PIO requires these to be sequentially increasing - do not change
#define TFT_D0    9
#define TFT_D1    14
#define TFT_D2    3
#define TFT_D3    8
#define TFT_D4   18
#define TFT_D5   17
#define TFT_D6   16
#define TFT_D7   15
//*/

////////////////////////////////////////////////////////////////////////////////////////////
// Fonts to be available
////////////////////////////////////////////////////////////////////////////////////////////
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

////////////////////////////////////////////////////////////////////////////////////////////
