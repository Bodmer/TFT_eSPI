// Change the width and height if required (defined in portrait mode)
// or use the constructor to over-ride defaults
#ifndef TFT_WIDTH
  #define TFT_WIDTH  240
#endif
#ifndef TFT_HEIGHT
  #define TFT_HEIGHT 320
#endif


// Color definitions for backwards compatibility with old sketches
// use colour definitions like TFT_BLACK to make sketches more portable
#define ST7735_BLACK       0x0000      /*   0,   0,   0 */
#define ST7735_NAVY        0x000F      /*   0,   0, 128 */
#define ST7735_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ST7735_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ST7735_MAROON      0x7800      /* 128,   0,   0 */
#define ST7735_PURPLE      0x780F      /* 128,   0, 128 */
#define ST7735_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ST7735_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ST7735_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ST7735_BLUE        0x001F      /*   0,   0, 255 */
#define ST7735_GREEN       0x07E0      /*   0, 255,   0 */
#define ST7735_CYAN        0x07FF      /*   0, 255, 255 */
#define ST7735_RED         0xF800      /* 255,   0,   0 */
#define ST7735_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ST7735_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ST7735_WHITE       0xFFFF      /* 255, 255, 255 */
#define ST7735_ORANGE      0xFD20      /* 255, 165,   0 */
#define ST7735_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ST7735_PINK        0xF81F

#define ILI9341_BLACK ST7735_BLACK
#define ILI9341_WHITE ST7735_WHITE

#define ILI9341_VSCRDEF ST7787_SCRLAR
#define ILI9341_VSCRSADD ST7787_VSCSAD

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80


// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0x00
#define TFT_SWRST   0x01

#define TFT_CASET   0x2A
#define TFT_PASET   0x2B
#define TFT_RAMWR   0x2C

#define TFT_RAMRD   0x2E
#define TFT_IDXRD   0x00 //0xDD // ILI9341 only, indexed control register read

#define TFT_MADCTL  0x36
#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH  0x04
#define TFT_MAD_RGB 0x00

#define TFT_INVOFF  0x20
#define TFT_INVON   0x21

// ST7787 specific commands used in init
#define ST7787_NOP     0x00
#define ST7787_SWRESET 0x01
#define ST7787_RDDID   0x04
#define ST7787_RDDST   0x09

#define ST7787_SLPIN   0x10
#define ST7787_SLPOUT  0x11
#define ST7787_PTLON   0x12
#define ST7787_NORON   0x13

#define ST7787_INVOFF  0x20
#define ST7787_INVON   0x21
#define ST7787_DISPOFF 0x28
#define ST7787_DISPON  0x29
#define ST7787_CASET   0x2A
#define ST7787_RASET   0x2B // PASET
#define ST7787_RAMWR   0x2C
#define ST7787_RAMRD   0x2E

#define ST7787_PTLAR   0x30
#define ST7787_SCRLAR  0x33
#define ST7787_TEOFF   0x34
#define ST7787_TEON    0x35
#define ST7787_MADCTL  0x36
#define ST7787_VSCSAD  0x37
#define ST7787_COLMOD  0x3A

#define ST7787_FRMCTR1 0xB1
#define ST7787_FRMCTR2 0xB2
#define ST7787_FRMCTR3 0xB3
#define ST7787_INVCTR  0xB4
#define ST7787_DISSET5 0xB6
#define ST7787_VSYNCOUT 0xBC
#define ST7787_VSYNCIN 0xBD

#define ST7787_PWCTR1  0xC0
#define ST7787_PWCTR2  0xC1
#define ST7787_PWCTR3  0xC2
#define ST7787_PWCTR4  0xC3
#define ST7787_PWCTR5  0xC4
#define ST7787_VMCTR1  0xC5

#define ST7787_RDID1   0xDA
#define ST7787_RDID2   0xDB
#define ST7787_RDID3   0xDC

#define ST7787_GMCTRP1 0xE0
#define ST7787_GMCTRN1 0xE1

/* Timing delays from ST7787 datasheet (in nanoseconds). */
#define ST7787_T_CSS 60
#define ST7787_T_SCC 20
#define ST7787_T_CHW 40
#define ST7787_T_SHW 20
#define ST7787_T_SLW 20
#define ST7787_T_SHR 60
#define ST7787_T_SLR 60
