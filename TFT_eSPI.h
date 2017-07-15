/***************************************************
  Arduino TFT graphics library targetted at ESP8266
  based boards. (ESP32 support is planned!)

  This library has been derived from the Adafruit_GFX
  library and the associated driver library. See text
  at the end of this file.

  This is a standalone library that contains the
  hardware driver, the graphics funtions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce
  their FLASH footprint.

 ****************************************************/

// Stop fonts etc being loaded multiple times
#ifndef _TFT_eSPIH_
#define _TFT_eSPIH_

//#define ESP32 //Just used to test ESP32 options

// Include header file that defines the fonts loaded, the TFT drivers
// available and the pins to be used
#include <User_Setup_Select.h>

// If the frequency is not defined, set a default
#ifndef SPI_FREQUENCY
  #define SPI_FREQUENCY  20000000
#endif

// Only load the fonts defined in User_Setup.h (to save space)
// Set flag so RLE rendering code is optionally compiled
#ifdef LOAD_GLCD
  #include <Fonts/glcdfont.c>
#endif

#ifdef LOAD_FONT2
  #include <Fonts/Font16.h>
#endif

#ifdef LOAD_FONT4
  #include <Fonts/Font32rle.h>
  #define LOAD_RLE
#endif

#ifdef LOAD_FONT6
  #include <Fonts/Font64rle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#ifdef LOAD_FONT7
  #include <Fonts/Font7srle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#ifdef LOAD_FONT8
  #include <Fonts/Font72rle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#include <Arduino.h>
#include <Print.h>

#include <pgmspace.h>

#include <SPI.h>

#if defined (ESP8266) && defined (D0_USED_FOR_DC)
  #define DC_C digitalWrite(TFT_DC, LOW)
  #define DC_D digitalWrite(TFT_DC, HIGH)
#elif defined (ESP32)
  //#define DC_C digitalWrite(TFT_DC, HIGH); GPIO.out_w1tc = (1 << TFT_DC)//digitalWrite(TFT_DC, LOW)
  //#define DC_D digitalWrite(TFT_DC, LOW); GPIO.out_w1ts = (1 << TFT_DC)//digitalWrite(TFT_DC, HIGH)
  #define DC_C GPIO.out_w1ts = (1 << TFT_DC); GPIO.out_w1ts = (1 << TFT_DC); GPIO.out_w1tc = (1 << TFT_DC)
  #define DC_D GPIO.out_w1tc = (1 << TFT_DC); GPIO.out_w1ts = (1 << TFT_DC)
#else
  #define DC_C GPOC=dcpinmask
  #define DC_D GPOS=dcpinmask
#endif

#if defined (TFT_SPI_OVERLAP)
  #undef TFT_CS
#endif

#ifndef TFT_CS
  #define CS_L // No macro allocated so it generates no code
  #define CS_H // No macro allocated so it generates no code
#else
  #if defined (ESP8266) && defined (D0_USED_FOR_CS)
    #define CS_L digitalWrite(TFT_CS, LOW)
    #define CS_H digitalWrite(TFT_CS, HIGH)
  #elif defined (ESP32)
    //#define CS_L digitalWrite(TFT_CS, HIGH); GPIO.out_w1tc = (1 << TFT_CS)//digitalWrite(TFT_CS, LOW)
    //#define CS_H digitalWrite(TFT_CS, LOW); GPIO.out_w1ts = (1 << TFT_CS)//digitalWrite(TFT_CS, HIGH)
    #define CS_L GPIO.out_w1ts = (1 << TFT_CS);GPIO.out_w1tc = (1 << TFT_CS)
    #define CS_H GPIO.out_w1ts = (1 << TFT_CS)
  #else
    #define CS_L GPOC=cspinmask
    #define CS_H GPOS=cspinmask
  #endif
#endif

#ifdef TFT_WR
  #if defined (ESP32)
    #define WR_L GPIO.out_w1tc = (1 << TFT_WR)
    //digitalWrite(TFT_WR, LOW)
    #define WR_H GPIO.out_w1ts = (1 << TFT_WR)
    //digitalWrite(TFT_WR, HIGH)
  #else
    #define WR_L GPOC=wrpinmask
    #define WR_H GPOS=wrpinmask
  #endif
#endif

