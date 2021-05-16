        ////////////////////////////////////////////////////
        //       TFT_eSPI generic driver functions        //
        ////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////////////

#if !defined (TFT_PARALLEL_8_BIT)
  // Select the SPI port and board package to use
  #ifdef ARDUINO_ARCH_MBED
    // Arduino RP2040 board package
    MbedSPI spi = MbedSPI(TFT_MISO, TFT_MOSI, TFT_SCLK);
  #else
    // Community RP2040 board package by Earle Philhower
    //SPIClass& spi = SPI; // will use board package default pins
    SPIClassRP2040 spi = SPIClassRP2040(spi0, TFT_MISO, -1, TFT_SCLK, TFT_MOSI);
  #endif
#endif

#ifdef RP2040_DMA
  uint32_t           dma_tx_channel;
  dma_channel_config dma_tx_config;
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
  spi.end();
}

/***************************************************************************************
** Function name:           endSDA
** Description:             Attach SPI pins after software SPI
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
#if defined (TFT_PARALLEL_8_BIT) // Code for generic (i.e. any) processor
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for generic processor and parallel display
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){

  while (len>1) {tft_Write_32D(color); len-=2;}
  if (len) {tft_Write_16(color);}
}

/***************************************************************************************
** Function name:           pushPixels - for gereric processor and parallel display
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  if(_swapBytes) {
    while (len>1) {tft_Write_16(*data); data++; tft_Write_16(*data); data++; len -=2;}
    if (len) {tft_Write_16(*data);}
    return;
  }

  while (len>1) {tft_Write_16S(*data); data++; tft_Write_16S(*data); data++; len -=2;}
  if (len) {tft_Write_16S(*data);}
}

/***************************************************************************************
** Function name:           GPIO direction control  - supports class functions
** Description:             Set parallel bus to INPUT or OUTPUT
***************************************************************************************/
void TFT_eSPI::busDir(uint32_t mask, uint8_t mode)
{
  // mask is unused for generic processor
  // Arduino native functions suited well to a generic driver
  pinMode(TFT_D0, mode);
  pinMode(TFT_D1, mode);
  pinMode(TFT_D2, mode);
  pinMode(TFT_D3, mode);
  pinMode(TFT_D4, mode);
  pinMode(TFT_D5, mode);
  pinMode(TFT_D6, mode);
  pinMode(TFT_D7, mode);
  return;
}

/***************************************************************************************
** Function name:           GPIO direction control  - supports class functions
** Description:             Faster GPIO pin input/output switch
***************************************************************************************/
void TFT_eSPI::gpioMode(uint8_t gpio, uint8_t mode)
{
  // No fast port based generic approach available
}

/***************************************************************************************
** Function name:           read byte  - supports class functions
** Description:             Read a byte - parallel bus only
***************************************************************************************/
uint8_t TFT_eSPI::readByte(void)
{
  uint8_t b = 0;

  busDir(0, INPUT);
  digitalWrite(TFT_RD, LOW);

  b |= digitalRead(TFT_D0) << 0;
  b |= digitalRead(TFT_D1) << 1;
  b |= digitalRead(TFT_D2) << 2;
  b |= digitalRead(TFT_D3) << 3;
  b |= digitalRead(TFT_D4) << 4;
  b |= digitalRead(TFT_D5) << 5;
  b |= digitalRead(TFT_D6) << 6;
  b |= digitalRead(TFT_D7) << 7;

  digitalWrite(TFT_RD, HIGH);
  busDir(0, OUTPUT); 

  return b;
}

////////////////////////////////////////////////////////////////////////////////////////
#elif defined (RPI_WRITE_STROBE)  // For RPi TFT with write strobe                      
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for ESP32 or RP2040 RPi TFT
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){

  if(len) { tft_Write_16(color); len--; }
  while(len--) {WR_L; WR_H;}
}

/***************************************************************************************
** Function name:           pushPixels - for ESP32 or RP2040 RPi TFT
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len)
{
  uint16_t *data = (uint16_t*)data_in;

  if (_swapBytes) while ( len-- ) {tft_Write_16S(*data); data++;}
  else while ( len-- ) {tft_Write_16(*data); data++;}
}

////////////////////////////////////////////////////////////////////////////////////////
#elif defined (SPI_18BIT_DRIVER) // SPI 18 bit colour                         
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for RP2040 and 3 byte RGB display
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len)
{
  uint16_t r = (color & 0xF800)>>8;
  uint16_t g = (color & 0x07E0)>>3;
  uint16_t b = (color & 0x001F)<<3;

  // If more than 32 pixels then change to 16 bit transfers with concatenated pixels
  if (len > 32) {
    uint32_t rg = r<<8 | g;
    uint32_t br = b<<8 | r;
    uint32_t gb = g<<8 | b;
    // Must wait before changing to 16 bit
    while (spi_get_hw(spi0)->sr & SPI_SSPSR_BSY_BITS) {};
    spi_set_format(spi0,  16, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST);
    while ( len > 1 ) {
      while (!spi_is_writable(spi0)){}; spi_get_hw(spi0)->dr = rg;
      while (!spi_is_writable(spi0)){}; spi_get_hw(spi0)->dr = br;
      while (!spi_is_writable(spi0)){}; spi_get_hw(spi0)->dr = gb;
      len -= 2;
    }
    // Must wait before changing back to 8 bit
    while (spi_get_hw(spi0)->sr & SPI_SSPSR_BSY_BITS) {};
    spi_set_format(spi0,  8, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST);
  }

  // Mop up the remaining pixels
  while ( len-- ) {tft_Write_8N(r);tft_Write_8N(g);tft_Write_8N(b);}
}

/***************************************************************************************
** Function name:           pushPixels - for RP2040 and 3 byte RGB display
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;
  if (_swapBytes) {
    while ( len-- ) {
      uint32_t col = *data++;
      tft_Write_16(col);
    }
  }
  else {
    while ( len-- ) {
      uint32_t col = *data++;
      tft_Write_16S(col);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////
#else //                   Standard SPI 16 bit colour TFT                               
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           pushBlock - for RP2040
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){
  while(len--)
  {
    while (!spi_is_writable(spi0)){};
    spi_get_hw(spi0)->dr = (uint32_t)color;
  }
}

/***************************************************************************************
** Function name:           pushPixels - for RP2040
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){
  uint16_t *data = (uint16_t*)data_in;
  if (_swapBytes) {
    while(len--)
    {
      while (!spi_is_writable(spi0)){};
      spi_get_hw(spi0)->dr = (uint32_t)(*data++);
    }
  }
  else
  {
    while(len--)
    {
      uint16_t color = *data++;
      color = color >> 8 | color << 8;
      while (!spi_is_writable(spi0)){};
      spi_get_hw(spi0)->dr = (uint32_t)color;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // End of display interface specific functions
////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
#if defined (RP2040_DMA) && !defined (TFT_PARALLEL_8_BIT) //       DMA FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////

/*
Minimal function set to support DMA:
  uint32_t           dma_tx_channel;
  dma_channel_config dma_tx_config;
*/

