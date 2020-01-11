/***************************************************
  Arduino TFT graphics library targeted at ESP8266
  and ESP32 based boards.

  This is a standalone library that contains the
  hardware driver, the graphics functions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce
  their FLASH footprint.

 ****************************************************/

// Stop fonts etc being loaded multiple times
#ifndef _TFT_eSPIH_
#define _TFT_eSPIH_

#define TFT_ESPI_VERSION "1.5.0"

// Include header file that defines the fonts loaded, the TFT drivers
// available and the pins to be used, etc, etc
#include <User_Setup_Select.h>

//Standard support
#include <Arduino.h>
#include <Print.h>
#include <SPI.h>

#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
  #include <pgmspace.h>
#else
  #define PROGMEM
#endif

// Include the processor specific drivers
#if defined (ESP32)
  #include "Processors/TFT_eSPI_ESP32.h"
#elif defined (ESP8266)
  #include "Processors/TFT_eSPI_ESP8266.h"
#elif defined (STM32)
  #include "Processors/TFT_eSPI_STM32.h"
#else
  #include "Processors/TFT_eSPI_Generic.h"
#endif

#ifndef TAB_COLOUR
  #define TAB_COLOUR 0
#endif

// If the frequency is not defined, set a default
#ifndef SPI_FREQUENCY
  #define SPI_FREQUENCY  20000000
#endif

// If the frequency is not defined, set a default
#ifndef SPI_READ_FREQUENCY
  #define SPI_READ_FREQUENCY SPI_FREQUENCY
#endif

#if defined(ST7789_DRIVER) || defined(ST7789_2_DRIVER)
  #define TFT_SPI_MODE SPI_MODE3
#else
  #define TFT_SPI_MODE SPI_MODE0
#endif

// If the frequency is not defined, set a default
#ifndef SPI_TOUCH_FREQUENCY
  #define SPI_TOUCH_FREQUENCY  2500000
#endif

// Use GLCD font in error case where user requests a smooth font file
// that does not exist (this is a temporary fix to stop ESP32 reboot)
#ifdef SMOOTH_FONT
  #ifndef LOAD_GLCD
    #define LOAD_GLCD
  #endif
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
#elif defined LOAD_FONT8N
  #define LOAD_FONT8
  #include <Fonts/Font72x53rle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#ifdef LOAD_GFXFF
  // We can include all the free fonts and they will only be built into
  // the sketch if they are used
  #include <Fonts/GFXFF/gfxfont.h>
  // Call up any user custom fonts
  #include <User_Setups/User_Custom_Fonts.h>
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
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFC9F

// Next is a special 16 bit colour value that encodes to 8 bits
// and will then decode back to the same 16 bit value.
// Convenient for 8 bit and 16 bit transparent sprites.
#define TFT_TRANSPARENT 0x0120

// Swap any type
template <typename T> static inline void
swap_coord(T& a, T& b) { T t = a; a = b; b = t; }

#ifndef min
  // Return minimum of two numbers, may already be defined
  // #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

// This structure allows sketches to retrieve the user setup parameters at runtime
// by calling getSetup(), zero impact on code size unless used, mainly for diagnostics
typedef struct
{
String  version = TFT_ESPI_VERSION;
int32_t esp;
uint8_t trans;
uint8_t serial;
uint8_t overlap;

#if defined (ESP32)
  #if defined (USE_HSPI_PORT)
    uint8_t  port = HSPI;
  #else
    uint8_t  port = VSPI;
  #endif
#endif

uint16_t tft_driver; // Hexadecimal code
uint16_t tft_width;  // Rotation 0 width and height
uint16_t tft_height;

uint8_t r0_x_offset; // Offsets, not all used yet
uint8_t r0_y_offset;
uint8_t r1_x_offset;
uint8_t r1_y_offset;
uint8_t r2_x_offset;
uint8_t r2_y_offset;
uint8_t r3_x_offset;
uint8_t r3_y_offset;

int8_t pin_tft_mosi;
int8_t pin_tft_miso;
int8_t pin_tft_clk;
int8_t pin_tft_cs;

int8_t pin_tft_dc;
int8_t pin_tft_rd;
int8_t pin_tft_wr;
int8_t pin_tft_rst;

int8_t pin_tft_d0;
int8_t pin_tft_d1;
int8_t pin_tft_d2;
int8_t pin_tft_d3;
int8_t pin_tft_d4;
int8_t pin_tft_d5;
int8_t pin_tft_d6;
int8_t pin_tft_d7;

int8_t pin_tch_cs;

int16_t tft_spi_freq;
int16_t tft_rd_freq;
int16_t tch_spi_freq;
} setup_t;

