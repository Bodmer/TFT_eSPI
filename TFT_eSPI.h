/***************************************************
  Arduino TFT graphics library targeted at ESP8266
  and ESP32 based boards.

  This is a standalone library that contains the
  hardware driver, the graphics functions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce
  their FLASH footprint.

 ****************************************************/

// Stop fonts etc being loaded multiple times
#ifndef _TFT_eSPIH_
#define _TFT_eSPIH_

#define TFT_ESPI_VERSION "1.4.21"

//#define ESP32 //Just used to test ESP32 options

// Include header file that defines the fonts loaded, the TFT drivers
// available and the pins to be used
#include <User_Setup_Select.h>

#ifndef TAB_COLOUR
  #define TAB_COLOUR 0
#endif

// If the frequency is not defined, set a default
#ifndef SPI_FREQUENCY
  #define SPI_FREQUENCY  20000000
#endif

// If the frequency is not defined, set a default
#ifndef SPI_READ_FREQUENCY
  #define SPI_READ_FREQUENCY SPI_FREQUENCY
#endif

#if defined(ST7789_DRIVER) || defined(ST7789_2_DRIVER)
  #define TFT_SPI_MODE SPI_MODE3
#else
  #define TFT_SPI_MODE SPI_MODE0
#endif

// If the frequency is not defined, set a default
#ifndef SPI_TOUCH_FREQUENCY
  #define SPI_TOUCH_FREQUENCY  2500000
#endif

// Use GLCD font in error case where user requests a smooth font file
// that does not exist (this is a temporary fix to stop ESP32 reboot)
#ifdef SMOOTH_FONT
  #ifndef LOAD_GLCD
    #define LOAD_GLCD
  #endif
#endif

// Only load the fonts defined in User_Setup.h (to save space)
// Set flag so RLE rendering code is optionally compiled
#ifdef LOAD_GLCD
  #include <Fonts/glcdfont.c>
#endif

#ifdef LOAD_FONT2
  #include <Fonts/Font16.h>
#endif

#ifdef LOAD_FONT4
  #include <Fonts/Font32rle.h>
  #define LOAD_RLE
#endif

#ifdef LOAD_FONT6
  #include <Fonts/Font64rle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#ifdef LOAD_FONT7
  #include <Fonts/Font7srle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#ifdef LOAD_FONT8
  #include <Fonts/Font72rle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#elif defined LOAD_FONT8N
  #define LOAD_FONT8
  #include <Fonts/Font72x53rle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#include <Arduino.h>
#include <Print.h>

#include <pgmspace.h>

#include <SPI.h>

#ifdef ESP32
  #include "soc/spi_reg.h"
  #ifdef USE_HSPI_PORT
    #define SPI_PORT HSPI
  #else
    #define SPI_PORT VSPI
  #endif
#endif

#ifdef SMOOTH_FONT
  // Call up the SPIFFS FLASH filing system for the anti-aliased fonts
  #define FS_NO_GLOBALS
  #include <FS.h>

  #ifdef ESP32
    #include "SPIFFS.h"
  #endif
#endif

#ifndef TFT_DC
  #define DC_C // No macro allocated so it generates no code
  #define DC_D // No macro allocated so it generates no code
#else
  #if defined (ESP8266) && (TFT_DC == 16)
    #define DC_C digitalWrite(TFT_DC, LOW)
    #define DC_D digitalWrite(TFT_DC, HIGH)
  #elif defined (ESP32)
    #if defined (ESP32_PARALLEL)
      #define DC_C GPIO.out_w1tc = (1 << TFT_DC)
      #define DC_D GPIO.out_w1ts = (1 << TFT_DC)

    #else
      #if TFT_DC >= 32
        #ifdef RPI_ILI9486_DRIVER  // RPi display needs a slower DC change
          #define DC_C GPIO.out1_w1ts.val = (1 << (TFT_DC - 32)); \
                       GPIO.out1_w1tc.val = (1 << (TFT_DC - 32))
          #define DC_D GPIO.out1_w1tc.val = (1 << (TFT_DC - 32)); \
                       GPIO.out1_w1ts.val = (1 << (TFT_DC - 32))
        #else
          #define DC_C GPIO.out1_w1tc.val = (1 << (TFT_DC - 32))//;GPIO.out1_w1tc.val = (1 << (TFT_DC - 32))
          #define DC_D GPIO.out1_w1ts.val = (1 << (TFT_DC - 32))//;GPIO.out1_w1ts.val = (1 << (TFT_DC - 32))
        #endif
      #else
        #if TFT_DC >= 0
          #ifdef RPI_ILI9486_DRIVER  // RPi display needs a slower DC change
            #define DC_C GPIO.out_w1ts = (1 << TFT_DC); \
                         GPIO.out_w1tc = (1 << TFT_DC)
            #define DC_D GPIO.out_w1tc = (1 << TFT_DC); \
                         GPIO.out_w1ts = (1 << TFT_DC)
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
  #else
    #define DC_C GPOC=dcpinmask
    #define DC_D GPOS=dcpinmask
  #endif
