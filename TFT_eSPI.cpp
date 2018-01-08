/***************************************************
  Arduino TFT graphics library targetted at ESP8266
  and ESP32 based boards.

  This is a standalone library that contains the
  hardware driver, the graphics funtions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce their
  size.

  Created by Bodmer 2/12/16
  Bodmer: Added RPi 16 bit display support
 ****************************************************/

#include "TFT_eSPI.h"

#include <pgmspace.h>

#include <SPI.h>

  // SUPPORT_TRANSACTIONS is manadatory for ESP32 so the hal mutex is toggled
#if defined (ESP32) && !defined (SUPPORT_TRANSACTIONS)
  #define SUPPORT_TRANSACTIONS
#endif

// If it is a 16bit serial display we must transfer 16 bits every time
#ifdef RPI_ILI9486_DRIVER
  #define SEND_16_BITS
  #define CMD_BITS 16-1
#else
  #define CMD_BITS 8-1
#endif

// Fast SPI block write prototype
void spiWriteBlock(uint16_t color, uint32_t repeat);

// If the SPI library has transaction support, these functions
// establish settings and protect from interference from other
// libraries.  Otherwise, they simply do nothing.

inline void TFT_eSPI::spi_begin(void){
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS)
  if (locked) {locked = false; SPI.beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));}
#endif
}

inline void TFT_eSPI::spi_end(void){
#if defined (SPI_HAS_TRANSACTION) && defined (SUPPORT_TRANSACTIONS)
  if(!inTransaction) {if (!locked) {locked = true; SPI.endTransaction();}}
#endif
}

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

  _width_orig  = _width  = w; // Set by specific xxxxx_Defines.h file or by users sketch
  _height_orig = _height = h; // Set by specific xxxxx_Defines.h file or by users sketch
  rotation  = 0;
  cursor_y  = cursor_x    = 0;
  textfont  = 1;
  textsize  = 1;
  textcolor   = 0xFFFF; // White
  textbgcolor = 0x0000; // Black
  padX = 0;             // No padding
  textwrap  = true;     // Wrap text when using print stream
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

}


/***************************************************************************************
** Function name:           begin
** Description:             Included for backwards compatibility
***************************************************************************************/
void TFT_eSPI::begin(void)
{
 init();
}


/***************************************************************************************
** Function name:           init
** Description:             Reset, then initialise the TFT display registers
***************************************************************************************/
void TFT_eSPI::init(void)
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
  #if defined (TFT_MOSI) && !defined (TFT_SPI_OVERLAP)
    SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, -1);
  #else
    SPI.begin();
  #endif

#endif

  inTransaction = false;
  locked = true;

  // SUPPORT_TRANSACTIONS is manadatory for ESP32 so the hal mutex is toggled
  // so the code here is for ESP8266 only
#if !defined (SUPPORT_TRANSACTIONS) && defined (ESP8266)
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setFrequency(SPI_FREQUENCY);
#endif

  // Set to output once again in case D6 (MISO) is used for CS
#ifdef TFT_CS
  digitalWrite(TFT_CS, HIGH); // Chip select high (inactive)
  pinMode(TFT_CS, OUTPUT);
#else
  SPI.setHwCs(1); // Use hardware SS toggling
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
    #include "TFT_Drivers/ST7735_Init.h"

#elif defined (ILI9163_DRIVER)
    #include "TFT_Drivers/ILI9163_Init.h"

#elif defined (S6D02A1_DRIVER)
    #include "TFT_Drivers/S6D02A1_Init.h"
     
#elif defined (RPI_ILI9486_DRIVER)
    #include "TFT_Drivers/RPI_ILI9486_Init.h"

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
  SPI.transfer(c);
}


/***************************************************************************************
** Function name:           writecommand
** Description:             Send an 8 bit command to the TFT
***************************************************************************************/
void TFT_eSPI::writecommand(uint8_t c)
{
  DC_C;
  CS_L;
  #ifdef SEND_16_BITS
    SPI.transfer(0);
  #endif
  SPI.transfer(c);
  CS_H;
  DC_D;
}


/***************************************************************************************
** Function name:           writedata
** Description:             Send a 8 bit data value to the TFT
***************************************************************************************/
void TFT_eSPI::writedata(uint8_t c)
{
  CS_L;
  #ifdef SEND_16_BITS
    SPI.transfer(0);
  #endif
  SPI.transfer(c);
  CS_H;
}


/***************************************************************************************
** Function name:           readcommand8 (for ILI9341 Interface II i.e. IM [3:0] = "1101")
** Description:             Read a 8 bit data value from an indexed command register
***************************************************************************************/
uint8_t TFT_eSPI::readcommand8(uint8_t cmd_function, uint8_t index)
{
  spi_begin();
  index = 0x10 + (index & 0x0F);

  DC_C;
  CS_L;
  SPI.transfer(0xD9);
  DC_D;
  SPI.transfer(index);
  CS_H;

  DC_C;
  CS_L;
  SPI.transfer(cmd_function);
  DC_D;
  uint8_t reg = SPI.transfer(0);
  CS_H;

  spi_end();
  return reg;
}


/***************************************************************************************
** Function name:           readcommand16 (for ILI9341 Interface II i.e. IM [3:0] = "1101")
** Description:             Read a 16 bit data value from an indexed command register
***************************************************************************************/
uint16_t TFT_eSPI::readcommand16(uint8_t cmd_function, uint8_t index)
{
  uint32_t reg = 0;
  reg |= (readcommand8(cmd_function, index + 0) <<  8);
  reg |= (readcommand8(cmd_function, index + 1) <<  0);

  return reg;
}


/***************************************************************************************
** Function name:           readcommand32 (for ILI9341 Interface II i.e. IM [3:0] = "1101")
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
  spi_begin();

  readAddrWindow(x0, y0, x0, y0); // Sets CS low

  // Dummy read to throw away don't care value
  SPI.transfer(0);
    
  // Read window pixel 24 bit RGB values
  uint8_t r = SPI.transfer(0);
  uint8_t g = SPI.transfer(0);
  uint8_t b = SPI.transfer(0);

  CS_H;

  spi_end();
    
  return color565(r, g, b);
}


/***************************************************************************************
** Function name:           read rectangle (for SPI Interface II i.e. IM [3:0] = "1101")
** Description:             Read 565 pixel colours from a defined area
***************************************************************************************/
void TFT_eSPI::readRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t *data)
{
  if ((x > _width) || (y > _height) || (w == 0) || (h == 0)) return;
  
  spi_begin();

  readAddrWindow(x, y, x + w - 1, y + h - 1); // Sets CS low

  // Dummy read to throw away don't care value
  SPI.transfer(0);

  // Read window pixel 24 bit RGB values
  uint32_t len = w * h;
  while (len--) {
    // Read the 3 RGB bytes, colour is actually only in the top 6 bits of each byte
    // as the TFT stores colours as 18 bits
    uint8_t r = SPI.transfer(0);
    uint8_t g = SPI.transfer(0);
    uint8_t b = SPI.transfer(0);
    // Swapped colour byte order for compatibility with pushRect()
    *data++ = (r & 0xF8) | (g & 0xE0) >> 5 | (b & 0xF8) << 5 | (g & 0x1C) << 11;
  }

  // Write NOP command to stop read mode
  //DC_C;
  //SPI.transfer(TFT_NOP);
  //DC_D;

  CS_H;

  spi_end();
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
void TFT_eSPI::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t *data)
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

  // Line buffer makes plotting faster
  uint16_t  lineBuf[dw];

  uint8_t  blue[] = {0, 11, 21, 31}; // blue 2 to 5 bit colour lookup table

  _lastColor = -1; // Set to illegal value
  
  // Used to store last shifted colour
  uint8_t msbColor = 0;
  uint8_t lsbColor = 0;

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

  CS_H;

  inTransaction = false;
  spi_end();
}


