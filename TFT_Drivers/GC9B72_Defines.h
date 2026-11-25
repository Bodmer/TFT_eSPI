// Change the width and height if required (defined in portrait mode)
// or use the constructor to over-ride defaults
#ifndef TFT_WIDTH
  #define TFT_WIDTH  360
#endif
#ifndef TFT_HEIGHT
  #define TFT_HEIGHT 360
#endif

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80

// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0x00 //No Operation
#define TFT_SWRST   0x01 //Software_Reset

#define TFT_SLPIN   0x10 //Going Sleep
#define TFT_SLPOUT  0x11 //Wake up

#define TFT_INVOFF  0x20 //Inversion off
#define TFT_INVON   0x21 //Inversion on

#define TFT_DISPOFF 0x28 //Display off
#define TFT_DISPON  0x29 //Display on

#define TFT_CASET   0x2A //Column Adress Set
#define TFT_PASET   0x2B //Page_Row Adress Set
#define TFT_RAMWR   0x2C //Memory write

#define TFT_RAMRD   0x2E //Memory read
#define TFT_IDXRD   0x00 //0xDD // ILI9341 only, indexed control register read

#define TFT_MADCTL  0x36
#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH  0x04
#define TFT_MAD_RGB 0x00