#ifdef LOAD_GFXFF
  // We can include all the free fonts and they will only be built into
  // the sketch if they are used

  #include <Fonts/GFXFF/gfxfont.h>

  // Call up any user custom fonts
  #include <User_Setups/User_Custom_Fonts.h>

  // Original Adafruit_GFX "Free Fonts"
  #include <Fonts/GFXFF/TomThumb.h>  // TT1

  #include <Fonts/GFXFF/FreeMono9pt7b.h>  // FF1 or FM9
  #include <Fonts/GFXFF/FreeMono12pt7b.h> // FF2 or FM12
  #include <Fonts/GFXFF/FreeMono18pt7b.h> // FF3 or FM18
  #include <Fonts/GFXFF/FreeMono24pt7b.h> // FF4 or FM24

  #include <Fonts/GFXFF/FreeMonoOblique9pt7b.h>  // FF5 or FMO9
  #include <Fonts/GFXFF/FreeMonoOblique12pt7b.h> // FF6 or FMO12
  #include <Fonts/GFXFF/FreeMonoOblique18pt7b.h> // FF7 or FMO18
  #include <Fonts/GFXFF/FreeMonoOblique24pt7b.h> // FF8 or FMO24
  
  #include <Fonts/GFXFF/FreeMonoBold9pt7b.h>  // FF9  or FMB9
  #include <Fonts/GFXFF/FreeMonoBold12pt7b.h> // FF10 or FMB12
  #include <Fonts/GFXFF/FreeMonoBold18pt7b.h> // FF11 or FMB18
  #include <Fonts/GFXFF/FreeMonoBold24pt7b.h> // FF12 or FMB24
  
  #include <Fonts/GFXFF/FreeMonoBoldOblique9pt7b.h>  // FF13 or FMBO9
  #include <Fonts/GFXFF/FreeMonoBoldOblique12pt7b.h> // FF14 or FMBO12
  #include <Fonts/GFXFF/FreeMonoBoldOblique18pt7b.h> // FF15 or FMBO18
  #include <Fonts/GFXFF/FreeMonoBoldOblique24pt7b.h> // FF16 or FMBO24
  
  // Sans serif fonts
  #include <Fonts/GFXFF/FreeSans9pt7b.h>  // FF17 or FSS9
  #include <Fonts/GFXFF/FreeSans12pt7b.h> // FF18 or FSS12
  #include <Fonts/GFXFF/FreeSans18pt7b.h> // FF19 or FSS18
  #include <Fonts/GFXFF/FreeSans24pt7b.h> // FF20 or FSS24
  
  #include <Fonts/GFXFF/FreeSansOblique9pt7b.h>  // FF21 or FSSO9
  #include <Fonts/GFXFF/FreeSansOblique12pt7b.h> // FF22 or FSSO12
  #include <Fonts/GFXFF/FreeSansOblique18pt7b.h> // FF23 or FSSO18
  #include <Fonts/GFXFF/FreeSansOblique24pt7b.h> // FF24 or FSSO24
  
  #include <Fonts/GFXFF/FreeSansBold9pt7b.h>  // FF25 or FSSB9
  #include <Fonts/GFXFF/FreeSansBold12pt7b.h> // FF26 or FSSB12
  #include <Fonts/GFXFF/FreeSansBold18pt7b.h> // FF27 or FSSB18
  #include <Fonts/GFXFF/FreeSansBold24pt7b.h> // FF28 or FSSB24
  
  #include <Fonts/GFXFF/FreeSansBoldOblique9pt7b.h>  // FF29 or FSSBO9
  #include <Fonts/GFXFF/FreeSansBoldOblique12pt7b.h> // FF30 or FSSBO12
  #include <Fonts/GFXFF/FreeSansBoldOblique18pt7b.h> // FF31 or FSSBO18
  #include <Fonts/GFXFF/FreeSansBoldOblique24pt7b.h> // FF32 or FSSBO24
  
  // Serif fonts
  #include <Fonts/GFXFF/FreeSerif9pt7b.h>  // FF33 or FS9
  #include <Fonts/GFXFF/FreeSerif12pt7b.h> // FF34 or FS12
  #include <Fonts/GFXFF/FreeSerif18pt7b.h> // FF35 or FS18
  #include <Fonts/GFXFF/FreeSerif24pt7b.h> // FF36 or FS24
  
  #include <Fonts/GFXFF/FreeSerifItalic9pt7b.h>  // FF37 or FSI9
  #include <Fonts/GFXFF/FreeSerifItalic12pt7b.h> // FF38 or FSI12
  #include <Fonts/GFXFF/FreeSerifItalic18pt7b.h> // FF39 or FSI18
  #include <Fonts/GFXFF/FreeSerifItalic24pt7b.h> // FF40 or FSI24
  
  #include <Fonts/GFXFF/FreeSerifBold9pt7b.h>  // FF41 or FSB9
  #include <Fonts/GFXFF/FreeSerifBold12pt7b.h> // FF42 or FSB12
  #include <Fonts/GFXFF/FreeSerifBold18pt7b.h> // FF43 or FSB18
  #include <Fonts/GFXFF/FreeSerifBold24pt7b.h> // FF44 or FSB24
  
  #include <Fonts/GFXFF/FreeSerifBoldItalic9pt7b.h>  // FF45 or FSBI9
  #include <Fonts/GFXFF/FreeSerifBoldItalic12pt7b.h> // FF46 or FSBI12
  #include <Fonts/GFXFF/FreeSerifBoldItalic18pt7b.h> // FF47 or FSBI18
  #include <Fonts/GFXFF/FreeSerifBoldItalic24pt7b.h> // FF48 or FSBI24
  
