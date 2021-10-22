        ////////////////////////////////////////////////////
        //       TFT_eSPI Software SPI driver functions        //
        ////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           tft_Write8_8
** Description:             basic softare SPI write function
***************************************************************************************/
void tft_Write_8(uint8_t c)
{
  for(uint8_t bit = 0x80; bit; bit >>= 1) {
    if(c & bit) digitalWrite(TFT_MOSI, HIGH);
    else        digitalWrite(TFT_MOSI, LOW);
    digitalWrite(TFT_SCLK, HIGH);
    digitalWrite(TFT_SCLK, LOW);
  }
}

/***************************************************************************************
** Function name:           pushPixels - for software SPI
** Description:             Write a sequence of pixels
***************************************************************************************/
void TFT_eSPI::pushPixels(const void* data_in, uint32_t len){

  uint16_t *data = (uint16_t*)data_in;

  if (_swapBytes) while ( len-- ) {tft_Write_16(*data); data++;}
  else while ( len-- ) {tft_Write_16S(*data); data++;}
}

/***************************************************************************************
** Function name:           pushBlock  - for software SPI
** Description:             Write a block of pixels of the same colour
***************************************************************************************/
void TFT_eSPI::pushBlock(uint16_t color, uint32_t len){
  while (len--) {tft_Write_16(color);}
}