        ////////////////////////////////////////////////////
        //       TFT_eSPI generic driver functions        //
        ////////////////////////////////////////////////////

// This is a generic driver for Arduino boards, it supports SPI interface displays
// 8 bit parallel interface to TFT is not supported for generic processors

#ifndef _TFT_eSPI_RP2040H_
#define _TFT_eSPI_RP2040H_

// Required for both the official and community board packages
#include "hardware/dma.h"
#include "hardware/pio.h"

// Processor ID reported by getSetup()
#define PROCESSOR_ID 0x2040

// Include processor specific header
// None


#if !defined (TFT_PARALLEL_8_BIT) // SPI
  // Use SPI0 as default if not defined
  #ifndef TFT_SPI_PORT
    #define TFT_SPI_PORT 0
  #endif

  #if (TFT_SPI_PORT == 0)
    #define SPI_X spi0
  #else
    #define SPI_X spi1
  #endif

  // Processor specific code used by SPI bus transaction begin/end_tft_write functions
  #define SET_BUS_WRITE_MODE spi_set_format(SPI_X,  8, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST)
  #define SET_BUS_READ_MODE  // spi_set_format(SPI_X,  8, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST)
#else
  // Processor specific code used by SPI bus transaction begin/end_tft_write functions
  #define SET_BUS_WRITE_MODE
  #define SET_BUS_READ_MODE
#endif

// Code to check if SPI or DMA is busy, used by SPI bus transaction startWrite and/or endWrite functions
#if !defined(SPI_18BIT_DRIVER)
  #define RP2040_DMA
  // Code to check if DMA is busy, used by SPI DMA + transaction + endWrite functions
  #define DMA_BUSY_CHECK dmaWait()
#else
  #define DMA_BUSY_CHECK
#endif

#if !defined (TFT_PARALLEL_8_BIT) // SPI
  // Initialise processor specific SPI functions, used by init()
  #define INIT_TFT_DATA_BUS  // Not used

  // Wait for tx to end, flush rx FIFO, clear rx overrun
  #define SPI_BUSY_CHECK while (spi_get_hw(SPI_X)->sr & SPI_SSPSR_BSY_BITS) {};     \
                         while (spi_is_readable(SPI_X)) (void)spi_get_hw(SPI_X)->dr; \
                         spi_get_hw(SPI_X)->icr = SPI_SSPICR_RORIC_BITS

  // To be safe, SUPPORT_TRANSACTIONS is assumed mandatory
  #if !defined (SUPPORT_TRANSACTIONS)
    #define SUPPORT_TRANSACTIONS
  #endif
#else
  // Initialise TFT data bus
  #define INIT_TFT_DATA_BUS

  #define SPI_BUSY_CHECK

  // ILI9481 needs a slower cycle time
  // Byte rate = (CPU clock/(4 * divider))
  #ifdef ILI9481_DRIVER
    #define DIV_UNITS 1
    #define DIV_FRACT 160
  #else
    #define DIV_UNITS 1
    #define DIV_FRACT 0
  #endif

  // Set the state machine clock divider (from integer and fractional parts - 16:8) 
  #define CONSTRUCTOR_INIT_TFT_DATA_BUS pioinit(TFT_D0, TFT_WR, DIV_UNITS, DIV_FRACT);

#endif


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
  #if !defined (TFT_PARALLEL_8_BIT) // SPI
    //#define DC_C sio_hw->gpio_clr = (1ul << TFT_DC)
    //#define DC_D sio_hw->gpio_set = (1ul << TFT_DC)
    #if  defined (RPI_DISPLAY_TYPE)
      #define DC_C digitalWrite(TFT_DC, LOW);
      #define DC_D digitalWrite(TFT_DC, HIGH);
    #else
      #define DC_C sio_hw->gpio_clr = (1ul << TFT_DC)
      #define DC_D sio_hw->gpio_set = (1ul << TFT_DC)
    #endif
  #else
    // Must wait for data to flush through before changing DC line
    #define DC_C  WAIT_FOR_STALL; \
                  sio_hw->gpio_clr = (1ul << TFT_DC)

    // Flush has happened before this and mode changed back to 16 bit
    #define DC_D  sio_hw->gpio_set = (1ul << TFT_DC)
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the CS (TFT chip select) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_CS
  #define CS_L // No macro allocated so it generates no code
  #define CS_H // No macro allocated so it generates no code