// This is a structure to conveniently hold information on the default fonts
// Stores pointer to font character image address table, width table and height

// Create a null set in case some fonts not used (to prevent crash)
const  uint8_t widtbl_null[1] = {0};
PROGMEM const uint8_t chr_null[1] = {0};
PROGMEM const uint8_t* const chrtbl_null[1] = {chr_null};

typedef struct {
    const uint8_t *chartbl;
    const uint8_t *widthtbl;
    uint8_t height;
    uint8_t baseline;
    } fontinfo;

// Now fill the structure
const PROGMEM fontinfo fontdata [] = {
  #ifdef LOAD_GLCD
   { (const uint8_t *)font, widtbl_null, 0, 0 },
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif
   // GLCD font (Font 1) does not have all parameters
   { (const uint8_t *)chrtbl_null, widtbl_null, 8, 7 },

  #ifdef LOAD_FONT2
   { (const uint8_t *)chrtbl_f16, widtbl_f16, chr_hgt_f16, baseline_f16},
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif

   // Font 3 current unused
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },

  #ifdef LOAD_FONT4
   { (const uint8_t *)chrtbl_f32, widtbl_f32, chr_hgt_f32, baseline_f32},
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif

   // Font 5 current unused
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },

  #ifdef LOAD_FONT6
   { (const uint8_t *)chrtbl_f64, widtbl_f64, chr_hgt_f64, baseline_f64},
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif

  #ifdef LOAD_FONT7
   { (const uint8_t *)chrtbl_f7s, widtbl_f7s, chr_hgt_f7s, baseline_f7s},
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif

  #ifdef LOAD_FONT8
   { (const uint8_t *)chrtbl_f72, widtbl_f72, chr_hgt_f72, baseline_f72}
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 }
  #endif
};

typedef uint16_t (*getColorCallback)(uint16_t x, uint16_t y);

// Class functions and variables
class TFT_eSPI : public Print {

 public:

  TFT_eSPI(int16_t _W = TFT_WIDTH, int16_t _H = TFT_HEIGHT);

  void     init(uint8_t tc = TAB_COLOUR), begin(uint8_t tc = TAB_COLOUR); // Same - begin included for backwards compatibility