#endif // #ifdef LOAD_GFXFF

//These enumerate the text plotting alignment (reference datum point)
#define TL_DATUM 0 // Top left (default)
#define TC_DATUM 1 // Top centre
#define TR_DATUM 2 // Top right
#define ML_DATUM 3 // Middle left
#define CL_DATUM 3 // Centre left, same as above
#define MC_DATUM 4 // Middle centre
#define CC_DATUM 4 // Centre centre, same as above
#define MR_DATUM 5 // Middle right
#define CR_DATUM 5 // Centre right, same as above
#define BL_DATUM 6 // Bottom left
#define BC_DATUM 7 // Bottom centre
#define BR_DATUM 8 // Bottom right
#define L_BASELINE  9 // Left character baseline (Line the 'A' character would sit on)
#define C_BASELINE 10 // Centre character baseline
#define R_BASELINE 11 // Right character baseline


// New color definitions use for all my libraries
#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFD20      /* 255, 165,   0 */
#define TFT_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define TFT_PINK        0xF81F


// Swap any type
template <typename T> static inline void
swap_coord(T& a, T& b) { T t = a; a = b; b = t; }

// This is a structure to conveniently hold infomation on the default fonts
// Stores pointer to font character image address table, width table and height

typedef struct {
    const uint8_t *chartbl;
    const uint8_t *widthtbl;
    uint8_t height;
    uint8_t baseline;
    } fontinfo;

// Now fill the structure
const PROGMEM fontinfo fontdata [] = {
   { 0, 0, 0, 0 },

   // GLCD font (Font 1) does not have all parameters
   { 0, 0, 8, 7 },

  #ifdef LOAD_FONT2
   { (const uint8_t *)chrtbl_f16, widtbl_f16, chr_hgt_f16, baseline_f16},
  #else
   { 0, 0, 0, 0 },
  #endif

   // Font 3 current unused
   { 0, 0, 0, 0 },

  #ifdef LOAD_FONT4
   { (const uint8_t *)chrtbl_f32, widtbl_f32, chr_hgt_f32, baseline_f32},
  #else
   { 0, 0, 0, 0 },
  #endif

   // Font 5 current unused
   { 0, 0, 0, 0 },

  #ifdef LOAD_FONT6
   { (const uint8_t *)chrtbl_f64, widtbl_f64, chr_hgt_f64, baseline_f64},
  #else
   { 0, 0, 0, 0 },
  #endif

  #ifdef LOAD_FONT7
   { (const uint8_t *)chrtbl_f7s, widtbl_f7s, chr_hgt_f7s, baseline_f7s},
  #else
   { 0, 0, 0, 0 },
  #endif

  #ifdef LOAD_FONT8
   { (const uint8_t *)chrtbl_f72, widtbl_f72, chr_hgt_f72, baseline_f72}
  #else
   { 0, 0, 0, 0 }
  #endif
};



// Class functions and variables
class TFT_eSPI : public Print {

 public:

  TFT_eSPI(int16_t _W = TFT_WIDTH, int16_t _H = TFT_HEIGHT);

  void     init(void), begin(void); // Same - begin included for backwards compatibility

  void     drawPixel(uint32_t x, uint32_t y, uint32_t color);

  void     drawChar(int32_t x, int32_t y, unsigned char c, uint32_t color, uint32_t bg, uint8_t font),
           setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1),

           pushColor(uint16_t color),
           pushColor(uint16_t color, uint16_t len),

           pushColors(uint16_t *data, uint8_t len),
           pushColors(uint8_t  *data, uint32_t len),

           fillScreen(uint32_t color),

           drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color),
           drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color),
           drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color),

           drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color),
           fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color),
           drawRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color),
           fillRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color),

           setRotation(uint8_t r),
           invertDisplay(boolean i),

           drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
           drawCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, uint32_t color),
           fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
           fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint32_t color),

           drawEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color),
           fillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color),

           drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color),
           fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color),

           drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color),

           setCursor(int16_t x, int16_t y),
           setCursor(int16_t x, int16_t y, uint8_t font),
           setTextColor(uint16_t color),
           setTextColor(uint16_t fgcolor, uint16_t bgcolor),
           setTextSize(uint8_t size),

           setTextWrap(boolean wrap),
           setTextDatum(uint8_t datum),
           setTextPadding(uint16_t x_width),