#endif

#if defined (TFT_SPI_OVERLAP)
  #undef TFT_CS
  #define SPI1U_WRITE (SPIUMOSI | SPIUSSE | SPIUCSSETUP | SPIUCSHOLD)
  #define SPI1U_READ  (SPIUMOSI | SPIUSSE | SPIUCSSETUP | SPIUCSHOLD | SPIUDUPLEX)
#else
  #ifdef ESP8266
    #define SPI1U_WRITE (SPIUMOSI | SPIUSSE)
    #define SPI1U_READ  (SPIUMOSI | SPIUSSE | SPIUDUPLEX)
  #endif
#endif

#ifndef TFT_CS
  #define CS_L // No macro allocated so it generates no code
  #define CS_H // No macro allocated so it generates no code
#else
  #if defined (ESP8266) && (TFT_CS == 16)
    #define CS_L digitalWrite(TFT_CS, LOW)
    #define CS_H digitalWrite(TFT_CS, HIGH)
  #elif defined (ESP32)
    #if defined (ESP32_PARALLEL)
      #define CS_L // The TFT CS is set permanently low during init()
      #define CS_H
    #else
      #if TFT_CS >= 32
        #ifdef RPI_ILI9486_DRIVER  // RPi display needs a slower CS change
          #define CS_L GPIO.out1_w1ts.val = (1 << (TFT_CS - 32)); \
                       GPIO.out1_w1tc.val = (1 << (TFT_CS - 32))
          #define CS_H GPIO.out1_w1tc.val = (1 << (TFT_CS - 32)); \
                       GPIO.out1_w1ts.val = (1 << (TFT_CS - 32))
        #else
          #define CS_L GPIO.out1_w1tc.val = (1 << (TFT_CS - 32)); GPIO.out1_w1tc.val = (1 << (TFT_CS - 32))
          #define CS_H GPIO.out1_w1ts.val = (1 << (TFT_CS - 32))//;GPIO.out1_w1ts.val = (1 << (TFT_CS - 32))
        #endif
      #else
        #if TFT_CS >= 0
          #ifdef RPI_ILI9486_DRIVER  // RPi display needs a slower CS change
            #define CS_L GPIO.out_w1ts = (1 << TFT_CS); GPIO.out_w1tc = (1 << TFT_CS)
            #define CS_H GPIO.out_w1tc = (1 << TFT_CS); GPIO.out_w1ts = (1 << TFT_CS)
          #else
            #define CS_L GPIO.out_w1tc = (1 << TFT_CS);GPIO.out_w1tc = (1 << TFT_CS)
            #define CS_H GPIO.out_w1ts = (1 << TFT_CS)//;GPIO.out_w1ts = (1 << TFT_CS)
          #endif
        #else
          #define CS_L
          #define CS_H
        #endif
      #endif
    #endif
  #else
    #define CS_L GPOC=cspinmask
    #define CS_H GPOS=cspinmask
  #endif
#endif

// Use single register write for CS_L and DC_C if pins are both in range 0-31
#ifdef ESP32
  #ifdef TFT_CS
    #if (TFT_CS >= 0) && (TFT_CS < 32) && (TFT_DC >= 0) && (TFT_DC < 32)
      #ifdef RPI_ILI9486_DRIVER  // RPi display needs a slower CD and DC change
        #define CS_L_DC_C GPIO.out_w1tc = ((1 << TFT_CS) | (1 << TFT_DC)); \
                          GPIO.out_w1tc = ((1 << TFT_CS) | (1 << TFT_DC))
      #else
        #define CS_L_DC_C GPIO.out_w1tc = ((1 << TFT_CS) | (1 << TFT_DC)); GPIO.out_w1tc = ((1 << TFT_CS) | (1 << TFT_DC))
      #endif
    #else
      #define CS_L_DC_C CS_L; DC_C
    #endif
  #else
    #define CS_L_DC_C CS_L; DC_C
  #endif
#else // ESP8266
  #define CS_L_DC_C CS_L; DC_C
#endif

// chip select signal for touchscreen
#ifndef TOUCH_CS
  #define T_CS_L // No macro allocated so it generates no code
  #define T_CS_H // No macro allocated so it generates no code