  // These are virtual so the TFT_eSprite class can override them with sprite specific functions
  virtual void     drawPixel(int32_t x, int32_t y, uint32_t color),
                   drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size),
                   drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color),
                   drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color),
                   drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color),
                   fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

  virtual int16_t  drawChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font),
                   drawChar(uint16_t uniCode, int32_t x, int32_t y),
                   height(void),
                   width(void);

  // The TFT_eSprite class inherits the following functions
  void     setWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye),
           pushColor(uint16_t color),
           pushColor(uint16_t color, uint32_t len),
           pushColors(uint16_t  *data, uint32_t len, bool swap = true), // With byte swap option
           pushColors(uint8_t  *data, uint32_t len),

           fillScreen(uint32_t color);

  void     drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color),
           drawRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color),
           fillRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color),

           setRotation(uint8_t r),
           invertDisplay(bool i),

           drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
           drawCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, uint32_t color),
           fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
           fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint32_t color),

           drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color),
           fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color),

           drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color),
           fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color),

           drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor),
           drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor),
           drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor),
           drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor),
           setBitmapColor(uint16_t fgcolor, uint16_t bgcolor), // For 1bpp sprites

           setPivot(int16_t x, int16_t y),
           setCursor(int16_t x, int16_t y),
           setCursor(int16_t x, int16_t y, uint8_t font),
           setTextColor(uint16_t color),
           setTextColor(uint16_t fgcolor, uint16_t bgcolor),
           setTextSize(uint8_t size),

           setTextWrap(bool wrapX, bool wrapY = false),
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

  uint8_t  readcommand8(uint8_t cmd_function, uint8_t index = 0);
  uint16_t readcommand16(uint8_t cmd_function, uint8_t index = 0);
  uint32_t readcommand32(uint8_t cmd_function, uint8_t index = 0);

           // Read the colour of a pixel at x,y and return value in 565 format 
  uint16_t readPixel(int32_t x0, int32_t y0);
  void     setCallback(getColorCallback getCol);

           // The next functions can be used as a pair to copy screen blocks (or horizontal/vertical lines) to another location
           // Read a block of pixels to a data buffer, buffer is 16 bit and the array size must be at least w * h
  void     readRect(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data);
           // Write a block of pixels to the screen
  void     pushRect(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data);

           // These are used to render images or sprites stored in RAM arrays
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data);
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data, uint16_t transparent);

           // These are used to render images stored in FLASH (PROGMEM)
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, const uint16_t *data, uint16_t transparent);
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, const uint16_t *data);

           // These are used by pushSprite for 1 and 8 bit colours
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t  *data, bool bpp8 = true);
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t  *data, uint8_t  transparent, bool bpp8 = true);

           // Write a solid block of a single colour
  void     pushBlock(uint16_t color, uint32_t len);

           // Write a set of pixels stored in memory, use setSwapBytes(true/false) function to correct endianess
  void     pushPixels(const void * data_in, uint32_t len);

           // Swap the byte order for pushImage() and pushPixels() - corrects endianness
  void     setSwapBytes(bool swap);
  bool     getSwapBytes(void);

           // This next function has been used successfully to dump the TFT screen to a PC for documentation purposes
           // It reads a screen area and returns the RGB 8 bit colour values of each pixel
           // Set w and h to 1 to read 1 pixel's colour. The data buffer must be at least w * h * 3 bytes
  void     readRectRGB(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t *data);

  uint8_t  getRotation(void),
           getTextDatum(void),
           color16to8(uint16_t color565); // Convert 16 bit colour to 8 bits

  int16_t  getCursorX(void),
           getCursorY(void);

  int16_t  getPivotX(void),
           getPivotY(void);

  uint16_t fontsLoaded(void),
           color565(uint8_t red, uint8_t green, uint8_t blue),   // Convert 8 bit red, green and blue to 16 bits
           color8to16(uint8_t color332);  // Convert 8 bit colour to 16 bits

  int16_t  drawNumber(long long_num, int32_t poX, int32_t poY, uint8_t font),
           drawNumber(long long_num, int32_t poX, int32_t poY),
           drawFloat(float floatNumber, uint8_t decimal, int32_t poX, int32_t poY, uint8_t font),
           drawFloat(float floatNumber, uint8_t decimal, int32_t poX, int32_t poY),

           // Handle char arrays
           drawString(const char *string, int32_t poX, int32_t poY, uint8_t font),
           drawString(const char *string, int32_t poX, int32_t poY),
           drawCentreString(const char *string, int32_t dX, int32_t poY, uint8_t font), // Deprecated, use setTextDatum() and drawString()
           drawRightString(const char *string, int32_t dX, int32_t poY, uint8_t font),  // Deprecated, use setTextDatum() and drawString()

           // Handle String type
           drawString(const String& string, int32_t poX, int32_t poY, uint8_t font),
           drawString(const String& string, int32_t poX, int32_t poY),
           drawCentreString(const String& string, int32_t dX, int32_t poY, uint8_t font), // Deprecated, use setTextDatum() and drawString()
           drawRightString(const String& string, int32_t dX, int32_t poY, uint8_t font);  // Deprecated, use setTextDatum() and drawString()

  int16_t  textWidth(const char *string, uint8_t font),
           textWidth(const char *string),
           textWidth(const String& string, uint8_t font),
           textWidth(const String& string),
           fontHeight(int16_t font),
           fontHeight(void);

  void     setAddrWindow(int32_t xs, int32_t ys, int32_t w, int32_t h);

           // Compatibility additions
  void     startWrite(void);                         // Begin SPI transaction
  void     writeColor(uint16_t color, uint32_t len); // Write colours without transaction overhead Deprecated, use pushBlock()
  void     endWrite(void);                           // End SPI transaction

  uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);
  uint16_t decodeUTF8(uint8_t c);
  uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);

  size_t   write(uint8_t);