/***************************************************************************************
** Function name:           pushImage
** Description:             plot 8 bit image or sprite with 1 colour being transparent
***************************************************************************************/
void TFT_eSPI::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t *data, uint8_t transp)
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

  // Line buffer makes plotting faster
  uint16_t  lineBuf[dw];

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
  spi_begin();

  readAddrWindow(x0, y0, x0 + w - 1, y0 + h - 1); // Sets CS low

  // Dummy read to throw away don't care value
  SPI.transfer(0);
  
  // Read window pixel 24 bit RGB values, buffer must be set in sketch to 3 * w * h
  uint32_t len = w * h;
  while (len--) {
    // Read the 3 RGB bytes, colour is actually only in the top 6 bits of each byte
    // as the TFT stores colours as 18 bits
    *data++ = SPI.transfer(0);
    *data++ = SPI.transfer(0);
    *data++ = SPI.transfer(0);
  }
  CS_H;

  spi_end();
}


/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle outline
***************************************************************************************/
/*
// Midpoint circle algorithm, we can optimise this since y = 0 on first pass
// and we can eliminate the multiply as well
void TFT_eSPI::drawCircle(int32_t x0, int32_t y0, int32_t radius, uint32_t color)
{
  int32_t x = radius;
  int32_t y = 0;
  int32_t err = 0;

  while (x >= y)
  {
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 - x, y0 + y, color);

    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
    drawPixel(x0 - y, y0 + x, color);

    if (err <= 0)
    {
      y += 1;
      err += 2*y + 1;
    }
    if (err > 0)
    {
      x -= 1;
      err -= 2*x + 1;
    }
  }
}
*/

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
void TFT_eSPI::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

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
** Function name:           setTextWrap
** Description:             Define if text should wrap at end of line
***************************************************************************************/
void TFT_eSPI::setTextWrap(boolean w)
{
  textwrap = w;
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
  unsigned int str_width  = 0;
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
        if (column[k] & mask) {
          SPI.write16(color);
        }
        else {
          SPI.write16(bg);
        }
      }

      mask <<= 1;

      SPI.write16(bg);
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

#if defined (RPI_ILI9486_DRIVER)
  SPI.write16(TFT_CASET);
#else
  SPI.write(TFT_CASET);
#endif

  DC_D;


#if defined (RPI_ILI9486_DRIVER)
  uint8_t xBin[] = { 0, (uint8_t) (x0>>8), 0, (uint8_t) (x0>>0), 0, (uint8_t) (x1>>8), 0, (uint8_t) (x1>>0), };
  SPI.writePattern(&xBin[0], 8, 1);
#else
  SPI.write32(xaw);
#endif

  // Row addr set
  DC_C;

#if defined (RPI_ILI9486_DRIVER)
  SPI.write16(TFT_PASET);
#else
  SPI.write(TFT_PASET);
#endif

  DC_D;


#if defined (RPI_ILI9486_DRIVER)
  uint8_t yBin[] = { 0, (uint8_t) (y0>>8), 0, (uint8_t) (y0>>0), 0, (uint8_t) (y1>>8), 0, (uint8_t) (y1>>0), };
  SPI.writePattern(&yBin[0], 8, 1);
#else
  SPI.write32(yaw);
#endif

  // write to RAM
  DC_C;

#if defined (RPI_ILI9486_DRIVER)
  SPI.write16(TFT_RAMWR);
#else
  SPI.write(TFT_RAMWR);
#endif

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

  SPI.write(TFT_CASET);

  DC_D;

  SPI.write32(xaw);

  // Row addr set
  DC_C;

  SPI.write(TFT_PASET);

  DC_D;

  SPI.write32(yaw);
  
  DC_C;
  SPI.transfer(TFT_RAMRD); // Read CGRAM command
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

#if defined (RPI_ILI9486_DRIVER)
    SPI.write16(TFT_CASET);
#else
    SPI.write(TFT_CASET);
#endif

    DC_D;

#if defined (RPI_ILI9486_DRIVER)
    uint8_t xBin[] = { 0, (uint8_t) (x>>8), 0, (uint8_t) (x>>0), 0, (uint8_t) (x>>8), 0, (uint8_t) (x>>0), };
    SPI.writePattern(&xBin[0], 8, 1);
#else
    SPI.write32(xaw);
#endif
    
    addr_col = x;
  }

  // No need to send y if it has not changed (speeds things up)
  if (addr_row != y) {

    DC_C;

#if defined (RPI_ILI9486_DRIVER)
    SPI.write16(TFT_PASET);
#else
    SPI.write(TFT_PASET);
#endif

    DC_D;

#if defined (RPI_ILI9486_DRIVER)
    uint8_t yBin[] = { 0, (uint8_t) (y>>8), 0, (uint8_t) (y>>0), 0, (uint8_t) (y>>8), 0, (uint8_t) (y>>0), };
    SPI.writePattern(&yBin[0], 8, 1);
#else
    SPI.write32(yaw);
#endif

    addr_row = y;
  }

  DC_C;

#if defined (RPI_ILI9486_DRIVER)
  SPI.write16(TFT_RAMWR);
#else
  SPI.write(TFT_RAMWR);
#endif

  DC_D;

  SPI.write16(color);

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
#if defined (ESP8266)
  SPI.write16(color, true);
#else
  SPI.write16(color);
#endif
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
  spiWriteBlock(color, len);
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

if (swap) SPI.writePixels(data,len<<1);
else SPI.writeBytes((uint8_t*)data,len<<1);

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

/* // Smaller version but slower
  uint32_t count = 0;
  while(len)
  {
    if(len>15) {count = 16; len -= 16;}
    else {count = len; len = 0;}
    uint32_t bits = (count*16-1); // bits left to shift - 1
    if (swap)
    {
      uint16_t* ptr = (uint16_t*)color;
      while(count--)
      {
        *ptr++ = (*(data) >> 8) | (uint16_t)(*(data) << 8);
        data++;
      }
    }
    else
    {
      memcpy(color,data,count<<1);
      data += 16;
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
*/

  while(SPI1CMD & SPIBUSY) {}

#endif

  CS_H;

  spi_end();
}

/***************************************************************************************
** Function name:           pushColors
** Description:             push an aray of pixels for BMP image drawing
***************************************************************************************/
// Sends an array of 16-bit color values to the TFT; used
// externally by BMP examples.  Assumes that setWindow() has
// previously been called to define the bounds.  Max 255 pixels at
// a time (BMP examples read in small chunks due to limited RAM).
/*
#define SWAP_COLOR_BYTES // We want to swap color bytes in this fn
void TFT_eSPI::pushColors(uint16_t *data, uint8_t len)
{
  spi_begin();

  CS_L;

#if defined (ESP32)

  SPI.writePixels(data,len<<1);

#else

	uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  SPI1U1 = (SPI1U1 & mask) | (255 << SPILMOSI) | (255 << SPILMISO);

  uint32_t color[8];

  while(len>15)
  {

#ifdef SWAP_COLOR_BYTES
    uint32_t i = 0;
    while(i<8)
    {
      color[i]  = (*(data) >> 8) | (uint16_t)(*(data) << 8);
      data++;
      color[i] |= ((*(data) >> 8) | (*(data) << 8)) << 16;
      data++;
      i++;
    }
#else
    uint32_t i = 0;
    while(i<8)
    {
      color[i]  = *data++;
      color[i] |= (*data++) << 16;
      i++;
    }
#endif

    len -= 16;
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
  if (len) 
  {
    while(SPI1CMD & SPIBUSY) {}
    SPI1U1 = (SPI1U1 & mask) | (15 << SPILMOSI) | (15 << SPILMISO);
    while(len--)
    {
      uint16_t color = (*(data) >> 8) | (*(data) << 8);
      data++;
      while(SPI1CMD & SPIBUSY) {}
      SPI1W0 = color;
      SPI1CMD |= SPIBUSY;
    }
  }
  while(SPI1CMD & SPIBUSY) {}

#endif

  CS_H;

  spi_end();
}
*/

