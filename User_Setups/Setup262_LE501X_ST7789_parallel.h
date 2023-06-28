// Setup for the LE501X with 8 bit parallel ST7789
// See SetupX_Template.h for all options available
#define USER_SETUP_ID 262

#define ST7789_DRIVER     // Configure all registers

#define TFT_WIDTH  240
#define TFT_HEIGHT 240


#define CGRAM_OFFSET      // Library will add offsets required

//#define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

#define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF


#define TFT_PARALLEL_8_BIT
// #define SPI_18BIT_DRIVER
// #define RPI_DISPLAY_TYPE

/*
    The time required to run 'tft.fillScreen (TFT_RED)' with a screen resolution of 240x240 is as follows:
    LE501X_PORTA_DATA_BUS   = 25ms
    LE501X_CUSTOM_DATA_BUS  = 34ms
    LE501X_ANYIO_DATA_BUS   = 134ms
    undefined               = 279ms    
*/
// #define LE501X_PORTA_DATA_BUS
#define LE501X_CUSTOM_DATA_BUS
// #define LE501X_ANYIO_DATA_BUS


#if defined(LE501X_PORTA_DATA_BUS)  // The fastest operating mode; Pin fixation using PA08~PA15
// #define TFT_CS   -1    // Do not define, read pin permanently connected to GND
#define TFT_DC   PB09    // Data Command control pin
#define TFT_RST  PB11    // Reset pin

#define TFT_WR   PB10
// #define TFT_RD   -1   // Do not define, read pin permanently connected to VDD

// The IO port in this mode is fixed and there is no need to define a data IO port
#define TFT_D0   PA08
#define TFT_D1   PA09
#define TFT_D2   PA10
#define TFT_D3   PA11
#define TFT_D4   PA12
#define TFT_D5   PA13
#define TFT_D6   PA14
#define TFT_D7   PA15

#else   // Custom IO port
// #define TFT_CS   -1      // Do not define, read pin permanently connected to GND
#define TFT_DC   PB11    // Data Command control pin
#define TFT_RST  PA00    // Reset pin

#define TFT_WR   PB15
// #define TFT_RD   -1      // Do not define, read pin permanently connected to VDD

#define TFT_D0   PA12 
#define TFT_D1   PA13 
#define TFT_D2   PA14
#define TFT_D3   PB10
#define TFT_D4   PB09
#define TFT_D5   PB14
#define TFT_D6   PB12
#define TFT_D7   PB13
#endif


#define WR_TWRL_0
// Extra write pulse high time (data hold time, delays next write cycle start)
#define WR_TWRH_0




#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

