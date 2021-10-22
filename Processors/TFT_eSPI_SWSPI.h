        ////////////////////////////////////////////////////
        //       TFT_eSPI Sotware SPI driver functions        //
        ////////////////////////////////////////////////////

// This is a software api driver for Arduino boards, it supports SPI interface displays

#ifndef _TFT_eSPI_SWSPI_
#define _TFT_eSPI_SWSPI_

// Initialise processor specific SPI functions, used by init()
#define INIT_TFT_DATA_BUS // Not used

// Processor specific code used by SPI bus transaction startWrite and endWrite functions
#define SET_BUS_WRITE_MODE // Not used
#define SET_BUS_READ_MODE  // Not used

// Code to check if DMA is busy, used by SPI bus transaction startWrite and endWrite functions
#define DMA_BUSY_CHECK // Not used so leave blank

////////////////////////////////////////////////////////////////////////////////////////
// Define the DC (TFT Data/Command or Register Select (RS))pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_DC
  #define DC_C // No macro allocated so it generates no code
  #define DC_D // No macro allocated so it generates no code
#else
  #define DC_C digitalWrite(TFT_DC, LOW)
  #define DC_D digitalWrite(TFT_DC, HIGH)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the CS (TFT chip select) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_CS
  #define CS_L // No macro allocated so it generates no code
  #define CS_H // No macro allocated so it generates no code
#else
  #define CS_L digitalWrite(TFT_CS, LOW)
  #define CS_H digitalWrite(TFT_CS, HIGH)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Macros to write commands/pixel colour data to a SPI
////////////////////////////////////////////////////////////////////////////////////////

  // Write 16 bits to TFT
  #define tft_Write_16(C)  tft_Write_8(C>>8); tft_Write_8(C)

  // Write Swapped 16 bits to TFT
  #define tft_Write_16S(C) uint16_t Cswap = ((C) >>8 | (C) << 8); \
                           tft_Write_16(Cswap)

  // Future option for transfer without wait
  #define tft_Write_16N(C) tft_Write_16(C)

    // Write two concatenated 16 bit values to TFT
  #define tft_Write_32C(C,D) 	tft_Write_16(C); tft_Write_16(D);

  // Write same value twice
  #define tft_Write_32D(C) tft_Write_16((uint16_t) (C)); tft_Write_16((uint16_t) (C))

#endif // Header end