/***************************************************************************************
** Function name:           pushColors
** Description:             push an array of pixels for 16 bit raw image drawing
***************************************************************************************/
// Assumed that setWindow() has previously been called
// There is no control of endianness
/*void TFT_eSPI::pushColors(uint8_t *data, uint32_t len)
{
  spi_begin();

  CS_L;

  // We cannot align to 32 bits since array may be created as 8 bit originally
  while ( len >=64 ) {SPI.writePattern(data, 64, 1); data += 64; len -= 64; }
  if (len) SPI.writePattern(data, len, 1);

  CS_H;

  spi_end();
}
*/

/***************************************************************************************
** Function name:           pushColors
** Description:             push an aray of pixels with byte swap option
***************************************************************************************/
/*
void TFT_eSPI::pushColors(uint16_t *data, uint32_t len, bool swap)
{
  spi_begin();

  CS_L;

#if defined (ESP32)
  if (swap)
  {
    //while (len--) SPI.write16(*(data++));
    SPI.writePixels(data,len<<1);
  }
  else
  {
    SPI.writeBytes((uint8_t*)data,len<<1);
  }
#else
  if (swap)
  {
    while (len>239) { pushColors(data, 240); len -= 240; data += 240; }
    if (len) pushColors(data, len);
  }
  else
  {
    // Check alignment of 16 bit pointer to 32 bits
    uint8_t offset = ((uint32_t)data & 0x2)>>1;
    if (offset > len) offset = len;

    // Make pointer 32 bit align using immune call then use the faster writeBytes()
    if (offset) { SPI.writePattern((uint8_t*)data, offset<<1, 1); len -= offset; data += offset; }

    if (len) SPI.writeBytes((uint8_t*)data, len<<1);
  }
#endif
  CS_H;

  spi_end();
}
*/

/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
***************************************************************************************/
// Bresenham's algorithm - thx wikipedia - speed enhanced by Bodmer to use
// an eficient FastH/V Line draw routine for line segments of 2 pixels or more

#if defined (RPI_ILI9486_DRIVER) || defined (ESP32) || defined (RPI_WRITE_STROBE)

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

  spiWriteBlock(color, h);
  
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
    SPI1W0 = (color >> 8) | (color << 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
  #else
    SPI.write16(color);
  #endif
  h--;
  while(h--) {WR_L; WR_H;}
#else
  //while(h--) SPI.write16(color);
  spiWriteBlock(color, h);
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

  spiWriteBlock(color, w);
  
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
    SPI1W0 = (color >> 8) | (color << 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
  #else
    SPI.write16(color);
  #endif
  w--;
  while(w--) {WR_L; WR_H;}
#else
  //while(w--) SPI.write16(color);
  spiWriteBlock(color, w);
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
void TFT_eSPI::fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color)
{
  // rudimentary clipping (drawChar w/big text requires this)
  if ((x > _width) || (y > _height) || (w < 1) || (h < 1)) return;
  if ((x + w - 1) > _width)  w = _width  - x;
  if ((y + h - 1) > _height) h = _height - y;

  spi_begin();
  setAddrWindow(x, y, x + w - 1, y + h - 1);

  spiWriteBlock(color, w * h);
  
  CS_H;

  spi_end();
}

#else

void TFT_eSPI::fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color)
{
  // rudimentary clipping (drawChar w/big text requires this)
  if ((x > _width) || (y > _height) || (w < 1) || (h < 1)) return;
  if ((x + w - 1) > _width)  w = _width  - x;
  if ((y + h - 1) > _height) h = _height - y;

  spi_begin();
  setAddrWindow(x, y, x + w - 1, y + h - 1);

  uint32_t n = (uint32_t)w * (uint32_t)h;

#ifdef RPI_WRITE_STROBE
  if(n) {SPI.write16(color); n--;}
  while(n--) {WR_L; WR_H;}
#else
  //while(n--) SPI.write16(color);
  spiWriteBlock(color, n);
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
** Function name:           color332
** Description:             convert 16 bit colour to an 8 bit 332 RGB colour value
***************************************************************************************/
uint8_t TFT_eSPI::color332(uint16_t c)
{
  return ((c & 0xE000)>>8) | ((c & 0x0700)>>6) | ((c & 0x0018)>>3);
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
    width = width * 8;        // Width converted back to pixles
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
    if (textwrap && (cursor_x + width * textsize > _width))
    {
      cursor_y += height;
      cursor_x = 0;
    }
    if (cursor_y >= _height) cursor_y = 0;
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
        if(textwrap && ((cursor_x + textsize * (xo + w)) > _width)) {
          // Drawing character would go off right edge; wrap to new line
          cursor_x  = 0;
          cursor_y += (int16_t)textsize *
                      (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        }
        if (cursor_y >= _height) cursor_y = 0;
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
** Description:             draw a unicode onto the screen
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
            if (line & mask) {
              SPI.write16(textcolor);
            }
            else {
              SPI.write16(textbgcolor);
            }
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
        flash_address++; // 20 bytes smaller by incrementing here
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
              while (tnp--) {
                SPI.write16(textcolor);
              }
            }
            else {
              SPI.write16(textcolor);
            }
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
          spiWriteBlock(textcolor,line);
#endif
        }
        else {
          line++; w -= line;
#ifdef RPI_WRITE_STROBE
          SPI.writePattern(&textbgcolorBin[0], 2, 1); line--;
          while(line--) {WR_L; WR_H;}
#else
          spiWriteBlock(textbgcolor,line);
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

      // We need to make an adjustment for the botom of the string (eg 'y' character)
      if ((textdatum == BL_DATUM) || (textdatum == BC_DATUM) || (textdatum == BR_DATUM)) {
        cheight += glyph_bb * textsize;
      }
    }
  }
#endif

  if (textdatum || padX)
  {

    // If it is not font 1 (GLCD or free font) get the basline and pixel height of the font
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
    if (poX+cwidth>_width)   poX = _width - cwidth;
    if (poY < 0) poY = 0;
    if (poY+cheight-baseline>_height) poY = _height - cheight;
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
#if defined (ESP8266) // && (SPI_FREQUENCY != 80000000)
void spiWriteBlock(uint16_t color, uint32_t repeat)
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
#elif ESP8266 // ESP32 or a ESP8266 running at 80MHz SPI so slow things down

#define BUFFER_SIZE 64
void spiWriteBlock(uint16_t color, uint32_t repeat)
{

  uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color};
  SPI.writePattern(&colorBin[0], 2, repeat);

}

#else // Low level register based ESP32 code

#include "soc/spi_reg.h"
#define SPI_NUM 0x3

void spiWriteBlock(uint16_t color, uint32_t repeat)
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