#else
  #define T_CS_L digitalWrite(TOUCH_CS, LOW)
  #define T_CS_H digitalWrite(TOUCH_CS, HIGH)
#endif


#ifdef TFT_WR
  #if defined (ESP32)
    #define WR_L GPIO.out_w1tc = (1 << TFT_WR)
    #define WR_H GPIO.out_w1ts = (1 << TFT_WR)
  #else
    #define WR_L GPOC=wrpinmask
    #define WR_H GPOS=wrpinmask
  #endif
#endif

#ifdef ESP8266
  // Concatenate two 16 bit values for the SPI 32 bit register write
  #define SPI_32(H,L) ( (H)<<16 | (L) )
  #define COL_32(H,L) ( (H)<<16 | (L) )
#else
  #if defined (ESP32_PARALLEL) || defined (ILI9488_DRIVER)
    #define SPI_32(H,L) ( (H)<<16 | (L) )
  #else
    #define SPI_32(H,L) ( ((H)<<8 | (H)>>8) | (((L)<<8 | (L)>>8)<<16 ) )
  #endif
  // Swap byte order for concatenated 16 bit colors
  // AB CD -> DCBA for 32 bit register write
  #define COL_32(H,L) ( ((H)<<8 | (H)>>8) | (((L)<<8 | (L)>>8)<<16 ) )
#endif

#if defined (ESP32) && defined (ESP32_PARALLEL)
  // Mask for the 8 data bits to set pin directions
  #define dir_mask ((1 << TFT_D0) | (1 << TFT_D1) | (1 << TFT_D2) | (1 << TFT_D3) | (1 << TFT_D4) | (1 << TFT_D5) | (1 << TFT_D6) | (1 << TFT_D7))

  // Data bits and the write line are cleared to 0 in one step
  #define clr_mask (dir_mask | (1 << TFT_WR))

  // A lookup table is used to set the different bit patterns, this uses 1kByte of RAM
  #define set_mask(C) xset_mask[C] // 63fps Sprite rendering test 33% faster, graphicstest only 1.8% faster than shifting in real time

  // Real-time shifting alternative to above to save 1KByte RAM, 47 fps Sprite rendering test
  /*#define set_mask(C) ((C&0x80)>>7)<<TFT_D7 | ((C&0x40)>>6)<<TFT_D6 | ((C&0x20)>>5)<<TFT_D5 | ((C&0x10)>>4)<<TFT_D4 | \
                        ((C&0x08)>>3)<<TFT_D3 | ((C&0x04)>>2)<<TFT_D2 | ((C&0x02)>>1)<<TFT_D1 | ((C&0x01)>>0)<<TFT_D0
  //*/

  // Write 8 bits to TFT
  #define tft_Write_8(C)  GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t)C); WR_H

  // Write 16 bits to TFT
  #ifdef PSEUDO_8_BIT
    #define tft_Write_16(C) WR_L;GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t)(C >> 0)); WR_H
  #else
    #define tft_Write_16(C) GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t)(C >> 8)); WR_H; \
                            GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t)(C >> 0)); WR_H
  #endif

  // 16 bit write with swapped bytes
  #define tft_Write_16S(C) GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) (C >>  0)); WR_H; \
                           GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) (C >>  8)); WR_H

  // Write 32 bits to TFT
  #define tft_Write_32(C) GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) (C >> 24)); WR_H; \
                          GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) (C >> 16)); WR_H; \
                          GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) (C >>  8)); WR_H; \
                          GPIO.out_w1tc = clr_mask; GPIO.out_w1ts = set_mask((uint8_t) (C >>  0)); WR_H

  #ifdef TFT_RD
    #define RD_L GPIO.out_w1tc = (1 << TFT_RD)
    //#define RD_L digitalWrite(TFT_WR, LOW)
    #define RD_H GPIO.out_w1ts = (1 << TFT_RD)
    //#define RD_H digitalWrite(TFT_WR, HIGH)
  #endif

#elif  defined (ILI9488_DRIVER) // 16 bit colour converted to 3 bytes for 18 bit RGB

  // Write 8 bits to TFT
  #define tft_Write_8(C)   spi.transfer(C)

  // Convert 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16(C)  spi.transfer((C & 0xF800)>>8); \
                           spi.transfer((C & 0x07E0)>>3); \
                           spi.transfer((C & 0x001F)<<3)

  // Convert swapped byte 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16S(C) spi.transfer(C & 0xF8); \
                           spi.transfer((C & 0xE000)>>11 | (C & 0x07)<<5); \
                           spi.transfer((C & 0x1F00)>>5)
  // Write 32 bits to TFT
  #define tft_Write_32(C)  spi.write32(C)

