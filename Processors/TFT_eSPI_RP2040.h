        ////////////////////////////////////////////////////
        //       TFT_eSPI generic driver functions        //
        ////////////////////////////////////////////////////

// This is a generic driver for Arduino boards, it supports SPI interface displays
// 8 bit parallel interface to TFT is not supported for generic processors

#ifndef _TFT_eSPI_RP2040H_
#define _TFT_eSPI_RP2040H_

#include "hardware/dma.h"

// Processor ID reported by getSetup()
#define PROCESSOR_ID 0x2040

// Include processor specific header
// None

// Processor specific code used by SPI bus transaction begin/end_tft_write functions
#define SET_BUS_WRITE_MODE spi_set_format(spi0,  16, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST)
#define SET_BUS_READ_MODE  // spi_set_format(spi0,  8, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST)

// Code to check if SPI or DMA is busy, used by SPI bus transaction startWrite and/or endWrite functions
#if !defined(TFT_PARALLEL_8_BIT) && !defined(SPI_18BIT_DRIVER)
  #define RP2040_DMA
  // Code to check if DMA is busy, used by SPI DMA + transaction + endWrite functions
  #define DMA_BUSY_CHECK dmaWait()
#else
  #define DMA_BUSY_CHECK
#endif

// Wait for tx to end, flush rx FIFO, clear rx overrun
#define SPI_BUSY_CHECK while (spi_get_hw(spi0)->sr & SPI_SSPSR_BSY_BITS) {};     \
                       while (spi_is_readable(spi0)) (void)spi_get_hw(spi0)->dr; \
                       spi_get_hw(spi0)->icr = SPI_SSPICR_RORIC_BITS

// To be safe, SUPPORT_TRANSACTIONS is assumed mandatory
#if !defined (SUPPORT_TRANSACTIONS)
  #define SUPPORT_TRANSACTIONS
#endif

// Initialise processor specific SPI functions, used by init()
#define INIT_TFT_DATA_BUS  // Not used

// If smooth fonts are enabled the filing system may need to be loaded
#ifdef SMOOTH_FONT
  // Call up the filing system for the anti-aliased fonts
  //#define FS_NO_GLOBALS
  //#include <FS.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the DC (TFT Data/Command or Register Select (RS))pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_DC
  #define DC_C // No macro allocated so it generates no code
  #define DC_D // No macro allocated so it generates no code
#else
  #define DC_C sio_hw->gpio_clr = (1ul << TFT_DC)
  #define DC_D sio_hw->gpio_set = (1ul << TFT_DC)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the CS (TFT chip select) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_CS
  #define CS_L // No macro allocated so it generates no code
  #define CS_H // No macro allocated so it generates no code
#else
  #define CS_L sio_hw->gpio_clr = (1ul << TFT_CS)
  #define CS_H sio_hw->gpio_set = (1ul << TFT_CS)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Make sure TFT_RD is defined if not used to avoid an error message
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_RD
  #define TFT_RD -1
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the WR (TFT Write) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifdef TFT_WR
  #define WR_L digitalWrite(TFT_WR, LOW)
  #define WR_H digitalWrite(TFT_WR, HIGH)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the touch screen chip select pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if !defined TOUCH_CS || (TOUCH_CS < 0)
  #define T_CS_L // No macro allocated so it generates no code
  #define T_CS_H // No macro allocated so it generates no code
#else
  #define T_CS_L digitalWrite(TOUCH_CS, LOW)
  #define T_CS_H digitalWrite(TOUCH_CS, HIGH)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Make sure TFT_MISO is defined if not used to avoid an error message
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_MISO
  #define TFT_MISO -1
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Macros to write commands/pixel colour data to a SPI ILI948x TFT
////////////////////////////////////////////////////////////////////////////////////////
#if  defined (SPI_18BIT_DRIVER) // SPI 18 bit colour

  // Write 8 bits to TFT
  #define tft_Write_8(C)   spi.transfer(C)

  // Convert 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16(C)  spi.transfer(((C) & 0xF800)>>8); \
                           spi.transfer(((C) & 0x07E0)>>3); \
                           spi.transfer(((C) & 0x001F)<<3)

  // Convert 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16N(C)  spi.transfer(((C) & 0xF800)>>8); \
                            spi.transfer(((C) & 0x07E0)>>3); \
                            spi.transfer(((C) & 0x001F)<<3)

  // Convert swapped byte 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16S(C) spi.transfer((C) & 0xF8); \
                           spi.transfer(((C) & 0xE000)>>11 | ((C) & 0x07)<<5); \
                           spi.transfer(((C) & 0x1F00)>>5)
  // Write 32 bits to TFT
  #define tft_Write_32(C)  spi.transfer16((C)>>16); spi.transfer16((uint16_t)(C))

  // Write two address coordinates
  #define tft_Write_32C(C,D) spi.transfer16(C); spi.transfer16(D)

  // Write same value twice
  #define tft_Write_32D(C) spi.transfer16(C); spi.transfer16(C)

