        ////////////////////////////////////////////////////
        // TFT_eSPI driver functions for ESP32 processors //
        ////////////////////////////////////////////////////

#ifndef _TFT_eSPI_ESP32H_
#define _TFT_eSPI_ESP32H_

// Processor ID reported by getSetup()
#define PROCESSOR_ID 0x32

// Include processor specific header
#include "soc/spi_reg.h"
#include "driver/spi_master.h"

// Processor specific code used by SPI bus transaction startWrite and endWrite functions
#define SET_BUS_WRITE_MODE // Not used
#define SET_BUS_READ_MODE  // Not used

// SUPPORT_TRANSACTIONS is mandatory for ESP32 so the hal mutex is toggled
#if !defined (SUPPORT_TRANSACTIONS)
  #define SUPPORT_TRANSACTIONS
#endif

// ESP32 specific SPI port selection
#ifdef USE_HSPI_PORT
  #define SPI_PORT HSPI
#else
  #define SPI_PORT VSPI
#endif

#ifdef RPI_DISPLAY_TYPE
  #define CMD_BITS (16-1)
#else
  #define CMD_BITS (8-1)
#endif

// Initialise processor specific SPI functions, used by init()
#define INIT_TFT_DATA_BUS // Not used

// Define a generic flag for 8 bit parallel
#if defined (ESP32_PARALLEL) // Specific to ESP32 for backwards compatibility
  #if !defined (TFT_PARALLEL_8_BIT)
    #define TFT_PARALLEL_8_BIT // Generic parallel flag
  #endif
#endif

// Ensure ESP32 specific flag is defined for 8 bit parallel
#if defined (TFT_PARALLEL_8_BIT)
  #if !defined (ESP32_PARALLEL)
    #define ESP32_PARALLEL
  #endif
#endif

// Code to check if DMA is busy, used by SPI bus transaction transaction and endWrite functions
#if !defined(TFT_PARALLEL_8_BIT) && !defined(ILI9488_DRIVER) && !defined (RPI_DISPLAY_TYPE)
  #define ESP32_DMA
  // Code to check if DMA is busy, used by SPI DMA + transaction + endWrite functions
  #define DMA_BUSY_CHECK  dmaWait()
#else
  #define DMA_BUSY_CHECK
#endif

// If smooth font is used then it is likely SPIFFS will be needed
#ifdef SMOOTH_FONT
  // Call up the SPIFFS (SPI FLASH Filing System) for the anti-aliased fonts
  #define FS_NO_GLOBALS
  #include <FS.h>
  #include "SPIFFS.h" // ESP32 only
  #define FONT_FS_AVAILABLE
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the DC (TFT Data/Command or Register Select (RS))pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_DC
  #define DC_C // No macro allocated so it generates no code
  #define DC_D // No macro allocated so it generates no code