#elif  defined (RPI_ILI9486_DRIVER)

  #define tft_Write_8(C)   spi.transfer(0); spi.transfer(C)
  #define tft_Write_16(C)  spi.write16(C)
  #define tft_Write_16S(C) spi.write16(C<<8 | C>>8)
  #define tft_Write_32(C)  spi.write32(C)

#elif defined ESP8266

  #define tft_Write_8(C)   spi.write(C)
  #define tft_Write_16(C)  spi.write16(C)
  #define tft_Write_32(C)  spi.write32(C)

#else // ESP32 using SPI with 16 bit color display

  // ESP32 low level SPI writes for 8, 16 and 32 bit values
  // to avoid the function call overhead
  
  // Write 8 bits
  #define tft_Write_8(C) \
  WRITE_PERI_REG(SPI_MOSI_DLEN_REG(SPI_PORT), 8-1); \
  WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), C); \
  SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR); \
  while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);

  // Write 16 bits with corrected endianess for 16 bit colours
  #define tft_Write_16(C) \
  WRITE_PERI_REG(SPI_MOSI_DLEN_REG(SPI_PORT), 16-1); \
  WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), C<<8 | C>>8); \
  SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR); \
  while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);

  // Write 16 bits
  #define tft_Write_16S(C) \
  WRITE_PERI_REG(SPI_MOSI_DLEN_REG(SPI_PORT), 16-1); \
  WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), C); \
  SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR); \
  while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);

  // Write 32 bits
  #define tft_Write_32(C) \
  WRITE_PERI_REG(SPI_MOSI_DLEN_REG(SPI_PORT), 32-1); \
  WRITE_PERI_REG(SPI_W0_REG(SPI_PORT), C); \
  SET_PERI_REG_MASK(SPI_CMD_REG(SPI_PORT), SPI_USR); \
  while (READ_PERI_REG(SPI_CMD_REG(SPI_PORT))&SPI_USR);

#endif


#if !defined (ESP32_PARALLEL)

  // Read from display using SPI or software SPI
  #if defined (ESP8266) && defined (TFT_SDA_READ)
    // Use a bit banged function call for ESP8266 and bi-directional SDA pin
    #define SCLK_L GPOC=sclkpinmask
    #define SCLK_H GPOS=sclkpinmask
  #else
    // Use a SPI read transfer
    #define tft_Read_8() spi.transfer(0)
  #endif

  // Make sure TFT_MISO is defined if not used to avoid an error message
  #ifndef TFT_MISO
    #define TFT_MISO -1
  #endif

#endif