#ifdef TFT_SDA_READ
  #if defined (TFT_eSPI_ENABLE_8_BIT_READ)
  uint8_t  tft_Read_8(void);
  #endif
  void     begin_SDA_Read(void);
  void     end_SDA_Read(void);
#endif

  // Set or get an arbitrary library attribute or configuration option
  void     setAttribute(uint8_t id = 0, uint8_t a = 0);
  uint8_t  getAttribute(uint8_t id = 0);

  void     getSetup(setup_t& tft_settings); // Sketch provides the instance to populate

  // DMA support functions
  bool     initDMA(void);
  void     deInitDMA(void);
  void     pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* data, uint16_t* buffer = nullptr);
  //void   pushBlockDMA(uint16_t color, uint32_t len);
  void     pushPixelsDMA(uint16_t* image, uint32_t len);
  bool     dmaBusy(void);
  bool     DMA_Enabled = false;

  static   SPIClass& getSPIinstance(void);

  int32_t  cursor_x, cursor_y, padX;
  uint32_t textcolor, textbgcolor;

  uint32_t bitmap_fg, bitmap_bg;

  uint8_t  textfont,  // Current selected font
           textsize,  // Current font size multiplier
           textdatum, // Text reference datum
           rotation;  // Display rotation (0-3)

  int16_t _xpivot;   // x pivot point coordinate
  int16_t _ypivot;   // x pivot point coordinate

  uint8_t  decoderState = 0;   // UTF8 decoder state
  uint16_t decoderBuffer;      // Unicode code-point buffer

 private:

  inline void spi_begin() __attribute__((always_inline));
  inline void spi_end()   __attribute__((always_inline));

  inline void spi_begin_read() __attribute__((always_inline));
  inline void spi_end_read()   __attribute__((always_inline));

           // Private function, sketches must use pushPixels() with setSwapBytes(true)
  void     pushSwapBytePixels(const void* data_in, uint32_t len);

  void     readAddrWindow(int32_t xs, int32_t ys, int32_t w, int32_t h);

  // Byte read prototype
  uint8_t  readByte(void);

  // GPIO parallel input/output control
  void     busDir(uint32_t mask, uint8_t mode);

  // Single GPIO input/output direction control
  void     gpioMode(uint8_t gpio, uint8_t mode);

  uint8_t  tabcolor,
           colstart = 0, rowstart = 0; // some ST7735 displays need this changed

  volatile uint32_t *dcport, *csport;

  uint32_t cspinmask, dcpinmask, wrpinmask, sclkpinmask;

#if defined(ESP32_PARALLEL)
  uint32_t  xclr_mask, xdir_mask, xset_mask[256];
#endif

  uint32_t lastColor = 0xFFFF;

  getColorCallback getColor = nullptr;

 protected:

  int32_t  win_xe, win_ye;

  int32_t  _init_width, _init_height; // Display w/h as input, used by setRotation()
  int32_t  _width, _height;           // Display w/h as modified by current rotation
  int32_t  addr_row, addr_col;

  uint32_t fontsloaded;

  uint8_t  glyph_ab,   // glyph delta Y (height) above baseline
           glyph_bb;   // glyph delta Y (height) below baseline

  bool     isDigits;   // adjust bounding box for numbers to reduce visual jiggling
  bool     textwrapX, textwrapY;   // If set, 'wrap' text at right and optionally bottom edge of display
  bool     _swapBytes; // Swap the byte order for TFT pushImage()
  bool     locked, inTransaction; // Transaction and mutex lock flags

  bool     _booted;    // init() or begin() has already run once
  bool     _cp437;     // If set, use correct CP437 charset (default is ON)
  bool     _utf8;      // If set, use UTF-8 decoder in print stream 'write()' function (default ON)

  uint32_t _lastColor; // Buffered value of last colour used

#ifdef LOAD_GFXFF
  GFXfont  *gfxFont;
#endif

// Load the Touch extension
#ifdef TOUCH_CS
  #include "Extensions/Touch.h"
#endif

// Load the Anti-aliased font extension
#ifdef SMOOTH_FONT
  #include "Extensions/Smooth_font.h"
#endif

}; // End of class TFT_eSPI

// Load the Button Class
#include "Extensions/Button.h"

// Load the Sprite Class
#include "Extensions/Sprite.h"

#endif
