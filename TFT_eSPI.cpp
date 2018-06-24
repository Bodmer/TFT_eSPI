/***************************************************
  Arduino TFT graphics library targeted at ESP8266
  and ESP32 based boards.

  This is a standalone library that contains the
  hardware driver, the graphics functions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce their
  size.

  Created by Bodmer 2/12/16
  Bodmer: Added RPi 16 bit display support
 ****************************************************/

#include "TFT_eSPI.h"

#include <pgmspace.h>

#ifndef ESP32_PARALLEL
  #include <SPI.h>
#endif

  // SUPPORT_TRANSACTIONS is mandatory for ESP32 so the hal mutex is toggled
#if defined (ESP32) && !defined (SUPPORT_TRANSACTIONS)
  #define SUPPORT_TRANSACTIONS
#endif

// If it is a 16bit serial display we must transfer 16 bits every time
#ifdef RPI_ILI9486_DRIVER
  #define CMD_BITS 16-1
#else
  #define CMD_BITS 8-1
#endif

// Fast block write prototype
void writeBlock(uint16_t color, uint32_t repeat);

// Byte read prototype
uint8_t readByte(void);

// GPIO parallel input/output control
void busDir(uint32_t mask, uint8_t mode);

// If the SPI library has transaction support, these functions
// establish settings and protect from interference from other
// libraries.  Otherwise, they simply do nothing.

inline void TFT_eSPI::spi_begin(void){
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
  if (locked) {locked = false; SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));}
#endif
}

inline void TFT_eSPI::spi_end(void){
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS) && !defined(ESP32_PARALLEL)
  if(!inTransaction) {if (!locked) {locked = true; SPI.endTransaction();}}
#endif
}

#if defined (TOUCH_CS) && defined (SPI_TOUCH_FREQUENCY) // && !defined(ESP32_PARALLEL)

  inline void TFT_eSPI::spi_begin_touch(void){
  #if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS)
    if (locked) {locked = false; SPI.beginTransaction(SPISettings(SPI_TOUCH_FREQUENCY, MSBFIRST, SPI_MODE0));}
  #else
    SPI.setFrequency(SPI_TOUCH_FREQUENCY);
  #endif
  }

  inline void TFT_eSPI::spi_end_touch(void){
  #if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS)
    if(!inTransaction) {if (!locked) {locked = true; SPI.endTransaction();}}
  #else
    SPI.setFrequency(SPI_FREQUENCY);
  #endif
  }

#endif

/***************************************************************************************
** Function name:           TFT_eSPI
** Description:             Constructor , we must use hardware SPI pins
***************************************************************************************/
TFT_eSPI::TFT_eSPI(int16_t w, int16_t h)
{

// The control pins are deliberately set to the inactive state (CS high) as setup()
// might call and initialise other SPI peripherals which would could cause conflicts
// if CS is floating or undefined.
#ifdef TFT_CS
  digitalWrite(TFT_CS, HIGH); // Chip select high (inactive)
  pinMode(TFT_CS, OUTPUT);
#endif

// Configure chip select for touchscreen controller if present
#ifdef TOUCH_CS
  digitalWrite(TOUCH_CS, HIGH); // Chip select high (inactive)
  pinMode(TOUCH_CS, OUTPUT);
#endif

#ifdef TFT_WR
  digitalWrite(TFT_WR, HIGH); // Set write strobe high (inactive)
  pinMode(TFT_WR, OUTPUT);
#endif

#ifdef TFT_DC
  digitalWrite(TFT_DC, HIGH); // Data/Command high = data mode
  pinMode(TFT_DC, OUTPUT);
#endif

#ifdef TFT_RST
  if (TFT_RST >= 0) {
    digitalWrite(TFT_RST, HIGH); // Set high, do not share pin with another SPI device
    pinMode(TFT_RST, OUTPUT);
  }
#endif

#ifdef ESP32_PARALLEL
  // Create a data bus and Write line GPIO bit clear mask
  //clr_mask = (1 << TFT_D0) | (1 << TFT_D1) | (1 << TFT_D2) | (1 << TFT_D3) | (1 << TFT_D4) | (1 << TFT_D5) | (1 << TFT_D6) | (1 << TFT_D7) | (1 << TFT_WR);

  // Create a data bus GPIO bit direction mask
  //dir_mask = (1 << TFT_D0) | (1 << TFT_D1) | (1 << TFT_D2) | (1 << TFT_D3) | (1 << TFT_D4) | (1 << TFT_D5) | (1 << TFT_D6) | (1 << TFT_D7);


  // Create a bit set lookup table for data bus - wastes 1kbyte of RAM but speeds things up dramatically
  for (int c = 0; c<256; c++)
  {
    xset_mask[c] = 0;
    if ( c & 0x01 ) xset_mask[c] |= (1 << TFT_D0);
    if ( c & 0x02 ) xset_mask[c] |= (1 << TFT_D1);
    if ( c & 0x04 ) xset_mask[c] |= (1 << TFT_D2);
    if ( c & 0x08 ) xset_mask[c] |= (1 << TFT_D3);
    if ( c & 0x10 ) xset_mask[c] |= (1 << TFT_D4);
    if ( c & 0x20 ) xset_mask[c] |= (1 << TFT_D5);
    if ( c & 0x40 ) xset_mask[c] |= (1 << TFT_D6);
    if ( c & 0x80 ) xset_mask[c] |= (1 << TFT_D7);
  }

  // Make sure read is high before we set the bus to output
  digitalWrite(TFT_RD, HIGH);
  pinMode(TFT_RD, OUTPUT);
  
  GPIO.out_w1ts = set_mask(255); // Set data bus to 0xFF

  // Set TFT data bus lines to output
  busDir(dir_mask, OUTPUT);

#endif

  _init_width  = _width  = w; // Set by specific xxxxx_Defines.h file or by users sketch
  _init_height = _height = h; // Set by specific xxxxx_Defines.h file or by users sketch
  rotation  = 0;
  cursor_y  = cursor_x  = 0;
  textfont  = 1;
  textsize  = 1;
  textcolor   = bitmap_fg = 0xFFFF; // White
  textbgcolor = bitmap_bg = 0x0000; // Black
  padX = 0;             // No padding
  textwrapX  = true;    // Wrap text at end of line when using print stream
  textwrapY  = false;   // Wrap text at bottom of screen when using print stream
  textdatum = TL_DATUM; // Top Left text alignment is default
  fontsloaded = 0;

  _swapBytes = false;   // Do not swap colour bytes by default

  locked = true;        // ESP32 transaction mutex lock flags
  inTransaction = false;

  addr_row = 0xFFFF;
  addr_col = 0xFFFF;

#ifdef LOAD_GLCD
  fontsloaded  = 0x0002; // Bit 1 set
#endif

#ifdef LOAD_FONT2
  fontsloaded |= 0x0004; // Bit 2 set
#endif

#ifdef LOAD_FONT4
  fontsloaded |= 0x0010; // Bit 4 set
#endif

#ifdef LOAD_FONT6
  fontsloaded |= 0x0040; // Bit 6 set
#endif

#ifdef LOAD_FONT7
  fontsloaded |= 0x0080; // Bit 7 set
#endif

#ifdef LOAD_FONT8
  fontsloaded |= 0x0100; // Bit 8 set
#endif

#ifdef LOAD_FONT8N
  fontsloaded |= 0x0200; // Bit 9 set
#endif

#ifdef SMOOTH_FONT
  fontsloaded |= 0x8000; // Bit 15 set
#endif
}


/***************************************************************************************
** Function name:           begin
** Description:             Included for backwards compatibility
***************************************************************************************/
void TFT_eSPI::begin(uint8_t tc)
{
 init(tc);
}


/***************************************************************************************
** Function name:           init
** Description:             Reset, then initialise the TFT display registers
***************************************************************************************/
void TFT_eSPI::init(uint8_t tc)
{
#if !defined (ESP32)
  #ifdef TFT_CS
    cspinmask = (uint32_t) digitalPinToBitMask(TFT_CS);
  #endif

  #ifdef TFT_DC
    dcpinmask = (uint32_t) digitalPinToBitMask(TFT_DC);
  #endif
  
  #ifdef TFT_WR
    wrpinmask = (uint32_t) digitalPinToBitMask(TFT_WR);
  #endif
  
  #ifdef TFT_SPI_OVERLAP
    // Overlap mode SD0=MISO, SD1=MOSI, CLK=SCLK must use D3 as CS
    //    pins(int8_t sck, int8_t miso, int8_t mosi, int8_t ss);
    //SPI.pins(        6,          7,           8,          0);
    SPI.pins(6, 7, 8, 0);
  #endif

  SPI.begin(); // This will set HMISO to input
#else
  #if !defined(ESP32_PARALLEL)
    #if defined (TFT_MOSI) && !defined (TFT_SPI_OVERLAP)
      SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, -1);
    #else
      SPI.begin();
    #endif
  #endif
#endif

  inTransaction = false;
  locked = true;

  // SUPPORT_TRANSACTIONS is mandatory for ESP32 so the hal mutex is toggled
  // so the code here is for ESP8266 only
#if !defined (SUPPORT_TRANSACTIONS) && defined (ESP8266)
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setFrequency(SPI_FREQUENCY);
#endif

#if defined(ESP32_PARALLEL)
    digitalWrite(TFT_CS, LOW); // Chip select low permanently
    pinMode(TFT_CS, OUTPUT);
#else
  #ifdef TFT_CS
    // Set to output once again in case D6 (MISO) is used for CS
    digitalWrite(TFT_CS, HIGH); // Chip select high (inactive)
    pinMode(TFT_CS, OUTPUT);
  #else
    SPI.setHwCs(1); // Use hardware SS toggling
  #endif
#endif
  
  // Set to output once again in case D6 (MISO) is used for DC
#ifdef TFT_DC
  digitalWrite(TFT_DC, HIGH); // Data/Command high = data mode
  pinMode(TFT_DC, OUTPUT);
#endif

  // Toggle RST low to reset
#ifdef TFT_RST
  if (TFT_RST >= 0) {
    digitalWrite(TFT_RST, HIGH);
    delay(5);
    digitalWrite(TFT_RST, LOW);
    delay(20);
    digitalWrite(TFT_RST, HIGH);
    delay(150);
  }
#endif

  spi_begin();
  writecommand(TFT_SWRST); // Software reset
  spi_end();
  
  delay(5); // Wait for software reset to complete

  spi_begin();
  
  // This loads the driver specific initialisation code  <<<<<<<<<<<<<<<<<<<<< ADD NEW DRIVERS TO THE LIST HERE <<<<<<<<<<<<<<<<<<<<<<<
#if   defined (ILI9341_DRIVER)
    #include "TFT_Drivers/ILI9341_Init.h"

#elif defined (ST7735_DRIVER)
    tabcolor = tc;
    #include "TFT_Drivers/ST7735_Init.h"

#elif defined (ILI9163_DRIVER)
    #include "TFT_Drivers/ILI9163_Init.h"

#elif defined (S6D02A1_DRIVER)
    #include "TFT_Drivers/S6D02A1_Init.h"
     
#elif defined (RPI_ILI9486_DRIVER)
    #include "TFT_Drivers/RPI_ILI9486_Init.h"

#elif defined (ILI9486_DRIVER)
    #include "TFT_Drivers/ILI9486_Init.h"

#elif defined (ILI9481_DRIVER)
    #include "TFT_Drivers/ILI9481_Init.h"

#elif defined (ILI9488_DRIVER)
    #include "TFT_Drivers/ILI9488_Init.h"

#elif defined (HX8357D_DRIVER)
    #include "TFT_Drivers/HX8357D_Init.h"

#elif defined (ST7789_DRIVER)
    #include "TFT_Drivers/ST7789_Init.h"

#endif

  spi_end();

}


/***************************************************************************************
** Function name:           setRotation
** Description:             rotate the screen orientation m = 0-3 or 4-7 for BMP drawing
***************************************************************************************/
void TFT_eSPI::setRotation(uint8_t m)
{

  spi_begin();

    // This loads the driver specific rotation code  <<<<<<<<<<<<<<<<<<<<< ADD NEW DRIVERS TO THE LIST HERE <<<<<<<<<<<<<<<<<<<<<<<
#if   defined (ILI9341_DRIVER)
    #include "TFT_Drivers/ILI9341_Rotation.h"

#elif defined (ST7735_DRIVER)
    #include "TFT_Drivers/ST7735_Rotation.h"

#elif defined (ILI9163_DRIVER)
    #include "TFT_Drivers/ILI9163_Rotation.h"

#elif defined (S6D02A1_DRIVER)
    #include "TFT_Drivers/S6D02A1_Rotation.h"

#elif defined (RPI_ILI9486_DRIVER)
    #include "TFT_Drivers/RPI_ILI9486_Rotation.h"

#elif defined (ILI9486_DRIVER)
    #include "TFT_Drivers/ILI9486_Rotation.h"

#elif defined (ILI9481_DRIVER)
    #include "TFT_Drivers/ILI9481_Rotation.h"

#elif defined (ILI9488_DRIVER)
    #include "TFT_Drivers/ILI9488_Rotation.h"

#elif defined (HX8357D_DRIVER)
    #include "TFT_Drivers/HX8357D_Rotation.h"

#elif defined (ST7789_DRIVER)
    #include "TFT_Drivers/ST7789_Rotation.h"

#endif

  delayMicroseconds(10);

  spi_end();

  addr_row = 0xFFFF;
  addr_col = 0xFFFF;
}


/***************************************************************************************
** Function name:           commandList, used for FLASH based lists only (e.g. ST7735)
** Description:             Get initialisation commands from FLASH and send to TFT
***************************************************************************************/
void TFT_eSPI::commandList (const uint8_t *addr)
{
  uint8_t  numCommands;
  uint8_t  numArgs;
  uint8_t  ms;

  spi_begin();

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow

  while (numCommands--)                  // For each command...
  {
    writecommand(pgm_read_byte(addr++)); // Read, issue command
    numArgs = pgm_read_byte(addr++);     // Number of args to follow
    ms = numArgs & TFT_INIT_DELAY;       // If hibit set, delay follows args
    numArgs &= ~TFT_INIT_DELAY;          // Mask out delay bit

    while (numArgs--)                    // For each argument...
    {
      writedata(pgm_read_byte(addr++));  // Read, issue argument
    }

    if (ms)
    {
      ms = pgm_read_byte(addr++);        // Read post-command delay time (ms)
      delay( (ms==255 ? 500 : ms) );
    }
  }
  spi_end();
}


/***************************************************************************************
** Function name:           spiwrite
** Description:             Write 8 bits to SPI port (legacy support only)
***************************************************************************************/
void TFT_eSPI::spiwrite(uint8_t c)
{
  tft_Write_8(c);
}


/***************************************************************************************
** Function name:           writecommand
** Description:             Send an 8 bit command to the TFT
***************************************************************************************/
void TFT_eSPI::writecommand(uint8_t c)
{
  DC_C;
  CS_L;

  tft_Write_8(c);

  CS_H;
  DC_D;
}


/***************************************************************************************
** Function name:           writedata
** Description:             Send a 8 bit data value to the TFT
***************************************************************************************/
void TFT_eSPI::writedata(uint8_t d)
{
  CS_L;

  tft_Write_8(d);

  CS_H;
}