#ifdef LOAD_GFXFF
  // We can include all the free fonts and they will only be built into
  // the sketch if they are used

  #include <Fonts/GFXFF/gfxfont.h>

  // Call up any user custom fonts
  #include <User_Setups/User_Custom_Fonts.h>

  // Original Adafruit_GFX "Free Fonts"
  #include <Fonts/GFXFF/TomThumb.h>  // TT1

  #include <Fonts/GFXFF/FreeMono9pt7b.h>  // FF1 or FM9
  #include <Fonts/GFXFF/FreeMono12pt7b.h> // FF2 or FM12
  #include <Fonts/GFXFF/FreeMono18pt7b.h> // FF3 or FM18
  #include <Fonts/GFXFF/FreeMono24pt7b.h> // FF4 or FM24

  #include <Fonts/GFXFF/FreeMonoOblique9pt7b.h>  // FF5 or FMO9
  #include <Fonts/GFXFF/FreeMonoOblique12pt7b.h> // FF6 or FMO12
  #include <Fonts/GFXFF/FreeMonoOblique18pt7b.h> // FF7 or FMO18
  #include <Fonts/GFXFF/FreeMonoOblique24pt7b.h> // FF8 or FMO24
  
  #include <Fonts/GFXFF/FreeMonoBold9pt7b.h>  // FF9  or FMB9
  #include <Fonts/GFXFF/FreeMonoBold12pt7b.h> // FF10 or FMB12
  #include <Fonts/GFXFF/FreeMonoBold18pt7b.h> // FF11 or FMB18
  #include <Fonts/GFXFF/FreeMonoBold24pt7b.h> // FF12 or FMB24
  
  #include <Fonts/GFXFF/FreeMonoBoldOblique9pt7b.h>  // FF13 or FMBO9
  #include <Fonts/GFXFF/FreeMonoBoldOblique12pt7b.h> // FF14 or FMBO12
  #include <Fonts/GFXFF/FreeMonoBoldOblique18pt7b.h> // FF15 or FMBO18
  #include <Fonts/GFXFF/FreeMonoBoldOblique24pt7b.h> // FF16 or FMBO24
  
  // Sans serif fonts
  #include <Fonts/GFXFF/FreeSans9pt7b.h>  // FF17 or FSS9
  #include <Fonts/GFXFF/FreeSans12pt7b.h> // FF18 or FSS12
  #include <Fonts/GFXFF/FreeSans18pt7b.h> // FF19 or FSS18
  #include <Fonts/GFXFF/FreeSans24pt7b.h> // FF20 or FSS24
  
  #include <Fonts/GFXFF/FreeSansOblique9pt7b.h>  // FF21 or FSSO9
  #include <Fonts/GFXFF/FreeSansOblique12pt7b.h> // FF22 or FSSO12
  #include <Fonts/GFXFF/FreeSansOblique18pt7b.h> // FF23 or FSSO18
  #include <Fonts/GFXFF/FreeSansOblique24pt7b.h> // FF24 or FSSO24
  
  #include <Fonts/GFXFF/FreeSansBold9pt7b.h>  // FF25 or FSSB9
  #include <Fonts/GFXFF/FreeSansBold12pt7b.h> // FF26 or FSSB12
  #include <Fonts/GFXFF/FreeSansBold18pt7b.h> // FF27 or FSSB18
  #include <Fonts/GFXFF/FreeSansBold24pt7b.h> // FF28 or FSSB24
  
  #include <Fonts/GFXFF/FreeSansBoldOblique9pt7b.h>  // FF29 or FSSBO9
  #include <Fonts/GFXFF/FreeSansBoldOblique12pt7b.h> // FF30 or FSSBO12
  #include <Fonts/GFXFF/FreeSansBoldOblique18pt7b.h> // FF31 or FSSBO18
  #include <Fonts/GFXFF/FreeSansBoldOblique24pt7b.h> // FF32 or FSSBO24
  
  // Serif fonts
  #include <Fonts/GFXFF/FreeSerif9pt7b.h>  // FF33 or FS9
  #include <Fonts/GFXFF/FreeSerif12pt7b.h> // FF34 or FS12
  #include <Fonts/GFXFF/FreeSerif18pt7b.h> // FF35 or FS18
  #include <Fonts/GFXFF/FreeSerif24pt7b.h> // FF36 or FS24
  
  #include <Fonts/GFXFF/FreeSerifItalic9pt7b.h>  // FF37 or FSI9
  #include <Fonts/GFXFF/FreeSerifItalic12pt7b.h> // FF38 or FSI12
  #include <Fonts/GFXFF/FreeSerifItalic18pt7b.h> // FF39 or FSI18
  #include <Fonts/GFXFF/FreeSerifItalic24pt7b.h> // FF40 or FSI24
  
  #include <Fonts/GFXFF/FreeSerifBold9pt7b.h>  // FF41 or FSB9
  #include <Fonts/GFXFF/FreeSerifBold12pt7b.h> // FF42 or FSB12
  #include <Fonts/GFXFF/FreeSerifBold18pt7b.h> // FF43 or FSB18
  #include <Fonts/GFXFF/FreeSerifBold24pt7b.h> // FF44 or FSB24
  
  #include <Fonts/GFXFF/FreeSerifBoldItalic9pt7b.h>  // FF45 or FSBI9
  #include <Fonts/GFXFF/FreeSerifBoldItalic12pt7b.h> // FF46 or FSBI12
  #include <Fonts/GFXFF/FreeSerifBoldItalic18pt7b.h> // FF47 or FSBI18
  #include <Fonts/GFXFF/FreeSerifBoldItalic24pt7b.h> // FF48 or FSBI24
  
#endif // #ifdef LOAD_GFXFF

//These enumerate the text plotting alignment (reference datum point)
#define TL_DATUM 0 // Top left (default)
#define TC_DATUM 1 // Top centre
#define TR_DATUM 2 // Top right
#define ML_DATUM 3 // Middle left
#define CL_DATUM 3 // Centre left, same as above
#define MC_DATUM 4 // Middle centre
#define CC_DATUM 4 // Centre centre, same as above
#define MR_DATUM 5 // Middle right
#define CR_DATUM 5 // Centre right, same as above
#define BL_DATUM 6 // Bottom left
#define BC_DATUM 7 // Bottom centre
#define BR_DATUM 8 // Bottom right
#define L_BASELINE  9 // Left character baseline (Line the 'A' character would sit on)
#define C_BASELINE 10 // Centre character baseline
#define R_BASELINE 11 // Right character baseline


// New color definitions use for all my libraries
#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
#define TFT_PINK        0xFC9F

// Next is a special 16 bit colour value that encodes to 8 bits
// and will then decode back to the same 16 bit value.
// Convenient for 8 bit and 16 bit transparent sprites.
#define TFT_TRANSPARENT 0x0120

