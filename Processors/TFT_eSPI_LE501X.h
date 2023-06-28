        ////////////////////////////////////////////////////
        // TFT_eSPI driver functions for LE501X processors //
        ////////////////////////////////////////////////////

#ifndef _TFT_eSPI_LE501XH_
#define _TFT_eSPI_LE501XH_

// Processor ID reported by getSetup()
#define PROCESSOR_ID 0x5010

// Include processor specific header
#include "le501x-hal-gpio.h"
#include "le501x-hal-spi.h"
#include "SPI.h"


// SUPPORT_TRANSACTIONS is mandatory for LE501X so the hal mutex is toggled
#if !defined (SUPPORT_TRANSACTIONS)
  #define SUPPORT_TRANSACTIONS
#endif

#define IS_SSI_PORT  1
#define IS_SPI_PORT  2

// LE501X specific SPI port selection
#ifdef USE_SSI_PORT
  #define SPI_CLASS SSI  //SSI
  #define TFT_SPI_PORT IS_SSI_PORT
#else
  #define SPI_CLASS SPI
  #define TFT_SPI_PORT IS_SPI_PORT
#endif



#ifdef RPI_DISPLAY_TYPE
  #define CMD_BITS (16-1)
#else
  #define CMD_BITS (8-1)
#endif

// Initialise processor specific SPI functions, used by init()
#define INIT_TFT_DATA_BUS // Not used

// Define a generic flag for 8 bit parallel
#if defined (LE501X_PARALLEL) // Specific to LE501X for backwards compatibility
  #if !defined (TFT_PARALLEL_8_BIT)
    #define TFT_PARALLEL_8_BIT // Generic parallel flag
  #endif
#endif

// Ensure LE501X specific flag is defined for 8 bit parallel
#if defined (TFT_PARALLEL_8_BIT)
  #if !defined (LE501X_PARALLEL)
    #define LE501X_PARALLEL
  #endif

/*
    The time required to run 'tft.fillScreen (TFT_RED)' with a screen resolution of 240x240 is as follows:
    LE501X_PORTA_DATA_BUS   = 25ms
    LE501X_CUSTOM_DATA_BUS  = 34ms
    LE501X_ANYIO_DATA_BUS   = 134ms
    undefined               = 279ms    
*/
// #define LE501X_PORTA_DATA_BUS
// #define LE501X_CUSTOM_DATA_BUS
// #define LE501X_ANYIO_DATA_BUS
#endif


// Processor specific code used by SPI bus transaction startWrite and endWrite functions
    // Not applicable to parallel bus
    #define SET_BUS_WRITE_MODE
    #define SET_BUS_READ_MODE

// Code to check if DMA is busy, used by SPI bus transaction transaction and endWrite functions
#if !defined(TFT_PARALLEL_8_BIT) && !defined(SPI_18BIT_DRIVER)
  // #define LE501X_DMA
#endif

#ifdef LE501X_DMA
  // Code to check if DMA is busy, used by SPI DMA + transaction + endWrite functions
  // #define DMA_BUSY_CHECK { if (DMA_Enabled) while(dmaBusy()); }
  #define DMA_BUSY_CHECK {}
#else
  #define DMA_BUSY_CHECK
#endif

#if defined(TFT_PARALLEL_8_BIT)
  #define SPI_BUSY_CHECK
#else
  #define SPI_BUSY_CHECK  { while(spi.isBusy()); }
#endif


// If smooth font is used then it is likely SPIFFS will be needed
#ifdef SMOOTH_FONT
  // Call up the SPIFFS (SPI FLASH Filing System) for the anti-aliased fonts
  // #define FS_NO_GLOBALS
  // #include <FS.h>
  // #define FONT_FS_AVAILABLE
#endif


////////////////////////////////////////////////////////////////////////////////////////
// Define the DC (TFT Data/Command or Register Select (RS))pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_DC
  #define DC_C // No macro allocated so it generates no code
  #define DC_D // No macro allocated so it generates no code
#else
  #define DC_C fastClrPin(TFT_DC)
  #define DC_D fastSetPin(TFT_DC)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the CS (TFT chip select) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_CS
  #define CS_L // No macro allocated so it generates no code
  #define CS_H // No macro allocated so it generates no code
#else
  #define CS_L fastClrPin(TFT_CS)
  #define CS_H fastSetPin(TFT_CS)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the WR (TFT Write) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_WR)
  #define WR_L fastClrPin(TFT_WR)
  #define WR_H fastSetPin(TFT_WR)
#endif


////////////////////////////////////////////////////////////////////////////////////////
// Define the RD (TFT Read) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
  // Read pin
#ifdef TFT_RD
    #define RD_L fastClrPin(TFT_RD)
    #define RD_H fastSetPin(TFT_RD)
#else
  #define TFT_RD -1
  #define RD_L
  #define RD_H
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
// Make sure TFT_MISO is defined if not used to avoid an error message
////////////////////////////////////////////////////////////////////////////////////////
#if !defined (TFT_PARALLEL_8_BIT)

    #ifndef TFT_MISO
      #define TFT_MISO -1
    #endif

