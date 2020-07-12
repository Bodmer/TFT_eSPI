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

/* Manufacturer Command Set */
#define MCS_ADRSFT	0x0000	/* Address Shift Function */
#define MCS_PANSET	0xB3A6	/* Panel Type Setting */
#define MCS_SD_CTRL	0xC0A2	/* Source Driver Timing Setting */
#define MCS_P_DRV_M	0xC0B4	/* Panel Driving Mode */
#define MCS_OSC_ADJ	0xC181	/* Oscillator Adjustment for Idle/Normal mode */
#define MCS_RGB_VID_SET	0xC1A1	/* RGB Video Mode Setting */
#define MCS_SD_PCH_CTRL	0xC480	/* Source Driver Precharge Control */
#define MCS_NO_DOC1	0xC48A	/* Command not documented */
#define MCS_PWR_CTRL1	0xC580	/* Power Control Setting 1 */
#define MCS_PWR_CTRL2	0xC590	/* Power Control Setting 2 for Normal Mode */
#define MCS_PWR_CTRL4	0xC5B0	/* Power Control Setting 4 for DC Voltage */
#define MCS_PANCTRLSET1	0xCB80	/* Panel Control Setting 1 */
#define MCS_PANCTRLSET2	0xCB90	/* Panel Control Setting 2 */
#define MCS_PANCTRLSET3	0xCBA0	/* Panel Control Setting 3 */
#define MCS_PANCTRLSET4	0xCBB0	/* Panel Control Setting 4 */
#define MCS_PANCTRLSET5	0xCBC0	/* Panel Control Setting 5 */
#define MCS_PANCTRLSET6	0xCBD0	/* Panel Control Setting 6 */
#define MCS_PANCTRLSET7	0xCBE0	/* Panel Control Setting 7 */
#define MCS_PANCTRLSET8	0xCBF0	/* Panel Control Setting 8 */
#define MCS_PANU2D1	0xCC80	/* Panel U2D Setting 1 */
#define MCS_PANU2D2	0xCC90	/* Panel U2D Setting 2 */
#define MCS_PANU2D3	0xCCA0	/* Panel U2D Setting 3 */
#define MCS_PAND2U1	0xCCB0	/* Panel D2U Setting 1 */
#define MCS_PAND2U2	0xCCC0	/* Panel D2U Setting 2 */
#define MCS_PAND2U3	0xCCD0	/* Panel D2U Setting 3 */
#define MCS_GOAVST	0xCE80	/* GOA VST Setting */
#define MCS_GOACLKA1	0xCEA0	/* GOA CLKA1 Setting */
#define MCS_GOACLKA2	0xCEA7	/* GOA CLKA2 Setting */
#define MCS_GOACLKA3	0xCEB0	/* GOA CLKA3 Setting */
#define MCS_GOACLKA4	0xCEB7	/* GOA CLKA4 Setting */
#define MCS_GOAECLK		0xCFC0	/* GOA ECLK Setting */
#define MCS_GOAOPT1		0xCFC6	/* GOA Other Options 1 */
#define MCS_GOATGOPT	0xCFC7	/* GOA Signal Toggle Option Setting */
#define MCS_NO_DOC2	0xCFD0	/* Command not documented */
#define MCS_GVDDSET	0xD800	/* GVDD/NGVDD */
#define MCS_VCOMDC	0xD900	/* VCOM Voltage Setting */
#define MCS_GMCT2_2P	0xE100	/* Gamma Correction 2.2+ Setting */
#define MCS_GMCT2_2N	0xE200	/* Gamma Correction 2.2- Setting */
#define MCS_NO_DOC3	0xF5B6	/* Command not documented */
#define MCS_CMD2_ENA1	0xFF00	/* Enable Access Command2 "CMD2" */
#define MCS_CMD2_ENA2	0xFF80	/* Enable Access Orise Command2 */

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