#ifdef LOAD_GFXFF
           setFreeFont(const GFXfont *f = NULL),
           setTextFont(uint8_t font),
#else
           setFreeFont(uint8_t font),
           setTextFont(uint8_t font),
#endif
           spiwrite(uint8_t),
           writecommand(uint8_t c),
           writedata(uint8_t d),
           commandList(const uint8_t *addr);

  uint8_t  readcommand8(uint8_t cmd_function, uint8_t index);
  uint16_t readcommand16(uint8_t cmd_function, uint8_t index);
  uint32_t readcommand32(uint8_t cmd_function, uint8_t index);

           // Read the colour of a pixel at x,y and return value in 565 format 
  uint16_t readPixel(int32_t x0, int32_t y0);

           // The next functions can be used as a pair to copy screen blocks (or horizontal/vertical lines) to another location
           // Read a block of pixels to a data buffer, buffer is 16 bit and the array size must be at least w * h
  void     readRect(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, uint16_t *data);
           // Write a block of pixels to the screen
  void     pushRect(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h, uint16_t *data);

           // This next function has been used successfully to dump the TFT screen to a PC for documentation purposes
           // It reads a screen area and returns the RGB 8 bit colour values of each pixel
           // Set w and h to 1 to read 1 pixel's colour. The data buffer must be at least w * h * 3 bytes
  void     readRectRGB(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t *data);

  uint8_t  getRotation(void);

  uint16_t fontsLoaded(void),
           color565(uint8_t r, uint8_t g, uint8_t b);

  int16_t  drawChar(unsigned int uniCode, int x, int y, int font),
           drawChar(unsigned int uniCode, int x, int y),
           drawNumber(long long_num,int poX, int poY, int font),
           drawNumber(long long_num,int poX, int poY),
           drawFloat(float floatNumber,int decimal,int poX, int poY, int font),
           drawFloat(float floatNumber,int decimal,int poX, int poY),
           
           // Handle char arrays
           drawString(const char *string, int poX, int poY, int font),
           drawString(const char *string, int poX, int poY),
           drawCentreString(const char *string, int dX, int poY, int font), // Deprecated, use setTextDatum() and drawString()
           drawRightString(const char *string, int dX, int poY, int font),  // Deprecated, use setTextDatum() and drawString()

           // Handle String type
           drawString(const String& string, int poX, int poY, int font),
           drawString(const String& string, int poX, int poY),
           drawCentreString(const String& string, int dX, int poY, int font), // Deprecated, use setTextDatum() and drawString()
           drawRightString(const String& string, int dX, int poY, int font);  // Deprecated, use setTextDatum() and drawString()
           
  int16_t  height(void),
           width(void),
           textWidth(const char *string, int font),
           textWidth(const char *string),
           textWidth(const String& string, int font),
           textWidth(const String& string),
           fontHeight(int16_t font);

    void   setAddrWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye);

 virtual   size_t write(uint8_t);

 private:

inline void spi_begin() __attribute__((always_inline));
inline void spi_end() __attribute__((always_inline));

    void   readAddrWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye);
    
  uint8_t  tabcolor,
           colstart = 0, rowstart = 0; // some ST7735 displays need this changed

  volatile uint32_t *dcport, *csport;//, *mosiport, *clkport, *rsport;

  uint32_t  cspinmask, dcpinmask, wrpinmask;//, mosipinmask, clkpinmask;

  uint32_t lastColor = 0xFFFF;

 protected:

  int32_t  cursor_x, cursor_y, win_xe, win_ye, padX;

  uint32_t _width, _height; // Display w/h as modified by current rotation
  uint32_t textcolor, textbgcolor, fontsloaded, addr_row, addr_col;

  uint8_t  glyph_ab,  // glyph height above baseline
           glyph_bb,  // glyph height below baseline
           textfont,  // Current selected font
           textsize,  // Current font size multiplier
           textdatum, // Text reference datum
           rotation;  // Display rotation (0-3)

  boolean  textwrap; // If set, 'wrap' text at right edge of display

  boolean  locked, inTransaction; // Transaction and mutex lock flags for ESP32

#ifdef LOAD_GFXFF
  GFXfont
    *gfxFont;
#endif

};

#endif

/***************************************************

  ORIGINAL LIBRARY HEADER

  This is our library for the Adafruit  ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

  Updated with new functions by Bodmer 14/4/15
 ****************************************************/