/***************************************************************************************
** Function name:           readcommand8
** Description:             Read a 8 bit data value from an indexed command register
***************************************************************************************/
uint8_t TFT_eSPI::readcommand8(uint8_t cmd_function, uint8_t index)
{
  uint8_t reg = 0;
#ifdef ESP32_PARALLEL

  writecommand(cmd_function); // Sets DC and CS high 

  busDir(dir_mask, INPUT);

  CS_L;

  // Read nth parameter (assumes caller discards 1st parameter or points index to 2nd)
  while(index--) reg = readByte();

  busDir(dir_mask, OUTPUT);

  CS_H;

#else
  // for ILI9341 Interface II i.e. IM [3:0] = "1101"
  spi_begin();
  index = 0x10 + (index & 0x0F);

  DC_C;
  CS_L;
  tft_Write_8(0xD9);
  DC_D;
  tft_Write_8(index);
  CS_H;

  DC_C;
  CS_L;
  tft_Write_8(cmd_function);
  DC_D;
  reg = tft_Write_8(0);
  CS_H;

  spi_end();
#endif
  return reg;
}


/***************************************************************************************
** Function name:           readcommand16
** Description:             Read a 16 bit data value from an indexed command register
***************************************************************************************/
uint16_t TFT_eSPI::readcommand16(uint8_t cmd_function, uint8_t index)
{
  uint32_t reg;

  reg |= (readcommand8(cmd_function, index + 0) <<  8);
  reg |= (readcommand8(cmd_function, index + 1) <<  0);

  return reg;
}


/***************************************************************************************
** Function name:           readcommand32
** Description:             Read a 32 bit data value from an indexed command register
***************************************************************************************/
uint32_t TFT_eSPI::readcommand32(uint8_t cmd_function, uint8_t index)
{
  uint32_t reg;

  reg  = (readcommand8(cmd_function, index + 0) << 24);
  reg |= (readcommand8(cmd_function, index + 1) << 16);
  reg |= (readcommand8(cmd_function, index + 2) <<  8);
  reg |= (readcommand8(cmd_function, index + 3) <<  0);

  return reg;
}


/***************************************************************************************
** Function name:           read pixel (for SPI Interface II i.e. IM [3:0] = "1101")
** Description:             Read 565 pixel colours from a pixel
***************************************************************************************/
uint16_t TFT_eSPI::readPixel(int32_t x0, int32_t y0)
{
#if defined(ESP32_PARALLEL)

  readAddrWindow(x0, y0, x0, y0); // Sets CS low

  // Set masked pins D0- D7 to input
  busDir(dir_mask, INPUT); 

  // Dummy read to throw away don't care value
  readByte();

  // Fetch the 16 bit BRG pixel
  //uint16_t rgb = (readByte() << 8) | readByte();

#if defined (ILI9341_DRIVER) | defined (ILI9488_DRIVER) // Read 3 bytes

  // Read window pixel 24 bit RGB values and fill in LS bits
  uint16_t rgb = ((readByte() & 0xF8) << 8) | ((readByte() & 0xFC) << 3) | (readByte() >> 3);

  CS_H;

  // Set masked pins D0- D7 to output
  busDir(dir_mask, OUTPUT);

  return rgb;

#else // ILI9481 16 bit read

  // Fetch the 16 bit BRG pixel
  uint16_t bgr = (readByte() << 8) | readByte();

  CS_H;

  // Set masked pins D0- D7 to output
  busDir(dir_mask, OUTPUT);

  // Swap Red and Blue (could check MADCTL setting to see if this is needed)
  return  (bgr>>11) | (bgr<<11) | (bgr & 0x7E0);
#endif

#else // Not ESP32_PARALLEL

  spi_begin();

  readAddrWindow(x0, y0, x0, y0); // Sets CS low

  // Dummy read to throw away don't care value
  tft_Write_8(0);
    
  // Read window pixel 24 bit RGB values
  uint8_t r = tft_Write_8(0);
  uint8_t g = tft_Write_8(0);
  uint8_t b = tft_Write_8(0);

  CS_H;

  spi_end();
    
  return color565(r, g, b);

#endif
}


/***************************************************************************************
** Function name:           read byte  - supports class functions
** Description:             Read a byte from ESP32 8 bit data port
***************************************************************************************/
// Bus MUST be set to input before calling this function!
uint8_t readByte(void)
{
  uint8_t b = 0;

#ifdef ESP32_PARALLEL
  RD_L;
  uint32_t reg;           // Read all GPIO pins 0-31
  reg = gpio_input_get(); // Read three times to allow for bus access time
  reg = gpio_input_get();
  reg = gpio_input_get(); // Data should be stable now
  RD_H;

  // Check GPIO bits used and build value
  b  = (((reg>>TFT_D0)&1) << 0);
  b |= (((reg>>TFT_D1)&1) << 1);
  b |= (((reg>>TFT_D2)&1) << 2);
  b |= (((reg>>TFT_D3)&1) << 3);
  b |= (((reg>>TFT_D4)&1) << 4);
  b |= (((reg>>TFT_D5)&1) << 5);
  b |= (((reg>>TFT_D6)&1) << 6);
  b |= (((reg>>TFT_D7)&1) << 7);
#endif

  return b;
}

/***************************************************************************************
** Function name:           masked GPIO direction control  - supports class functions
** Description:             Set masked ESP32 GPIO pins to input or output
***************************************************************************************/
void busDir(uint32_t mask, uint8_t mode)
{
#ifdef ESP32_PARALLEL

  // Supports GPIO 0 - 31 on ESP32 only
  gpio_config_t gpio;

  gpio.pin_bit_mask = mask;
  gpio.mode         = GPIO_MODE_INPUT;
  gpio.pull_up_en   = GPIO_PULLUP_ENABLE;
  gpio.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpio.intr_type    = GPIO_INTR_DISABLE;

  if (mode == OUTPUT) gpio.mode = GPIO_MODE_OUTPUT;

  gpio_config(&gpio);

#endif
}

/***************************************************************************************
** Function name:           read rectangle (for SPI Interface II i.e. IM [3:0] = "1101")
** Description:             Read 565 pixel colours from a defined area
***************************************************************************************/
void TFT_eSPI::readRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t *data)
{
  if ((x > _width) || (y > _height) || (w == 0) || (h == 0)) return;

#if defined(ESP32_PARALLEL)

  readAddrWindow(x, y, x + w - 1, y + h - 1); // Sets CS low

  // Set masked pins D0- D7 to input
  busDir(dir_mask, INPUT);

  // Dummy read to throw away don't care value
  readByte();

  // Total pixel count
  uint32_t len = w * h;

#if defined (ILI9341_DRIVER) | defined (ILI9488_DRIVER) // Read 3 bytes
  // Fetch the 24 bit RGB value
  while (len--) {
    // Assemble the RGB 16 bit colour
    uint16_t rgb = ((readByte() & 0xF8) << 8) | ((readByte() & 0xFC) << 3) | (readByte() >> 3);

    // Swapped byte order for compatibility with pushRect()
    *data++ = (rgb<<8) | (rgb>>8);
  }
#else // ILI9481 reads as 16 bits
  // Fetch the 16 bit BRG pixels
  while (len--) {
    // Read the BRG 16 bit colour
    uint16_t bgr = (readByte() << 8) | readByte();

    // Swap Red and Blue (could check MADCTL setting to see if this is needed)
    uint16_t rgb = (bgr>>11) | (bgr<<11) | (bgr & 0x7E0);

    // Swapped byte order for compatibility with pushRect()
    *data++ = (rgb<<8) | (rgb>>8);
  }
#endif
  CS_H;

  // Set masked pins D0- D7 to output
  busDir(dir_mask, OUTPUT);

#else // Not ESP32_PARALLEL

  spi_begin();

  readAddrWindow(x, y, x + w - 1, y + h - 1); // Sets CS low

  // Dummy read to throw away don't care value
  tft_Write_8(0);

  // Read window pixel 24 bit RGB values
  uint32_t len = w * h;
  while (len--) {
    // Read the 3 RGB bytes, colour is actually only in the top 6 bits of each byte
    // as the TFT stores colours as 18 bits
    uint8_t r = tft_Write_8(0);
    uint8_t g = tft_Write_8(0);
    uint8_t b = tft_Write_8(0);

    // Swapped colour byte order for compatibility with pushRect()
    *data++ = (r & 0xF8) | (g & 0xE0) >> 5 | (b & 0xF8) << 5 | (g & 0x1C) << 11;
  }

  CS_H;

  spi_end();
#endif
}


/***************************************************************************************
** Function name:           push rectangle (for SPI Interface II i.e. IM [3:0] = "1101")
** Description:             push 565 pixel colours into a defined area
***************************************************************************************/
void TFT_eSPI::pushRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t *data)
{
  // Function deprecated, remains for backwards compatibility
  // pushImage() is better as it will crop partly off-screen image blocks
  pushImage(x, y, w, h, data);
}


/***************************************************************************************
** Function name:           pushImage
** Description:             plot 16 bit colour sprite or image onto TFT
***************************************************************************************/
void TFT_eSPI::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, uint16_t *data)
{

  if ((x >= (int32_t)_width) || (y >= (int32_t)_height)) return;

  int32_t dx = 0;
  int32_t dy = 0;
  int32_t dw = w;
  int32_t dh = h;

  if (x < 0) { dw += x; dx = -x; x = 0; }
  if (y < 0) { dh += y; dy = -y; y = 0; }

  if ((x + w) > _width ) dw = _width  - x;
  if ((y + h) > _height) dh = _height - y;

  if (dw < 1 || dh < 1) return;

  spi_begin();
  inTransaction = true;

  setAddrWindow(x, y, x + dw - 1, y + dh - 1); // Sets CS low and sent RAMWR

  data += dx + dy * w;

  while (dh--)
  {
    pushColors(data, dw, _swapBytes);
    data += w;
  }

  CS_H;

  inTransaction = false;
  spi_end();
}

