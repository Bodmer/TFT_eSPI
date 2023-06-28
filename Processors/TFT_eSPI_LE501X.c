        ////////////////////////////////////////////////////
        // TFT_eSPI driver functions for LE501X processors //
        ////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////////////

// Select the SPI port to use, LE501X has 2 options
#if !defined (TFT_PARALLEL_8_BIT)
    #ifdef USE_SSI_PORT
      SPIClass spi = SPIClass(SSI);
    #else // use default VSPI port
      SPIClass spi = SPIClass(SPI);
    #endif
#endif


#ifdef LE501X_DMA
// hw cs pin
static uint8_t _cs_pin = 0xFF;
#endif


  // Buffer for SPI transmit byte padding and byte order manipulation
  uint8_t   spiBuffer[8];


////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_SDA_READ) && !defined (TFT_PARALLEL_8_BIT)
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           beginSDA - VSPI port only, FPSI port only for S2
** Description:             Detach MOSI and attach MISO to SDA for reads
***************************************************************************************/
void TFT_eSPI::begin_SDA_Read(void)
{
  // Release configured SPI port for SDA read
  spi.end();
}

/***************************************************************************************
** Function name:           endSDA - VSPI port only, FPSI port only for S2
** Description:             Attach MOSI to SDA and detach MISO for writes
***************************************************************************************/
void TFT_eSPI::end_SDA_Read(void)
{
  // Configure SPI port ready for next TFT access
  spi.begin();
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // #if defined (TFT_SDA_READ)
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
#ifdef TFT_PARALLEL_8_BIT
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           GPIO direction control  - supports class functions
** Description:             Set parallel bus to INPUT or OUTPUT
***************************************************************************************/
void TFT_eSPI::busDir(uint32_t mask, uint8_t mode)
{
  // Arduino generic native function
  pinMode(TFT_D0, mode);
  pinMode(TFT_D1, mode);
  pinMode(TFT_D2, mode);
  pinMode(TFT_D3, mode);
  pinMode(TFT_D4, mode);
  pinMode(TFT_D5, mode);
  pinMode(TFT_D6, mode);
  pinMode(TFT_D7, mode);
}

/***************************************************************************************
** Function name:           GPIO direction control  - supports class functions
** Description:             Set LE501X GPIO pin to input or output (set high) ASAP
***************************************************************************************/
void TFT_eSPI::gpioMode(uint8_t gpio, uint8_t mode)
{
  pinMode(gpio, mode);
}

/***************************************************************************************
** Function name:           read byte  - supports class functions
** Description:             Read a byte from LE501X 8 bit data port
***************************************************************************************/
// Parallel bus MUST be set to input before calling this function!
uint8_t TFT_eSPI::readByte(void)
{
  uint8_t b = 0;

  RD_L;

#if defined (LE501X_PORTA_DATA_BUS)
  b = LSGPIOA->DIN;
  b = LSGPIOA->DIN;
  b = LSGPIOA->DIN;
  b = ((LSGPIOA->DIN)>>8) & 0xFF;
#else
  b  = RD_TFT_D0 | RD_TFT_D0 | RD_TFT_D0 | RD_TFT_D0; //Delay for bits to settle

  b  = RD_TFT_D0 | RD_TFT_D1 | RD_TFT_D2 | RD_TFT_D3;
  b |= RD_TFT_D4 | RD_TFT_D5 | RD_TFT_D6 | RD_TFT_D7;
#endif

  RD_H;

  return b;
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // #ifdef TFT_PARALLEL_8_BIT
////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_PARALLEL_8_BIT) // Now the code for LE501X 8 bit parallel
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for LE501X and parallel display
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){
  #if defined (SSD1963_DRIVER)
  if ( ((color & 0xF800)>> 8) == ((color & 0x07E0)>> 3) && ((color & 0xF800)>> 8)== ((color & 0x001F)<< 3) )
  #else
  if ( (color >> 8) == (color & 0x00FF) )
  #endif
  { if (!len) return;
    tft_Write_16(color);
  #if defined (SSD1963_DRIVER)
    while (--len) {WR_L; WR_H; WR_L; WR_H; WR_L; WR_H;}
  #else
    #ifdef PSEUDO_16_BIT
      while (--len) {WR_L; WR_H;}
    #else
      while (--len) {WR_L; WR_H; WR_L; WR_H;}
    #endif
  #endif
  }
  else while (len--) {tft_Write_16(color);}
}

/***************************************************************************************
** Function name:           pushPixels - for LE501X and parallel display
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  if(_swapBytes) { while ( len-- ) {tft_Write_16(*data); data++; } }
  else { while ( len-- ) {tft_Write_16S(*data); data++;} }
}

////////////////////////////////////////////////////////////////////////////////////////
#elif defined (RPI_WRITE_STROBE)  // Code for RPi TFT
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for LE501X RPi TFT
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len)
{
  if(len) { tft_Write_16(color); len--; }
  while(len--) {WR_L; WR_H;}
}

/***************************************************************************************
** Function name:           pushPixels - for LE501X RPi TFT
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len)
{
  uint16_t *data = (uint16_t*)data_in;

  if (_swapBytes) while ( len-- ) { tft_Write_16S(*data); data++;}
  else while ( len-- ) {tft_Write_16(*data); data++;}
}


////////////////////////////////////////////////////////////////////////////////////////
#elif defined (SPI_18BIT_DRIVER) // SPI 18 bit colour
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for LE501X and 3 byte RGB display
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len)
{
  uint8_t col[BUF_SIZE];
  // Always using swapped bytes is a peculiarity of this function...
  //color = color>>8 | color<<8;
  uint8_t r = (color & 0xF800)>>8; // Red
  uint8_t g = (color & 0x07E0)>>3; // Green
  uint8_t b = (color & 0x001F)<<3; // Blue

  if  (len<BUF_SIZE/3) {
    for (uint32_t i = 0; i < len*3; i++) {
      col[i]   = r;
      col[++i] = g;
      col[++i] = b;
    }
    spi.writeBytes(col, len*3);
    return;
  }

  for (uint32_t i = 0; i < BUF_SIZE; i++) {
      col[i]   = r;
      col[++i] = g;
      col[++i] = b;
  }
  do {
    spi.writeBytes(col, BUF_SIZE);
    len -= BUF_SIZE/3;
  } while ( len>=BUF_SIZE/3 ) ;
  // Send remaining pixels
  if (len) spi.writeBytes(col, len*3); //*/
}