#else
  #if  defined (RPI_DISPLAY_TYPE)
    #define CS_L digitalWrite(TFT_CS, LOW);
    #define CS_H digitalWrite(TFT_CS, HIGH);
  #else
    #define CS_L sio_hw->gpio_clr = (1ul << TFT_CS)
    #define CS_H sio_hw->gpio_set = (1ul << TFT_CS)
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Make sure TFT_RD is defined if not used to avoid an error message
////////////////////////////////////////////////////////////////////////////////////////
// At the moment read is not supported for parallel mode, tie TFT signal high
#ifndef TFT_RD
  #define TFT_RD -1
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the WR (TFT Write) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if !defined (TFT_PARALLEL_8_BIT) // SPI
  #ifdef TFT_WR
    #define WR_L digitalWrite(TFT_WR, LOW)
    #define WR_H digitalWrite(TFT_WR, HIGH)
  #endif
#else
  // The PIO manages the write line
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the touch screen chip select pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if !defined (TFT_PARALLEL_8_BIT) // SPI
  #if !defined TOUCH_CS || (TOUCH_CS < 0)
    #define T_CS_L // No macro allocated so it generates no code
    #define T_CS_H // No macro allocated so it generates no code
  #else
    #define T_CS_L digitalWrite(TOUCH_CS, LOW)
    #define T_CS_H digitalWrite(TOUCH_CS, HIGH)
  #endif
#else
  #ifdef TOUCH_CS
    #error Touch screen not supported in parallel mode, use a separate library.
  #endif
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
#if !defined (TFT_PARALLEL_8_BIT) // SPI

  #if  defined (SPI_18BIT_DRIVER) // SPI 18 bit colour

    // Write 8 bits to TFT
    #define tft_Write_8(C)      spi_get_hw(SPI_X)->dr = (uint32_t)(C); \
                                while (spi_get_hw(SPI_X)->sr & SPI_SSPSR_BSY_BITS) {}; \

    //#define tft_Write_8(C)   spi.transfer(C);
    #define tft_Write_8N(B)   while (!spi_is_writable(SPI_X)){}; \
                             spi_get_hw(SPI_X)->dr = (uint8_t)(B)

    // Convert 16 bit colour to 18 bit and write in 3 bytes
    #define tft_Write_16(C)  tft_Write_8N(((C) & 0xF800)>>8); \
                             tft_Write_8N(((C) & 0x07E0)>>3); \
                             tft_Write_8N(((C) & 0x001F)<<3)

    // Convert 16 bit colour to 18 bit and write in 3 bytes
    #define tft_Write_16N(C)  tft_Write_8N(((C) & 0xF800)>>8); \
                              tft_Write_8N(((C) & 0x07E0)>>3); \
                              tft_Write_8N(((C) & 0x001F)<<3)

    // Convert swapped byte 16 bit colour to 18 bit and write in 3 bytes
    #define tft_Write_16S(C) tft_Write_8N((C) & 0xF8); \
                             tft_Write_8N(((C) & 0xE000)>>11 | ((C) & 0x07)<<5); \
                             tft_Write_8N(((C) & 0x1F00)>>5)
    // Write 32 bits to TFT
    #define tft_Write_32(C)  tft_Write_8N(C>>24); \
                             tft_Write_8N(C>>16); \
                             tft_Write_8N(C>>8);  \
                             tft_Write_8N(C)

    // Write two address coordinates
    #define tft_Write_32C(C,D) tft_Write_8N(C>>8); \
                               tft_Write_8N(C);    \
                               tft_Write_8N(D>>8); \
                               tft_Write_8N(D)

    // Write same value twice
    #define tft_Write_32D(C) tft_Write_8N(C>>8); \
                             tft_Write_8N(C);    \
                             tft_Write_8N(C>>8); \
                             tft_Write_8N(C)

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
      #define tft_Write_8(C)      while (spi_get_hw(SPI_X)->sr & SPI_SSPSR_BSY_BITS) {}; \
                                  spi_set_format(SPI_X,  8, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST); \
                                  spi_get_hw(SPI_X)->dr = (uint32_t)(C); \
                                  while (spi_get_hw(SPI_X)->sr & SPI_SSPSR_BSY_BITS) {}; \
                                  spi_set_format(SPI_X, 16, (spi_cpol_t)0, (spi_cpha_t)0, SPI_MSB_FIRST)

      // Note: the following macros do not wait for the end of transmission

      #define tft_Write_16(C)     while (!spi_is_writable(SPI_X)){}; spi_get_hw(SPI_X)->dr = (uint32_t)(C)

      #define tft_Write_16N(C)    while (!spi_is_writable(SPI_X)){}; spi_get_hw(SPI_X)->dr = (uint32_t)(C)

      #define tft_Write_16S(C)    while (!spi_is_writable(SPI_X)){}; spi_get_hw(SPI_X)->dr = (uint32_t)(C)<<8 | (C)>>8

      #define tft_Write_32(C)     spi_get_hw(SPI_X)->dr = (uint32_t)((C)>>16); spi_get_hw(SPI_X)->dr = (uint32_t)(C)

      #define tft_Write_32C(C,D)  spi_get_hw(SPI_X)->dr = (uint32_t)(C); spi_get_hw(SPI_X)->dr = (uint32_t)(D)

      #define tft_Write_32D(C)    spi_get_hw(SPI_X)->dr = (uint32_t)(C); spi_get_hw(SPI_X)->dr = (uint32_t)(C)

    #endif // RPI_DISPLAY_TYPE
  #endif

