#ifndef TFT_WIDTH
  #define TFT_WIDTH 160
#endif
#ifndef TFT_HEIGHT
  #define TFT_HEIGHT 128
#endif

#define TFT_SPI_MODE SPI_MODE0

#define CGRAM_OFFSET

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80

// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0xE3
#define TFT_SWRST   TFT_NOP
#define TFT_CASET   0x15 // SETCOLUMN
#define TFT_PASET   0x75 // SETROW
#define TFT_RAMWR   0x5C // WRITERAM
#define TFT_RAMRD   0x5D
#define TFT_IDXRD   TFT_NOP
#define TFT_INVOFF  0xA4 // NORMALDISPLAY
#define TFT_INVON   0xA7 // INVERTDISPLAY
#define TFT_DISPOFF 0xA6 // DISPLAYALLOFF
#define TFT_DISPON  0xA5 // DISPLAYALLON