/***************************************************************************************
** Function name:           pushImage
** Description:             plot 16 bit sprite or image with 1 colour being transparent
***************************************************************************************/
void TFT_eSPI::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, uint16_t *data, uint16_t transp)
{

  if ((x >= (int32_t)_width) || (y >= (int32_t)_height)) return;

  int32_t dx = 0;
  int32_t dy = 0;
  int32_t dw = w;
  int32_t dh = h;

  if (x < 0) { dw += x; dx = -x; x = 0; }
  if (y < 0) { dh += y; dy = -y; y = 0; }
  
  if ((x + w) > _width ) dw = _width  - x;
  if ((y + h) > _height) dh = _height - y;

  if (dw < 1 || dh < 1) return;

  spi_begin();
  inTransaction = true;

  data += dx + dy * w;

  int32_t xe = x + dw - 1, ye = y + dh - 1;

  uint16_t  lineBuf[dw];

  if (!_swapBytes) transp = transp >> 8 | transp << 8;

  while (dh--)
  {
    int32_t len = dw;
    uint16_t* ptr = data;
    int32_t px = x;
    boolean move = true;
    uint16_t np = 0;

    while (len--)
    {
      if (transp != *ptr)
      {
        if (move) { move = false; setAddrWindow(px, y, xe, ye); }
        lineBuf[np] = *ptr;
        np++;
      }
      else
      {
        move = true;
        if (np)
        {
           pushColors((uint16_t*)lineBuf, np, _swapBytes);
           np = 0;
        }
      }
      px++;
      ptr++;
    }
    if (np) pushColors((uint16_t*)lineBuf, np, _swapBytes);

    y++;
    data += w;
  }

  CS_H;

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           pushImage - for FLASH (PROGMEM) stored images
** Description:             plot 16 bit image
***************************************************************************************/
void TFT_eSPI::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint16_t *data)
{

  if ((x >= (int32_t)_width) || (y >= (int32_t)_height)) return;

  int32_t dx = 0;
  int32_t dy = 0;
  int32_t dw = w;
  int32_t dh = h;

  if (x < 0) { dw += x; dx = -x; x = 0; }
  if (y < 0) { dh += y; dy = -y; y = 0; }
  
  if ((x + w) > _width ) dw = _width  - x;
  if ((y + h) > _height) dh = _height - y;

  if (dw < 1 || dh < 1) return;

  spi_begin();
  inTransaction = true;

  data += dx + dy * w;

  uint16_t  buffer[64];
  uint16_t* pix_buffer = buffer;

  setAddrWindow(x, y, x + dw - 1, y + dh - 1);

  // Work out the number whole buffers to send
  uint16_t nb = (dw * dh) / 64;

  // Fill and send "nb" buffers to TFT
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < 64; j++) {
      pix_buffer[j] = pgm_read_word(&data[i * 64 + j]);
    }
    pushColors(pix_buffer, 64, !_swapBytes);
  }

  // Work out number of pixels not yet sent
  uint16_t np = (dw * dh) % 64;

  // Send any partial buffer left over
  if (np) {
    for (int i = 0; i < np; i++)
    {
      pix_buffer[i] = pgm_read_word(&data[nb * 64 + i]);
    }
    pushColors(pix_buffer, np, !_swapBytes);
  }

  CS_H;

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           pushImage - for FLASH (PROGMEM) stored images
** Description:             plot 16 bit image with 1 colour being transparent
***************************************************************************************/
void TFT_eSPI::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint16_t *data, uint16_t transp)
{

  if ((x >= (int32_t)_width) || (y >= (int32_t)_height)) return;

  int32_t dx = 0;
  int32_t dy = 0;
  int32_t dw = w;
  int32_t dh = h;

  if (x < 0) { dw += x; dx = -x; x = 0; }
  if (y < 0) { dh += y; dy = -y; y = 0; }
  
  if ((x + w) > _width ) dw = _width  - x;
  if ((y + h) > _height) dh = _height - y;

  if (dw < 1 || dh < 1) return;

  spi_begin();
  inTransaction = true;

  data += dx + dy * w;

  int32_t xe = x + dw - 1, ye = y + dh - 1;

  uint16_t  lineBuf[dw];

  if (_swapBytes) transp = transp >> 8 | transp << 8;

  while (dh--)
  {
    int32_t len = dw;
    uint16_t* ptr = (uint16_t*)data;
    int32_t px = x;
    boolean move = true;

    uint16_t np = 0;

    while (len--)
    {
      uint16_t color = pgm_read_word(ptr);
      if (transp != color)
      {
        if (move) { move = false; setAddrWindow(px, y, xe, ye); }
        lineBuf[np] = color;
        np++;
      }
      else
      {
        move = true;
        if (np)
        {
           pushColors(lineBuf, np, !_swapBytes);
           np = 0;
        }
      }
      px++;
      ptr++;
    }
    if (np) pushColors(lineBuf, np, !_swapBytes);

    y++;
    data += w;
  }

  CS_H;

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           pushImage
** Description:             plot 8 bit image or sprite using a line buffer
***************************************************************************************/
void TFT_eSPI::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t *data, bool bpp8)
{
  if ((x >= (int32_t)_width) || (y >= (int32_t)_height)) return;

  int32_t dx = 0;
  int32_t dy = 0;
  int32_t dw = w;
  int32_t dh = h;

  if (x < 0) { dw += x; dx = -x; x = 0; }
  if (y < 0) { dh += y; dy = -y; y = 0; }
  
  if ((x + w) > _width ) dw = _width  - x;
  if ((y + h) > _height) dh = _height - y;

  if (dw < 1 || dh < 1) return;

  spi_begin();
  inTransaction = true;

  setAddrWindow(x, y, x + dw - 1, y + dh - 1); // Sets CS low and sent RAMWR

  // Line buffer makes plotting faster
  uint16_t  lineBuf[dw];

  if (bpp8)
  {
    uint8_t  blue[] = {0, 11, 21, 31}; // blue 2 to 5 bit colour lookup table

    _lastColor = -1; // Set to illegal value

    // Used to store last shifted colour
    uint8_t msbColor = 0;
    uint8_t lsbColor = 0;

    data += dx + dy * w;
    while (dh--)
    {
      uint32_t len = dw;
      uint8_t* ptr = data;
      uint8_t* linePtr = (uint8_t*)lineBuf;

      while(len--)
      {
        uint32_t color = *ptr++;

        // Shifts are slow so check if colour has changed first
        if (color != _lastColor) {
          //          =====Green=====     ===============Red==============
          msbColor = (color & 0x1C)>>2 | (color & 0xC0)>>3 | (color & 0xE0);
          //          =====Green=====    =======Blue======
          lsbColor = (color & 0x1C)<<3 | blue[color & 0x03];
          _lastColor = color;
        }

       *linePtr++ = msbColor;
       *linePtr++ = lsbColor;
      }

      pushColors(lineBuf, dw, false);

      data += w;
    }
  }
  else
  {
    while (dh--)
    {
      w =  (w+7) & 0xFFF8;

      int32_t len = dw;
      uint8_t* ptr = data;
      uint8_t* linePtr = (uint8_t*)lineBuf;
      uint8_t  bits = 8;
      while(len>0)
      {
        if (len < 8) bits = len;
        uint32_t xp = dx;
        for (uint16_t i = 0; i < bits; i++)
        {
          uint8_t col = (ptr[(xp + dy * w)>>3] << (xp & 0x7)) & 0x80;
          if (col) {*linePtr++ = bitmap_fg>>8; *linePtr++ = (uint8_t) bitmap_fg;}
          else     {*linePtr++ = bitmap_bg>>8; *linePtr++ = (uint8_t) bitmap_bg;}
          //if (col) drawPixel((dw-len)+xp,h-dh,bitmap_fg);
          //else     drawPixel((dw-len)+xp,h-dh,bitmap_bg);
          xp++;
        }
        *ptr++;
        len -= 8;
      }

      pushColors(lineBuf, dw, false);

      dy++;
    }
  }

  CS_H;

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           pushImage
** Description:             plot 8 or 1 bit image or sprite with a transparent colour
***************************************************************************************/
void TFT_eSPI::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t *data, uint8_t transp, bool bpp8)
{
  if ((x >= (int32_t)_width) || (y >= (int32_t)_height)) return;

  int32_t dx = 0;
  int32_t dy = 0;
  int32_t dw = w;
  int32_t dh = h;

  if (x < 0) { dw += x; dx = -x; x = 0; }
  if (y < 0) { dh += y; dy = -y; y = 0; }
  
  if ((x + w) > _width ) dw = _width  - x;
  if ((y + h) > _height) dh = _height - y;

  if (dw < 1 || dh < 1) return;

  spi_begin();
  inTransaction = true;

  int32_t xe = x + dw - 1, ye = y + dh - 1;

  // Line buffer makes plotting faster
  uint16_t  lineBuf[dw];

  if (bpp8)
  {
    data += dx + dy * w;

    uint8_t  blue[] = {0, 11, 21, 31}; // blue 2 to 5 bit colour lookup table

    _lastColor = -1; // Set to illegal value

    // Used to store last shifted colour
    uint8_t msbColor = 0;
    uint8_t lsbColor = 0;

    int32_t spx = x, spy = y;

    while (dh--)
    {
      int32_t len = dw;
      uint8_t* ptr = data;
      uint8_t* linePtr = (uint8_t*)lineBuf;

      int32_t px = x;
      boolean move = true;
      uint16_t np = 0;

      while (len--)
      {
        if (transp != *ptr)
        {
          if (move) { move = false; setAddrWindow(px, y, xe, ye);}
          uint8_t color = *ptr;

          // Shifts are slow so check if colour has changed first
          if (color != _lastColor) {
            //          =====Green=====     ===============Red==============
            msbColor = (color & 0x1C)>>2 | (color & 0xC0)>>3 | (color & 0xE0);
            //          =====Green=====    =======Blue======
            lsbColor = (color & 0x1C)<<3 | blue[color & 0x03];
            _lastColor = color;
          }
          *linePtr++ = msbColor;
          *linePtr++ = lsbColor;
          np++;
        }
        else
        {
          move = true;
          if (np)
          {
            pushColors(lineBuf, np, false);
            linePtr = (uint8_t*)lineBuf;
            np = 0;
          }
        }
        px++;
        ptr++;
      }

      if (np) pushColors(lineBuf, np, false);

      y++;
      data += w;
    }
  }
  else
  {
    w =  (w+7) & 0xFFF8;
    while (dh--)
    {
      int32_t px = x;
      boolean move = true;
      uint16_t np = 0;
      int32_t len = dw;
      uint8_t* ptr = data;
      uint8_t  bits = 8;
      while(len>0)
      {
        if (len < 8) bits = len;
        uint32_t xp = dx;
        uint32_t yp = (dy * w)>>3;
        for (uint16_t i = 0; i < bits; i++)
        {
          //uint8_t col = (ptr[(xp + dy * w)>>3] << (xp & 0x7)) & 0x80;
          if ((ptr[(xp>>3) + yp] << (xp & 0x7)) & 0x80)
          {
            if (move)
            {
              move = false;
              setAddrWindow(px, y, xe, ye);
            }
            np++;
          }
          else
          {
            if (np)
            {
              pushColor(bitmap_fg, np);
              np = 0;
              move = true;
            }
          }
          px++;
          xp++;
        }
        *ptr++;
        len -= 8;
      }
      if (np) pushColor(bitmap_fg, np);
      y++;
      dy++;
    }
  }

  CS_H;

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           setSwapBytes
** Description:             Used by 16 bit pushImage() to swap byte order in colours
***************************************************************************************/
void TFT_eSPI::setSwapBytes(bool swap)
{
  _swapBytes = swap;
}


/***************************************************************************************
** Function name:           getSwapBytes
** Description:             Return the swap byte order for colours
***************************************************************************************/
bool TFT_eSPI::getSwapBytes(void)
{
  return _swapBytes;
}

/***************************************************************************************
** Function name:           read rectangle (for SPI Interface II i.e. IM [3:0] = "1101")
** Description:             Read RGB pixel colours from a defined area
***************************************************************************************/
// If w and h are 1, then 1 pixel is read, *data array size must be 3 bytes per pixel
void  TFT_eSPI::readRectRGB(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t *data)
{
#if !defined(ESP32_PARALLEL)
  spi_begin();

  readAddrWindow(x0, y0, x0 + w - 1, y0 + h - 1); // Sets CS low

  // Dummy read to throw away don't care value
  tft_Write_8(0);
  
  // Read window pixel 24 bit RGB values, buffer must be set in sketch to 3 * w * h
  uint32_t len = w * h;
  while (len--) {
    // Read the 3 RGB bytes, colour is actually only in the top 6 bits of each byte
    // as the TFT stores colours as 18 bits
    *data++ = tft_Write_8(0);
    *data++ = tft_Write_8(0);
    *data++ = tft_Write_8(0);
  }
  CS_H;

  spi_end();
#endif
}


/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle outline
***************************************************************************************/
// Optimised midpoint circle algorithm
void TFT_eSPI::drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color)
{
  int32_t  x  = 0;
  int32_t  dx = 1;
  int32_t  dy = r+r;
  int32_t  p  = -(r>>1);

  spi_begin();
  inTransaction = true;

  // These are ordered to minimise coordinate changes in x or y
  // drawPixel can then send fewer bounding box commands
  drawPixel(x0 + r, y0, color);
  drawPixel(x0 - r, y0, color);
  drawPixel(x0, y0 - r, color);
  drawPixel(x0, y0 + r, color);

  while(x<r){

    if(p>=0) {
      dy-=2;
      p-=dy;
      r--;
    }

    dx+=2;
    p+=dx;

    x++;

    // These are ordered to minimise coordinate changes in x or y
    // drawPixel can then send fewer bounding box commands
    drawPixel(x0 + x, y0 + r, color);
    drawPixel(x0 - x, y0 + r, color);
    drawPixel(x0 - x, y0 - r, color);
    drawPixel(x0 + x, y0 - r, color);

    drawPixel(x0 + r, y0 + x, color);
    drawPixel(x0 - r, y0 + x, color);
    drawPixel(x0 - r, y0 - x, color);
    drawPixel(x0 + r, y0 - x, color);
  }

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           drawCircleHelper
** Description:             Support function for circle drawing
***************************************************************************************/
void TFT_eSPI::drawCircleHelper( int32_t x0, int32_t y0, int32_t r, uint8_t cornername, uint32_t color)
{
  int32_t f     = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -2 * r;
  int32_t x     = 0;

  while (x < r) {
    if (f >= 0) {
      r--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + r, color);
      drawPixel(x0 + r, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - r, color);
      drawPixel(x0 + r, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - r, y0 + x, color);
      drawPixel(x0 - x, y0 + r, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - r, y0 - x, color);
      drawPixel(x0 - x, y0 - r, color);
    }
  }
}


/***************************************************************************************
** Function name:           fillCircle
** Description:             draw a filled circle
***************************************************************************************/
// Optimised midpoint circle algorithm, changed to horizontal lines (faster in sprites)
void TFT_eSPI::fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color)
{
  int32_t  x  = 0;
  int32_t  dx = 1;
  int32_t  dy = r+r;
  int32_t  p  = -(r>>1);

  spi_begin();
  inTransaction = true;

  drawFastHLine(x0 - r, y0, dy+1, color);

  while(x<r){

    if(p>=0) {
      dy-=2;
      p-=dy;
      r--;
    }

    dx+=2;
    p+=dx;

    x++;

    drawFastHLine(x0 - r, y0 + x, 2 * r+1, color);
    drawFastHLine(x0 - r, y0 - x, 2 * r+1, color);
    drawFastHLine(x0 - x, y0 + r, 2 * x+1, color);
    drawFastHLine(x0 - x, y0 - r, 2 * x+1, color);

  }

  inTransaction = false;  
  spi_end();
}


/***************************************************************************************
** Function name:           fillCircleHelper
** Description:             Support function for filled circle drawing
***************************************************************************************/
// Used to support drawing roundrects, changed to horizontal lines (faster in sprites)
void TFT_eSPI::fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint32_t color)
{
  int32_t f     = 1 - r;
  int32_t ddF_x = 1;
  int32_t ddF_y = -r - r;
  int32_t y     = 0;

  delta++;
  while (y < r) {
    if (f >= 0) {
      r--;
      ddF_y += 2;
      f     += ddF_y;
    }
    y++;
    //x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1)
    {
      drawFastHLine(x0 - r, y0 + y, r + r + delta, color);
      drawFastHLine(x0 - y, y0 + r, y + y + delta, color);
    }
    if (cornername & 0x2) {
      drawFastHLine(x0 - r, y0 - y, r + r + delta, color); // 11995, 1090
      drawFastHLine(x0 - y, y0 - r, y + y + delta, color);
    }
  }
}


/***************************************************************************************
** Function name:           drawEllipse
** Description:             Draw a ellipse outline
***************************************************************************************/
void TFT_eSPI::drawEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
{
  if (rx<2) return;
  if (ry<2) return;
  int32_t x, y;
  int32_t rx2 = rx * rx;
  int32_t ry2 = ry * ry;
  int32_t fx2 = 4 * rx2;
  int32_t fy2 = 4 * ry2;
  int32_t s;

  spi_begin();
  inTransaction = true;

  for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++)
  {
    // These are ordered to minimise coordinate changes in x or y
    // drawPixel can then send fewer bounding box commands
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + x, y0 - y, color);
    if (s >= 0)
    {
      s += fx2 * (1 - y);
      y--;
    }
    s += ry2 * ((4 * x) + 6);
  }

  for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++)
  {
    // These are ordered to minimise coordinate changes in x or y
    // drawPixel can then send fewer bounding box commands
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + x, y0 - y, color);
    if (s >= 0)
    {
      s += fy2 * (1 - x);
      x--;
    }
    s += rx2 * ((4 * y) + 6);
  }

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           fillEllipse
** Description:             draw a filled ellipse
***************************************************************************************/
void TFT_eSPI::fillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
{
  if (rx<2) return;
  if (ry<2) return;
  int32_t x, y;
  int32_t rx2 = rx * rx;
  int32_t ry2 = ry * ry;
  int32_t fx2 = 4 * rx2;
  int32_t fy2 = 4 * ry2;
  int32_t s;

  spi_begin();
  inTransaction = true;

  for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++)
  {
    drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
    drawFastHLine(x0 - x, y0 + y, x + x + 1, color);

    if (s >= 0)
    {
      s += fx2 * (1 - y);
      y--;
    }
    s += ry2 * ((4 * x) + 6);
  }

  for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++)
  {
    drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
    drawFastHLine(x0 - x, y0 + y, x + x + 1, color);

    if (s >= 0)
    {
      s += fy2 * (1 - x);
      x--;
    }
    s += rx2 * ((4 * y) + 6);
  }

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           fillScreen
** Description:             Clear the screen to defined colour
***************************************************************************************/
void TFT_eSPI::fillScreen(uint32_t color)
{
  fillRect(0, 0, _width, _height, color);
}


/***************************************************************************************
** Function name:           drawRect
** Description:             Draw a rectangle outline
***************************************************************************************/
// Draw a rectangle
void TFT_eSPI::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
  spi_begin();
  inTransaction = true;

  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           drawRoundRect
** Description:             Draw a rounded corner rectangle outline
***************************************************************************************/
// Draw a rounded rectangle
void TFT_eSPI::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color)
{
  spi_begin();
  inTransaction = true;

  // smarter version
  drawFastHLine(x + r  , y    , w - r - r, color); // Top
  drawFastHLine(x + r  , y + h - 1, w - r - r, color); // Bottom
  drawFastVLine(x    , y + r  , h - r - r, color); // Left
  drawFastVLine(x + w - 1, y + r  , h - r - r, color); // Right
  // draw four corners
  drawCircleHelper(x + r    , y + r    , r, 1, color);
  drawCircleHelper(x + w - r - 1, y + r    , r, 2, color);
  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  drawCircleHelper(x + r    , y + h - r - 1, r, 8, color);
  
  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
// Fill a rounded rectangle, changed to horizontal lines (faster in sprites)
void TFT_eSPI::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color)
{
  spi_begin();
  inTransaction = true;

  // smarter version
  fillRect(x, y + r, w, h - r - r, color);

  // draw four corners
  fillCircleHelper(x + r, y + h - r - 1, r, 1, w - r - r - 1, color);
  fillCircleHelper(x + r    , y + r, r, 2, w - r - r - 1, color);
  
  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           drawTriangle
** Description:             Draw a triangle outline using 3 arbitrary points
***************************************************************************************/
// Draw a triangle
void TFT_eSPI::drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
  spi_begin();
  inTransaction = true;

  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           fillTriangle
** Description:             Draw a filled triangle using 3 arbitrary points
***************************************************************************************/
// Fill a triangle - original Adafruit function works well and code footprint is small
void TFT_eSPI::fillTriangle ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
  int32_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    swap_coord(y0, y1); swap_coord(x0, x1);
  }
  if (y1 > y2) {
    swap_coord(y2, y1); swap_coord(x2, x1);
  }
  if (y0 > y1) {
    swap_coord(y0, y1); swap_coord(x0, x1);
  }

  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)      a = x1;
    else if (x1 > b) b = x1;
    if (x2 < a)      a = x2;
    else if (x2 > b) b = x2;
    drawFastHLine(a, y0, b - a + 1, color);
    return;
  }

  spi_begin();
  inTransaction = true;

  int32_t
  dx01 = x1 - x0,
  dy01 = y1 - y0,
  dx02 = x2 - x0,
  dy02 = y2 - y0,
  dx12 = x2 - x1,
  dy12 = y2 - y1,
  sa   = 0,
  sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2) last = y1;  // Include y1 scanline
  else         last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if (a > b) swap_coord(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for (; y <= y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if (a > b) swap_coord(a, b);
    drawFastHLine(a, y, b - a + 1, color);
  }

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           drawBitmap
** Description:             Draw an image stored in an array on the TFT
***************************************************************************************/
void TFT_eSPI::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
  spi_begin();
  inTransaction = true;

  int32_t i, j, byteWidth = (w + 7) / 8;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++ ) {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        drawPixel(x + i, y + j, color);
      }
    }
  }

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           drawXBitmap
** Description:             Draw an image stored in an XBM array onto the TFT
***************************************************************************************/
void TFT_eSPI::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
  spi_begin();
  inTransaction = true;

  int32_t i, j, byteWidth = (w + 7) / 8;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++ ) {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i & 7))) {
        drawPixel(x + i, y + j, color);
      }
    }
  }

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           drawXBitmap
** Description:             Draw an XBM image with foreground and background colors
***************************************************************************************/
void TFT_eSPI::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bgcolor)
{
  spi_begin();
  inTransaction = true;

  int32_t i, j, byteWidth = (w + 7) / 8;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++ ) {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i & 7)))
           drawPixel(x + i, y + j,   color);
      else drawPixel(x + i, y + j, bgcolor);
    }
  }

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position
***************************************************************************************/
void TFT_eSPI::setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}