#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the parallel bus interface chip pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_PARALLEL_8_BIT)

  // Write low extra setup time
  #if   defined WR_TWRL_0
    #define     WR_TWRL
  #elif defined WR_TWRL_1 // 1 extra low period
    #define     WR_TWRL WR_L
  #elif defined WR_TWRL_2 // 2 extra low periods
    #define     WR_TWRL WR_L; WR_L
  #elif defined WR_TWRL_3 // 3 extra low periods
    #define     WR_TWRL WR_L; WR_L; WR_L
  #elif defined WR_TWRL_4 // 4 extra low periods
    #define     WR_TWRL WR_L; WR_L; WR_L; WR_L
  #elif defined WR_TWRL_5 // 5 extra low periods
    #define     WR_TWRL WR_L; WR_L; WR_L; WR_L; WR_L
  #endif

  // Write high extra hold time
  #if   defined WR_TWRH_0
    #define     WR_TWRH WR_H
  #elif defined WR_TWRH_1 // 1 extra high period
    #define     WR_TWRH  WR_H; WR_H
  #elif defined WR_TWRH_2 // 2 extra high periods
    #define     WR_TWRH  WR_H; WR_H; WR_H
  #elif defined WR_TWRH_3 // 3 extra high periods
    #define     WR_TWRH  WR_H; WR_H; WR_H; WR_H
  #elif defined WR_TWRH_4 // 4 extra high periods
    #define     WR_TWRH  WR_H; WR_H; WR_H; WR_H; WR_H
  #elif defined WR_TWRH_5 // 5 extra high periods
    #define     WR_TWRH  WR_H; WR_H; WR_H; WR_H; WR_H; WR_H
  #endif

  #define WR_STB WR_TWRL; WR_TWRH // Rising edge write strobe


  // Temporary - to be deleted
  #define GPIO_DIR_MASK 0

  #define PARALLEL_INIT_TFT_DATA_BUS // Not used

  #if defined(LE501X_PORTA_DATA_BUS)  // The fastest operating mode; Pin fixation using PA08~PA15

    #if defined(TFT_D0)
      #define TFT_D0  PA08
    #endif
    #if defined(TFT_D1)
      #define TFT_D1  PA09
    #endif
    #if defined(TFT_D2)
      #define TFT_D2  PA10
    #endif
    #if defined(TFT_D3)
      #define TFT_D3  PA11
    #endif
    #if defined(TFT_D4)
      #define TFT_D4  PA12
    #endif
    #if defined(TFT_D5)
      #define TFT_D5  PA13
    #endif
    #if defined(TFT_D6)
      #define TFT_D6  PA14
    #endif
    #if defined(TFT_D7)
      #define TFT_D7  PA15
    #endif

    // Write 8 bits to TFT
    #define tft_Write_8(C)   LSGPIOA->BSBR = (0xFF000000 | (uint16_t)((C)<<8)); WR_L; WR_STB

      #if defined (SSD1963_DRIVER)

        // Write 18 bit color to TFT (untested)
        #define tft_Write_16(C)  r6 = (((C) & 0xF800)>> 8); g6 = (((C) & 0x07E0)>> 3); b6 = (((C) & 0x001F)<< 3); \
                             LSGPIOA->BSBR = (0xFF000000 | (uint16_t)((r6)<<8)); WR_L; WR_STB; \
                             LSGPIOA->BSBR = (0xFF000000 | (uint16_t)((g6)<<8)); WR_L; WR_STB; \
                             LSGPIOA->BSBR = (0xFF000000 | (uint16_t)((b6)<<8)); WR_L; WR_STB

        // 18 bit color write with swapped bytes
        #define tft_Write_16S(C) Cswap = ((C) >>8 | (C) << 8); tft_Write_16(Cswap)

      #else

          // Write 16 bits to TFT
          #define tft_Write_16(C)  LSGPIOA->BSBR = (0xFF000000 | (uint16_t)((C) & 0xFF00)); WR_L; WR_STB;  \
                                   LSGPIOA->BSBR = (0xFF000000 | (uint16_t)((C)<<8)); WR_L; WR_STB

          // 16 bit write with swapped bytes
          #define tft_Write_16S(C) LSGPIOA->BSBR = (0xFF000000 | (uint16_t)((C)<<8)); WR_L; WR_STB; \
                                   LSGPIOA->BSBR = (0xFF000000 | (uint16_t)((C) & 0xFF00)); WR_L; WR_STB
                                   
      #endif


    #define tft_Write_32(C)    tft_Write_16((uint16_t)((C)>>16)); tft_Write_16((uint16_t)(C))

    #define tft_Write_32C(C,D) tft_Write_16((uint16_t)(C)); tft_Write_16((uint16_t)(D))

    #define tft_Write_32D(C)   tft_Write_16((uint16_t)(C)); tft_Write_16((uint16_t)(C))

    // Read a data bit
    #define RD_TFT_D0 ((LSGPIOA->DIN) & 0x0100) // Read pin TFT_D0
    #define RD_TFT_D1 ((LSGPIOA->DIN) & 0x0200) // Read pin TFT_D1
    #define RD_TFT_D2 ((LSGPIOA->DIN) & 0x0400) // Read pin TFT_D2
    #define RD_TFT_D3 ((LSGPIOA->DIN) & 0x0800) // Read pin TFT_D3
    #define RD_TFT_D4 ((LSGPIOA->DIN) & 0x1000) // Read pin TFT_D4
    #define RD_TFT_D5 ((LSGPIOA->DIN) & 0x2000) // Read pin TFT_D5
    #define RD_TFT_D6 ((LSGPIOA->DIN) & 0x4000) // Read pin TFT_D6
    #define RD_TFT_D7 ((LSGPIOA->DIN) & 0x8000) // Read pin TFT_D7

  #elif defined(LE501X_CUSTOM_DATA_BUS)   // The second fastest operating mode; Pins need to be customized here.


    // Pin masks for set/clear
    #define D0_PIN_MASK (1UL<< 12) // Set/clear mask for PA12
    #define D1_PIN_MASK (1UL<< 13) // Set/clear mask for PA13
    #define D2_PIN_MASK (1UL<< 14) // Set/clear mask for PA14
    #define D3_PIN_MASK (1UL<< 10) // Set/clear mask for PB10
    #define D4_PIN_MASK (1UL<<  9) // Set/clear mask for PB9
    #define D5_PIN_MASK (1UL<< 14) // Set/clear mask for PB14
    #define D6_PIN_MASK (1UL<< 12) // Set/clear mask for PB12
    #define D7_PIN_MASK (1UL<< 13) // Set/clear mask for PB13

    // Create bit set/reset mask based on LS byte of value B
    #define  D0_BSR_MASK(B) ((D0_PIN_MASK<<16)>>(((B)<< 4)&0x10))
    #define  D1_BSR_MASK(B) ((D1_PIN_MASK<<16)>>(((B)<< 3)&0x10))
    #define  D2_BSR_MASK(B) ((D2_PIN_MASK<<16)>>(((B)<< 2)&0x10))
    #define  D3_BSR_MASK(B) ((D3_PIN_MASK<<16)>>(((B)<< 1)&0x10))
    #define  D4_BSR_MASK(B) ((D4_PIN_MASK<<16)>>(((B)<< 0)&0x10))
    #define  D5_BSR_MASK(B) ((D5_PIN_MASK<<16)>>(((B)>> 1)&0x10))
    #define  D6_BSR_MASK(B) ((D6_PIN_MASK<<16)>>(((B)>> 2)&0x10))
    #define  D7_BSR_MASK(B) ((D7_PIN_MASK<<16)>>(((B)>> 3)&0x10))
    // Create bit set/reset mask for top byte of 16 bit value B
    #define  D8_BSR_MASK(B) ((D0_PIN_MASK<<16)>>(((B)>> 4)&0x10))
    #define  D9_BSR_MASK(B) ((D1_PIN_MASK<<16)>>(((B)>> 5)&0x10))
    #define D10_BSR_MASK(B) ((D2_PIN_MASK<<16)>>(((B)>> 6)&0x10))
    #define D11_BSR_MASK(B) ((D3_PIN_MASK<<16)>>(((B)>> 7)&0x10))
    #define D12_BSR_MASK(B) ((D4_PIN_MASK<<16)>>(((B)>> 8)&0x10))
    #define D13_BSR_MASK(B) ((D5_PIN_MASK<<16)>>(((B)>> 9)&0x10))
    #define D14_BSR_MASK(B) ((D6_PIN_MASK<<16)>>(((B)>>10)&0x10))
    #define D15_BSR_MASK(B) ((D7_PIN_MASK<<16)>>(((B)>>11)&0x10))

    // Write 8 bits to TFT
    #define tft_Write_8(C)   LSGPIOA->BSBR = D0_BSR_MASK(C) | D1_BSR_MASK(C) | D2_BSR_MASK(C); \
                            WR_L; \
                            LSGPIOB->BSBR = D3_BSR_MASK(C) | D4_BSR_MASK(C) | D5_BSR_MASK(C) | D6_BSR_MASK(C) | D7_BSR_MASK(C); \
                            WR_STB

    #if defined (SSD1963_DRIVER)

      // Write 18 bit color to TFT (untested)
      #define tft_Write_16(C)  r6 = (((C) & 0xF800)>> 8); g6 = (((C) & 0x07E0)>> 3); b6 = (((C) & 0x001F)<< 3); \
                              LSGPIOA->BSBR = D0_BSR_MASK(r6) | D1_BSR_MASK(r6) | D2_BSR_MASK(r6); \
                              WR_L; \
                              LSGPIOB->BSBR = D3_BSR_MASK(r6) | D4_BSR_MASK(r6) | D5_BSR_MASK(r6) | D6_BSR_MASK(r6) | D7_BSR_MASK(r6); \
                              WR_STB; \
                              LSGPIOA->BSBR = D0_BSR_MASK(g6) | D1_BSR_MASK(g6) | D2_BSR_MASK(g6); \
                              WR_L; \
                              LSGPIOB->BSBR = D3_BSR_MASK(g6) | D4_BSR_MASK(g6) | D5_BSR_MASK(g6) | D6_BSR_MASK(g6) | D7_BSR_MASK(g6); \
                              WR_STB; \
                              LSGPIOA->BSBR = D0_BSR_MASK(b6) | D1_BSR_MASK(b6) | D2_BSR_MASK(b6); \
                              WR_L; \
                              LSGPIOB->BSBR = D3_BSR_MASK(b6) | D4_BSR_MASK(b6) | D5_BSR_MASK(b6) | D6_BSR_MASK(b6) | D7_BSR_MASK(b6); \
                              WR_STB

      // 18 bit color write with swapped bytes
      #define tft_Write_16S(C) Cswap = ((C) >>8 | (C) << 8); tft_Write_16(Cswap)

    #else
      // Write 16 bits to TFT
      #define tft_Write_16(C)  LSGPIOA->BSBR = D8_BSR_MASK(C) | D9_BSR_MASK(C) | D10_BSR_MASK(C); \
                              WR_L; \
                              LSGPIOB->BSBR = D11_BSR_MASK(C) | D12_BSR_MASK(C) | D13_BSR_MASK(C) | D14_BSR_MASK(C) | D15_BSR_MASK(C); \
                              WR_STB; \
                              LSGPIOA->BSBR = D0_BSR_MASK(C) | D1_BSR_MASK(C) | D2_BSR_MASK(C); \
                              WR_L; \
                              LSGPIOB->BSBR = D3_BSR_MASK(C) | D4_BSR_MASK(C) | D5_BSR_MASK(C) | D6_BSR_MASK(C) | D7_BSR_MASK(C); \
                              WR_STB

      // 16 bit write with swapped bytes
      #define tft_Write_16S(C) LSGPIOA->BSBR = D0_BSR_MASK(C) | D1_BSR_MASK(C) | D2_BSR_MASK(C); \
                              WR_L; \
                              LSGPIOB->BSBR = D3_BSR_MASK(C) | D4_BSR_MASK(C) | D5_BSR_MASK(C) | D6_BSR_MASK(C) | D7_BSR_MASK(C); \
                              WR_STB; \
                              LSGPIOA->BSBR = D8_BSR_MASK(C) | D9_BSR_MASK(C) | D10_BSR_MASK(C); \
                              WR_L; \
                              LSGPIOB->BSBR = D11_BSR_MASK(C) | D12_BSR_MASK(C) | D13_BSR_MASK(C) | D14_BSR_MASK(C) | D15_BSR_MASK(C); \
                              WR_STB
    #endif


    #define tft_Write_32(C)    tft_Write_16((uint16_t)((C)>>16)); tft_Write_16((uint16_t)(C))

    #define tft_Write_32C(C,D) tft_Write_16((uint16_t)(C)); tft_Write_16((uint16_t)(D))

    #define tft_Write_32D(C)   tft_Write_16((uint16_t)(C)); tft_Write_16((uint16_t)(C))

    // Read a data bit
    #define RD_TFT_D0 (((LSGPIOA->DIN)&(D0_PIN_MASK))>>(12-0)) // Read pin PA12
    #define RD_TFT_D1 (((LSGPIOA->DIN)&(D1_PIN_MASK))>>(13-1)) // Read pin PA13
    #define RD_TFT_D2 (((LSGPIOA->DIN)&(D2_PIN_MASK))>>(14-2)) // Read pin PA14
    #define RD_TFT_D3 (((LSGPIOB->DIN)&(D3_PIN_MASK))>>(10-3)) // Read pin PB10
    #define RD_TFT_D4 (((LSGPIOB->DIN)&(D4_PIN_MASK))>>( 9-4)) // Read pin PB9
    #define RD_TFT_D5 (((LSGPIOB->DIN)&(D5_PIN_MASK))<<(14-5)) // Read pin PB14
    #define RD_TFT_D6 (((LSGPIOB->DIN)&(D6_PIN_MASK))>>(12-6)) // Read pin PB12
    #define RD_TFT_D7 (((LSGPIOA->DIN)&(D7_PIN_MASK))>>(13-7)) // Read pin PB13

  #elif defined(LE501X_ANYIO_DATA_BUS) // The speed is slower than the previous one, and the pins are flexible and variable


    // Convert Arduino pin reference Dx to port and mask
    #define D0_PIN_NAME  digitalPinToPinName(TFT_D0)
    #define D1_PIN_NAME  digitalPinToPinName(TFT_D1)
    #define D2_PIN_NAME  digitalPinToPinName(TFT_D2)
    #define D3_PIN_NAME  digitalPinToPinName(TFT_D3)
    #define D4_PIN_NAME  digitalPinToPinName(TFT_D4)
    #define D5_PIN_NAME  digitalPinToPinName(TFT_D5)
    #define D6_PIN_NAME  digitalPinToPinName(TFT_D6)
    #define D7_PIN_NAME  digitalPinToPinName(TFT_D7)

    // Pin port bit number 0-15
    #define D0_PIN_BIT  (D0_PIN_NAME & 0xF)
    #define D1_PIN_BIT  (D1_PIN_NAME & 0xF)
    #define D2_PIN_BIT  (D2_PIN_NAME & 0xF)
    #define D3_PIN_BIT  (D3_PIN_NAME & 0xF)
    #define D4_PIN_BIT  (D4_PIN_NAME & 0xF)
    #define D5_PIN_BIT  (D5_PIN_NAME & 0xF)
    #define D6_PIN_BIT  (D6_PIN_NAME & 0xF)
    #define D7_PIN_BIT  (D7_PIN_NAME & 0xF)

    // Pin port
    #define D0_PIN_PORT  digitalPinToPort(TFT_D0)
    #define D1_PIN_PORT  digitalPinToPort(TFT_D1)
    #define D2_PIN_PORT  digitalPinToPort(TFT_D2)
    #define D3_PIN_PORT  digitalPinToPort(TFT_D3)
    #define D4_PIN_PORT  digitalPinToPort(TFT_D4)
    #define D5_PIN_PORT  digitalPinToPort(TFT_D5)
    #define D6_PIN_PORT  digitalPinToPort(TFT_D6)
    #define D7_PIN_PORT  digitalPinToPort(TFT_D7)

    // Pin masks for set/clear
    #define D0_PIN_MASK  digitalPinToBitMask(TFT_D0)
    #define D1_PIN_MASK  digitalPinToBitMask(TFT_D1)
    #define D2_PIN_MASK  digitalPinToBitMask(TFT_D2)
    #define D3_PIN_MASK  digitalPinToBitMask(TFT_D3)
    #define D4_PIN_MASK  digitalPinToBitMask(TFT_D4)
    #define D5_PIN_MASK  digitalPinToBitMask(TFT_D5)
    #define D6_PIN_MASK  digitalPinToBitMask(TFT_D6)
    #define D7_PIN_MASK  digitalPinToBitMask(TFT_D7)

    // Create bit set/reset mask based on LS byte of value B
    #define  D0_BSR_MASK(B) ((D0_PIN_MASK<<16)>>(((B)<< 4)&0x10))
    #define  D1_BSR_MASK(B) ((D1_PIN_MASK<<16)>>(((B)<< 3)&0x10))
    #define  D2_BSR_MASK(B) ((D2_PIN_MASK<<16)>>(((B)<< 2)&0x10))
    #define  D3_BSR_MASK(B) ((D3_PIN_MASK<<16)>>(((B)<< 1)&0x10))
    #define  D4_BSR_MASK(B) ((D4_PIN_MASK<<16)>>(((B)<< 0)&0x10))
    #define  D5_BSR_MASK(B) ((D5_PIN_MASK<<16)>>(((B)>> 1)&0x10))
    #define  D6_BSR_MASK(B) ((D6_PIN_MASK<<16)>>(((B)>> 2)&0x10))
    #define  D7_BSR_MASK(B) ((D7_PIN_MASK<<16)>>(((B)>> 3)&0x10))
    // Create bit set/reset mask for top byte of 16 bit value B
    #define  D8_BSR_MASK(B) ((D0_PIN_MASK<<16)>>(((B)>> 4)&0x10))
    #define  D9_BSR_MASK(B) ((D1_PIN_MASK<<16)>>(((B)>> 5)&0x10))
    #define D10_BSR_MASK(B) ((D2_PIN_MASK<<16)>>(((B)>> 6)&0x10))
    #define D11_BSR_MASK(B) ((D3_PIN_MASK<<16)>>(((B)>> 7)&0x10))
    #define D12_BSR_MASK(B) ((D4_PIN_MASK<<16)>>(((B)>> 8)&0x10))
    #define D13_BSR_MASK(B) ((D5_PIN_MASK<<16)>>(((B)>> 9)&0x10))
    #define D14_BSR_MASK(B) ((D6_PIN_MASK<<16)>>(((B)>>10)&0x10))
    #define D15_BSR_MASK(B) ((D7_PIN_MASK<<16)>>(((B)>>11)&0x10))

                              
    // Write 8 bits to TFT
    #define tft_Write_8(C)    D0_PIN_PORT->BSBR = D0_BSR_MASK(C); \
                              D1_PIN_PORT->BSBR = D1_BSR_MASK(C); \
                              D2_PIN_PORT->BSBR = D2_BSR_MASK(C); \
                              D3_PIN_PORT->BSBR = D3_BSR_MASK(C); \
                              WR_L; \
                              D4_PIN_PORT->BSBR = D4_BSR_MASK(C); \
                              D5_PIN_PORT->BSBR = D5_BSR_MASK(C); \
                              D6_PIN_PORT->BSBR = D6_BSR_MASK(C); \
                              D7_PIN_PORT->BSBR = D7_BSR_MASK(C); \
                              WR_STB

    #if defined (SSD1963_DRIVER)

      // Write 18 bit color to TFT (untested)
      #define tft_Write_16(C)  r6 = (((C) & 0xF800)>> 8); g6 = (((C) & 0x07E0)>> 3); b6 = (((C) & 0x001F)<< 3); \
                                D0_PIN_PORT->BSBR = D8_BSR_MASK(r6);  \
                                D1_PIN_PORT->BSBR = D9_BSR_MASK(r6);  \
                                D2_PIN_PORT->BSBR = D10_BSR_MASK(r6); \
                                D3_PIN_PORT->BSBR = D11_BSR_MASK(r6); \
                                WR_L; \
                                D4_PIN_PORT->BSBR = D12_BSR_MASK(r6); \
                                D5_PIN_PORT->BSBR = D13_BSR_MASK(r6); \
                                D6_PIN_PORT->BSBR = D14_BSR_MASK(r6); \
                                D7_PIN_PORT->BSBR = D15_BSR_MASK(r6); \
                                WR_STB;\
                                D0_PIN_PORT->BSBR = D8_BSR_MASK(g6);  \
                                D1_PIN_PORT->BSBR = D9_BSR_MASK(g6);  \
                                D2_PIN_PORT->BSBR = D10_BSR_MASK(g6); \
                                D3_PIN_PORT->BSBR = D11_BSR_MASK(g6); \
                                WR_L; \
                                D4_PIN_PORT->BSBR = D12_BSR_MASK(g6); \
                                D5_PIN_PORT->BSBR = D13_BSR_MASK(g6); \
                                D6_PIN_PORT->BSBR = D14_BSR_MASK(g6); \
                                D7_PIN_PORT->BSBR = D15_BSR_MASK(g6); \
                                WR_STB;\
                                D0_PIN_PORT->BSBR = D0_BSR_MASK(b6); \
                                D1_PIN_PORT->BSBR = D1_BSR_MASK(b6); \
                                D2_PIN_PORT->BSBR = D2_BSR_MASK(b6); \
                                D3_PIN_PORT->BSBR = D3_BSR_MASK(b6); \
                                WR_L; \
                                D4_PIN_PORT->BSBR = D4_BSR_MASK(b6); \
                                D5_PIN_PORT->BSBR = D5_BSR_MASK(b6); \
                                D6_PIN_PORT->BSBR = D6_BSR_MASK(b6); \
                                D7_PIN_PORT->BSBR = D7_BSR_MASK(b6); \
                                WR_STB

      // 18 bit color write with swapped bytes
      #define tft_Write_16S(C) Cswap = ((C) >>8 | (C) << 8); tft_Write_16(Cswap)

    #else

      // Write 16 bits to TFT
      #define tft_Write_16(C)   D0_PIN_PORT->BSBR = D8_BSR_MASK(C);  \
                                D1_PIN_PORT->BSBR = D9_BSR_MASK(C);  \
                                D2_PIN_PORT->BSBR = D10_BSR_MASK(C); \
                                D3_PIN_PORT->BSBR = D11_BSR_MASK(C); \
                                WR_L; \
                                D4_PIN_PORT->BSBR = D12_BSR_MASK(C); \
                                D5_PIN_PORT->BSBR = D13_BSR_MASK(C); \
                                D6_PIN_PORT->BSBR = D14_BSR_MASK(C); \
                                D7_PIN_PORT->BSBR = D15_BSR_MASK(C); \
                                WR_STB;\
                                D0_PIN_PORT->BSBR = D0_BSR_MASK(C); \
                                D1_PIN_PORT->BSBR = D1_BSR_MASK(C); \
                                D2_PIN_PORT->BSBR = D2_BSR_MASK(C); \
                                D3_PIN_PORT->BSBR = D3_BSR_MASK(C); \
                                WR_L; \
                                D4_PIN_PORT->BSBR = D4_BSR_MASK(C); \
                                D5_PIN_PORT->BSBR = D5_BSR_MASK(C); \
                                D6_PIN_PORT->BSBR = D6_BSR_MASK(C); \
                                D7_PIN_PORT->BSBR = D7_BSR_MASK(C); \
                                WR_STB

      // 16 bit write with swapped bytes
      #define tft_Write_16S(C)  D0_PIN_PORT->BSBR = D0_BSR_MASK(C); \
                                D1_PIN_PORT->BSBR = D1_BSR_MASK(C); \
                                D2_PIN_PORT->BSBR = D2_BSR_MASK(C); \
                                D3_PIN_PORT->BSBR = D3_BSR_MASK(C); \
                                WR_L; \
                                D4_PIN_PORT->BSBR = D4_BSR_MASK(C); \
                                D5_PIN_PORT->BSBR = D5_BSR_MASK(C); \
                                D6_PIN_PORT->BSBR = D6_BSR_MASK(C); \
                                D7_PIN_PORT->BSBR = D7_BSR_MASK(C); \
                                WR_STB; \
                                D0_PIN_PORT->BSBR = D8_BSR_MASK(C);  \
                                D1_PIN_PORT->BSBR = D9_BSR_MASK(C);  \
                                D2_PIN_PORT->BSBR = D10_BSR_MASK(C); \
                                D3_PIN_PORT->BSBR = D11_BSR_MASK(C); \
                                WR_L; \
                                D4_PIN_PORT->BSBR = D12_BSR_MASK(C); \
                                D5_PIN_PORT->BSBR = D13_BSR_MASK(C); \
                                D6_PIN_PORT->BSBR = D14_BSR_MASK(C); \
                                D7_PIN_PORT->BSBR = D15_BSR_MASK(C); \
                                WR_STB
    #endif


    #define tft_Write_32(C)    tft_Write_16((uint16_t)((C)>>16)); tft_Write_16((uint16_t)(C))

    #define tft_Write_32C(C,D) tft_Write_16((uint16_t)(C)); tft_Write_16((uint16_t)(D))

    #define tft_Write_32D(C)   tft_Write_16((uint16_t)(C)); tft_Write_16((uint16_t)(C))    
    

  // Read a data bit
    #define RD_TFT_D0 ((((D0_PIN_PORT->DIN) >> (D0_PIN_BIT))&1)<<0) // Read pin TFT_D0
    #define RD_TFT_D1 ((((D1_PIN_PORT->DIN) >> (D1_PIN_BIT))&1)<<1) // Read pin TFT_D1
    #define RD_TFT_D2 ((((D2_PIN_PORT->DIN) >> (D2_PIN_BIT))&1)<<2) // Read pin TFT_D2
    #define RD_TFT_D3 ((((D3_PIN_PORT->DIN) >> (D3_PIN_BIT))&1)<<3) // Read pin TFT_D3
    #define RD_TFT_D4 ((((D4_PIN_PORT->DIN) >> (D4_PIN_BIT))&1)<<4) // Read pin TFT_D4
    #define RD_TFT_D5 ((((D5_PIN_PORT->DIN) >> (D5_PIN_BIT))&1)<<5) // Read pin TFT_D5
    #define RD_TFT_D6 ((((D6_PIN_PORT->DIN) >> (D6_PIN_BIT))&1)<<6) // Read pin TFT_D6
    #define RD_TFT_D7 ((((D7_PIN_PORT->DIN) >> (D7_PIN_BIT))&1)<<7) // Read pin TFT_D7

  #else // Undefined slowest mode

    #define  D0_IS_MASK(B) (((B)>> 0)&0x01)
    #define  D1_IS_MASK(B) (((B)>> 1)&0x01)
    #define  D2_IS_MASK(B) (((B)>> 2)&0x01)
    #define  D3_IS_MASK(B) (((B)>> 3)&0x01)
    #define  D4_IS_MASK(B) (((B)>> 4)&0x01)
    #define  D5_IS_MASK(B) (((B)>> 5)&0x01)
    #define  D6_IS_MASK(B) (((B)>> 6)&0x01)
    #define  D7_IS_MASK(B) (((B)>> 7)&0x01)
    #define  D8_IS_MASK(B) (((B)>> 8)&0x01)
    #define  D9_IS_MASK(B) (((B)>> 9)&0x01)
    #define  D10_IS_MASK(B) (((B)>> 10)&0x01)
    #define  D11_IS_MASK(B) (((B)>> 11)&0x01)
    #define  D12_IS_MASK(B) (((B)>> 12)&0x01)
    #define  D13_IS_MASK(B) (((B)>> 13)&0x01)
    #define  D14_IS_MASK(B) (((B)>> 14)&0x01)
    #define  D15_IS_MASK(B) (((B)>> 15)&0x01)

    // Write 8 bits to TFT
    #define tft_Write_8(C)    fastWritePin(TFT_D0, D0_IS_MASK(C)); fastWritePin(TFT_D1, D1_IS_MASK(C)); \
                              WR_L; \
                              fastWritePin(TFT_D2, D2_IS_MASK(C)); fastWritePin(TFT_D3, D3_IS_MASK(C)); \
                              fastWritePin(TFT_D4, D4_IS_MASK(C)); fastWritePin(TFT_D5, D5_IS_MASK(C)); \
                              fastWritePin(TFT_D6, D6_IS_MASK(C)); fastWritePin(TFT_D7, D7_IS_MASK(C)); \
                              WR_STB

    #if defined (SSD1963_DRIVER)

      // Write 18 bit color to TFT (untested)
      #define tft_Write_16(C)  r6 = (((C) & 0xF800)>> 8); g6 = (((C) & 0x07E0)>> 3); b6 = (((C) & 0x001F)<< 3); \
                                tft_Write_8(r6); \
                                tft_Write_8(g6); \
                                tft_Write_8(b6)

      // 18 bit color write with swapped bytes
      #define tft_Write_16S(C) Cswap = ((C) >>8 | (C) << 8); tft_Write_16(Cswap)

    #else
      // Write 16 bits to TFT
      #define tft_Write_16(C)   tft_Write_8((uint8_t) ((C) >> 8)); \
                                tft_Write_8((uint8_t) ((C) >> 0))

      // 16 bit write with swapped bytes
      #define tft_Write_16S(C)  tft_Write_8((uint8_t) ((C) >> 0)); \
                                tft_Write_8((uint8_t) ((C) >> 8))
    #endif


    #define tft_Write_32(C)    tft_Write_16((uint16_t)((C)>>16)); tft_Write_16((uint16_t)(C))

    #define tft_Write_32C(C,D) tft_Write_16((uint16_t)(C)); tft_Write_16((uint16_t)(D))

    #define tft_Write_32D(C)   tft_Write_16((uint16_t)(C)); tft_Write_16((uint16_t)(C))


    // Read a data bit
    #define RD_TFT_D0 ((fastReadPin(TFT_D0))<<0) // Read pin TFT_D0
    #define RD_TFT_D1 ((fastReadPin(TFT_D1))<<1) // Read pin TFT_D1
    #define RD_TFT_D2 ((fastReadPin(TFT_D2))<<2) // Read pin TFT_D2
    #define RD_TFT_D3 ((fastReadPin(TFT_D3))<<3) // Read pin TFT_D3
    #define RD_TFT_D4 ((fastReadPin(TFT_D4))<<4) // Read pin TFT_D4
    #define RD_TFT_D5 ((fastReadPin(TFT_D5))<<5) // Read pin TFT_D5
    #define RD_TFT_D6 ((fastReadPin(TFT_D6))<<6) // Read pin TFT_D6
    #define RD_TFT_D7 ((fastReadPin(TFT_D7))<<7) // Read pin TFT_D7
  #endif