// The following touch screen support code by maxpautsch was merged 1/10/17
// https://github.com/maxpautsch
// Define TOUCH_CS is the user setup file to enable this code
// A demo is provided in examples Generic folder
// Additions by Bodmer to double sample and use Z value to improve detection reliability
#ifdef TOUCH_CS
/***************************************************************************************
** Function name:           getTouchRaw
** Description:             read raw touch position. Return false if not pressed. 
***************************************************************************************/
uint8_t TFT_eSPI::getTouchRaw(uint16_t *x, uint16_t *y){
  uint16_t tmp;
  CS_H;

  spi_begin_touch();

  T_CS_L;
  
  // Start bit + YP sample request for x position
  tmp = SPI.transfer(0xd0);
  tmp = SPI.transfer(0);
  tmp = tmp <<5;
  tmp |= 0x1f & (SPI.transfer(0)>>3);

  *x = tmp;

  // Start bit + XP sample request for y position
  SPI.transfer(0x90);
  tmp = SPI.transfer(0);
  tmp = tmp <<5;
  tmp |= 0x1f & (SPI.transfer(0)>>3);

  *y = tmp;

  T_CS_H;

  spi_end_touch();

  return true;
}

/***************************************************************************************
** Function name:           getTouchRawZ
** Description:             read raw pressure on touchpad and return Z value. 
***************************************************************************************/
uint16_t TFT_eSPI::getTouchRawZ(void){
  CS_H;

  spi_begin_touch();

  T_CS_L;

  // Z sample request
  uint16_t tz = 0xFFF;
  SPI.transfer(0xb1);
  tz += SPI.transfer16(0xc1) >> 3;
  tz -= SPI.transfer16(0x91) >> 3;
  
  T_CS_H;

  spi_end_touch();

  return tz;
}

/***************************************************************************************
** Function name:           validTouch
** Description:             read validated position. Return false if not pressed. 
***************************************************************************************/
#define _RAWERR 10 // Deadband in position samples
uint8_t TFT_eSPI::validTouch(uint16_t *x, uint16_t *y, uint16_t threshold){
  uint16_t x_tmp, y_tmp, x_tmp2, y_tmp2;

  // Wait until pressure stops increasing
  uint16_t z1 = 1;
  uint16_t z2 = 0;
  while (z1 > z2)
  {
    z2 = z1;
    z1 = getTouchRawZ();
    delay(1);
  }

  //  Serial.print("Z = ");Serial.println(z1);

  if (z1 <= threshold) return false;
    
  getTouchRaw(&x_tmp,&y_tmp);

  //  Serial.print("Sample 1 x,y = "); Serial.print(x_tmp);Serial.print(",");Serial.print(y_tmp);
  //  Serial.print(", Z = ");Serial.println(z1);

  delay(1); // Small delay to the next sample
  if (getTouchRawZ() <= threshold) return false;

  delay(2); // Small delay to the next sample
  getTouchRaw(&x_tmp2,&y_tmp2);
  
  //  Serial.print("Sample 2 x,y = "); Serial.print(x_tmp2);Serial.print(",");Serial.println(y_tmp2);
  //  Serial.print("Sample difference = ");Serial.print(abs(x_tmp - x_tmp2));Serial.print(",");Serial.println(abs(y_tmp - y_tmp2));

  if (abs(x_tmp - x_tmp2) > _RAWERR) return false;
  if (abs(y_tmp - y_tmp2) > _RAWERR) return false;
  
  *x = x_tmp;
  *y = y_tmp;
  
  return true;
}
  
/***************************************************************************************
** Function name:           getTouch
** Description:             read callibrated position. Return false if not pressed. 
***************************************************************************************/
#define Z_THRESHOLD 350 // Touch pressure threshold for validating touches
uint8_t TFT_eSPI::getTouch(uint16_t *x, uint16_t *y, uint16_t threshold){
  uint16_t x_tmp, y_tmp, xx, yy;
  
  if (threshold<20) threshold = 20;
  if (_pressTime > millis()) threshold=20;

  uint8_t n = 5;
  uint8_t valid = 0;
  while (n--)
  {
    if (validTouch(&x_tmp, &y_tmp, threshold)) valid++;;
  }

  if (valid<1) { _pressTime = 0; return false; }
  
  _pressTime = millis() + 50;

  if(!touchCalibration_rotate){
    xx=(x_tmp-touchCalibration_x0)*_width/touchCalibration_x1;
    yy=(y_tmp-touchCalibration_y0)*_height/touchCalibration_y1;
    if(touchCalibration_invert_x)
      xx = _width - xx;
    if(touchCalibration_invert_y)
      yy = _height - yy;
  } else {
    yy=(x_tmp-touchCalibration_x0)*_height/touchCalibration_x1;
    xx=(y_tmp-touchCalibration_y0)*_width/touchCalibration_y1;
    if(touchCalibration_invert_x)
      xx = _width - xx;
    if(touchCalibration_invert_y)
      yy = _height - yy;
  }

  if (xx >= _width || yy >= _height) return valid;

  _pressX = xx;
  _pressY = yy;
  *x = _pressX;
  *y = _pressY;
  return valid;
}

/***************************************************************************************
** Function name:           calibrateTouch
** Description:             generates calibration parameters for touchscreen. 
***************************************************************************************/
void TFT_eSPI::calibrateTouch(uint16_t *parameters, uint32_t color_fg, uint32_t color_bg, uint8_t size){
  int16_t values[] = {0,0,0,0,0,0,0,0};
  uint16_t x_tmp, y_tmp;



  for(uint8_t i = 0; i<4; i++){
    fillRect(0, 0, size+1, size+1, color_bg);
    fillRect(0, _height-size-1, size+1, size+1, color_bg);
    fillRect(_width-size-1, 0, size+1, size+1, color_bg);
    fillRect(_width-size-1, _height-size-1, size+1, size+1, color_bg);

    if (i == 5) break; // used to clear the arrows
    
    switch (i) {
      case 0: // up left
        drawLine(0, 0, 0, size, color_fg);
        drawLine(0, 0, size, 0, color_fg);
        drawLine(0, 0, size , size, color_fg);
        break;
      case 1: // bot left
        drawLine(0, _height-size-1, 0, _height-1, color_fg);
        drawLine(0, _height-1, size, _height-1, color_fg);
        drawLine(size, _height-size-1, 0, _height-1 , color_fg);
        break;
      case 2: // up right
        drawLine(_width-size-1, 0, _width-1, 0, color_fg);
        drawLine(_width-size-1, size, _width-1, 0, color_fg);
        drawLine(_width-1, size, _width-1, 0, color_fg);
        break;
      case 3: // bot right
        drawLine(_width-size-1, _height-size-1, _width-1, _height-1, color_fg);
        drawLine(_width-1, _height-1-size, _width-1, _height-1, color_fg);
        drawLine(_width-1-size, _height-1, _width-1, _height-1, color_fg);
        break;
      }

    // user has to get the chance to release
    if(i>0) delay(1000);

    for(uint8_t j= 0; j<8; j++){
      // Use a lower detect threshold as corners tend to be less sensitive
      while(!validTouch(&x_tmp, &y_tmp, Z_THRESHOLD/2)) delay(10);
      values[i*2  ] += x_tmp;
      values[i*2+1] += y_tmp;
      }
    values[i*2  ] /= 8;
    values[i*2+1] /= 8;
  }



  // check orientation
  // from case 0 to case 1, the y value changed. 
  // If the meassured delta of the touch x axis is bigger than the delta of the y axis, the touch and TFT axes are switched.
  touchCalibration_rotate = false;
  if(abs(values[0]-values[2]) > abs(values[1]-values[3])){
    touchCalibration_rotate = true;
    touchCalibration_x1 = (values[0] + values[4])/2; // calc min x
    touchCalibration_x0 = (values[2] + values[6])/2; // calc max x
    touchCalibration_y1 = (values[1] + values[3])/2; // calc min y
    touchCalibration_y0 = (values[5] + values[7])/2; // calc max y
  } else {
    touchCalibration_x0 = (values[0] + values[2])/2; // calc min x
    touchCalibration_x1 = (values[4] + values[6])/2; // calc max x
    touchCalibration_y0 = (values[1] + values[5])/2; // calc min y
    touchCalibration_y1 = (values[3] + values[7])/2; // calc max y
  }

  // in addition, the touch screen axis could be in the opposit direction of the TFT axis
  touchCalibration_invert_x = false;
  if(touchCalibration_x0 > touchCalibration_x1){
    values[0]=touchCalibration_x0;
    touchCalibration_x0 = touchCalibration_x1;
    touchCalibration_x1 = values[0];
    touchCalibration_invert_x = true;
  }
  touchCalibration_invert_y = false;
  if(touchCalibration_y0 > touchCalibration_y1){
    values[0]=touchCalibration_y0;
    touchCalibration_y0 = touchCalibration_y1;
    touchCalibration_y1 = values[0];
    touchCalibration_invert_y = true;
  }

  // pre calculate
  touchCalibration_x1 -= touchCalibration_x0;
  touchCalibration_y1 -= touchCalibration_y0;

  if(touchCalibration_x0 == 0) touchCalibration_x0 = 1;
  if(touchCalibration_x1 == 0) touchCalibration_x1 = 1;
  if(touchCalibration_y0 == 0) touchCalibration_y0 = 1;
  if(touchCalibration_y1 == 0) touchCalibration_y1 = 1;

  // export parameters, if pointer valid
  if(parameters != NULL){
    parameters[0] = touchCalibration_x0;
    parameters[1] = touchCalibration_x1;
    parameters[2] = touchCalibration_y0;
    parameters[3] = touchCalibration_y1;
    parameters[4] = touchCalibration_rotate | (touchCalibration_invert_x <<1) | (touchCalibration_invert_y <<2);
  }
}