/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position and font
***************************************************************************************/
void TFT_eSPI::setCursor(int16_t x, int16_t y, uint8_t font)
{
  textfont = font;
  cursor_x = x;
  cursor_y = y;
}


/***************************************************************************************
** Function name:           getCursorX
** Description:             Get the text cursor x position
***************************************************************************************/
int16_t TFT_eSPI::getCursorX(void)
{
  return cursor_x;
}

/***************************************************************************************
** Function name:           getCursorY
** Description:             Get the text cursor y position
***************************************************************************************/
int16_t TFT_eSPI::getCursorY(void)
{
  return cursor_y;
}


/***************************************************************************************
** Function name:           setTextSize
** Description:             Set the text size multiplier
***************************************************************************************/
void TFT_eSPI::setTextSize(uint8_t s)
{
  if (s>7) s = 7; // Limit the maximum size multiplier so byte variables can be used for rendering
  textsize = (s > 0) ? s : 1; // Don't allow font size 0
}


/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground colour (background is transparent)
***************************************************************************************/
void TFT_eSPI::setTextColor(uint16_t c)
{
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}


/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground and background colour
***************************************************************************************/
void TFT_eSPI::setTextColor(uint16_t c, uint16_t b)
{
  textcolor   = c;
  textbgcolor = b;
}


/***************************************************************************************
** Function name:           setBitmapColor
** Description:             Set the foreground foreground and background colour
***************************************************************************************/
void TFT_eSPI::setBitmapColor(uint16_t c, uint16_t b)
{
  if (c == b) b = ~c;
  bitmap_fg = c;
  bitmap_bg = b;
}


/***************************************************************************************
** Function name:           setTextWrap
** Description:             Define if text should wrap at end of line
***************************************************************************************/
void TFT_eSPI::setTextWrap(boolean wrapX, boolean wrapY)
{
  textwrapX = wrapX;
  textwrapY = wrapY;
}


/***************************************************************************************
** Function name:           setTextDatum
** Description:             Set the text position reference datum
***************************************************************************************/
void TFT_eSPI::setTextDatum(uint8_t d)
{
  textdatum = d;
}


/***************************************************************************************
** Function name:           setTextPadding
** Description:             Define padding width (aids erasing old text and numbers)
***************************************************************************************/
void TFT_eSPI::setTextPadding(uint16_t x_width)
{
  padX = x_width;
}


/***************************************************************************************
** Function name:           getRotation
** Description:             Return the rotation value (as used by setRotation())
***************************************************************************************/
uint8_t TFT_eSPI::getRotation(void)
{
  return rotation;
}

/***************************************************************************************
** Function name:           getTextDatum
** Description:             Return the text datum value (as used by setTextDatum())
***************************************************************************************/
uint8_t TFT_eSPI::getTextDatum(void)
{
  return textdatum;
}


/***************************************************************************************
** Function name:           width
** Description:             Return the pixel width of display (per current rotation)
***************************************************************************************/
// Return the size of the display (per current rotation)
int16_t TFT_eSPI::width(void)
{
  return _width;
}


/***************************************************************************************
** Function name:           height
** Description:             Return the pixel height of display (per current rotation)
***************************************************************************************/
int16_t TFT_eSPI::height(void)
{
  return _height;
}


/***************************************************************************************
** Function name:           textWidth
** Description:             Return the width in pixels of a string in a given font
***************************************************************************************/
int16_t TFT_eSPI::textWidth(const String& string)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return textWidth(buffer, textfont);
}

int16_t TFT_eSPI::textWidth(const String& string, int font)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return textWidth(buffer, font);
}

int16_t TFT_eSPI::textWidth(const char *string)
{
  return textWidth(string, textfont);
}

int16_t TFT_eSPI::textWidth(const char *string, int font)
{
  int str_width  = 0;

#ifdef SMOOTH_FONT
  if(fontLoaded)
  {
    while (*string)
    {
      uint16_t unicode = decodeUTF8(*string++);
      if (unicode)
      {
        if (unicode == 0x20) str_width += gFont.spaceWidth;
        else
        {
          uint16_t gNum = 0;
          bool found = getUnicodeIndex(unicode, &gNum);
          if (found)
          {
            if(str_width == 0 && gdX[gNum] < 0) str_width -= gdX[gNum];
            if (*string) str_width += gxAdvance[gNum];
            else str_width += (gdX[gNum] + gWidth[gNum]);
          }
          else str_width += gFont.spaceWidth + 1;
        }
      }
    }
    return str_width;
  }
#endif

  unsigned char uniCode;
  char *widthtable;

  if (font>1 && font<9)
  {
    widthtable = (char *)pgm_read_dword( &(fontdata[font].widthtbl ) ) - 32; //subtract the 32 outside the loop

    while (*string)
    {
      uniCode = *(string++);
      if (uniCode > 31 && uniCode < 128)
      str_width += pgm_read_byte( widthtable + uniCode); // Normally we need to subract 32 from uniCode
      else str_width += pgm_read_byte( widthtable + 32); // Set illegal character = space width
    }
  }
  else
  {

#ifdef LOAD_GFXFF
    if(gfxFont) // New font
    {
      while (*string)
      {
        uniCode = *(string++);
        if ((uniCode >= (uint8_t)pgm_read_byte(&gfxFont->first)) && (uniCode <= (uint8_t)pgm_read_byte(&gfxFont->last )))
        {
          uniCode -= pgm_read_byte(&gfxFont->first);
          GFXglyph *glyph  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[uniCode]);
          // If this is not the  last character then use xAdvance
          if (*string) str_width += pgm_read_byte(&glyph->xAdvance);
          // Else use the offset plus width since this can be bigger than xAdvance
          else str_width += ((int8_t)pgm_read_byte(&glyph->xOffset) + pgm_read_byte(&glyph->width));
        }
      }
    }
    else
#endif
    {
#ifdef LOAD_GLCD
      while (*string++) str_width += 6;
#endif
    }
  }
  return str_width * textsize;
}


/***************************************************************************************
** Function name:           fontsLoaded
** Description:             return an encoded 16 bit value showing the fonts loaded
***************************************************************************************/
// Returns a value showing which fonts are loaded (bit N set =  Font N loaded)

uint16_t TFT_eSPI::fontsLoaded(void)
{
  return fontsloaded;
}


/***************************************************************************************
** Function name:           fontHeight
** Description:             return the height of a font (yAdvance for free fonts)
***************************************************************************************/
int16_t TFT_eSPI::fontHeight(int16_t font)
{
#ifdef SMOOTH_FONT
  if(fontLoaded) return gFont.yAdvance;
#endif

#ifdef LOAD_GFXFF
  if (font==1)
  {
    if(gfxFont) // New font
    {
      return pgm_read_byte(&gfxFont->yAdvance) * textsize;
    }
  }
#endif
  return pgm_read_byte( &fontdata[font].height ) * textsize;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a single character in the Adafruit GLCD font
***************************************************************************************/
void TFT_eSPI::drawChar(int32_t x, int32_t y, unsigned char c, uint32_t color, uint32_t bg, uint8_t size)
{
  if ((x >= (int16_t)_width)            || // Clip right
      (y >= (int16_t)_height)           || // Clip bottom
      ((x + 6 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
    return;

  if (c < 32) return;
#ifdef LOAD_GLCD
//>>>>>>>>>>>>>>>>>>
#ifdef LOAD_GFXFF
  if(!gfxFont) { // 'Classic' built-in font
#endif
//>>>>>>>>>>>>>>>>>>

  boolean fillbg = (bg != color);

  if ((size==1) && fillbg)
  {
    uint8_t column[6];
    uint8_t mask = 0x1;
    spi_begin();
    //inTransaction = true;
    setAddrWindow(x, y, x+5, y+8);
    for (int8_t i = 0; i < 5; i++ ) column[i] = pgm_read_byte(font + (c * 5) + i);
    column[5] = 0;

#if defined (ESP8266)
    color = (color >> 8) | (color << 8);
    bg = (bg >> 8) | (bg << 8);
    uint32_t spimask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
    SPI1U1 = (SPI1U1 & spimask) | (15 << SPILMOSI) | (15 << SPILMISO);
    for (int8_t j = 0; j < 8; j++) {
      for (int8_t k = 0; k < 5; k++ ) {
        if (column[k] & mask) {
          SPI1W0 = color;
        }
        else {
          SPI1W0 = bg;
        }
        SPI1CMD |= SPIBUSY;
        while(SPI1CMD & SPIBUSY) {}
      }

      mask <<= 1;

      SPI1W0 = bg;
      SPI1CMD |= SPIBUSY;
      while(SPI1CMD & SPIBUSY) {}
    }
#else // for ESP32

    for (int8_t j = 0; j < 8; j++) {
      for (int8_t k = 0; k < 5; k++ ) {
        if (column[k] & mask) {tft_Write_16(color);}
        else {tft_Write_16(bg);}
      }
      mask <<= 1;
      tft_Write_16(bg);
    }

#endif
    CS_H;
    //inTransaction = false;
    spi_end();
  }
  else
  {
    spi_begin();
    inTransaction = true;
    for (int8_t i = 0; i < 6; i++ ) {
      uint8_t line;
      if (i == 5)
        line = 0x0;
      else
        line = pgm_read_byte(font + (c * 5) + i);

      if (size == 1) // default size
      {
        for (int8_t j = 0; j < 8; j++) {
          if (line & 0x1) drawPixel(x + i, y + j, color);
          line >>= 1;
        }
      }
      else {  // big size
        for (int8_t j = 0; j < 8; j++) {
          if (line & 0x1) fillRect(x + (i * size), y + (j * size), size, size, color);
          else if (fillbg) fillRect(x + i * size, y + j * size, size, size, bg);
          line >>= 1;
        }
      }
    }
    inTransaction = false;
    spi_end();
  }

//>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LOAD_GFXFF
  } else { // Custom font
#endif
//>>>>>>>>>>>>>>>>>>>>>>>>>>>
#endif // LOAD_GLCD

#ifdef LOAD_GFXFF
    // Filter out bad characters not present in font
    if ((c >= (uint8_t)pgm_read_byte(&gfxFont->first)) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last )))
    {
      spi_begin();
      inTransaction = true;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>

      c -= pgm_read_byte(&gfxFont->first);
      GFXglyph *glyph  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c]);
      uint8_t  *bitmap = (uint8_t *)pgm_read_dword(&gfxFont->bitmap);

      uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
      uint8_t  w  = pgm_read_byte(&glyph->width),
               h  = pgm_read_byte(&glyph->height),
               xa = pgm_read_byte(&glyph->xAdvance);
      int8_t   xo = pgm_read_byte(&glyph->xOffset),
               yo = pgm_read_byte(&glyph->yOffset);
      uint8_t  xx, yy, bits, bit=0;
      int16_t  xo16 = 0, yo16 = 0;
  
      if(size > 1) {
        xo16 = xo;
        yo16 = yo;
      }

// Here we have 3 versions of the same function just for evaluation purposes
// Comment out the next two #defines to revert to the slower Adafruit implementation

// If FAST_LINE is defined then the free fonts are rendered using horizontal lines
// this makes rendering fonts 2-5 times faster. Particularly good for large fonts.
// This is an elegant solution since it still uses generic functions present in the
// stock library.

// If FAST_SHIFT is defined then a slightly faster (at least for AVR processors)
// shifting bit mask is used

// Free fonts don't look good when the size multiplier is >1 so we could remove
// code if this is not wanted and speed things up

#define FAST_HLINE
#define FAST_SHIFT
//FIXED_SIZE is an option in User_Setup.h that only works with FAST_LINE enabled

#ifdef FIXED_SIZE
      x+=xo; // Save 88 bytes of FLASH
      y+=yo;
#endif

#ifdef FAST_HLINE

  #ifdef FAST_SHIFT
      uint16_t hpc = 0; // Horizontal foreground pixel count
      for(yy=0; yy<h; yy++) {
        for(xx=0; xx<w; xx++) {
          if(bit == 0) {
            bits = pgm_read_byte(&bitmap[bo++]);
            bit  = 0x80;
          }
          if(bits & bit) hpc++;
          else {
           if (hpc) {
#ifndef FIXED_SIZE
              if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
              else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
#else
              drawFastHLine(x+xx-hpc, y+yy, hpc, color);
#endif
              hpc=0;
            }
          }
          bit >>= 1;
        }
      // Draw pixels for this line as we are about to increment yy
        if (hpc) {
#ifndef FIXED_SIZE
          if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
          else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
#else
          drawFastHLine(x+xx-hpc, y+yy, hpc, color);
#endif
          hpc=0;
        }
      }
  #else
      uint16_t hpc = 0; // Horizontal foreground pixel count
      for(yy=0; yy<h; yy++) {
        for(xx=0; xx<w; xx++) {
          if(!(bit++ & 7)) {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          if(bits & 0x80) hpc++;
          else {
            if (hpc) {
              if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
              else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
              hpc=0;
            }
          }
          bits <<= 1;
        }
        // Draw pixels for this line as we are about to increment yy
        if (hpc) {
          if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
          else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
          hpc=0;
        }
      }
  #endif

#else
      for(yy=0; yy<h; yy++) {
        for(xx=0; xx<w; xx++) {
          if(!(bit++ & 7)) {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          if(bits & 0x80) {
            if(size == 1) {
              drawPixel(x+xo+xx, y+yo+yy, color);
            } else {
              fillRect(x+(xo16+xx)*size, y+(yo16+yy)*size, size, size, color);
            }
          }
          bits <<= 1;
        }
      }
#endif
      inTransaction = false;
      spi_end();
    }
#endif


#ifdef LOAD_GLCD
  #ifdef LOAD_GFXFF
  } // End classic vs custom font
  #endif
#endif

}


/***************************************************************************************
** Function name:           setWindow
** Description:             define an area to receive a stream of pixels
***************************************************************************************/
// Chip select is high at the end of this function
void TFT_eSPI::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  spi_begin();
  setAddrWindow(x0, y0, x1, y1);
  CS_H;
  spi_end();
}


/***************************************************************************************
** Function name:           setAddrWindow
** Description:             define an area to receive a stream of pixels
***************************************************************************************/
// Chip select stays low, use setWindow() from sketches

#if defined (ESP8266) && !defined (RPI_WRITE_STROBE) && !defined (RPI_ILI9486_DRIVER)
inline void TFT_eSPI::setAddrWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye)
{
  //spi_begin();

#ifdef CGRAM_OFFSET
  xs+=colstart;
  xe+=colstart;
  ys+=rowstart;
  ye+=rowstart;
#endif

  // Column addr set
  DC_C;
  CS_L;

  uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  mask = SPI1U1 & mask;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

  SPI1W0 = TFT_CASET;
  SPI1CMD |= SPIBUSY;

  addr_col = 0xFFFF;
  addr_row = 0xFFFF;
  
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  SPI1U1 = mask | (31 << SPILMOSI) | (31 << SPILMISO);
  // Load the two coords as a 32 bit value and shift in one go
  SPI1W0 = (xs >> 8) | (uint16_t)(xs << 8) | ((uint8_t)(xe >> 8)<<16 | (xe << 24));
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  // Row addr set
  DC_C;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

  SPI1W0 = TFT_PASET;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  SPI1U1 = mask | (31 << SPILMOSI) | (31 << SPILMISO);
  // Load the two coords as a 32 bit value and shift in one go
  SPI1W0 = (ys >> 8) | (uint16_t)(ys << 8) | ((uint8_t)(ye >> 8)<<16 | (ye << 24));
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  // write to RAM
  DC_C;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
  SPI1W0 = TFT_RAMWR;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  //spi_end();
}