#else
  #if defined (TFT_PARALLEL_8_BIT)
    // TFT_DC, by design, must be in range 0-31 for single register parallel write
    #if (TFT_DC >= 0) &&  (TFT_DC < 32)
      #define DC_C GPIO.out_w1tc = (1 << TFT_DC)
      #define DC_D GPIO.out_w1ts = (1 << TFT_DC)
    #else
      #define DC_C
      #define DC_D
    #endif
  #else
    #if (TFT_DC >= 32)
      #ifdef RPI_DISPLAY_TYPE  // RPi displays need a slower DC change
        #define DC_C GPIO.out1_w1ts.val = (1 << (TFT_DC - 32)); \
                     GPIO.out1_w1tc.val = (1 << (TFT_DC - 32))
        #define DC_D GPIO.out1_w1tc.val = (1 << (TFT_DC - 32)); \
                     GPIO.out1_w1ts.val = (1 << (TFT_DC - 32))
      #else
        #define DC_C GPIO.out1_w1tc.val = (1 << (TFT_DC - 32))//;GPIO.out1_w1tc.val = (1 << (TFT_DC - 32))
        #define DC_D GPIO.out1_w1ts.val = (1 << (TFT_DC - 32))//;GPIO.out1_w1ts.val = (1 << (TFT_DC - 32))
      #endif
    #elif (TFT_DC >= 0)
      #if defined (RPI_DISPLAY_TYPE)
        #if defined (ILI9486_DRIVER)
          // RPi ILI9486 display needs a slower DC change
          #define DC_C GPIO.out_w1tc = (1 << TFT_DC); \
                       GPIO.out_w1tc = (1 << TFT_DC)
          #define DC_D GPIO.out_w1tc = (1 << TFT_DC); \
                       GPIO.out_w1ts = (1 << TFT_DC)
        #else
          // Other RPi displays need a slower C->D change
          #define DC_C GPIO.out_w1tc = (1 << TFT_DC)
          #define DC_D GPIO.out_w1tc = (1 << TFT_DC); \
                       GPIO.out_w1ts = (1 << TFT_DC)
        #endif
      #else
        #define DC_C GPIO.out_w1tc = (1 << TFT_DC)//;GPIO.out_w1tc = (1 << TFT_DC)
        #define DC_D GPIO.out_w1ts = (1 << TFT_DC)//;GPIO.out_w1ts = (1 << TFT_DC)
      #endif
    #else
      #define DC_C
      #define DC_D
    #endif
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the CS (TFT chip select) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_CS
  #define TFT_CS -1  // Keep DMA code happy
  #define CS_L       // No macro allocated so it generates no code
  #define CS_H       // No macro allocated so it generates no code
#else
  #if defined (TFT_PARALLEL_8_BIT)
    #if TFT_CS >= 32
        #define CS_L GPIO.out1_w1tc.val = (1 << (TFT_CS - 32))
        #define CS_H GPIO.out1_w1ts.val = (1 << (TFT_CS - 32))
    #elif TFT_CS >= 0
        #define CS_L GPIO.out_w1tc = (1 << TFT_CS)
        #define CS_H GPIO.out_w1ts = (1 << TFT_CS)
    #else
      #define CS_L
      #define CS_H
    #endif
  #else
    #if (TFT_CS >= 32)
      #ifdef RPI_DISPLAY_TYPE  // RPi display needs a slower CS change
        #define CS_L GPIO.out1_w1ts.val = (1 << (TFT_CS - 32)); \
                     GPIO.out1_w1tc.val = (1 << (TFT_CS - 32))
        #define CS_H GPIO.out1_w1tc.val = (1 << (TFT_CS - 32)); \
                     GPIO.out1_w1ts.val = (1 << (TFT_CS - 32))
      #else
        #define CS_L GPIO.out1_w1tc.val = (1 << (TFT_CS - 32)); GPIO.out1_w1tc.val = (1 << (TFT_CS - 32))
        #define CS_H GPIO.out1_w1ts.val = (1 << (TFT_CS - 32))//;GPIO.out1_w1ts.val = (1 << (TFT_CS - 32))
      #endif
    #elif (TFT_CS >= 0)
      #ifdef RPI_DISPLAY_TYPE  // RPi display needs a slower CS change
        #define CS_L GPIO.out_w1ts = (1 << TFT_CS); GPIO.out_w1tc = (1 << TFT_CS)
        #define CS_H GPIO.out_w1tc = (1 << TFT_CS); GPIO.out_w1ts = (1 << TFT_CS)
      #else
        #define CS_L GPIO.out_w1tc = (1 << TFT_CS); GPIO.out_w1tc = (1 << TFT_CS)
        #define CS_H GPIO.out_w1ts = (1 << TFT_CS)//;GPIO.out_w1ts = (1 << TFT_CS)
      #endif
    #else
      #define CS_L
      #define CS_H
    #endif
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the WR (TFT Write) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_WR)
  #if (TFT_WR >= 0)
    // TFT_WR, by design, must be in range 0-31 for single register parallel write
    #define WR_L GPIO.out_w1tc = (1 << TFT_WR)
    #define WR_H GPIO.out_w1ts = (1 << TFT_WR)
  #else
    #define WR_L
    #define WR_H
  #endif