/***************************************************************************************
** Function name:           setTouch
** Description:             imports calibration parameters for touchscreen. 
***************************************************************************************/
void TFT_eSPI::setTouch(uint16_t *parameters){
  touchCalibration_x0 = parameters[0];
  touchCalibration_x1 = parameters[1];
  touchCalibration_y0 = parameters[2];
  touchCalibration_y1 = parameters[3];

  if(touchCalibration_x0 == 0) touchCalibration_x0 = 1;
  if(touchCalibration_x1 == 0) touchCalibration_x1 = 1;
  if(touchCalibration_y0 == 0) touchCalibration_y0 = 1;
  if(touchCalibration_y1 == 0) touchCalibration_y1 = 1;

  touchCalibration_rotate = parameters[4] & 0x01;
  touchCalibration_invert_x = parameters[4] & 0x02;
  touchCalibration_invert_y = parameters[4] & 0x04;
}

#else // TOUCH CS is not defined so generate dummy functions that do nothing
    
/***************************************************************************************
** Function name:           getTouchRaw
** Description:             read raw touch position. Return false if not pressed. 
***************************************************************************************/
uint8_t TFT_eSPI::getTouchRaw(uint16_t *x, uint16_t *y){
  return true;
}

/***************************************************************************************
** Function name:           getTouchRawZ
** Description:             read raw pressure on touchpad and return Z value. 
***************************************************************************************/
uint16_t TFT_eSPI::getTouchRawZ(void){
  return true;
}

/***************************************************************************************
** Function name:           validTouch
** Description:             read validated position. Return false if not pressed. 
***************************************************************************************/
uint8_t TFT_eSPI::validTouch(uint16_t *x, uint16_t *y, uint16_t threshold){
  return true;
}
  
/***************************************************************************************
** Function name:           getTouch
** Description:             read callibrated position. Return false if not pressed. 
***************************************************************************************/
uint8_t TFT_eSPI::getTouch(uint16_t *x, uint16_t *y, uint16_t threshold){
  return true;
}

/***************************************************************************************
** Function name:           calibrateTouch
** Description:             generates calibration parameters for touchscreen. 
***************************************************************************************/
void TFT_eSPI::calibrateTouch(uint16_t *parameters, uint32_t color_bg, uint32_t color_fg, uint8_t size){
}


/***************************************************************************************
** Function name:           setTouch
** Description:             imports calibration parameters for touchscreen. 
***************************************************************************************/
void TFT_eSPI::setTouch(uint16_t *parameters){
}

#endif // TOUCH_CS



/***************************************************************************************
** Code for the GFX button UI element
** Grabbed from Adafruit_GFX library and enhanced to handle any label font
***************************************************************************************/
TFT_eSPI_Button::TFT_eSPI_Button(void) {
  _gfx = 0;
}

// Classic initButton() function: pass center & size
void TFT_eSPI_Button::initButton(
 TFT_eSPI *gfx, int16_t x, int16_t y, uint16_t w, uint16_t h,
 uint16_t outline, uint16_t fill, uint16_t textcolor,
 char *label, uint8_t textsize)
{
  // Tweak arguments and pass to the newer initButtonUL() function...
  initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill,
    textcolor, label, textsize);
}

// Newer function instead accepts upper-left corner & size
void TFT_eSPI_Button::initButtonUL(
 TFT_eSPI *gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h,
 uint16_t outline, uint16_t fill, uint16_t textcolor,
 char *label, uint8_t textsize)
{
  _x1           = x1;
  _y1           = y1;
  _w            = w;
  _h            = h;
  _outlinecolor = outline;
  _fillcolor    = fill;
  _textcolor    = textcolor;
  _textsize     = textsize;
  _gfx          = gfx;
  strncpy(_label, label, 9);
}

void TFT_eSPI_Button::drawButton(boolean inverted) {
  uint16_t fill, outline, text;

  if(!inverted) {
    fill    = _fillcolor;
    outline = _outlinecolor;
    text    = _textcolor;
  } else {
    fill    = _textcolor;
    outline = _outlinecolor;
    text    = _fillcolor;
  }

  uint8_t r = min(_w, _h) / 4; // Corner radius
  _gfx->fillRoundRect(_x1, _y1, _w, _h, r, fill);
  _gfx->drawRoundRect(_x1, _y1, _w, _h, r, outline);

  _gfx->setTextColor(text);
  _gfx->setTextSize(_textsize);

  uint8_t tempdatum = _gfx->getTextDatum();
  _gfx->setTextDatum(MC_DATUM);
  _gfx->drawString(_label, _x1 + (_w/2), _y1 + (_h/2));
  _gfx->setTextDatum(tempdatum);
}

boolean TFT_eSPI_Button::contains(int16_t x, int16_t y) {
  return ((x >= _x1) && (x < (_x1 + _w)) &&
          (y >= _y1) && (y < (_y1 + _h)));
}

void TFT_eSPI_Button::press(boolean p) {
  laststate = currstate;
  currstate = p;
}