#elif defined (ESP8266) && !defined (RPI_WRITE_STROBE) && defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits

void TFT_eSPI::setAddrWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye)
{
  //spi_begin();

  addr_col = 0xFFFF;
  addr_row = 0xFFFF;
  
  // Column addr set
  DC_C;
  CS_L;

  uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  mask = SPI1U1 & mask;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

  SPI1W0 = TFT_CASET<<8;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  uint8_t xBin[] = { 0, (uint8_t) (xs>>8), 0, (uint8_t) (xs>>0), 0, (uint8_t) (xe>>8), 0, (uint8_t) (xe>>0), };
  SPI.writePattern(&xBin[0], 8, 1);

  // Row addr set
  DC_C;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

  SPI1W0 = TFT_PASET<<8;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  uint8_t yBin[] = { 0, (uint8_t) (ys>>8), 0, (uint8_t) (ys>>0), 0, (uint8_t) (ye>>8), 0, (uint8_t) (ye>>0), };
  SPI.writePattern(&yBin[0], 8, 1);

  // write to RAM
  DC_C;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
  SPI1W0 = TFT_RAMWR<<8;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  //spi_end();
}

#else

#if defined (ESP8266) && defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits
inline void TFT_eSPI::setAddrWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
  //spi_begin();

  CS_L;
  uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  mask = SPI1U1 & mask;
  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

  // Column addr set
  DC_C;

  SPI1W0 = TFT_CASET<<(CMD_BITS + 1 - 8);
  SPI1CMD |= SPIBUSY;
  addr_col = 0xFFFF; // Use the waiting time to do something useful
  addr_row = 0xFFFF;
  while(SPI1CMD & SPIBUSY) {}
  DC_D;

  SPI1W0 = x0 >> 0;
  SPI1CMD |= SPIBUSY;
  x0 = x0 << 8; // Use the waiting time to do something useful
  while(SPI1CMD & SPIBUSY) {}

  SPI1W0 = x0;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  SPI1W0 = x1 >> 0;
  SPI1CMD |= SPIBUSY;
  x1 = x1 << 8; // Use the waiting time to do something useful
  while(SPI1CMD & SPIBUSY) {}

  SPI1W0 = x1;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}
  
  // Row addr set
  DC_C;

  SPI1W0 = TFT_PASET<<(CMD_BITS + 1 - 8);
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}
  DC_D;

  SPI1W0 = y0 >> 0;
  SPI1CMD |= SPIBUSY;
  y0 = y0 << 8; // Use the waiting time to do something useful
  while(SPI1CMD & SPIBUSY) {}

  SPI1W0 = y0;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  SPI1W0 = y1 >> 0;
  SPI1CMD |= SPIBUSY;
  y1 = y1 << 8; // Use the waiting time to do something useful
  while(SPI1CMD & SPIBUSY) {}

  SPI1W0 = y1;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}
  
  // write to RAM
  DC_C;

  SPI1W0 = TFT_RAMWR<<(CMD_BITS + 1 - 8);
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  //spi_end();
}

#else // This is for the ESP32

inline void TFT_eSPI::setAddrWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
  //spi_begin();

  addr_col = 0xFFFF;
  addr_row = 0xFFFF;

#ifdef CGRAM_OFFSET
  x0+=colstart;
  x1+=colstart;
  y0+=rowstart;
  y1+=rowstart;
#endif

#if !defined (RPI_ILI9486_DRIVER)
  uint32_t xaw = ((uint32_t)x0 << 16) | x1;
  uint32_t yaw = ((uint32_t)y0 << 16) | y1;
#endif

  // Column addr set
  DC_C;
  CS_L;

  tft_Write_8(TFT_CASET);

  DC_D;

#if defined (RPI_ILI9486_DRIVER)
  uint8_t xBin[] = { 0, (uint8_t) (x0>>8), 0, (uint8_t) (x0>>0), 0, (uint8_t) (x1>>8), 0, (uint8_t) (x1>>0), };
  SPI.writePattern(&xBin[0], 8, 1);
#else
  tft_Write_32(xaw);
#endif

  // Row addr set
  DC_C;
  tft_Write_8(TFT_PASET);

  DC_D;

#if defined (RPI_ILI9486_DRIVER)
  uint8_t yBin[] = { 0, (uint8_t) (y0>>8), 0, (uint8_t) (y0>>0), 0, (uint8_t) (y1>>8), 0, (uint8_t) (y1>>0), };
  SPI.writePattern(&yBin[0], 8, 1);
#else
  tft_Write_32(yaw);
#endif

  // write to RAM
  DC_C;

  tft_Write_8(TFT_RAMWR);

  DC_D;

  //spi_end();
}
#endif // end RPI_ILI9486_DRIVER check
#endif // end ESP32 check


/***************************************************************************************
** Function name:           readAddrWindow
** Description:             define an area to read a stream of pixels
***************************************************************************************/
// Chip select stays low
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
void TFT_eSPI::readAddrWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye)
{
  //spi_begin();

  addr_col = 0xFFFF;
  addr_row = 0xFFFF;
  
#ifdef CGRAM_OFFSET
  xs+=colstart;
  xe+=colstart;
  ys+=rowstart;
  ye+=rowstart;
#endif

  // Column addr set
  DC_C;
  CS_L;

  uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  mask = SPI1U1 & mask;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

  SPI1W0 = TFT_CASET;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  SPI1U1 = mask | (31 << SPILMOSI) | (31 << SPILMISO);
  // Load the two coords as a 32 bit value and shift in one go
  SPI1W0 = (xs >> 8) | (uint16_t)(xs << 8) | ((uint8_t)(xe >> 8)<<16 | (xe << 24));
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  // Row addr set
  DC_C;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

  SPI1W0 = TFT_PASET;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  SPI1U1 = mask | (31 << SPILMOSI) | (31 << SPILMISO);
  // Load the two coords as a 32 bit value and shift in one go
  SPI1W0 = (ys >> 8) | (uint16_t)(ys << 8) | ((uint8_t)(ye >> 8)<<16 | (ye << 24));
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  // read from RAM
  DC_C;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
  SPI1W0 = TFT_RAMRD;
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;
  //spi_end();
}

#else //ESP32

void TFT_eSPI::readAddrWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
  //spi_begin();

  addr_col = 0xFFFF;
  addr_row = 0xFFFF;

#ifdef CGRAM_OFFSET
  x0+=colstart;
  x1+=colstart;
  y0+=rowstart;
  y1+=rowstart;
#endif

  uint32_t xaw = ((uint32_t)x0 << 16) | x1;
  uint32_t yaw = ((uint32_t)y0 << 16) | y1;
  
  // Column addr set
  DC_C;
  CS_L;

  tft_Write_8(TFT_CASET);


  DC_D;

  tft_Write_32(xaw);

  // Row addr set
  DC_C;

  tft_Write_8(TFT_PASET);

  DC_D;

  tft_Write_32(yaw);
  
  DC_C;
  tft_Write_8(TFT_RAMRD); // Read CGRAM command

  DC_D;

  //spi_end();
}

#endif

/***************************************************************************************
** Function name:           drawPixel
** Description:             push a single pixel at an arbitrary position
***************************************************************************************/
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
void TFT_eSPI::drawPixel(uint32_t x, uint32_t y, uint32_t color)
{
  // Faster range checking, possible because x and y are unsigned
  if ((x >= _width) || (y >= _height)) return;
  
#ifdef CGRAM_OFFSET
  x+=colstart;
  y+=rowstart;
#endif

  spi_begin();

  CS_L;

  uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  mask = SPI1U1 & mask;
  // No need to send x if it has not changed (speeds things up)
  if (addr_col != x) {

    DC_C;

    SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
    SPI1W0 = TFT_CASET<<(CMD_BITS + 1 - 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    DC_D;

#if defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits per byte
    uint8_t cBin[] = { 0, (uint8_t) (x>>8), 0, (uint8_t) (x>>0)};
    SPI.writePattern(&cBin[0], 4, 2);
#else
    SPI1U1 = mask | (31 << SPILMOSI) | (31 << SPILMISO);
    // Load the two coords as a 32 bit value and shift in one go
    uint32_t xswap = (x >> 8) | (uint16_t)(x << 8);
    SPI1W0 = xswap | (xswap << 16);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
#endif

    addr_col = x;
  }

  // No need to send y if it has not changed (speeds things up)
  if (addr_row != y) {

    DC_C;

    SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

    SPI1W0 = TFT_PASET<<(CMD_BITS + 1 - 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    DC_D;

#if defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits per byte
    uint8_t cBin[] = { 0, (uint8_t) (y>>8), 0, (uint8_t) (y>>0)};
    SPI.writePattern(&cBin[0], 4, 2);
#else
    SPI1U1 = mask | (31 << SPILMOSI) | (31 << SPILMISO);
    // Load the two coords as a 32 bit value and shift in one go
    uint32_t yswap = (y >> 8) | (uint16_t)(y << 8);
    SPI1W0 = yswap | (yswap << 16);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
#endif

    addr_row = y;
  }

  DC_C;

  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);

  SPI1W0 = TFT_RAMWR<<(CMD_BITS + 1 - 8);
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  SPI1U1 = mask | (15 << SPILMOSI) | (15 << SPILMISO);

  SPI1W0 = (color >> 8) | (color << 8);
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  CS_H;

  spi_end();
}

#else

#if defined (ESP8266) && defined (RPI_ILI9486_DRIVER) // This is for the RPi display that needs 16 bits

void TFT_eSPI::drawPixel(uint32_t x, uint32_t y, uint32_t color)
{
  // Faster range checking, possible because x and y are unsigned
  if ((x >= _width) || (y >= _height)) return;
  spi_begin();

  CS_L;
  uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  mask = SPI1U1 & mask;
  SPI1U1 = mask | (CMD_BITS << SPILMOSI) | (CMD_BITS << SPILMISO);
  // No need to send x if it has not changed (speeds things up)
  if (addr_col != x) {
    DC_C;

    SPI1W0 = TFT_CASET<<(CMD_BITS + 1 - 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
    DC_D;

    SPI1W0 = x >> 0;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    SPI1W0 = x << 8;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    SPI1W0 = x >> 0;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    SPI1W0 = x << 8;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
    
    addr_col = x;
  }

  // No need to send y if it has not changed (speeds things up)
  if (addr_row != y) {
    DC_C;

    SPI1W0 = TFT_PASET<<(CMD_BITS + 1 - 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
    DC_D;

    SPI1W0 = y >> 0;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    SPI1W0 = y << 8;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    SPI1W0 = y >> 0;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    SPI1W0 = y << 8;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
    
    addr_row = y;
  }

  DC_C;

  SPI1W0 = TFT_RAMWR<<(CMD_BITS + 1 - 8);
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  DC_D;

  SPI1W0 = (color >> 8) | (color << 8);
  SPI1CMD |= SPIBUSY;
  while(SPI1CMD & SPIBUSY) {}

  CS_H;

  spi_end();
}

#else // ESP32

void TFT_eSPI::drawPixel(uint32_t x, uint32_t y, uint32_t color)
{
  // Faster range checking, possible because x and y are unsigned
  if ((x >= _width) || (y >= _height)) return;
  spi_begin();

#ifdef CGRAM_OFFSET
  x+=colstart;
  y+=rowstart;
#endif

#if !defined (RPI_ILI9486_DRIVER)
  uint32_t xaw = ((uint32_t)x << 16) | x;
  uint32_t yaw = ((uint32_t)y << 16) | y;
#endif

  CS_L;

  // No need to send x if it has not changed (speeds things up)
  if (addr_col != x) {

    DC_C;

    tft_Write_8(TFT_CASET);

    DC_D;

#if defined (RPI_ILI9486_DRIVER)
    uint8_t xBin[] = { 0, (uint8_t) (x>>8), 0, (uint8_t) (x>>0), 0, (uint8_t) (x>>8), 0, (uint8_t) (x>>0), };
    SPI.writePattern(&xBin[0], 8, 1);
#else
    tft_Write_32(xaw);
#endif
    
    addr_col = x;
  }

  // No need to send y if it has not changed (speeds things up)
  if (addr_row != y) {

    DC_C;

    tft_Write_8(TFT_PASET);

    DC_D;

#if defined (RPI_ILI9486_DRIVER)
    uint8_t yBin[] = { 0, (uint8_t) (y>>8), 0, (uint8_t) (y>>0), 0, (uint8_t) (y>>8), 0, (uint8_t) (y>>0), };
    SPI.writePattern(&yBin[0], 8, 1);
#else
    tft_Write_32(yaw);
#endif

    addr_row = y;
  }

  DC_C;

  tft_Write_8(TFT_RAMWR);

  DC_D;

  tft_Write_16(color);

  CS_H;

  spi_end();
}
#endif
#endif


/***************************************************************************************
** Function name:           pushColor
** Description:             push a single pixel
***************************************************************************************/
void TFT_eSPI::pushColor(uint16_t color)
{
  spi_begin();

  CS_L;

  tft_Write_16(color);

  CS_H;

  spi_end();
}


/***************************************************************************************
** Function name:           pushColor
** Description:             push a single colour to "len" pixels
***************************************************************************************/
void TFT_eSPI::pushColor(uint16_t color, uint16_t len)
{
  spi_begin();

  CS_L;

#ifdef RPI_WRITE_STROBE
  uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
  if(len) SPI.writePattern(&colorBin[0], 2, 1); len--;
  while(len--) {WR_L; WR_H;}
#else
  #ifdef ESP32_PARALLEL
    while (len--) {tft_Write_16(color);}
  #else
    writeBlock(color, len);
  #endif
#endif

  CS_H;

  spi_end();
}

/***************************************************************************************
** Function name:           pushColors
** Description:             push an array of pixels for 16 bit raw image drawing
***************************************************************************************/
// Assumed that setWindow() has previously been called

void TFT_eSPI::pushColors(uint8_t *data, uint32_t len)
{
  spi_begin();

  CS_L;

#if defined (RPI_WRITE_STROBE)
  while ( len >=64 ) {SPI.writePattern(data, 64, 1); data += 64; len -= 64; }
  if (len) SPI.writePattern(data, len, 1);
#else
  #ifdef ESP32_PARALLEL
    while (len--) {tft_Write_8(*data); data++;}
  #else
    #if (SPI_FREQUENCY == 80000000)
      while ( len >=64 ) {SPI.writePattern(data, 64, 1); data += 64; len -= 64; }
      if (len) SPI.writePattern(data, len, 1);
    #else
      SPI.writeBytes(data, len);
    #endif
  #endif
#endif

  CS_H;

  spi_end();
}


/***************************************************************************************
** Function name:           pushColors
** Description:             push an array of pixels, for image drawing
***************************************************************************************/
void TFT_eSPI::pushColors(uint16_t *data, uint32_t len, bool swap)
{
  spi_begin();

  CS_L;

#if defined (ESP32)
  #ifdef ESP32_PARALLEL
    if (swap) while ( len-- ) {tft_Write_16(*data); data++;}
    else while ( len-- ) {tft_Write_16S(*data); data++;}
  #else
    if (swap) SPI.writePixels(data,len<<1);
    else SPI.writeBytes((uint8_t*)data,len<<1);
  #endif
#else

  uint32_t color[8];

  uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));

  SPI1U1 = (SPI1U1 & mask) | (255 << SPILMOSI) | (255 << SPILMISO);

  while(len>15)
  {

    if (swap)
    {
      uint32_t i = 0;
      while(i<8)
      {
        color[i]  = (*data >> 8) | (uint16_t)(*data << 8);
        data++;
        color[i] |= ((*data >> 8) | (*data << 8)) << 16;
        data++;
        i++;
      }
    }
    else
    {
      memcpy(color,data,32);
      data+=16;
    }

    len -= 16;

    // ESP8266 wait time here at 40MHz SPI is ~5.45us
    while(SPI1CMD & SPIBUSY) {}
    SPI1W0 = color[0];
    SPI1W1 = color[1];
    SPI1W2 = color[2];
    SPI1W3 = color[3];
    SPI1W4 = color[4];
    SPI1W5 = color[5];
    SPI1W6 = color[6];
    SPI1W7 = color[7];
    SPI1CMD |= SPIBUSY;
  }

  if(len)
  {
    uint32_t bits = (len*16-1); // bits left to shift - 1
    if (swap)
    {
      uint16_t* ptr = (uint16_t*)color;
      while(len--)
      {
        *ptr++ = (*(data) >> 8) | (uint16_t)(*(data) << 8);
        data++;
      }
    }
    else
    {
      memcpy(color,data,len<<1);
    }
    while(SPI1CMD & SPIBUSY) {}
    SPI1U1 = (SPI1U1 & mask) | (bits << SPILMOSI) | (bits << SPILMISO);
    SPI1W0 = color[0];
    SPI1W1 = color[1];
    SPI1W2 = color[2];
    SPI1W3 = color[3];
    SPI1W4 = color[4];
    SPI1W5 = color[5];
    SPI1W6 = color[6];
    SPI1W7 = color[7];
    SPI1CMD |= SPIBUSY;
  }

  while(SPI1CMD & SPIBUSY) {}

#endif

  CS_H;

  spi_end();
}


/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
***************************************************************************************/
// Bresenham's algorithm - thx wikipedia - speed enhanced by Bodmer to use
// an efficient FastH/V Line draw routine for line segments of 2 pixels or more

#if defined (RPI_ILI9486_DRIVER) || defined (ESP32) || defined (RPI_WRITE_STROBE) || defined (HX8357D_DRIVER)

void TFT_eSPI::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
  spi_begin();
  inTransaction = true;
  boolean steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap_coord(x0, y0);
    swap_coord(x1, y1);
  }

  if (x0 > x1) {
    swap_coord(x0, x1);
    swap_coord(y0, y1);
  }

  int32_t dx = x1 - x0, dy = abs(y1 - y0);;

  int32_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

  if (y0 < y1) ystep = 1;

  // Split into steep and not steep for FastH/V separation
  if (steep) {
    for (; x0 <= x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;
        if (dlen == 1) drawPixel(y0, xs, color);
        else drawFastVLine(y0, xs, dlen, color);
        dlen = 0; y0 += ystep; xs = x0 + 1;
      }
    }
    if (dlen) drawFastVLine(y0, xs, dlen, color);
  }
  else
  {
    for (; x0 <= x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;
        if (dlen == 1) drawPixel(xs, y0, color);
        else drawFastHLine(xs, y0, dlen, color);
        dlen = 0; y0 += ystep; xs = x0 + 1;
      }
    }
    if (dlen) drawFastHLine(xs, y0, dlen, color);
  }
  inTransaction = false;
  spi_end();
}