#else
  #define WR_L
  #define WR_H
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the touch screen chip select pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TOUCH_CS
  #define T_CS_L // No macro allocated so it generates no code
  #define T_CS_H // No macro allocated so it generates no code
#else // XPT2046 is slow, so use slower digitalWrite here
  #define T_CS_L digitalWrite(TOUCH_CS, LOW)
  #define T_CS_H digitalWrite(TOUCH_CS, HIGH)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Make sure SPI default pins are assigned if not specified by user or set to -1
////////////////////////////////////////////////////////////////////////////////////////
#if !defined (TFT_PARALLEL_8_BIT)

  #ifdef USE_HSPI_PORT

    #ifndef TFT_MISO
      #define TFT_MISO 12
    #endif
    #if (TFT_MISO == -1)
      #undef TFT_MISO
      #define TFT_MISO 12
    #endif

    #ifndef TFT_MOSI
      #define TFT_MOSI 13
    #endif
    #if (TFT_MOSI == -1)
      #undef TFT_MOSI
      #define TFT_MOSI 13
    #endif

    #ifndef TFT_SCLK
      #define TFT_SCLK 14
    #endif
    #if (TFT_SCLK == -1)
      #undef TFT_SCLK
      #define TFT_SCLK 14
    #endif

  #else // VSPI port

    #ifndef TFT_MISO
      #define TFT_MISO 19
    #endif
    #if (TFT_MISO == -1)
      #undef TFT_MISO
      #define TFT_MISO 19
    #endif

    #ifndef TFT_MOSI
      #define TFT_MOSI 23
    #endif
    #if (TFT_MOSI == -1)
      #undef TFT_MOSI
      #define TFT_MOSI 23
    #endif

    #ifndef TFT_SCLK
      #define TFT_SCLK 18
    #endif
    #if (TFT_SCLK == -1)
      #undef TFT_SCLK
      #define TFT_SCLK 18
    #endif

  #endif

