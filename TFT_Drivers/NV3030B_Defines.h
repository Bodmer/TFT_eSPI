// Change the width and height if required (defined in portrait mode)
// or use the constructor to over-ride defaults
#ifndef TFT_WIDTH
  #define TFT_WIDTH  240
#endif
#ifndef TFT_HEIGHT
  #define TFT_HEIGHT 320
#endif

// 1.83" Round Rectangle Color IPS TFT Display
#if (TFT_HEIGHT == 280) && (TFT_WIDTH == 240)
  #ifndef CGRAM_OFFSET
    #define CGRAM_OFFSET
  #endif
#endif

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80 // Not used unless commandlist invoked


// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0x00
#define TFT_SWRST   0x01

#define TFT_SLPIN   0x10
#define TFT_SLPOUT  0x11
#define TFT_NORON   0x13

#define TFT_INVOFF  0x20
#define TFT_INVON   0x21
#define TFT_DISPOFF 0x28
#define TFT_DISPON  0x29
#define TFT_CASET   0x2A
#define TFT_PASET   0x2B
#define TFT_RAMWR   0x2C
#define TFT_RAMRD   0x2E
#define TFT_MADCTL  0x36

// Flags for TFT_MADCTL
#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_RGB 0x00
#define TFT_MAD_BGR 0x08

#ifdef TFT_RGB_ORDER
  #if (TFT_RGB_ORDER == 1)
    #define TFT_MAD_COLOR_ORDER TFT_MAD_RGB
  #else
    #define TFT_MAD_COLOR_ORDER TFT_MAD_BGR
  #endif
#else
  #ifdef CGRAM_OFFSET
    #define TFT_MAD_COLOR_ORDER TFT_MAD_BGR
  #else
    #define TFT_MAD_COLOR_ORDER TFT_MAD_RGB
  #endif
#endif

// All NV3030B specific commands some are used by init()
#define NV3030B_NOP 	0x00     // No-op register
#define NV3030B_SWRESET	0x01     // Software reset (NV3030A only)

#define NV3030B_RDDID 0x04   // Read display identification information
#define NV3030B_RDDST 0x09   // Read Display Status

#define NV3030B_SLPIN 0x10  // Enter Sleep Mode
#define NV3030B_SLPOUT 0x11 // Sleep Out
#define NV3030B_PTLON 0x12  // Partial Mode ON
#define NV3030B_NORON 0x13  // Normal Display Mode ON and Partial Mode OFF

#define NV3030B_RDMODE 0x0A     // Read Display Power Mode
#define NV3030B_RDMADCTL 0x0B   // Read Display MADCTL
#define NV3030B_RDPIXFMT 0x0C   // Read Display Pixel Format
#define NV3030B_RDIMGMODE 0x0D  // Read Display Image Mode
#define NV3030B_RDSIGNMODE 0x0E // Read Display Signal mode
#define NV3030B_RDSELFDIAG 0x0F // Read Display Self-Diagnostic Result

#define NV3030B_INVOFF 0x20   // Display Inversion OFF
#define NV3030B_INVON 0x21    // Display Inversion ON
#define NV3030B_DISPOFF 0x28  // Display OFF
#define NV3030B_DISPON 0x29   // Display ON

#define NV3030B_CASET 0x2A  // Column Address Set
#define NV3030B_PASET 0x2B  // Page Address Set
#define NV3030B_RAMWR 0x2C  // Memory Write
#define NV3030B_RAMRD 0x2E  // Memory Read (NV3030A only)

#define NV3030B_PTLAR 0x30    // Partial Area
#define NV3030B_VSCRDEF 0x33  // Vertical Scrolling Definition
#define NV3030B_TEOFF 0x34    // Tearing effect line off
#define NV3030B_TEON 0x35     // Tearing effect line on
#define NV3030B_MADCTL 0x36   // Memory Access Control
#define NV3030B_VSCRSADD 0x37 // Vertical Scrolling Start Address
#define NV3030B_IDLEOFF 0x38  // Idle mode off
#define NV3030B_IDLEON 0x39   // Idle mode on and other mode off

#define NV3030B_PIXFMT 0x3A    // Interface Pixel Format
#define NV3030B_RAMWRCNT 0x3C  // Write memory continue
#define NV3030B_RAMRDCNT 0x3E  // Read memory continue (NV3030A only)
#define NV3030B_TESCANSET 0x44 // Set tear scanline
#define NV3030B_TESCANGET 0x45 // Get tear scanline
#define NV3030B_WRDISBV 0x53   // Write display brightness (NV3030B only)
#define NV3030B_RDDISBV 0x54   // Read display brightness (NV3030B only)

#define NV3030B_RDIDD3 0xD3 // Read idd3
#define NV3030B_RDID1 0xDA  // Read display ID 1
#define NV3030B_RDID2 0xDB  // Read display ID 2
#define NV3030B_RDID3 0xDC  // Read display ID 3