#else

// This is a weeny bit faster
void TFT_eSPI::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{

  boolean steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep) {
    swap_coord(x0, y0);
    swap_coord(x1, y1);
  }

  if (x0 > x1) {
    swap_coord(x0, x1);
    swap_coord(y0, y1);
  }

  if (x1 < 0) return;

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int8_t ystep = (y0 < y1) ? 1 : (-1);

  spi_begin();

  uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  mask = (SPI1U1 & mask) | (15 << SPILMOSI) | (15 << SPILMISO);
  SPI1U = SPIUMOSI | SPIUSSE;
  int16_t swapped_color = (color >> 8) | (color << 8);

  if (steep)  // y increments every iteration (y0 is x-axis, and x0 is y-axis)
  {
    if (x1 >= _height) x1 = _height - 1;

    for (; x0 <= x1; x0++) {
      if ((x0 >= 0) && (y0 >= 0) && (y0 < _width)) break;
      err -= dy;
      if (err < 0) {
        err += dx;
        y0 += ystep;
      }
    }

    if (x0 > x1) {spi_end(); return;}

    setAddrWindow(y0, x0, y0, _height);
    SPI1U1 = mask;
    SPI1W0 = swapped_color;
    for (; x0 <= x1; x0++) {
      while(SPI1CMD & SPIBUSY) {}
      SPI1CMD |= SPIBUSY;

      err -= dy;
      if (err < 0) {
        y0 += ystep;
        if ((y0 < 0) || (y0 >= _width)) break;
        err += dx;
        while(SPI1CMD & SPIBUSY) {}
        setAddrWindow(y0, x0+1, y0, _height);
        SPI1U1 = mask;
        SPI1W0 = swapped_color;
      }
    }
  }
  else    // x increments every iteration (x0 is x-axis, and y0 is y-axis)
  {
    if (x1 >= _width) x1 = _width - 1;

    for (; x0 <= x1; x0++) {
      if ((x0 >= 0) && (y0 >= 0) && (y0 < _height)) break;
      err -= dy;
      if (err < 0) {
          err += dx;
          y0 += ystep;
      }
    }

    if (x0 > x1) {spi_end(); return;}

    setAddrWindow(x0, y0, _width, y0);
    SPI1U1 = mask;
    SPI1W0 = swapped_color;
    for (; x0 <= x1; x0++) {
      while(SPI1CMD & SPIBUSY) {}
      SPI1CMD |= SPIBUSY;

      err -= dy;
      if (err < 0) {
        y0 += ystep;
        if ((y0 < 0) || (y0 >= _height)) break;
        err += dx;
        while(SPI1CMD & SPIBUSY) {}
        setAddrWindow(x0+1, y0, _width, y0);
        SPI1U1 = mask;
        SPI1W0 = swapped_color;
      }
    }
  }

  while(SPI1CMD & SPIBUSY) {}
  SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
  CS_H;
  spi_end();
}

#endif


/***************************************************************************************
** Function name:           drawFastVLine
** Description:             draw a vertical line
***************************************************************************************/
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
void TFT_eSPI::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color)
{
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height) || (h < 1)) return;
  if ((y + h - 1) >= _height) h = _height - y;

  spi_begin();

  setAddrWindow(x, y, x, y + h - 1);

  writeBlock(color, h);
  
  CS_H;

  spi_end();
}

#else

void TFT_eSPI::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color)
{
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height) || (h < 1)) return;
  if ((y + h - 1) >= _height) h = _height - y;

  spi_begin();

  setAddrWindow(x, y, x, y + h - 1);
    
#ifdef RPI_WRITE_STROBE
  #if defined (ESP8266)
    // SPI1U1 will already be set to transfer 16 bits by setAddrWindow()
    SPI1W0 = (color >> 8) | (color << 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
  #else
    tft_Write_16(color);
  #endif
    h--;
    while(h--) {WR_L; WR_H;}
#else
  #ifdef ESP32_PARALLEL
    while (h--) {tft_Write_16(color);}
  #else
    writeBlock(color, h);
  #endif
#endif

  CS_H;

  spi_end();
}
#endif

/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
***************************************************************************************/
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
void TFT_eSPI::drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color)
{
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height) || (w < 1)) return;
  if ((x + w - 1) >= _width)  w = _width - x;

  spi_begin();

  setAddrWindow(x, y, x + w - 1, y);

  writeBlock(color, w);
  
  CS_H;

  spi_end();
}

#else

void TFT_eSPI::drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color)
{
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height) || (w < 1)) return;
  if ((x + w - 1) >= _width)  w = _width - x;

  spi_begin();
  setAddrWindow(x, y, x + w - 1, y);

#ifdef RPI_WRITE_STROBE
  #if defined (ESP8266)
    // SPI1U1 will already be set to transfer 16 bits by setAddrWindow()
    SPI1W0 = (color >> 8) | (color << 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
  #else
    tft_Write_16(color);
  #endif
    w--;
    while(w--) {WR_L; WR_H;}
#else
  #ifdef ESP32_PARALLEL
    while (w--) {tft_Write_16(color);}
  #else
    writeBlock(color, w);
  #endif
#endif

  CS_H;

  spi_end();
}
#endif

/***************************************************************************************
** Function name:           fillRect
** Description:             draw a filled rectangle
***************************************************************************************/
#if defined (ESP8266) && !defined (RPI_WRITE_STROBE)
void TFT_eSPI::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
  // rudimentary clipping (drawChar w/big text requires this)
  if ((x > _width) || (y > _height) || (w < 1) || (h < 1)) return;
  if ((x + w - 1) > _width)  w = _width  - x;
  if ((y + h - 1) > _height) h = _height - y;

  spi_begin();
  setAddrWindow(x, y, x + w - 1, y + h - 1);

  writeBlock(color, w * h);
  
  CS_H;

  spi_end();
}

#else

void TFT_eSPI::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
  // rudimentary clipping (drawChar w/big text requires this)
  if ((x > _width) || (y > _height) || (w < 1) || (h < 1)) return;
  if ((x + w - 1) > _width)  w = _width  - x;
  if ((y + h - 1) > _height) h = _height - y;

  spi_begin();
  setAddrWindow(x, y, x + w - 1, y + h - 1);

  uint32_t n = (uint32_t)w * (uint32_t)h;

#ifdef RPI_WRITE_STROBE
  tft_Write_16(color);
  while(n--) {WR_L; WR_H;}
#else
  #ifdef ESP32_PARALLEL
    if (color>>8 == (uint8_t)color)
    {
      tft_Write_8(color);
      n--; WR_L; WR_H;
      while (n) {WR_L; WR_H; n--; WR_L; WR_H;}
    }
    else
    {
      while (n--) {tft_Write_16(color);}
    }
  #else
    writeBlock(color, n);
  #endif
#endif

  CS_H;

  spi_end();
}
#endif

/***************************************************************************************
** Function name:           color565
** Description:             convert three 8 bit RGB levels to a 16 bit colour value
***************************************************************************************/
uint16_t TFT_eSPI::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


/***************************************************************************************
** Function name:           color16to8
** Description:             convert 16 bit colour to an 8 bit 332 RGB colour value
***************************************************************************************/
uint8_t TFT_eSPI::color16to8(uint16_t c)
{
  return ((c & 0xE000)>>8) | ((c & 0x0700)>>6) | ((c & 0x0018)>>3);
}


/***************************************************************************************
** Function name:           color8to16
** Description:             convert 8 bit colour to a 16 bit 565 colour value
***************************************************************************************/
uint16_t TFT_eSPI::color8to16(uint8_t color)
{
  uint8_t  blue[] = {0, 11, 21, 31}; // blue 2 to 5 bit colour lookup table
  uint16_t color16 = 0;

  //        =====Green=====     ===============Red==============
  color16  = (color & 0x1C)<<6 | (color & 0xC0)<<5 | (color & 0xE0)<<8;
  //        =====Green=====    =======Blue======
  color16 |= (color & 0x1C)<<3 | blue[color & 0x03];

  return color16;
}


/***************************************************************************************
** Function name:           invertDisplay
** Description:             invert the display colours i = 1 invert, i = 0 normal
***************************************************************************************/
void TFT_eSPI::invertDisplay(boolean i)
{
  spi_begin();
  // Send the command twice as otherwise it does not always work!
  writecommand(i ? TFT_INVON : TFT_INVOFF);
  writecommand(i ? TFT_INVON : TFT_INVOFF);
  spi_end();
}


/***************************************************************************************
** Function name:           write
** Description:             draw characters piped through serial stream
***************************************************************************************/
size_t TFT_eSPI::write(uint8_t utf8)
{
  if (utf8 == '\r') return 1;

#ifdef SMOOTH_FONT
  if(fontLoaded)
  {
    uint16_t unicode = decodeUTF8(utf8);
    if (!unicode) return 0;

    //fontFile = SPIFFS.open( _gFontFilename, "r" );

    if(!fontFile)
    {
      fontLoaded = false;
      return 0;
    }

    drawGlyph(unicode);

    //fontFile.close();
    return 1;
  }
#endif

  uint8_t uniCode = utf8;        // Work with a copy
  if (utf8 == '\n') uniCode+=22; // Make it a valid space character to stop errors
  else if (utf8 < 32) return 0;

  uint16_t width = 0;
  uint16_t height = 0;

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  //Serial.print((uint8_t) uniCode); // Debug line sends all printed TFT text to serial port
  //Serial.println(uniCode, HEX); // Debug line sends all printed TFT text to serial port
  //delay(5);                     // Debug optional wait for serial port to flush through
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  if(!gfxFont) {
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef LOAD_FONT2
  if (textfont == 2)
  {
    if (utf8 > 127) return 0;
    // This is 20us faster than using the fontdata structure (0.443ms per character instead of 0.465ms)
    width = pgm_read_byte(widtbl_f16 + uniCode-32);
    height = chr_hgt_f16;
    // Font 2 is rendered in whole byte widths so we must allow for this
    width = (width + 6) / 8;  // Width in whole bytes for font 2, should be + 7 but must allow for font width change
    width = width * 8;        // Width converted back to pixels
  }
  #ifdef LOAD_RLE
  else
  #endif
#endif

#ifdef LOAD_RLE
  {
    if ((textfont>2) && (textfont<9))
    {
      if (utf8 > 127) return 0;
      // Uses the fontinfo struct array to avoid lots of 'if' or 'switch' statements
      // A tad slower than above but this is not significant and is more convenient for the RLE fonts
      width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[textfont].widthtbl ) ) + uniCode-32 );
      height= pgm_read_byte( &fontdata[textfont].height );
    }
  }
#endif

#ifdef LOAD_GLCD
  if (textfont==1)
  {
      width =  6;
      height = 8;
  }
#else
  if (textfont==1) return 0;
