// Change the width and height if required (defined in portrait mode)
// or use the constructor to over-ride defaults
#define TFT_WIDTH  128
#define TFT_HEIGHT 160


// Color definitions for backwards compatibility with old sketches
// use colour definitions like TFT_BLACK to make sketches more portable
#define S6D02A1_BLACK       0x0000      /*   0,   0,   0 */
#define S6D02A1_NAVY        0x000F      /*   0,   0, 128 */
#define S6D02A1_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define S6D02A1_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define S6D02A1_MAROON      0x7800      /* 128,   0,   0 */
#define S6D02A1_PURPLE      0x780F      /* 128,   0, 128 */
#define S6D02A1_OLIVE       0x7BE0      /* 128, 128,   0 */
#define S6D02A1_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define S6D02A1_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define S6D02A1_BLUE        0x001F      /*   0,   0, 255 */
#define S6D02A1_GREEN       0x07E0      /*   0, 255,   0 */
#define S6D02A1_CYAN        0x07FF      /*   0, 255, 255 */
#define S6D02A1_RED         0xF800      /* 255,   0,   0 */
#define S6D02A1_MAGENTA     0xF81F      /* 255,   0, 255 */
#define S6D02A1_YELLOW      0xFFE0      /* 255, 255,   0 */
#define S6D02A1_WHITE       0xFFFF      /* 255, 255, 255 */
#define S6D02A1_ORANGE      0xFD20      /* 255, 165,   0 */
#define S6D02A1_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define S6D02A1_PINK        0xF81F


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

// Defines pixel color configuration: RGB/BGR
#define TFT_COLOR_ORDER TFT_MAD_BGR