boolean TFT_eSPI_Button::isPressed()    { return currstate; }
boolean TFT_eSPI_Button::justPressed()  { return (currstate && !laststate); }
boolean TFT_eSPI_Button::justReleased() { return (!currstate && laststate); }



 /**************************************************************************************
// The following class creates Sprites in RAM, graphics can then be drawn in the Sprite
// and rendered quickly onto the TFT screen. The class inherits the graphics functions
// from the TFT_eSPI class. Some functions are overridden by this class so that the
// graphics are written to the Sprite rather than the TFT.
// Coded by Bodmer
***************************************************************************************/
/***************************************************************************************
// Color bytes are swapped when writing to RAM, this introduces a small overhead but
// then rendering to screen can use the faster call. In general rendering graphics in
// the Sprite is very fast, but writing to the TFT is slow, so there is a performance
// gain by using swapped bytes.
***************************************************************************************/

/***************************************************************************************
** Function name:           TFT_eSprite
** Description:             Class constructor
*************************************************************************************x*/
TFT_eSprite::TFT_eSprite(TFT_eSPI *tft)
{
  _tft = tft;     // Pointer to tft class so we can call member functions

  _iwidth    = 0; // Initialise width and height to 0 (it does not exist yet)
  _iheight   = 0;
  _bpp16 = true;
  _iswapBytes = false;   // Do not swap pushImage colour bytes by default

  _created = false;

  _xs = 0;  // window bounds for pushColor
  _ys = 0;
  _xe = 0;
  _ye = 0;

  _xptr = 0; // pushColor coordinate
  _yptr = 0;

  _icursor_y  = _icursor_x    = 0; // Text cursor position
}


/***************************************************************************************
** Function name:           createSprite
** Description:             Create a sprite (bitmap) of defined width and height
*************************************************************************************x*/
// returns a uint8_t* pointer, cast returned value to (uint16_t*) for 16 bit colours
uint8_t* TFT_eSprite::createSprite(int16_t w, int16_t h)
{

  if ( _created )
  {
    if ( _bpp16 ) return ( uint8_t*)_img;
    return _img8;
  }

  if ( w < 1 || h < 1 ) return NULL;

  _iwidth    = w;
  _iheight   = h;

  _sx = 0;
  _sy = 0;
  _sw = w;
  _sh = h;
  _scolor = TFT_BLACK;

  // Add one extra "off screen" pixel to point out-of-bounds setWindow() coordinates
  // this means push/writeColor functions do not need additional bounds checks.
  if(_bpp16)
  {
    _img = (uint16_t*) malloc(w * h * 2 + 1);
    if (_img)
    {
      _created = true;
      fillSprite(TFT_BLACK);
      return (uint8_t*)_img;
    }
  }
  else
  {
    _img8 = ( uint8_t*) malloc(w * h + 1);
    if (_img8)
    {
      _created = true;
      fillSprite(TFT_BLACK);
      return _img8;
    }
  }
  
  return NULL;
}


/***************************************************************************************
** Function name:           setDepth
** Description:             Set bits per pixel for colour (8 or 16)
*************************************************************************************x*/

uint8_t* TFT_eSprite::setColorDepth(int8_t b)
{
  // Can't change an existing sprite's colour depth so delete it
  if (_created)
  {
    if (_bpp16) free(_img);
    else        free(_img8);
  }

  // Now define the new colour depth
  if ( b > 8 ) _bpp16 = true;  // Bytes per pixel
  else         _bpp16 = false;

  // If it existed, re-create the sprite with the new colour depth
  if (_created)
  {
    _created = false;
    return createSprite(_iwidth, _iheight);
  }

  return NULL;
}


/***************************************************************************************
** Function name:           deleteSprite
** Description:             Delete the sprite to free up memory (RAM)
*************************************************************************************x*/
void TFT_eSprite::deleteSprite(void)
{
  if (!_created ) return;

  if (_bpp16) free(_img);
  else        free(_img8);

  _created = false;
}


/***************************************************************************************
** Function name:           pushSprite
** Description:             Push the sprite to the TFT at x, y
*************************************************************************************x*/
void TFT_eSprite::pushSprite(int32_t x, int32_t y)
{
  if (!_created ) return;

  if (_bpp16) _tft->pushImage(x, y, _iwidth, _iheight, _img );
  else        _tft->pushImage(x, y, _iwidth, _iheight, _img8);
}


/***************************************************************************************
** Function name:           pushSprite
** Description:             Push the sprite to the TFT at x, y with transparent colour
*************************************************************************************x*/
void TFT_eSprite::pushSprite(int32_t x, int32_t y, uint16_t transp)
{
  if (!_created ) return;

  if (_bpp16) _tft->pushImage(x, y, _iwidth, _iheight, _img, transp );
  else
  {
    transp = (uint8_t)((transp & 0xE000)>>8 | (transp & 0x0700)>>6 | (transp & 0x0018)>>3);
    _tft->pushImage(x, y, _iwidth, _iheight, _img8, (uint8_t)transp);
  }
}


/***************************************************************************************
** Function name:           readPixel
** Description:             Read 565 colour of a pixel at defined coordinates
*************************************************************************************x*/
uint16_t TFT_eSprite::readPixel(int32_t x, int32_t y)
{
  if (!_created ) return 0;

  if (_bpp16)
  {
    uint16_t color = _img[x + y * _iwidth];
    return (color >> 8) | (color << 8);
  }

  uint16_t color = _img8[x + y * _iwidth];
  if (color != 0) {
      uint8_t  blue[] = {0, 11, 21, 31};
      color =     (color & 0xE0)<<8 | (color & 0xC0)<<5
                | (color & 0x1C)<<6 | (color & 0x1C)<<3
                | blue[color & 0x03];
  }

  return color;
}


/***************************************************************************************
** Function name:           pushImage
** Description:             push 565 colour image into a defined area of a sprite 
*************************************************************************************x*/
void  TFT_eSprite::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, uint16_t *data)
{
  if ((x > _iwidth) || (y > _iheight) || (w == 0) || (h == 0) || !_created) return;

  if (_bpp16)
  {
    for (uint32_t yp = y; yp < y + h; yp++)
    {
      for (uint32_t xp = x; xp < x + w; xp++)
      {
        uint16_t color =  *data++;
        if(!_iswapBytes) color = color<<8 | color>>8;
        _img[xp + yp * _iwidth] = color;
      }
    }
  }
  else
  {
    for (uint32_t yp = y; yp < y + h; yp++)
    {
      for (uint32_t xp = x; xp < x + w; xp++)
      {
        uint16_t color = *data++;
        if(_iswapBytes) color = color<<8 | color>>8;
        _img8[xp + yp * _iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
      }
    }
  }
}


/***************************************************************************************
** Function name:           pushImage
** Description:             push 565 colour FLASH (PROGMEM) image into a defined area
*************************************************************************************x*/
void  TFT_eSprite::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint16_t *data)
{
  if ((x > _iwidth) || (y > _iheight) || (w == 0) || (h == 0) || !_created) return;

  if (_bpp16)
  {
    for (uint32_t yp = y; yp < y + h; yp++)
    {
      for (uint32_t xp = x; xp < x + w; xp++)
      {
        uint16_t color = pgm_read_word(data++);
        if(!_iswapBytes) color = color<<8 | color>>8;
        _img[xp + yp * _iwidth] = color;
      }
    }
  }
  else
  {
    for (uint32_t yp = y; yp < y + h; yp++)
    {
      for (uint32_t xp = x; xp < x + w; xp++)
      {
        uint16_t color = pgm_read_word(data++);
        if(_iswapBytes) color = color<<8 | color>>8;
        _img8[xp + yp * _iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
      }
    }
  }
}


/***************************************************************************************
** Function name:           setSwapBytes
** Description:             Used by 16 bit pushImage() to swap byte order in colours
***************************************************************************************/
void TFT_eSprite::setSwapBytes(bool swap)
{
  _iswapBytes = swap;
}


/***************************************************************************************
** Function name:           getSwapBytes
** Description:             Return the swap byte order for colours
***************************************************************************************/
bool TFT_eSprite::getSwapBytes(void)
{
  return _iswapBytes;
}


/***************************************************************************************
** Function name:           setWindow
** Description:             Set the bounds of a window for pushColor and writeColor
*************************************************************************************x*/
void TFT_eSprite::setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
  bool duff_coord = false;

  if (x0 > x1) swap_coord(x0, x1);
  if (y0 > y1) swap_coord(y0, y1);

  if (x0 < 0) x0 = 0;
  if (x0 >= _iwidth) duff_coord = true;
  if (x1 < 0) x1 = 0;
  if (x1 >= _iwidth) x1 = _iwidth - 1;

  if (y0 < 0) y0 = 0;
  if (y0 >= _iheight) duff_coord = true;
  if (y1 < 0) y1 = 0;
  if (y1 >= _iheight) y1 = _iheight - 1;

  if (duff_coord)
  { // Point to that extra "off screen" pixel
    _xs = 0;
    _ys = _iheight;
    _xe = 0;
    _ye = _iheight;
  }
  else
  {
    _xs = x0;
    _ys = y0;
    _xe = x1;
    _ye = y1;
  }

  _xptr = _xs;
  _yptr = _ys;
}