// Swap any type
template <typename T> static inline void
swap_coord(T& a, T& b) { T t = a; a = b; b = t; }

#ifndef min
  // Return minimum of two numbers, may already be defined
  #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

// This structure allows sketches to retrieve the user setup parameters at runtime
// by calling getSetup(), zero impact on code size unless used, mainly for diagnostics
typedef struct
{
String  version = TFT_ESPI_VERSION;
int16_t esp;
uint8_t trans;
uint8_t serial;
uint8_t overlap;

#if defined (ESP32)
  #if defined (USE_HSPI_PORT)
    uint8_t  port = HSPI;
  #else
    uint8_t  port = VSPI;
  #endif
#endif

uint16_t tft_driver; // Hexadecimal code
uint16_t tft_width;  // Rotation 0 width and height
uint16_t tft_height;

uint8_t r0_x_offset; // Offsets, not all used yet
uint8_t r0_y_offset;
uint8_t r1_x_offset;
uint8_t r1_y_offset;
uint8_t r2_x_offset;
uint8_t r2_y_offset;
uint8_t r3_x_offset;
uint8_t r3_y_offset;

int8_t pin_tft_mosi;
int8_t pin_tft_miso;
int8_t pin_tft_clk;
int8_t pin_tft_cs;

int8_t pin_tft_dc;
int8_t pin_tft_rd;
int8_t pin_tft_wr;
int8_t pin_tft_rst;

int8_t pin_tft_d0;
int8_t pin_tft_d1;
int8_t pin_tft_d2;
int8_t pin_tft_d3;
int8_t pin_tft_d4;
int8_t pin_tft_d5;
int8_t pin_tft_d6;
int8_t pin_tft_d7;

int8_t pin_tch_cs;

int16_t tft_spi_freq;
int16_t tft_rd_freq;
int16_t tch_spi_freq;
} setup_t;

// This is a structure to conveniently hold information on the default fonts
// Stores pointer to font character image address table, width table and height

// Create a null set in case some fonts not used (to prevent crash)
const  uint8_t widtbl_null[1] = {0};
PROGMEM const uint8_t chr_null[1] = {0};
PROGMEM const uint8_t* const chrtbl_null[1] = {chr_null};

typedef struct {
    const uint8_t *chartbl;
    const uint8_t *widthtbl;
    uint8_t height;
    uint8_t baseline;
    } fontinfo;

// Now fill the structure
const PROGMEM fontinfo fontdata [] = {
  #ifdef LOAD_GLCD
   { (const uint8_t *)font, widtbl_null, 0, 0 },
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif
   // GLCD font (Font 1) does not have all parameters
   { (const uint8_t *)chrtbl_null, widtbl_null, 8, 7 },

  #ifdef LOAD_FONT2
   { (const uint8_t *)chrtbl_f16, widtbl_f16, chr_hgt_f16, baseline_f16},
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif

   // Font 3 current unused
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },

  #ifdef LOAD_FONT4
   { (const uint8_t *)chrtbl_f32, widtbl_f32, chr_hgt_f32, baseline_f32},
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif

   // Font 5 current unused
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },

  #ifdef LOAD_FONT6
   { (const uint8_t *)chrtbl_f64, widtbl_f64, chr_hgt_f64, baseline_f64},
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif

  #ifdef LOAD_FONT7
   { (const uint8_t *)chrtbl_f7s, widtbl_f7s, chr_hgt_f7s, baseline_f7s},
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 },
  #endif

  #ifdef LOAD_FONT8
   { (const uint8_t *)chrtbl_f72, widtbl_f72, chr_hgt_f72, baseline_f72}
  #else
   { (const uint8_t *)chrtbl_null, widtbl_null, 0, 0 }
  #endif
};

typedef uint16_t (*getColorCallback)(uint16_t x, uint16_t y);

// Class functions and variables
class TFT_eSPI : public Print {

 public:

  TFT_eSPI(int16_t _W = TFT_WIDTH, int16_t _H = TFT_HEIGHT);

  void     init(uint8_t tc = TAB_COLOUR), begin(uint8_t tc = TAB_COLOUR); // Same - begin included for backwards compatibility