////////////////////////////////////////////////////////////////////////////////////////
// Macros to write commands/pixel colour data to a SPI ILI948x TFT
////////////////////////////////////////////////////////////////////////////////////////
#elif  defined (SPI_18BIT_DRIVER) // SPI 18 bit colour

  // Write 8 bits to TFT
  #define tft_Write_8(C) \
  { spiBuffer[0] = C; \
  spi.writeBytes(spiBuffer, 1); }

  // Convert 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16(C) \
  { spiBuffer[0] = ((C) & 0xF800)>>8; spiBuffer[1] = ((C) & 0x07E0)>>3; spiBuffer[2] = ((C) & 0x001F)<<3; \
  spi.writeBytes(spiBuffer, 3); }

  // Convert swapped byte 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16S(C) \
  { spiBuffer[0] = (C) & 0xF8; spiBuffer[1] = ((C) & 0xE000)>>11 | ((C) & 0x07)<<5; spiBuffer[2] = ((C) & 0x1F00)>>5; \
  spi.writeBytes(spiBuffer, 3); }

  // Write 32 bits to TFT
  #define tft_Write_32(C) \
  { spiBuffer[0] = (C)>>24; spiBuffer[1] = (C)>>16; spiBuffer[2] = (C)>>8; spiBuffer[3] = C; \
  spi.writeBytes(spiBuffer, 4); }

  // Write two address coordinates
  #define tft_Write_32C(C,D) \
  { spiBuffer[0] = (C)>>8; spiBuffer[1] = C; spiBuffer[2] = (D)>>8; spiBuffer[3] = D; \
  spi.writeBytes(spiBuffer, 4); }

  // Write same value twice
  #define tft_Write_32D(C) \
  { spiBuffer[0] = spiBuffer[2] = (C)>>8; spiBuffer[1] = spiBuffer[3] = C; \
  spi.writeBytes(spiBuffer, 4); }