/***************************************************************************************
** Function name:           pushPixels - for LE501X and 3 byte RGB display
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len)
{
#if 1
  uint16_t *data = (uint16_t*)data_in;

  if(_swapBytes) {
    while ( len-- ) {
      // Split out the colours
      spiBuffer[0] = (*data & 0xF8); // Red
      spiBuffer[1] = (*data & 0xE000)>>11 | (*data & 0x07)<<5; // Green
      spiBuffer[2] = (*data & 0x1F00)>>5; // Blue
      data++;
      spi.writeBytes(spiBuffer, 3);
    }
  }
  else {
    while ( len-- ) {
      // Split out the colours
      spiBuffer[0] = (*data & 0xF800)>>8; // Red
      spiBuffer[1] = (*data & 0x07E0)>>3; // Green
      spiBuffer[2] = (*data & 0x001F)<<3; // Blue
      data++;
      spi.writeBytes(spiBuffer, 3);
    }
  }
#else
  uint16_t *data = (uint16_t*)data_in;
  // ILI9488 write macro is not endianess dependant, hence !_swapBytes
  if(!_swapBytes) { while ( len-- ) {tft_Write_16S(*data); data++;} }
  else { while ( len-- ) {tft_Write_16(*data); data++;} }
#endif
}


////////////////////////////////////////////////////////////////////////////////////////
#else //                   Standard SPI 16 bit colour TFT                               
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for LE501X
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
#define BUF_SIZE 480
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len)
{
  uint16_t col[BUF_SIZE];
  // Always using swapped bytes is a peculiarity of this function...
  uint16_t swapColor = color>>8 | color<<8;
  if  (len<BUF_SIZE) {
    for (uint32_t i = 0; i < len; i++) col[i] = swapColor;
    spi.writeBytes((uint8_t*)col, len<<1);
    return;
  }

  for (uint32_t i = 0; i < BUF_SIZE; i++) col[i] = swapColor;
  do {
    spi.writeBytes((uint8_t*)col, BUF_SIZE<<1);
    len -= BUF_SIZE;
  } while ( len>=BUF_SIZE ) ;
  // Send remaining pixels
  if (len) spi.writeBytes((uint8_t*)col, len<<1); //*/
}
/***************************************************************************************
** Function name:           pushPixels - for LE501X
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len)
{
  uint16_t *data = (uint16_t*)data_in;
  if(_swapBytes) {
    uint16_t col[BUF_SIZE]; // Buffer for swapped bytes
    while ( len>=BUF_SIZE ) {
      for (uint32_t i = 0; i < BUF_SIZE; i++) { col[i] = (*data>>8) | (*data<<8); data++; }
      spi.writeBytes((uint8_t*)col, BUF_SIZE<<1);
      len -= BUF_SIZE;
    }
    for (uint32_t i = 0; i < len; i++) { col[i] = (*data>>8) | (*data<<8); data++; }
    spi.writeBytes((uint8_t*)col, len<<1);
  }
  else {
  // HAL byte count for transmit is only 16 bits maximum so to avoid this constraint
  // transfers of small blocks are performed until HAL capacity is reached.
    while(len>0x7FFF) { // Transfer 16 bit pixels in blocks if len*2 over 65534 bytes
      spi.writeBytes((uint8_t*)data, 0x800<<1);
      len -= 0x800; data+= 0x800; // Arbitrarily use 2KByte blocks
    }
    // Send remaining pixels (max 65534 bytes)
    spi.writeBytes((uint8_t*)data, len<<1);
  }
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // End of display interface specific functions
////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////
//                                DMA FUNCTIONS                                         
////////////////////////////////////////////////////////////////////////////////////////

//                Placeholder for DMA functions

/*
Minimal function set to support DMA:

bool TFT_eSPI::initDMA(void)
void TFT_eSPI::deInitDMA(void)
bool TFT_eSPI::dmaBusy(void)
void TFT_eSPI::pushPixelsDMA(uint16_t* image, uint32_t len)
void TFT_eSPI::pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* image)

*/

////////////////////////////////////////////////////////////////////////////////////////
// End of DMA FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////