/***************************************************************************************
** Function name:           pushColor
** Description:             Send a new pixel to the set window
*************************************************************************************x*/
void TFT_eSprite::pushColor(uint32_t color)
{
  if (!_created ) return;

  // Write the colour to RAM in set window
  if (_bpp16)
    _img [_xptr + _yptr * _iwidth] = (uint16_t) (color >> 8) | (color << 8);

  else
    _img8[_xptr + _yptr * _iwidth] = (uint8_t )((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);

  // Increment x
  _xptr++;

  // Wrap on x and y to start, increment y if needed
  if (_xptr > _xe)
    {
	  _xptr = _xs;
	  _yptr++;
      if (_yptr > _ye) _yptr = _ys;
    }

}


/***************************************************************************************
** Function name:           pushColor
** Description:             Send a "len" new pixels to the set window
*************************************************************************************x*/
void TFT_eSprite::pushColor(uint32_t color, uint16_t len)
{
  if (!_created ) return;

  uint16_t pixelColor;
  if (_bpp16)
    pixelColor = (uint16_t) (color >> 8) | (color << 8);

  else
    pixelColor = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;

  while(len--) writeColor(pixelColor);
}


/***************************************************************************************
** Function name:           writeColor
** Description:             Write a pixel with pre-formatted colour to the set window
*************************************************************************************x*/
void TFT_eSprite::writeColor(uint16_t color)
{
  if (!_created ) return;

  // Write 16 bit RGB 565 encoded colour to RAM
  if (_bpp16) _img [_xptr + _yptr * _iwidth] = color;

  // Write 8 bit RGB 332 encoded colour to RAM
  else _img8[_xptr + _yptr * _iwidth] = (uint8_t) color;

  // Increment x
  _xptr++;

  // Wrap on x and y to start, increment y if needed
  if (_xptr > _xe)
  {
	_xptr = _xs;
	_yptr++;
    if (_yptr > _ye) _yptr = _ys;
  }
}


/***************************************************************************************
** Function name:           setScrollRect
** Description:             Set scroll area within the sprite and the gap fill colour
*************************************************************************************x*/
void TFT_eSprite::setScrollRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint16_t color)
{
  if ((x >= _iwidth) || (y >= _iheight) || !_created ) return;

  if (x < 0) x = 0;
  if (y < 0) y = 0;

  if ((x + w) > _iwidth ) w = _iwidth  - x;
  if ((y + h) > _iheight) h = _iheight - y;

  if ( w < 1 || h < 1) return;  

  _sx = x;
  _sy = y;
  _sw = w;
  _sh = h;

  _scolor = color;
}


/***************************************************************************************
** Function name:           scroll
** Description:             Scroll dx,dy pixels, positive right,down, negative left,up
*************************************************************************************x*/
void TFT_eSprite::scroll(int16_t dx, int16_t dy)
{
  if (abs(dx) >= _sw || abs(dy) >= _sh)
  {
    fillRect (_sx, _sy, _sw, _sh, _scolor);
    return;
  }

  // Fetch the scroll area width and height set by setScrollRect()
  uint32_t w  = _sw - abs(dx); // line width to copy
  uint32_t h  = _sh - abs(dy); // lines to copy
  int32_t iw  = _iwidth;       // width of sprite

  // Fetch the x,y origin set by setScrollRect()
  uint32_t tx = _sx; // to x
  uint32_t fx = _sx; // from x
  uint32_t ty = _sy; // to y
  uint32_t fy = _sy; // from y

  // Adjust for x delta
  if (dx <= 0) fx -= dx;
  else tx += dx;

  // Adjust for y delta
  if (dy <= 0) fy -= dy;
  else
  { // Scrolling down so start copy from bottom
    ty = ty + _sh - 1; // "To" pointer
    iw = -iw;          // Pointer moves backwards
    fy = ty - dy;      // "From" pointer
  }

  // Calculate "from y" and "to y" pointers in RAM
  uint32_t fyp = fx + fy * _iwidth;
  uint32_t typ = tx + ty * _iwidth;

  // Now move the pixels in RAM
  if (_bpp16)
  {
    while (h--)
    { // move pixel lines (to, from, byte count)
      memmove( _img + typ, _img + fyp, w<<1);
      typ += iw;
      fyp += iw;
    }
  }
  else
  {
    while (h--)
    { // move pixel lines (to, from, byte count)
      memmove( _img8 + typ, _img8 + fyp, w);
      typ += iw;
      fyp += iw;
    }
  }

  // Fill the gap left by the scrolling
  if (dx > 0) fillRect(_sx, _sy, dx, _sh, _scolor);
  if (dx < 0) fillRect(_sx + _sw + dx, _sy, -dx, _sh, _scolor);
  if (dy > 0) fillRect(_sx, _sy, _sw, dy, _scolor);
  if (dy < 0) fillRect(_sx, _sy + _sh + dy, _sw, -dy, _scolor);
}


/***************************************************************************************
** Function name:           fillSprite
** Description:             Fill the whole sprite with defined colour
*************************************************************************************x*/
void TFT_eSprite::fillSprite(uint32_t color)
{
  if (!_created ) return;

  // Use memset if possible as it is super fast
  if(( (uint8_t)color == (uint8_t)(color>>8) ) && _bpp16)
                    memset(_img,  (uint8_t)color, _iwidth * _iheight * 2);
  else if (!_bpp16)
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    memset(_img8, (uint8_t)color, _iwidth * _iheight);
  }

  else fillRect(0, 0, _iwidth, _iheight, color);
}


/***************************************************************************************
** Function name:           setCursor
** Description:             Set the sprite text cursor x,y position
*************************************************************************************x*/
void TFT_eSprite::setCursor(int16_t x, int16_t y)
{
  _icursor_x = x;
  _icursor_y = y;
}


/***************************************************************************************
** Function name:           width
** Description:             Return the width of sprite
*************************************************************************************x*/
// Return the size of the display
int16_t TFT_eSprite::width(void)
{
  if (!_created ) return 0;
  return _iwidth;
}