#endif

  height = height * textsize;

  if (utf8 == '\n') {
    cursor_y += height;
    cursor_x  = 0;
  }
  else
  {
    if (textwrapX && (cursor_x + width * textsize > _width))
    {
      cursor_y += height;
      cursor_x = 0;
    }
    if (textwrapY && (cursor_y >= _height)) cursor_y = 0;
    cursor_x += drawChar(uniCode, cursor_x, cursor_y, textfont);
  }

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  } // Custom GFX font
  else
  {

    if(utf8 == '\n') {
      cursor_x  = 0;
      cursor_y += (int16_t)textsize *
                  (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else {
      if (uniCode > (uint8_t)pgm_read_byte(&gfxFont->last )) return 0;
      if (uniCode < (uint8_t)pgm_read_byte(&gfxFont->first)) return 0;

      uint8_t   c2    = uniCode - pgm_read_byte(&gfxFont->first);
      GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
      uint8_t   w     = pgm_read_byte(&glyph->width),
                h     = pgm_read_byte(&glyph->height);
      if((w > 0) && (h > 0)) { // Is there an associated bitmap?
        int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);
        if(textwrapX && ((cursor_x + textsize * (xo + w)) > _width)) {
          // Drawing character would go off right edge; wrap to new line
          cursor_x  = 0;
          cursor_y += (int16_t)textsize *
                      (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        }
        if (textwrapY && (cursor_y >= _height)) cursor_y = 0;
        drawChar(cursor_x, cursor_y, uniCode, textcolor, textbgcolor, textsize);
      }
      cursor_x += pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
    }
  }
#endif // LOAD_GFXFF
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  return 1;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a Unicode onto the screen
***************************************************************************************/
int16_t TFT_eSPI::drawChar(unsigned int uniCode, int x, int y)
{
    return drawChar(uniCode, x, y, textfont);
}

int16_t TFT_eSPI::drawChar(unsigned int uniCode, int x, int y, int font)
{

  if (font==1)
  {
#ifdef LOAD_GLCD
  #ifndef LOAD_GFXFF
    drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
    return 6 * textsize;
  #endif
#else
  #ifndef LOAD_GFXFF
    return 0;
  #endif
#endif

#ifdef LOAD_GFXFF
    drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
    if(!gfxFont) { // 'Classic' built-in font
    #ifdef LOAD_GLCD
      return 6 * textsize;
    #else
      return 0;
    #endif
    }
    else
    {
      if((uniCode >= pgm_read_byte(&gfxFont->first)) && (uniCode <= pgm_read_byte(&gfxFont->last) ))
      {
        uint8_t   c2    = uniCode - pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
        return pgm_read_byte(&glyph->xAdvance) * textsize;
      }
      else
      {
        return 0;
      }
    }
#endif
  }

  if ((font>1) && (font<9) && ((uniCode < 32) || (uniCode > 127))) return 0;

  int width  = 0;
  int height = 0;
  uint32_t flash_address = 0;
  uniCode -= 32;

#ifdef LOAD_FONT2
  if (font == 2)
  {
    // This is faster than using the fontdata structure
    flash_address = pgm_read_dword(&chrtbl_f16[uniCode]);
    width = pgm_read_byte(widtbl_f16 + uniCode);
    height = chr_hgt_f16;
  }
  #ifdef LOAD_RLE
  else
  #endif
#endif

#ifdef LOAD_RLE
  {
    if ((font>2) && (font<9))
    {
      // This is slower than above but is more convenient for the RLE fonts
      flash_address = pgm_read_dword( pgm_read_dword( &(fontdata[font].chartbl ) ) + uniCode*sizeof(void *) );
      width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[font].widthtbl ) ) + uniCode );
      height= pgm_read_byte( &fontdata[font].height );
    }
  }
#endif

  int w = width;
  int pX      = 0;
  int pY      = y;
  uint8_t line = 0;

#ifdef LOAD_FONT2 // chop out code if we do not need it
  if (font == 2) {
    w = w + 6; // Should be + 7 but we need to compensate for width increment
    w = w / 8;
    if (x + width * textsize >= (int16_t)_width) return width * textsize ;

    if (textcolor == textbgcolor || textsize != 1) {
      spi_begin();
      inTransaction = true;

      for (int i = 0; i < height; i++)
      {
        if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize, textbgcolor);

        for (int k = 0; k < w; k++)
        {
          line = pgm_read_byte((uint8_t *)flash_address + w * i + k);
          if (line) {
            if (textsize == 1) {
              pX = x + k * 8;
              if (line & 0x80) drawPixel(pX, pY, textcolor);
              if (line & 0x40) drawPixel(pX + 1, pY, textcolor);
              if (line & 0x20) drawPixel(pX + 2, pY, textcolor);
              if (line & 0x10) drawPixel(pX + 3, pY, textcolor);
              if (line & 0x08) drawPixel(pX + 4, pY, textcolor);
              if (line & 0x04) drawPixel(pX + 5, pY, textcolor);
              if (line & 0x02) drawPixel(pX + 6, pY, textcolor);
              if (line & 0x01) drawPixel(pX + 7, pY, textcolor);
            }
            else {
              pX = x + k * 8 * textsize;
              if (line & 0x80) fillRect(pX, pY, textsize, textsize, textcolor);
              if (line & 0x40) fillRect(pX + textsize, pY, textsize, textsize, textcolor);
              if (line & 0x20) fillRect(pX + 2 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x10) fillRect(pX + 3 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x08) fillRect(pX + 4 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x04) fillRect(pX + 5 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x02) fillRect(pX + 6 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x01) fillRect(pX + 7 * textsize, pY, textsize, textsize, textcolor);
            }
          }
        }
        pY += textsize;
      }

      inTransaction = false;
      spi_end();
    }
    else
      // Faster drawing of characters and background using block write
    {
      spi_begin();
      setAddrWindow(x, y, (x + w * 8) - 1, y + height - 1);

      uint8_t mask;
      for (int i = 0; i < height; i++)
      {
        for (int k = 0; k < w; k++)
        {
          line = pgm_read_byte((uint8_t *)flash_address + w * i + k);
          pX = x + k * 8;
          mask = 0x80;
          while (mask) {
            if (line & mask) {tft_Write_16(textcolor);}
            else {tft_Write_16(textbgcolor);}
            mask = mask >> 1;
          }
        }
        pY += textsize;
      }

      CS_H;
      spi_end();
    }
  }

  #ifdef LOAD_RLE
  else
  #endif
#endif  //FONT2

#ifdef LOAD_RLE  //674 bytes of code
  // Font is not 2 and hence is RLE encoded
  {
    spi_begin();

    w *= height; // Now w is total number of pixels in the character
    if ((textsize != 1) || (textcolor == textbgcolor)) {
      if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize * height, textbgcolor);
      int px = 0, py = pY; // To hold character block start and end column and row values
      int pc = 0; // Pixel count
      uint8_t np = textsize * textsize; // Number of pixels in a drawn pixel

      uint8_t tnp = 0; // Temporary copy of np for while loop
      uint8_t ts = textsize - 1; // Temporary copy of textsize
      // 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
      // w is total number of pixels to plot to fill character block
      while (pc < w)
      {
        line = pgm_read_byte((uint8_t *)flash_address);
        flash_address++;
        if (line & 0x80) {
          line &= 0x7F;
          line++;
          if (ts) {
            px = x + textsize * (pc % width); // Keep these px and py calculations outside the loop as they are slow
            py = y + textsize * (pc / width);
          }
          else {
            px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
            py = y + pc / width;
          }
          while (line--) { // In this case the while(line--) is faster
            pc++; // This is faster than putting pc+=line before while()?
            setAddrWindow(px, py, px + ts, py + ts);

            if (ts) {
              tnp = np;
              while (tnp--) {tft_Write_16(textcolor);}
            }
            else {tft_Write_16(textcolor);}
            px += textsize;

            if (px >= (x + width * textsize))
            {
              px = x;
              py += textsize;
            }
          }
        }
        else {
          line++;
          pc += line;
        }
      }

      CS_H;
      spi_end();
    }
    else // Text colour != background && textsize = 1
         // so use faster drawing of characters and background using block write
    {
      //spi_begin();
      setAddrWindow(x, y, x + width - 1, y + height - 1);

      uint8_t textcolorBin[] = { (uint8_t) (textcolor >> 8), (uint8_t) textcolor };
      uint8_t textbgcolorBin[] = { (uint8_t) (textbgcolor >> 8), (uint8_t) textbgcolor };

      // Maximum font size is equivalent to 180x180 pixels in area
      while (w > 0)
      {
        line = pgm_read_byte((uint8_t *)flash_address++); // 8 bytes smaller when incrementing here
        if (line & 0x80) {
          line &= 0x7F;
          line++; w -= line;
#ifdef RPI_WRITE_STROBE
          SPI.writePattern(&textcolorBin[0], 2, 1); line--;
          while(line--) {WR_L; WR_H;}
#else
          #ifdef ESP32_PARALLEL
            while (line--) {tft_Write_16(textcolor);}
          #else
            writeBlock(textcolor,line);
          #endif
#endif
        }
        else {
          line++; w -= line;
#ifdef RPI_WRITE_STROBE
          SPI.writePattern(&textbgcolorBin[0], 2, 1); line--;
          while(line--) {WR_L; WR_H;}
#else
          #ifdef ESP32_PARALLEL
            while (line--) {tft_Write_16(textbgcolor);}
          #else
            writeBlock(textbgcolor,line);
          #endif
#endif
        }
      }
      CS_H;
      spi_end();
    }
  }
  // End of RLE font rendering
#endif
  return width * textsize;    // x +
}


/***************************************************************************************
** Function name:           drawString (with or without user defined font)
** Description :            draw string with padding if it is defined
***************************************************************************************/
// Without font number, uses font set by setTextFont()
int16_t TFT_eSPI::drawString(const String& string, int poX, int poY)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return drawString(buffer, poX, poY, textfont);
}
// With font number
int16_t TFT_eSPI::drawString(const String& string, int poX, int poY, int font)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return drawString(buffer, poX, poY, font);
}

// Without font number, uses font set by setTextFont()
int16_t TFT_eSPI::drawString(const char *string, int poX, int poY)
{
  return drawString(string, poX, poY, textfont);
}
// With font number
int16_t TFT_eSPI::drawString(const char *string, int poX, int poY, int font)
{
  int16_t sumX = 0;
  uint8_t padding = 1, baseline = 0;
  uint16_t cwidth = textWidth(string, font); // Find the pixel width of the string in the font
  uint16_t cheight = 8 * textsize;

#ifdef LOAD_GFXFF
  if (font == 1) {
    if(gfxFont) {
      cheight = glyph_ab * textsize;
      poY += cheight; // Adjust for baseline datum of free fonts
      baseline = cheight;
      padding =101; // Different padding method used for Free Fonts

      // We need to make an adjustment for the bottom of the string (eg 'y' character)
      if ((textdatum == BL_DATUM) || (textdatum == BC_DATUM) || (textdatum == BR_DATUM)) {
        cheight += glyph_bb * textsize;
      }
    }
  }
#endif

  if (textdatum || padX)
  {

    // If it is not font 1 (GLCD or free font) get the baseline and pixel height of the font
#ifdef SMOOTH_FONT
    if(fontLoaded) {
      baseline = gFont.maxAscent;
      cheight  = fontHeight(0);
    }

    else
#endif
    if (font!=1) {
      baseline = pgm_read_byte( &fontdata[font].baseline ) * textsize;
      cheight = fontHeight(font);
    }

    switch(textdatum) {
      case TC_DATUM:
        poX -= cwidth/2;
        padding += 1;
        break;
      case TR_DATUM:
        poX -= cwidth;
        padding += 2;
        break;
      case ML_DATUM:
        poY -= cheight/2;
        //padding += 0;
        break;
      case MC_DATUM:
        poX -= cwidth/2;
        poY -= cheight/2;
        padding += 1;
        break;
      case MR_DATUM:
        poX -= cwidth;
        poY -= cheight/2;
        padding += 2;
        break;
      case BL_DATUM:
        poY -= cheight;
        //padding += 0;
        break;
      case BC_DATUM:
        poX -= cwidth/2;
        poY -= cheight;
        padding += 1;
        break;
      case BR_DATUM:
        poX -= cwidth;
        poY -= cheight;
        padding += 2;
        break;
      case L_BASELINE:
        poY -= baseline;
        //padding += 0;
        break;
      case C_BASELINE:
        poX -= cwidth/2;
        poY -= baseline;
        padding += 1;
        break;
      case R_BASELINE:
        poX -= cwidth;
        poY -= baseline;
        padding += 2;
        break;
    }
    // Check coordinates are OK, adjust if not
    if (poX < 0) poX = 0;
    if (poX+cwidth > width())   poX = width() - cwidth;
    if (poY < 0) poY = 0;
    if (poY+cheight-baseline> height()) poY = height() - cheight;
  }


  int8_t xo = 0;
#ifdef LOAD_GFXFF
  if ((font == 1) && (gfxFont) && (textcolor!=textbgcolor))
    {
      cheight = (glyph_ab + glyph_bb) * textsize;
      // Get the offset for the first character only to allow for negative offsets
      uint8_t   c2    = *string;
      if((c2 >= pgm_read_byte(&gfxFont->first)) && (c2 <= pgm_read_byte(&gfxFont->last) ))
      {
        c2 -= pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
        xo = pgm_read_byte(&glyph->xOffset) * textsize;
        // Adjust for negative xOffset
        if (xo > 0) xo = 0;
        else cwidth -= xo;
        // Add 1 pixel of padding all round
        //cheight +=2;
        //fillRect(poX+xo-1, poY - 1 - glyph_ab * textsize, cwidth+2, cheight, textbgcolor);
        fillRect(poX+xo, poY - glyph_ab * textsize, cwidth, cheight, textbgcolor);
      }
      padding -=100;
    }
#endif

#ifdef SMOOTH_FONT
  if(fontLoaded)
  {
    if (textcolor!=textbgcolor) fillRect(poX, poY, cwidth, cheight, textbgcolor);
    //drawLine(poX - 5, poY, poX + 5, poY, TFT_GREEN);
    //drawLine(poX, poY - 5, poX, poY + 5, TFT_GREEN);
    //fontFile = SPIFFS.open( _gFontFilename, "r");
    if(!fontFile) return 0;
    uint16_t len = strlen(string);
    uint16_t n = 0;
    setCursor(poX, poY);
    while (n < len)
    {
      uint16_t unicode = decodeUTF8((uint8_t*)string, &n, len - n);
      drawGlyph(unicode);
    }
    sumX += cwidth;
    //fontFile.close();
  }
  else
#endif
    while (*string) sumX += drawChar(*(string++), poX+sumX, poY, font);

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Switch on debugging for the padding areas
//#define PADDING_DEBUG

#ifndef PADDING_DEBUG
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  if((padX>cwidth) && (textcolor!=textbgcolor))
  {
    int16_t padXc = poX+cwidth+xo;
#ifdef LOAD_GFXFF
    if ((font == 1) && (gfxFont))
    {
      poX +=xo; // Adjust for negative offset start character
      poY -= glyph_ab * textsize;
    }
#endif
    switch(padding) {
      case 1:
        fillRect(padXc,poY,padX-cwidth,cheight, textbgcolor);
        break;
      case 2:
        fillRect(padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);
        padXc = (padX-cwidth)>>1;
        if (padXc>poX) padXc = poX;
        fillRect(poX - padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);
        break;
      case 3:
        if (padXc>padX) padXc = padX;
        fillRect(poX + cwidth - padXc,poY,padXc-cwidth,cheight, textbgcolor);
        break;
    }
  }


#else

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// This is debug code to show text (green box) and blanked (white box) areas
// It shows that the padding areas are being correctly sized and positioned

  if((padX>sumX) && (textcolor!=textbgcolor))
  {
    int16_t padXc = poX+sumX; // Maximum left side padding
#ifdef LOAD_GFXFF
    if ((font == 1) && (gfxFont)) poY -= glyph_ab;
#endif
    drawRect(poX,poY,sumX,cheight, TFT_GREEN);
    switch(padding) {
      case 1:
        drawRect(padXc,poY,padX-sumX,cheight, TFT_WHITE);
        break;
      case 2:
        drawRect(padXc,poY,(padX-sumX)>>1, cheight, TFT_WHITE);
        padXc = (padX-sumX)>>1;
        if (padXc>poX) padXc = poX;
        drawRect(poX - padXc,poY,(padX-sumX)>>1,cheight, TFT_WHITE);
        break;
      case 3:
        if (padXc>padX) padXc = padX;
        drawRect(poX + sumX - padXc,poY,padXc-sumX,cheight, TFT_WHITE);
        break;
    }
  }
#endif
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

return sumX;
}