/***************************************************************************************
** Function name:           dmaBusy
** Description:             Check if DMA is busy
***************************************************************************************/
bool TFT_eSPI::dmaBusy(void) {
  if (!DMA_Enabled) return false;

  if (dma_channel_is_busy(dma_tx_channel)) return true;
  while (spi_get_hw(spi0)->sr & SPI_SSPSR_BSY_BITS) {};
  spi_set_format(spi0,  16, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST);
  return false;
}

/***************************************************************************************
** Function name:           dmaWait
** Description:             Wait until DMA is over (blocking!)
***************************************************************************************/
void TFT_eSPI::dmaWait(void)
{
  while (dma_channel_is_busy(dma_tx_channel));
  while (spi_get_hw(spi0)->sr & SPI_SSPSR_BSY_BITS) {};
  spi_set_format(spi0,  16, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST);
}

/***************************************************************************************
** Function name:           pushPixelsDMA
** Description:             Push pixels to TFT
***************************************************************************************/
void TFT_eSPI::pushPixelsDMA(uint16_t* image, uint32_t len)
{
  if ((len == 0) || (!DMA_Enabled)) return;

  dmaWait();

  channel_config_set_bswap(&dma_tx_config, !_swapBytes);
  dma_channel_configure(dma_tx_channel, &dma_tx_config, &spi_get_hw(spi0)->dr, (uint16_t*)image, len, true);
}

/***************************************************************************************
** Function name:           pushImageDMA
** Description:             Push image to a window
***************************************************************************************/
// This will clip to the viewport
void TFT_eSPI::pushImageDMA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t* image, uint16_t* buffer)
{
  if ((x >= _vpW) || (y >= _vpH) || (!DMA_Enabled)) return;

  int32_t dx = 0;
  int32_t dy = 0;
  int32_t dw = w;
  int32_t dh = h;

  if (x < _vpX) { dx = _vpX - x; dw -= dx; x = _vpX; }
  if (y < _vpY) { dy = _vpY - y; dh -= dy; y = _vpY; }

  if ((x + dw) > _vpW ) dw = _vpW - x;
  if ((y + dh) > _vpH ) dh = _vpH - y;

  if (dw < 1 || dh < 1) return;

  uint32_t len = dw*dh;

  if (buffer == nullptr) {
    buffer = image;
    dmaWait();
  }

  // If image is clipped, copy pixels into a contiguous block
  if ( (dw != w) || (dh != h) ) {
    for (int32_t yb = 0; yb < dh; yb++) {
      memcpy((uint8_t*) (buffer + yb * dw), (uint8_t*) (image + dx + w * (yb + dy)), dw << 1);
    }
  }
  // else, if a buffer pointer has been provided copy whole image to the buffer
  else if (buffer != image || _swapBytes) {
    memcpy(buffer, image, len*2);
  }

  dmaWait(); // In case we did not wait earlier

  setAddrWindow(x, y, dw, dh);

  channel_config_set_bswap(&dma_tx_config, !_swapBytes);
  dma_channel_configure(dma_tx_channel, &dma_tx_config, &spi_get_hw(spi0)->dr, (uint16_t*)buffer, len, true);

}

/***************************************************************************************
** Function name:           initDMA
** Description:             Initialise the DMA engine - returns true if init OK
***************************************************************************************/
bool TFT_eSPI::initDMA(bool ctrl_cs)
{
  if (DMA_Enabled) return false;

  dma_tx_channel = dma_claim_unused_channel(true);
  dma_tx_config = dma_channel_get_default_config(dma_tx_channel);
  
  channel_config_set_transfer_data_size(&dma_tx_config, DMA_SIZE_16);
  channel_config_set_dreq(&dma_tx_config, spi_get_index(spi0) ? DREQ_SPI1_TX : DREQ_SPI0_TX);

  DMA_Enabled = true;
  return true;
}

/***************************************************************************************
** Function name:           deInitDMA
** Description:             Disconnect the DMA engine from SPI
***************************************************************************************/
void TFT_eSPI::deInitDMA(void)
{
  if (!DMA_Enabled) return;
  dma_channel_unclaim(dma_tx_channel);
  DMA_Enabled = false;
}

////////////////////////////////////////////////////////////////////////////////////////
#endif // End of DMA FUNCTIONS    
////////////////////////////////////////////////////////////////////////////////////////