  // These are virtual so the TFT_eSprite class can override them with sprite specific functions
  virtual void     drawPixel(int32_t x, int32_t y, uint32_t color),
                   drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size),
                   drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color),
                   drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color),
                   drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color),
                   fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

  virtual int16_t  drawChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font),
                   drawChar(uint16_t uniCode, int32_t x, int32_t y),
                   height(void),
                   width(void);

  // The TFT_eSprite class inherits the following functions
  void     setWindow(int32_t xs, int32_t ys, int32_t xe, int32_t ye),
           pushColor(uint16_t color),
           pushColor(uint16_t color, uint32_t len),
           pushColors(uint16_t  *data, uint32_t len, bool swap = true), // With byte swap option
           pushColors(uint8_t  *data, uint32_t len),

           fillScreen(uint32_t color);

  void     drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color),
           drawRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color),
           fillRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color),

           setRotation(uint8_t r),
           invertDisplay(boolean i),

           drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
           drawCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, uint32_t color),
           fillCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color),
           fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint32_t color),

           drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color),
           fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color),

           drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color),
           fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color),

           drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color),
           drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color),
           drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor),
           setBitmapColor(uint16_t fgcolor, uint16_t bgcolor), // For 1bpp sprites
           setPivot(int16_t x, int16_t y),
           setCursor(int16_t x, int16_t y),
           setCursor(int16_t x, int16_t y, uint8_t font),
           setTextColor(uint16_t color),
           setTextColor(uint16_t fgcolor, uint16_t bgcolor),
           setTextSize(uint8_t size),

           setTextWrap(boolean wrapX, boolean wrapY = false),
           setTextDatum(uint8_t datum),
           setTextPadding(uint16_t x_width),

#ifdef LOAD_GFXFF
           setFreeFont(const GFXfont *f = NULL),
           setTextFont(uint8_t font),
#else
           setFreeFont(uint8_t font),
           setTextFont(uint8_t font),
#endif
           spiwrite(uint8_t),
           writecommand(uint8_t c),
           writedata(uint8_t d),

           commandList(const uint8_t *addr);

  uint8_t  readcommand8(uint8_t cmd_function, uint8_t index = 0);
  uint16_t readcommand16(uint8_t cmd_function, uint8_t index = 0);
  uint32_t readcommand32(uint8_t cmd_function, uint8_t index = 0);

           // Read the colour of a pixel at x,y and return value in 565 format 
  uint16_t readPixel(int32_t x0, int32_t y0);
  void     setCallback(getColorCallback getCol);

           // The next functions can be used as a pair to copy screen blocks (or horizontal/vertical lines) to another location
           // Read a block of pixels to a data buffer, buffer is 16 bit and the array size must be at least w * h
  void     readRect(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data);
           // Write a block of pixels to the screen
  void     pushRect(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data);

           // These are used to render images or sprites stored in RAM arrays
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data);
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data, uint16_t transparent);

           // These are used to render images stored in FLASH (PROGMEM)
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, const uint16_t *data, uint16_t transparent);
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, const uint16_t *data);

           // These are used by pushSprite for 1 and 8 bit colours
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t  *data, bool bpp8 = true);
  void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t  *data, uint8_t  transparent, bool bpp8 = true);

           // Swap the byte order for pushImage() - corrects endianness
  void     setSwapBytes(bool swap);
  bool     getSwapBytes(void);

           // This next function has been used successfully to dump the TFT screen to a PC for documentation purposes
           // It reads a screen area and returns the RGB 8 bit colour values of each pixel
           // Set w and h to 1 to read 1 pixel's colour. The data buffer must be at least w * h * 3 bytes
  void     readRectRGB(int32_t x0, int32_t y0, int32_t w, int32_t h, uint8_t *data);

  uint8_t  getRotation(void),
           getTextDatum(void),
           color16to8(uint16_t color565); // Convert 16 bit colour to 8 bits

  int16_t  getCursorX(void),
           getCursorY(void);

  int16_t  getPivotX(void),
           getPivotY(void);

  uint16_t fontsLoaded(void),
           color565(uint8_t red, uint8_t green, uint8_t blue),   // Convert 8 bit red, green and blue to 16 bits
           color8to16(uint8_t color332);  // Convert 8 bit colour to 16 bits

  int16_t  drawNumber(long long_num, int32_t poX, int32_t poY, uint8_t font),
           drawNumber(long long_num, int32_t poX, int32_t poY),
           drawFloat(float floatNumber, uint8_t decimal, int32_t poX, int32_t poY, uint8_t font),
           drawFloat(float floatNumber, uint8_t decimal, int32_t poX, int32_t poY),

           // Handle char arrays
           drawString(const char *string, int32_t poX, int32_t poY, uint8_t font),
           drawString(const char *string, int32_t poX, int32_t poY),
           drawCentreString(const char *string, int32_t dX, int32_t poY, uint8_t font), // Deprecated, use setTextDatum() and drawString()
           drawRightString(const char *string, int32_t dX, int32_t poY, uint8_t font),  // Deprecated, use setTextDatum() and drawString()

           // Handle String type
           drawString(const String& string, int32_t poX, int32_t poY, uint8_t font),
           drawString(const String& string, int32_t poX, int32_t poY),
           drawCentreString(const String& string, int32_t dX, int32_t poY, uint8_t font), // Deprecated, use setTextDatum() and drawString()
           drawRightString(const String& string, int32_t dX, int32_t poY, uint8_t font);  // Deprecated, use setTextDatum() and drawString()

  int16_t  textWidth(const char *string, uint8_t font),
           textWidth(const char *string),
           textWidth(const String& string, uint8_t font),
           textWidth(const String& string),
           fontHeight(int16_t font),
           fontHeight(void);

  void     setAddrWindow(int32_t xs, int32_t ys, int32_t w, int32_t h);

           // Compatibility additions
  void     startWrite(void);                         // Begin SPI transaction
  void     writeColor(uint16_t color, uint32_t len); // Write colours without transaction overhead
  void     endWrite(void);                           // End SPI transaction

  uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);
  uint16_t decodeUTF8(uint8_t c);
  size_t   write(uint8_t);