////////////////////////////////////////////////////////////////////////////////////////
// Macros to write commands/pixel colour data to other displays
////////////////////////////////////////////////////////////////////////////////////////
#else
  #if  defined (RPI_DISPLAY_TYPE) // RPi TFT type always needs 16 bit transfers
    #define tft_Write_8(C)   spi.transfer(C); spi.transfer(C)
    #define tft_Write_16(C)  spi.transfer((uint8_t)((C)>>8));spi.transfer((uint8_t)((C)>>0))
    #define tft_Write_16N(C) spi.transfer((uint8_t)((C)>>8));spi.transfer((uint8_t)((C)>>0))
    #define tft_Write_16S(C) spi.transfer((uint8_t)((C)>>0));spi.transfer((uint8_t)((C)>>8))

    #define tft_Write_32(C) \
      tft_Write_16((uint16_t) ((C)>>16)); \
      tft_Write_16((uint16_t) ((C)>>0))

    #define tft_Write_32C(C,D) \
      spi.transfer(0); spi.transfer((C)>>8); \
      spi.transfer(0); spi.transfer((C)>>0); \
      spi.transfer(0); spi.transfer((D)>>8); \
      spi.transfer(0); spi.transfer((D)>>0)

    #define tft_Write_32D(C) \
      spi.transfer(0); spi.transfer((C)>>8); \
      spi.transfer(0); spi.transfer((C)>>0); \
      spi.transfer(0); spi.transfer((C)>>8); \
      spi.transfer(0); spi.transfer((C)>>0)

  #else

    // This swaps to 8 bit mode, then back to 16 bit mode
    #define tft_Write_8(C)      while (spi_get_hw(spi0)->sr & SPI_SSPSR_BSY_BITS) {}; \
                                spi_set_format(spi0,  8, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST); \
                                spi_get_hw(spi0)->dr = (uint32_t)(C); \
                                while (spi_get_hw(spi0)->sr & SPI_SSPSR_BSY_BITS) {}; \
                                spi_set_format(spi0, 16, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST)

    // Note: the following macros do not wait for the end of transmission

    #define tft_Write_16(C)     while (!spi_is_writable(spi0)){}; spi_get_hw(spi0)->dr = (uint32_t)(C)

    #define tft_Write_16N(C)    while (!spi_is_writable(spi0)){}; spi_get_hw(spi0)->dr = (uint32_t)(C)

    #define tft_Write_16S(C)    while (!spi_is_writable(spi0)){}; spi_get_hw(spi0)->dr = (uint32_t)(C)<<8 | (C)>>8

    #define tft_Write_32(C)     while (!spi_is_writable(spi0)){}; spi_get_hw(spi0)->dr = (uint32_t)((C)>>8);spi_get_hw(spi0)->dr = (uint32_t)(C)

    #define tft_Write_32C(C,D)  while (!spi_is_writable(spi0)){}; spi_get_hw(spi0)->dr = (uint32_t)(C);spi_get_hw(spi0)->dr = (uint32_t)(D)

    #define tft_Write_32D(C)    while (!spi_is_writable(spi0)){}; spi_get_hw(spi0)->dr = (uint32_t)(C);spi_get_hw(spi0)->dr = (uint32_t)(C)

  #endif // RPI_DISPLAY_TYPE
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Macros to read from display using SPI or software SPI
////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_SDA_READ)
  // Use a bit banged function call for STM32 and bi-directional SDA pin
  #define TFT_eSPI_ENABLE_8_BIT_READ // Enable tft_Read_8(void);
  #define SCLK_L digitalWrite(TFT_SCLK, LOW)
  #define SCLK_H digitalWrite(TFT_SCLK, LOW)
#else
  // Use a SPI read transfer
  #define tft_Read_8() spi.transfer(0)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Temporary to keep the "Arduino Mbed OS RP2040 Boards" support package happy
////////////////////////////////////////////////////////////////////////////////////////
#if defined(ARDUINO_ARCH_RP2040)

  #define ltoa itoa

#endif

#endif // Header end