#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the parallel bus interface chip pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_PARALLEL_8_BIT)

  // Create a bit set lookup table for data bus - wastes 1kbyte of RAM but speeds things up dramatically
  // can then use e.g. GPIO.out_w1ts = set_mask(0xFF); to set data bus to 0xFF
  #define CONSTRUCTOR_INIT_TFT_DATA_BUS            \
  for (int32_t c = 0; c<256; c++)                  \
  {                                                \
    xset_mask[c] = 0;                              \
    if ( c & 0x01 ) xset_mask[c] |= (1 << TFT_D0); \
    if ( c & 0x02 ) xset_mask[c] |= (1 << TFT_D1); \
    if ( c & 0x04 ) xset_mask[c] |= (1 << TFT_D2); \
    if ( c & 0x08 ) xset_mask[c] |= (1 << TFT_D3); \
    if ( c & 0x10 ) xset_mask[c] |= (1 << TFT_D4); \
    if ( c & 0x20 ) xset_mask[c] |= (1 << TFT_D5); \
    if ( c & 0x40 ) xset_mask[c] |= (1 << TFT_D6); \
    if ( c & 0x80 ) xset_mask[c] |= (1 << TFT_D7); \
  }                                                \

  // Mask for the 8 data bits to set pin directions
  #define dir_mask ((1 << TFT_D0) | (1 << TFT_D1) | (1 << TFT_D2) | (1 << TFT_D3) | (1 << TFT_D4) | (1 << TFT_D5) | (1 << TFT_D6) | (1 << TFT_D7))

  // Data bits and the write line are cleared to 0 in one step
  #define clr_mask (dir_mask | (1 << TFT_WR))

  // A lookup table is used to set the different bit patterns, this uses 1kByte of RAM
  #define set_mask(C) xset_mask[C] // 63fps Sprite rendering test 33% faster, graphicstest only 1.8% faster than shifting in real time

  // Real-time shifting alternative to above to save 1KByte RAM, 47 fps Sprite rendering test
  /*#define set_mask(C) (((C)&0x80)>>7)<<TFT_D7 | (((C)&0x40)>>6)<<TFT_D6 | (((C)&0x20)>>5)<<TFT_D5 | (((C)&0x10)>>4)<<TFT_D4 | \
                        (((C)&0x08)>>3)<<TFT_D3 | (((C)&0x04)>>2)<<TFT_D2 | (((C)&0x02)>>1)<<TFT_D1 | (((C)&0x01)>>0)<<TFT_D0
  //*/

  // Write 8 bits to TFT
  #define tft_Write_8(C)  GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t)(C)); WR_H

  #if defined (SSD1963_DRIVER)

    // Write 18 bit color to TFT
    #define tft_Write_16(C) GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) (((C) & 0xF800)>> 8)); WR_H; \
                            GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) (((C) & 0x07E0)>> 3)); WR_H; \
                            GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) (((C) & 0x001F)<< 3)); WR_H

    // 18 bit color write with swapped bytes
    #define tft_Write_16S(C) uint16_t Cswap = ((C) >>8 | (C) << 8); tft_Write_16(Cswap)

  #else

    // Write 16 bits to TFT
    #define tft_Write_16(C) GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 8)); WR_H; \
                            GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 0)); WR_H

    // 16 bit write with swapped bytes
    #define tft_Write_16S(C) GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 0)); WR_H; \
                             GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 8)); WR_H
  #endif

  // Write 32 bits to TFT
  #define tft_Write_32(C) GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 24)); WR_H; \
                          GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 16)); WR_H; \
                          GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >>  8)); WR_H; \
                          GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >>  0)); WR_H

  // Write two concatenated 16 bit values to TFT
  #define tft_Write_32C(C,D) GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 8)); WR_H; \
                             GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 0)); WR_H; \
                             GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((D) >> 8)); WR_H; \
                             GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((D) >> 0)); WR_H

  // Write 16 bit value twice to TFT - used by drawPixel()
  #define tft_Write_32D(C) GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 8)); WR_H; \
                           GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 0)); WR_H; \
                           GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 8)); WR_H; \
                           GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) ((C) >> 0)); WR_H

   // Read pin
  #ifdef TFT_RD
    #if (TFT_RD >= 32)
      #define RD_L GPIO.out1_w1tc.val =  = (1 << (TFT_RD - 32))
      #define RD_H GPIO.out1_w1ts.val =  = (1 << (TFT_RD - 32))
    #elif (TFT_RD >= 0)
      #define RD_L GPIO.out_w1tc = (1 << TFT_RD)
      //#define RD_L digitalWrite(TFT_WR, LOW)
      #define RD_H GPIO.out_w1ts = (1 << TFT_RD)
      //#define RD_H digitalWrite(TFT_WR, HIGH)
    #else
      #define RD_L
      #define RD_H
    #endif
  #endif

////////////////////////////////////////////////////////////////////////////////////////
// Macros to write commands/pixel colour data to an ILI9488 TFT
////////////////////////////////////////////////////////////////////////////////////////
#elif  defined (ILI9488_DRIVER) // 16 bit colour converted to 3 bytes for 18 bit RGB

  // Write 8 bits to TFT
  #define tft_Write_8(C)   spi.transfer(C)

  // Convert 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16(C)  spi.transfer(((C) & 0xF800)>>8); \
                           spi.transfer(((C) & 0x07E0)>>3); \
                           spi.transfer(((C) & 0x001F)<<3)

  // Convert swapped byte 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16S(C) spi.transfer((C) & 0xF8); \
                           spi.transfer(((C) & 0xE000)>>11 | ((C) & 0x07)<<5); \
                           spi.transfer(((C) & 0x1F00)>>5)

  // Write 32 bits to TFT
  #define tft_Write_32(C)  spi.write32(C)

  // Write two concatenated 16 bit values to TFT
  #define tft_Write_32C(C,D) spi.write32((C)<<16 | (D))

  // Write 16 bit value twice to TFT
  #define tft_Write_32D(C)  spi.write32((C)<<16 | (C))

