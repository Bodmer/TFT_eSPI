// Change the width and height if required (defined in portrait mode)
// or use the constructor to over-ride defaults
#define TFT_WIDTH  480
#define TFT_HEIGHT 800

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80 // Not used unless commandlist invoked

// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0x0000
#define TFT_SWRST   0x0100

#define TFT_SLPIN   0x1000
#define TFT_SLPOUT  0x1100

#define TFT_INVOFF  0x2000
#define TFT_INVON   0x2100

#define TFT_DISPOFF 0x2800
#define TFT_DISPON  0x2900

#define TFT_CASET   0x2A00
#define TFT_PASET   0x2B00

#define TFT_RAMWR   0x2C00
#define TFT_RAMRD   0x2E00

#define TFT_MADCTL  0x3600

#define TFT_MAD_MY  0x80 /* Row Address Order */
#define TFT_MAD_MX  0x40 /* Column Address Order */
#define TFT_MAD_MV  0x20 /* Row / Column Exchange */
#define TFT_MAD_ML  0x10 /* Vertical Refresh Order - Bottom to Top */

#define TFT_MAD_RGB 0x00 /* RGB color filter panel */
#define TFT_MAD_BGR 0x08 /* BGR color filter panel */

#define Byte8H(ByteH) ((uint8_t)(((uint16_t)(ByteH)&0xFF00)>>8))
#define Byte8L(ByteL) ((uint8_t)( (uint16_t)(ByteL)&0x00FF))

#define TFT_CASET_CMD(x0, x1) \
  DC_C; tft_Write_16(TFT_CASET); \
  DC_D; tft_Write_16(Byte8H(x0)); \
  DC_C; tft_Write_16(TFT_CASET + 1); \
  DC_D; tft_Write_16(Byte8L(x0)); \
  DC_C; tft_Write_16(TFT_CASET + 2); \
  DC_D; tft_Write_16(Byte8H(x1)); \
  DC_C; tft_Write_16(TFT_CASET + 3); \
  DC_D; tft_Write_16(Byte8L(x1))

#define TFT_PASET_CMD(y0, y1) \
  DC_C; tft_Write_16(TFT_PASET); \
  DC_D; tft_Write_16(Byte8H(y0)); \
  DC_C; tft_Write_16(TFT_PASET + 1); \
  DC_D; tft_Write_16(Byte8L(y0)); \
  DC_C; tft_Write_16(TFT_PASET + 2); \
  DC_D; tft_Write_16(Byte8H(y1)); \
  DC_C; tft_Write_16(TFT_PASET + 3); \
  DC_D; tft_Write_16(Byte8L(y1))

#define TFT_RAMWR_CMD \
  DC_C; tft_Write_16(TFT_RAMWR)

#define TFT_RAMRD_CMD \
  DC_C; tft_Write_16(TFT_RAMRD)

#define writecommand16(cmd) \
	writecommand(Byte8H(cmd)); writecommand(Byte8L(cmd))

#define writedata16(data) \
	writedata(Byte8H(data)); writedata(Byte8L(data))