////////////////////////////////////////////////////////////////////////////////////////
// Macros to write commands/pixel colour data to an Raspberry Pi TFT
////////////////////////////////////////////////////////////////////////////////////////
#elif  defined (RPI_DISPLAY_TYPE)

   #define tft_Write_8(C) \
  { spiBuffer[0] = 0; spiBuffer[1] = C; \
  spi.writeBytes(spiBuffer, 2); }

  #define tft_Write_16(C) \
  { spiBuffer[0] = (C)>>8; spiBuffer[1] = C; \
  spi.writeBytes(spiBuffer, 2); }

  #define tft_Write_16S(C) \
  { spiBuffer[0] = C; spiBuffer[1] = (C)>>8; \
  spi.writeBytes(spiBuffer, 2); }

  #define tft_Write_32(C) \
  { spiBuffer[0] = (C)>>24; spiBuffer[1] = (C)>>16; spiBuffer[2] = (C)>>8; spiBuffer[3] = C; \
  spi.writeBytes(spiBuffer, 4); }

  #define tft_Write_32C(C,D) \
  { spiBuffer[1] = ((C)>>8); spiBuffer[3] = (C); spiBuffer[5] = ((D)>>8); spiBuffer[7] = D; \
  spi.writeBytes(spiBuffer, 8); }

  #define tft_Write_32D(C) \
  { spiBuffer[1] = ((C)>>8); spiBuffer[3] = (C); spiBuffer[5] = ((C)>>8); spiBuffer[7] = C; \
  spi.writeBytes(spiBuffer, 8); }