////////////////////////////////////////////////////////////////////////////////////////
// Macros to write commands/pixel colour data to an Raspberry Pi TFT
////////////////////////////////////////////////////////////////////////////////////////
#elif  defined (RPI_DISPLAY_TYPE)

  // ESP32 low level SPI writes for 8, 16 and 32 bit values
  // to avoid the function call overhead
  #define TFT_WRITE_BITS(D, B) \
  WRITE_PERI_REG(SPI_MOSI_DLEN_REG(SPI_PORT), B-1); \
  WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), D); \
  SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR); \
  while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);

  // Write 8 bits
  #define tft_Write_8(C) TFT_WRITE_BITS((C)<<8, 16)

  // Write 16 bits with corrected endianess for 16 bit colours
  #define tft_Write_16(C) TFT_WRITE_BITS((C)<<8 | (C)>>8, 16)

  // Write 16 bits
  #define tft_Write_16S(C) TFT_WRITE_BITS(C, 16)

  // Write 32 bits
  #define tft_Write_32(C) TFT_WRITE_BITS(C, 32)

  // Write two address coordinates
  #define tft_Write_32C(C,D)  TFT_WRITE_BITS((C)<<24 | (C), 32); \
                              TFT_WRITE_BITS((D)<<24 | (D), 32)

  // Write same value twice
  #define tft_Write_32D(C) tft_Write_32C(C,C)

////////////////////////////////////////////////////////////////////////////////////////
// Macros for all other SPI displays
////////////////////////////////////////////////////////////////////////////////////////
#else

  // ESP32 low level SPI writes for 8, 16 and 32 bit values
  // to avoid the function call overhead
  #define TFT_WRITE_BITS(D, B) \
  WRITE_PERI_REG(SPI_MOSI_DLEN_REG(SPI_PORT), B-1); \
  WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), D); \
  SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR); \
  while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);

  // Write 8 bits
  #define tft_Write_8(C) TFT_WRITE_BITS(C, 8)

  // Write 16 bits with corrected endianess for 16 bit colours
  #define tft_Write_16(C) TFT_WRITE_BITS((C)<<8 | (C)>>8, 16)

  // Write 16 bits
  #define tft_Write_16S(C) TFT_WRITE_BITS(C, 16)

  // Write 32 bits
  #define tft_Write_32(C) TFT_WRITE_BITS(C, 32)

  // Write two address coordinates
  #define tft_Write_32C(C,D)  TFT_WRITE_BITS((uint16_t)((D)<<8 | (D)>>8)<<16 | (uint16_t)((C)<<8 | (C)>>8), 32)

  // Write same value twice
  #define tft_Write_32D(C) TFT_WRITE_BITS((uint16_t)((C)<<8 | (C)>>8)<<16 | (uint16_t)((C)<<8 | (C)>>8), 32)

#endif

////////////////////////////////////////////////////////////////////////////////////////
// Macros to read from display using SPI or software SPI
////////////////////////////////////////////////////////////////////////////////////////
#if !defined (TFT_PARALLEL_8_BIT)
  // Read from display using SPI or software SPI
  // Use a SPI read transfer
  #define tft_Read_8() spi.transfer(0)
#endif

// Concatenate a byte sequence A,B,C,D to CDAB, P is a uint8_t pointer
#define DAT8TO32(P) ( (uint32_t)P[0]<<8 | P[1] | P[2]<<24 | P[3]<<16 )

#endif // Header end
