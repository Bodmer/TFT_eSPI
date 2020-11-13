        ////////////////////////////////////////////////////
        //  TFT_eSPI driver functions for K210 processor  //
        //  based on https://github.com/fukuen/TFT_eSPI   //
        ////////////////////////////////////////////////////

#include "TFT_eSPI_K210.h"

////////////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////////////

// Select the SPI port to use
#if defined (M5STICK_V)
  SPIClass spi_(SPI0, TFT_SCLK, TFT_MISO, TFT_MOSI, -1, SPI_FREQUENCY);
#else
  SPIClass spi_(SPI0);
#endif

////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_SDA_READ) && !defined (TFT_PARALLEL_8_BIT)
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           tft_Read_8
** Description:             Bit bashed SPI to read bidirectional SDA line
***************************************************************************************/
uint8_t TFT_eSPI::tft_Read_8(void)
{
  uint8_t  ret = 0;

  for (uint8_t i = 0; i < 8; i++) {  // read results
    ret <<= 1;
    SCLK_L;
    if (digitalRead(TFT_MOSI)) ret |= 1;
    SCLK_H;
  }

  return ret;
}

/***************************************************************************************
** Function name:           beginSDA
** Description:             Detach SPI from pin to permit software SPI
***************************************************************************************/
void TFT_eSPI::begin_SDA_Read(void)
{
  // Release configured SPI port for SDA read
  spi_.end();
}

/***************************************************************************************
** Function name:           endSDA
** Description:             Attach SPI pins after software SPI
***************************************************************************************/
void TFT_eSPI::end_SDA_Read(void)
{
  // Configure SPI port ready for next TFT access
  spi_.begin();
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // #if defined (TFT_SDA_READ)
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
#if !defined (RPI_WRITE_STROBE) && !defined (ILI9488_DRIVER)  // SPI 16 bit colour TFT
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for K210
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){

  uint32_t data = ((uint32_t)color << 16) | (uint32_t)color;
  tft_fill_data(&data, len);
}

/***************************************************************************************
** Function name:           pushPixels - for K210
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  tft_write_byte((uint8_t*)data, len * 2);
}

////////////////////////////////////////////////////////////////////////////////////////
#elif defined (RPI_WRITE_STROBE) // Code for RPi TFT with write strobe #### UNTESTED #####
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for K210 RPi TFT
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){

  if(len) { tft_Write_16(color); len--; }
  while(len--) {WR_L; WR_H;}
}

/***************************************************************************************
** Function name:           pushPixels - for K210 RPi TFT
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len)
{
  uint16_t *data = (uint16_t*)data_in;

  if (_swapBytes) while ( len-- ) {tft_Write_16S(*data); data++;}
  else while ( len-- ) {tft_Write_16(*data); data++;}
}

////////////////////////////////////////////////////////////////////////////////////////
#elif defined (ILI9488_DRIVER) // Now code for ILI9488 24 bit SPI colour TFT
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for K210 and 3 byte RGB display
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len)
{
  // Split out the colours
  uint8_t r = (color & 0xF800)>>8;
  uint8_t g = (color & 0x07E0)>>3;
  uint8_t b = (color & 0x001F)<<3;

  while ( len-- ) {tft_Write_8(r); tft_Write_8(g); tft_Write_8(b);}
}

/***************************************************************************************
** Function name:           pushPixels - for K210 and 3 byte RGB display
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  if (_swapBytes) {
    while ( len-- ) {
      uint16_t color = *data >> 8 | *data << 8;
      tft_Write_8((color & 0xF800)>>8);
      tft_Write_8((color & 0x07E0)>>3);
      tft_Write_8((color & 0x001F)<<3);
      data++;
    }
  }
  else {
    while ( len-- ) {
      tft_Write_8((*data & 0xF800)>>8);
      tft_Write_8((*data & 0x07E0)>>3);
      tft_Write_8((*data & 0x001F)<<3);
      data++;
    }
  }
}

/***************************************************************************************
** Function name:           pushSwapBytePixels - for K210 and 3 byte RGB display
** Description:             Write a sequence of pixels with swapped bytes
***************************************************************************************/
void TFT_eSPI::pushSwapBytePixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  // ILI9488 write macro is not endianess dependant, so swap byte macro not used here
  while ( len-- ) {
    uint16_t color = *data >> 8 | *data << 8;
    tft_Write_8((color & 0xF800)>>8);
    tft_Write_8((color & 0x07E0)>>3);
    tft_Write_8((color & 0x001F)<<3);
    data++;
  }
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // End of display interface specific functions
////////////////////////////////////////////////////////////////////////////////////////