////////////////////////////////////////////////////////////////////////////////////////
// Macros for all other SPI displays
////////////////////////////////////////////////////////////////////////////////////////
#else
    // #define tft_Write_8(C)   spi.write(C)
  #if defined(ST7789_DRIVER) || defined(ST7789_2_DRIVER)
    // Temporary workaround for issue #510 part 2
    // #define tft_Write_8(C)   spi.transfer(C)
    #define tft_Write_8(C)   spi.write(C)
  #else
    #define tft_Write_8(C) \
    { spiBuffer[0] = C; \
    spi.writeBytes(spiBuffer, 1);}
  #endif

    #define tft_Write_16(C)     \
    { spiBuffer[0] = (C)>>8; spiBuffer[1] = C; \
    spi.writeBytes(spiBuffer, 2); }
    
    #define tft_Write_16S(C)    \
    { spiBuffer[0] = C; spiBuffer[1] = (C)>>8; \
    spi.writeBytes(spiBuffer, 2); }

    #define tft_Write_32(C)   \
    { spiBuffer[0] = (C)>>24; spiBuffer[1] = (C)>>16; spiBuffer[2] = (C)>>8; spiBuffer[3] = C; \
    spi.writeBytes(spiBuffer, 4); }

    #define tft_Write_32C(C,D)    \
    { spiBuffer[0] = (C)>>8; spiBuffer[1] = C; spiBuffer[2] = (D)>>8; spiBuffer[3] = D; \
    spi.writeBytes(spiBuffer, 4); }

    #define tft_Write_32D(C) \
    { spiBuffer[0] = spiBuffer[2] = (C)>>8; spiBuffer[1] = spiBuffer[3] = C; \
    spi.writeBytes(spiBuffer, 4); }

#endif

#ifndef tft_Write_16N
  #define tft_Write_16N tft_Write_16
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Macros to read from display using SPI or software SPI
////////////////////////////////////////////////////////////////////////////////////////
#if !defined (TFT_PARALLEL_8_BIT)
  #if defined (TFT_SDA_READ)
    // Use a bit banged function call for STM32 and bi-directional SDA pin
    #define TFT_eSPI_ENABLE_8_BIT_READ // Enable tft_Read_8(void);
    #define SCLK_L fastCLrPin(TFT_SCLK)
    #define SCLK_H fastSetPin(TFT_SCLK)
  #else
    // Use a SPI read transfer
    #define tft_Read_8() spi.transfer(0)
  #endif
#endif


// Concatenate a byte sequence A,B,C,D to CDAB, P is a uint8_t pointer
#define DAT8TO32(P) ( (uint32_t)P[0]<<8 | P[1] | P[2]<<24 | P[3]<<16 )


#if defined(ARDUINO_ARCH_LE501X) || defined(LE501X)

  #define ltoa itoa

#endif

#endif // Header end