#ifdef TFT_SDA_READ
  #if defined (ESP8266) && defined (TFT_SDA_READ)
  uint8_t  tft_Read_8(void);
  #endif
  void     begin_SDA_Read(void);
  void     end_SDA_Read(void);
#endif

  // Set or get an arbitrary library attribute or configuration option
  void     setAttribute(uint8_t id = 0, uint8_t a = 0);
  uint8_t  getAttribute(uint8_t id = 0);

  void     getSetup(setup_t& tft_settings); // Sketch provides the instance to populate

  static   SPIClass& getSPIinstance(void);

  int32_t  cursor_x, cursor_y, padX;
  uint32_t textcolor, textbgcolor;

  uint32_t bitmap_fg, bitmap_bg;

  uint8_t  textfont,  // Current selected font
           textsize,  // Current font size multiplier
           textdatum, // Text reference datum
           rotation;  // Display rotation (0-3)

  int16_t _xpivot;   // x pivot point coordinate
  int16_t _ypivot;   // x pivot point coordinate

  uint8_t  decoderState = 0;   // UTF8 decoder state
  uint16_t decoderBuffer;      // Unicode code-point buffer

 private:

  inline void spi_begin() __attribute__((always_inline));
  inline void spi_end()   __attribute__((always_inline));

  inline void spi_begin_read() __attribute__((always_inline));
  inline void spi_end_read()   __attribute__((always_inline));

  void     readAddrWindow(int32_t xs, int32_t ys, int32_t w, int32_t h);

  uint8_t  tabcolor,
           colstart = 0, rowstart = 0; // some ST7735 displays need this changed

  volatile uint32_t *dcport, *csport;

  uint32_t cspinmask, dcpinmask, wrpinmask, sclkpinmask;

#if defined(ESP32_PARALLEL)
  uint32_t  xclr_mask, xdir_mask, xset_mask[256];
#endif

  uint32_t lastColor = 0xFFFF;

  getColorCallback getColor = nullptr;

 protected:

  int32_t  win_xe, win_ye;

  int32_t  _init_width, _init_height; // Display w/h as input, used by setRotation()
  int32_t  _width, _height;           // Display w/h as modified by current rotation
  int32_t  addr_row, addr_col;

  uint32_t fontsloaded;

  uint8_t  glyph_ab,   // glyph delta Y (height) above baseline
           glyph_bb;   // glyph delta Y (height) below baseline

  bool     isDigits;   // adjust bounding box for numbers to reduce visual jiggling
  bool     textwrapX, textwrapY;   // If set, 'wrap' text at right and optionally bottom edge of display
  bool     _swapBytes; // Swap the byte order for TFT pushImage()
  bool     locked, inTransaction; // Transaction and mutex lock flags for ESP32

  bool     _booted;    // init() or begin() has already run once
  bool     _cp437;     // If set, use correct CP437 charset (default is ON)
  bool     _utf8;      // If set, use UTF-8 decoder in print stream 'write()' function (default ON)

  uint32_t _lastColor; // Buffered value of last colour used

#ifdef LOAD_GFXFF
  GFXfont  *gfxFont;
#endif

// Load the Touch extension
#ifdef TOUCH_CS
  #include "Extensions/Touch.h"
#endif

// Load the Anti-aliased font extension
#ifdef SMOOTH_FONT
  #include "Extensions/Smooth_font.h"
#endif

}; // End of class TFT_eSPI

// Load the Button Class
#include "Extensions/Button.h"

// Load the Sprite Class
#include "Extensions/Sprite.h"

#endif