#else // Parallel 8 bit

  // On it's own this does not appear to reliably detect a stall, found that we must wait
  // for FIFO to empty before clearing the stall flag, and then wait for it to be set
  #define WAIT_FOR_STALL  pio->fdebug = pull_stall_mask; while (!(pio->fdebug & pull_stall_mask))

  // Wait until at least "S" locations free
  #define WAIT_FOR_FIFO_FREE(S) while (((pio->flevel >> (pio_sm * 8)) & 0x000F) > (8-S)){}

  // Wait until at least 5 locations free
  #define WAIT_FOR_FIFO_5_FREE while ((pio->flevel) & (0x000c << (pio_sm * 8))){}

  // Wait until at least 1 location free
  #define WAIT_FOR_FIFO_1_FREE while ((pio->flevel) & (0x0008 << (pio_sm * 8))){}

  // Wait for FIFO to empty (use before swapping to 8 bits)
  #define WAIT_FOR_FIFO_EMPTY  while(!(pio->fstat & (1u << (PIO_FSTAT_TXEMPTY_LSB + pio_sm))))

  // The write register of the TX FIFO.
  #define TX_FIFO  pio->txf[pio_sm]

  // Temporary - to be deleted
  #define dir_mask 0


      // This writes 8 bits, then switches back to 16 bit mode automatically
      // Have already waited for pio stalled (last data write complete) when DC switched to command mode
      // The wait for stall allows DC to be changed immediately afterwards
      #define tft_Write_8(C)      pio->sm[pio_sm].instr = pio_instr_jmp8; \
                                  TX_FIFO = C; \
                                  WAIT_FOR_STALL; \

      // Note: the following macros do not wait for the end of transmission

      #define tft_Write_16(C)     WAIT_FOR_FIFO_FREE(1); TX_FIFO = (C)

      #define tft_Write_16N(C)    WAIT_FOR_FIFO_FREE(1); TX_FIFO = (C)

      #define tft_Write_16S(C)    WAIT_FOR_FIFO_FREE(1); TX_FIFO = ((C)<<8) | ((C)>>8)

      #define tft_Write_32(C)     WAIT_FOR_FIFO_FREE(2); TX_FIFO = (C)>>16; TX_FIFO = (C)

      #define tft_Write_32C(C,D)  WAIT_FOR_FIFO_FREE(2); TX_FIFO = (C); TX_FIFO = (D)

      #define tft_Write_32D(C)    WAIT_FOR_FIFO_FREE(2); TX_FIFO = (C); TX_FIFO = (C)

#endif
////////////////////////////////////////////////////////////////////////////////////////
// Macros to read from display using SPI or software SPI
////////////////////////////////////////////////////////////////////////////////////////
#if !defined (TFT_PARALLEL_8_BIT) // SPI
  #if defined (TFT_SDA_READ)
    // Use a bit banged function call for STM32 and bi-directional SDA pin
    #define TFT_eSPI_ENABLE_8_BIT_READ // Enable tft_Read_8(void);
    #define SCLK_L digitalWrite(TFT_SCLK, LOW)
    #define SCLK_H digitalWrite(TFT_SCLK, LOW)
  #else
    // Use a SPI read transfer
    #define tft_Read_8() spi.transfer(0)
  #endif
#endif
////////////////////////////////////////////////////////////////////////////////////////
// Temporary to keep the "Arduino Mbed OS RP2040 Boards" support package happy
////////////////////////////////////////////////////////////////////////////////////////
#if defined(ARDUINO_ARCH_RP2040)

  #define ltoa itoa

#endif

#endif // Header end
