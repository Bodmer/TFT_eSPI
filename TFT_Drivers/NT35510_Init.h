
// This is the command sequence that initialises the NT35510 driver
// Configure NT35510 display

#define Byte8H(ByteH) ((uint8_t)(((uint16_t)(ByteH)&0xFF00)>>8))
#define Byte8L(ByteL) ((uint8_t)( (uint16_t)(ByteL)&0x00FF))

#ifndef TFT_CASET_CMD
	#define TFT_CASET_CMD(x0, x1) \
		DC_C; tft_Write_8(TFT_CASET); \
		DC_D; tft_Write_32C(x0, x1)
#endif

#ifndef TFT_PASET_CMD
	#define TFT_PASET_CMD(y0, y1) \
		DC_C; tft_Write_8(TFT_PASET); \
		DC_D; tft_Write_32C(y0, y1)
#endif

#define writecommand16(cmd) \
	writecommand(Byte8H(cmd)); writecommand(Byte8L(cmd))

#define writedata16(data) \
	writedata(Byte8H(data)); writedata(Byte8L(data))


// End of NT35510 display configuration