/***************************************************************************************
** Function name:           drawCentreString (deprecated, use setTextDatum())
** Descriptions:            draw string centred on dX
***************************************************************************************/
int16_t TFT_eSPI::drawCentreString(const String& string, int dX, int poY, int font)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return drawCentreString(buffer, dX, poY, font);
}

int16_t TFT_eSPI::drawCentreString(const char *string, int dX, int poY, int font)
{
  uint8_t tempdatum = textdatum;
  int sumX = 0;
  textdatum = TC_DATUM;
  sumX = drawString(string, dX, poY, font);
  textdatum = tempdatum;
  return sumX;
}


/***************************************************************************************
** Function name:           drawRightString (deprecated, use setTextDatum())
** Descriptions:            draw string right justified to dX
***************************************************************************************/
int16_t TFT_eSPI::drawRightString(const String& string, int dX, int poY, int font)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return drawRightString(buffer, dX, poY, font);
}

int16_t TFT_eSPI::drawRightString(const char *string, int dX, int poY, int font)
{
  uint8_t tempdatum = textdatum;
  int16_t sumX = 0;
  textdatum = TR_DATUM;
  sumX = drawString(string, dX, poY, font);
  textdatum = tempdatum;
  return sumX;
}


/***************************************************************************************
** Function name:           drawNumber
** Description:             draw a long integer
***************************************************************************************/
int16_t TFT_eSPI::drawNumber(long long_num, int poX, int poY)
{
  char str[12];
  ltoa(long_num, str, 10);
  return drawString(str, poX, poY, textfont);
}

int16_t TFT_eSPI::drawNumber(long long_num, int poX, int poY, int font)
{
  char str[12];
  ltoa(long_num, str, 10);
  return drawString(str, poX, poY, font);
}


/***************************************************************************************
** Function name:           drawFloat
** Descriptions:            drawFloat, prints 7 non zero digits maximum
***************************************************************************************/
// Assemble and print a string, this permits alignment relative to a datum
// looks complicated but much more compact and actually faster than using print class
int16_t TFT_eSPI::drawFloat(float floatNumber, int dp, int poX, int poY)
{
  return drawFloat(floatNumber, dp, poX, poY, textfont);
}

int16_t TFT_eSPI::drawFloat(float floatNumber, int dp, int poX, int poY, int font)
{
  char str[14];               // Array to contain decimal string
  uint8_t ptr = 0;            // Initialise pointer for array
  int8_t  digits = 1;         // Count the digits to avoid array overflow
  float rounding = 0.5;       // Round up down delta

  if (dp > 7) dp = 7; // Limit the size of decimal portion

  // Adjust the rounding value
  for (uint8_t i = 0; i < dp; ++i) rounding /= 10.0;

  if (floatNumber < -rounding)    // add sign, avoid adding - sign to 0.0!
  {
    str[ptr++] = '-'; // Negative number
    str[ptr] = 0; // Put a null in the array as a precaution
    digits = 0;   // Set digits to 0 to compensate so pointer value can be used later
    floatNumber = -floatNumber; // Make positive
  }

  floatNumber += rounding; // Round up or down

  // For error put ... in string and return (all TFT_eSPI library fonts contain . character)
  if (floatNumber >= 2147483647) {
    strcpy(str, "...");
    return drawString(str, poX, poY, font);
  }
  // No chance of overflow from here on

  // Get integer part
  unsigned long temp = (unsigned long)floatNumber;

  // Put integer part into array
  ltoa(temp, str + ptr, 10);

  // Find out where the null is to get the digit count loaded
  while ((uint8_t)str[ptr] != 0) ptr++; // Move the pointer along
  digits += ptr;                  // Count the digits

  str[ptr++] = '.'; // Add decimal point
  str[ptr] = '0';   // Add a dummy zero
  str[ptr + 1] = 0; // Add a null but don't increment pointer so it can be overwritten

  // Get the decimal portion
  floatNumber = floatNumber - temp;

  // Get decimal digits one by one and put in array
  // Limit digit count so we don't get a false sense of resolution
  uint8_t i = 0;
  while ((i < dp) && (digits < 9)) // while (i < dp) for no limit but array size must be increased
  {
    i++;
    floatNumber *= 10;       // for the next decimal
    temp = floatNumber;      // get the decimal
    ltoa(temp, str + ptr, 10);
    ptr++; digits++;         // Increment pointer and digits count
    floatNumber -= temp;     // Remove that digit
  }

  // Finally we can plot the string and return pixel length
  return drawString(str, poX, poY, font);
}


/***************************************************************************************
** Function name:           setFreeFont
** Descriptions:            Sets the GFX free font to use
***************************************************************************************/

#ifdef LOAD_GFXFF

void TFT_eSPI::setFreeFont(const GFXfont *f)
{
  textfont = 1;
  gfxFont = (GFXfont *)f;

  glyph_ab = 0;
  glyph_bb = 0;
  uint8_t numChars = pgm_read_byte(&gfxFont->last) - pgm_read_byte(&gfxFont->first);
  
  // Find the biggest above and below baseline offsets
  for (uint8_t c = 0; c < numChars; c++)
  {
    GFXglyph *glyph1  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c]);
    int8_t ab = -pgm_read_byte(&glyph1->yOffset);
    if (ab > glyph_ab) glyph_ab = ab;
    int8_t bb = pgm_read_byte(&glyph1->height) - ab;
    if (bb > glyph_bb) glyph_bb = bb;
  }
}


/***************************************************************************************
** Function name:           setTextFont
** Description:             Set the font for the print stream
***************************************************************************************/
void TFT_eSPI::setTextFont(uint8_t f)
{
  textfont = (f > 0) ? f : 1; // Don't allow font 0
  gfxFont = NULL;
}

#else

    
/***************************************************************************************
** Function name:           setFreeFont
** Descriptions:            Sets the GFX free font to use
***************************************************************************************/

// Alternative to setTextFont() so we don't need two different named functions
void TFT_eSPI::setFreeFont(uint8_t font)
{
  setTextFont(font);
}


/***************************************************************************************
** Function name:           setTextFont
** Description:             Set the font for the print stream
***************************************************************************************/
void TFT_eSPI::setTextFont(uint8_t f)
{
  textfont = (f > 0) ? f : 1; // Don't allow font 0
}

#endif


/***************************************************************************************
** Function name:           spiBlockWrite
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
//Clear screen test 76.8ms theoretical. 81.5ms TFT_eSPI, 967ms Adafruit_ILI9341
//Performance 26.15Mbps@26.66MHz, 39.04Mbps@40MHz, 75.4Mbps@80MHz SPI clock
//Efficiency:
//       TFT_eSPI       98.06%              97.59%          94.24%
//       Adafruit_GFX   19.62%              14.31%           7.94%
//
#if defined (ESP8266)
void writeBlock(uint16_t color, uint32_t repeat)
{
  uint16_t color16 = (color >> 8) | (color << 8);
  uint32_t color32 = color16 | color16 << 16;
  uint32_t mask = ~(SPIMMOSI << SPILMOSI);
  mask = SPI1U1 & mask;
  SPI1U = SPIUMOSI | SPIUSSE;

  SPI1W0 = color32;
  SPI1W1 = color32;
  SPI1W2 = color32;
  SPI1W3 = color32;
  if (repeat > 8)
  {
    SPI1W4 = color32;
    SPI1W5 = color32;
    SPI1W6 = color32;
    SPI1W7 = color32;
  }
  if (repeat > 16)
  {
    SPI1W8 = color32;
    SPI1W9 = color32;
    SPI1W10 = color32;
    SPI1W11 = color32;
  }
  if (repeat > 24)
  {
    SPI1W12 = color32;
    SPI1W13 = color32;
    SPI1W14 = color32;
    SPI1W15 = color32;
  }
  if (repeat > 31)
  {
    SPI1U1 = mask | (511 << SPILMOSI);
    while(repeat>31)
    {
#if defined SPI_FREQUENCY && (SPI_FREQUENCY == 80000000)
      if(SPI1CMD & SPIBUSY) // added to sync with flag change
#endif
      while(SPI1CMD & SPIBUSY) {}
      SPI1CMD |= SPIBUSY;
      repeat -= 32;
    }
    while(SPI1CMD & SPIBUSY) {}
  }

  if (repeat)
  {
    repeat = (repeat << 4) - 1;
    SPI1U1 = mask | (repeat << SPILMOSI);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
  }

  SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
}

#else // Low level register based ESP32 code

#include "soc/spi_reg.h"
#define SPI_NUM 0x3

void writeBlock(uint16_t color, uint32_t repeat)
{
  uint16_t color16 = (color >> 8) | (color << 8);
  uint32_t color32 = color16 | color16 << 16;

  if (repeat > 15)
  {
    SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(SPI_NUM), SPI_USR_MOSI_DBITLEN, 255, SPI_USR_MOSI_DBITLEN_S);

    while(repeat>15)
    {
      while (READ_PERI_REG(SPI_CMD_REG(SPI_NUM))&SPI_USR);
      for (uint32_t i=0; i<16; i++) WRITE_PERI_REG((SPI_W0_REG(SPI_NUM) + (i << 2)), color32);
      SET_PERI_REG_MASK(SPI_CMD_REG(SPI_NUM), SPI_USR);
      repeat -= 16;
    }
    while (READ_PERI_REG(SPI_CMD_REG(SPI_NUM))&SPI_USR);
  }

  if (repeat)
  {
    repeat = (repeat << 4) - 1;
    SET_PERI_REG_BITS(SPI_MOSI_DLEN_REG(SPI_NUM), SPI_USR_MOSI_DBITLEN, repeat, SPI_USR_MOSI_DBITLEN_S);
    for (uint32_t i=0; i<16; i++) WRITE_PERI_REG((SPI_W0_REG(SPI_NUM) + (i << 2)), color32);
    SET_PERI_REG_MASK(SPI_CMD_REG(SPI_NUM), SPI_USR);
    while (READ_PERI_REG(SPI_CMD_REG(SPI_NUM))&SPI_USR);
  }

}
#endif


/***************************************************************************************
** Function name:           getSetup
** Description:             Get the setup details for diagnostic and sketch access
***************************************************************************************/
void TFT_eSPI::getSetup(setup_t &tft_settings)
{

#if defined (ESP8266)
  tft_settings.esp = 8266;
#elif defined (ESP32)
  tft_settings.esp = 32;
#else
  tft_settings.esp = -1;
#endif

#if defined (SUPPORT_TRANSACTIONS)
  tft_settings.trans = true;
#else
  tft_settings.trans = false;
#endif

#if defined (ESP32_PARALLEL)
  tft_settings.serial = false;
  tft_settings.tft_spi_freq = 0;
#else
  tft_settings.serial = true;
  tft_settings.tft_spi_freq = SPI_FREQUENCY/100000;
#endif

#if defined(TFT_SPI_OVERLAP)
  tft_settings.overlap = true;
#else
  tft_settings.overlap = false;
#endif

  tft_settings.tft_driver = TFT_DRIVER;
  tft_settings.tft_width  = _init_width;
  tft_settings.tft_height = _init_height;

#ifdef CGRAM_OFFSET
  tft_settings.r0_x_offset = colstart;
  tft_settings.r0_y_offset = rowstart;
  tft_settings.r1_x_offset = 0;
  tft_settings.r1_y_offset = 0;
  tft_settings.r2_x_offset = 0;
  tft_settings.r2_y_offset = 0;
  tft_settings.r3_x_offset = 0;
  tft_settings.r3_y_offset = 0;
#else
  tft_settings.r0_x_offset = 0;
  tft_settings.r0_y_offset = 0;
  tft_settings.r1_x_offset = 0;
  tft_settings.r1_y_offset = 0;
  tft_settings.r2_x_offset = 0;
  tft_settings.r2_y_offset = 0;
  tft_settings.r3_x_offset = 0;
  tft_settings.r3_y_offset = 0;
#endif

#if defined (TFT_MOSI)
  tft_settings.pin_tft_mosi = TFT_MOSI;
#else
  tft_settings.pin_tft_mosi = -1;
#endif

#if defined (TFT_MISO)
  tft_settings.pin_tft_miso = TFT_MISO;
#else
  tft_settings.pin_tft_miso = -1;
#endif

#if defined (TFT_SCLK)
  tft_settings.pin_tft_clk  = TFT_SCLK;
#else
  tft_settings.pin_tft_clk  = -1;
#endif

#if defined (TFT_CS)
  tft_settings.pin_tft_cs   = TFT_CS;
#else
  tft_settings.pin_tft_cs   = -1;
#endif

#if defined (TFT_DC)
  tft_settings.pin_tft_dc  = TFT_DC;
#else
  tft_settings.pin_tft_dc  = -1;
#endif

#if defined (TFT_RD)
  tft_settings.pin_tft_rd  = TFT_RD;
#else
  tft_settings.pin_tft_rd  = -1;
#endif

#if defined (TFT_WR)
  tft_settings.pin_tft_wr  = TFT_WR;
#else
  tft_settings.pin_tft_wr  = -1;
#endif

#if defined (TFT_RST)
  tft_settings.pin_tft_rst = TFT_RST;
#else
  tft_settings.pin_tft_rst = -1;
#endif

#if defined (ESP32_PARALLEL)
  tft_settings.pin_tft_d0 = TFT_D0;
  tft_settings.pin_tft_d1 = TFT_D1;
  tft_settings.pin_tft_d2 = TFT_D2;
  tft_settings.pin_tft_d3 = TFT_D3;
  tft_settings.pin_tft_d4 = TFT_D4;
  tft_settings.pin_tft_d5 = TFT_D5;
  tft_settings.pin_tft_d6 = TFT_D6;
  tft_settings.pin_tft_d7 = TFT_D7;
#else
  tft_settings.pin_tft_d0 = -1;
  tft_settings.pin_tft_d1 = -1;
  tft_settings.pin_tft_d2 = -1;
  tft_settings.pin_tft_d3 = -1;
  tft_settings.pin_tft_d4 = -1;
  tft_settings.pin_tft_d5 = -1;
  tft_settings.pin_tft_d6 = -1;
  tft_settings.pin_tft_d7 = -1;
#endif

#if defined (TOUCH_CS)
  tft_settings.pin_tch_cs   = TOUCH_CS;
  tft_settings.tch_spi_freq = SPI_TOUCH_FREQUENCY/100000;
#else
  tft_settings.pin_tch_cs   = -1;
  tft_settings.tch_spi_freq = 0;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////
#ifdef TOUCH_CS
  #include "Extensions/Touch.cpp"
  #include "Extensions/Button.cpp"
#endif

#include "Extensions/Sprite.cpp"

#ifdef SMOOTH_FONT
  #include "Extensions/Smooth_font.cpp"
#endif

////////////////////////////////////////////////////////////////////////////////////////