/***************************************************************************************
** Function name:           height
** Description:             Return the height of sprite
*************************************************************************************x*/
int16_t TFT_eSprite::height(void)
{
  if (!_created ) return 0;
  return _iheight;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a single character in the Adafruit GLCD or freefont
*************************************************************************************x*/
void TFT_eSprite::drawChar(int32_t x, int32_t y, unsigned char c, uint32_t color, uint32_t bg, uint8_t size)
{
  if (!_created ) return;

  if ((x >= _iwidth)            || // Clip right
      (y >= _iheight)           || // Clip bottom
      ((x + 6 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
    return;

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

    for (int8_t i = 0; i < 5; i++ ) column[i] = pgm_read_byte(font + (c * 5) + i);
    column[5] = 0;

    int8_t j, k;
    for (j = 0; j < 8; j++) {
      for (k = 0; k < 5; k++ ) {
        if (column[k] & mask) {
          drawPixel(x + k, y + j, color);
        }
        else {
          drawPixel(x + k, y + j, bg);
        }
      }

      mask <<= 1;

      drawPixel(x + k, y + j, bg);
    }
  }
  else
  {
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
              if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
              else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
              hpc=0;
            }
          }
          bit >>= 1;
        }
        // Draw pixels for this line as we are about to increment yy
        if (hpc) {
          if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
          else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
          hpc=0;
        }
      }
    }
#endif


#ifdef LOAD_GLCD
  #ifdef LOAD_GFXFF
  } // End classic vs custom font
  #endif
#endif

}


/***************************************************************************************
** Function name:           drawPixel
** Description:             push a single pixel at an arbitrary position
*************************************************************************************x*/
void TFT_eSprite::drawPixel(uint32_t x, uint32_t y, uint32_t color)
{
  // x and y are unsigned so that -ve coordinates turn into large positive ones
  // this make bounds checking a bit faster
  if ((x >= _iwidth) || (y >= _iheight) || !_created) return;

  if (_bpp16)
  {
    color = (color >> 8) | (color << 8);
    _img[x+y*_iwidth] = (uint16_t) color;
  }
  else
  {
    _img8[x+y*_iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
  }
}


/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
*************************************************************************************x*/
void TFT_eSprite::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
  if (!_created ) return;

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
}


/***************************************************************************************
** Function name:           drawFastVLine
** Description:             draw a vertical line
*************************************************************************************x*/
void TFT_eSprite::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color)
{

  if ((x < 0) || (x >= _iwidth) || (y >= _iheight) || !_created) return;

  if (y < 0) { h += y; y = 0; }

  if ((y + h) > _iheight) h = _iheight - y;

  if (h < 1) return;

  if (_bpp16)
  {
    color = (color >> 8) | (color << 8);
    int32_t yp = x + _iwidth * y;
    while (h--) {_img[yp] = (uint16_t) color; yp += _iwidth;}
  }
  else
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    while (h--) _img8[x + _iwidth * y++] = (uint8_t) color;
  }
}


/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
*************************************************************************************x*/
void TFT_eSprite::drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color)
{

  if ((y < 0) || (x >= _iwidth) || (y >= _iheight) || !_created) return;

  if (x < 0) { w += x; x = 0; }

  if ((x + w) > _iwidth)  w = _iwidth  - x;

  if (w < 1) return;

  if (_bpp16)
  {
    color = (color >> 8) | (color << 8);
    while (w--) _img[_iwidth * y + x++] = (uint16_t) color;
  }
  else
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    memset(_img8+_iwidth * y + x, (uint8_t)color, w);
  }
}


/***************************************************************************************
** Function name:           fillRect
** Description:             draw a filled rectangle
*************************************************************************************x*/
void TFT_eSprite::fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color)
{
  if (!_created ) return;

  if (x < 0) { w += x; x = 0; }

  if ((x < 0) || (y < 0) || (x >= _iwidth) || (y >= _iheight)) return;
  if ((x + w) > _iwidth)  w = _iwidth  - x;
  if ((y + h) > _iheight) h = _iheight - y;
  if ((w < 1) || (h < 1)) return;

  int32_t yp = _iwidth * y + x;

  if (_bpp16)
  {
    color = (color >> 8) | (color << 8);
    uint32_t iw = w;
    int32_t ys = yp;
    if(h--)  {while (iw--) _img[yp++] = (uint16_t) color;}
    yp = ys;
    while (h--)
    {
      yp += _iwidth;
      memcpy( _img+yp, _img+ys, w<<1);
    }
  }
  else
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    while (h--)
    {
	  memset(_img8 + yp, (uint8_t)color, w);
      yp += _iwidth;
    }
  }
}


/***************************************************************************************
** Function name:           write
** Description:             draw characters piped through serial stream
*************************************************************************************x*/
size_t TFT_eSprite::write(uint8_t utf8)
{
  if (!_created ) return 0;

  if (utf8 == '\r') return 1;

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
    width = width * 8;        // Width converted back to pixles
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

  if (utf8 == '\n') 
  {
    _icursor_y += height;
    _icursor_x  = 0;
  }
  else
  {
    if (textwrap && (_icursor_x + width * textsize > _iwidth))
    {
      _icursor_y += height;
      _icursor_x = 0;
    }
    if (_icursor_y >= _iheight) _icursor_y = 0;
    _icursor_x += drawChar(uniCode, _icursor_x, _icursor_y, textfont);
  }

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  } // Custom GFX font
  else
  {

    if(utf8 == '\n') {
      _icursor_x  = 0;
      _icursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else {
      if (uniCode > (uint8_t)pgm_read_byte(&gfxFont->last )) return 0;
      if (uniCode < (uint8_t)pgm_read_byte(&gfxFont->first)) return 0;

      uint8_t   c2    = uniCode - pgm_read_byte(&gfxFont->first);
      GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
      uint8_t   w     = pgm_read_byte(&glyph->width),
                h     = pgm_read_byte(&glyph->height);
      if((w > 0) && (h > 0)) { // Is there an associated bitmap?
        int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);
        if(textwrap && ((_icursor_x + textsize * (xo + w)) > _iwidth)) {
          // Drawing character would go off right edge; wrap to new line
          _icursor_x  = 0;
          _icursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        }
        if (_icursor_y >= _iheight) _icursor_y = 0;
        drawChar(_icursor_x, _icursor_y, uniCode, textcolor, textbgcolor, textsize);
      }
      _icursor_x += pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
    }
  }
#endif // LOAD_GFXFF
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  return 1;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a unicode onto the screen
*************************************************************************************x*/
int16_t TFT_eSprite::drawChar(unsigned int uniCode, int x, int y)
{
  return drawChar(uniCode, x, y, textfont);
}

int16_t TFT_eSprite::drawChar(unsigned int uniCode, int x, int y, int font)
{
  if (!_created ) return 0;

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
    if (x + width * textsize >= _iwidth) return width * textsize ;

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
  }

  #ifdef LOAD_RLE
  else
  #endif
#endif  //FONT2

#ifdef LOAD_RLE  //674 bytes of code
  // Font is not 2 and hence is RLE encoded
  {
    w *= height; // Now w is total number of pixels in the character

    if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize * height, textbgcolor);
    int16_t color;
    if (_bpp16) color = (textcolor >> 8) | (textcolor << 8);
    else color = ((textcolor & 0xE000)>>8 | (textcolor & 0x0700)>>6 | (textcolor & 0x0018)>>3);
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
      flash_address++; // 20 bytes smaller by incrementing here
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
        while (line--) {
          pc++;
          setWindow(px, py, px + ts, py + ts);
          if (ts) { tnp = np; while (tnp--) writeColor(color); }
          else    writeColor(color);

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
  }
  // End of RLE font rendering
#endif
  return width * textsize;    // x +
}

