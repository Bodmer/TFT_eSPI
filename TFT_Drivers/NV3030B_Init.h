
// This is the command sequence that initialises the NV3030B driver

// Configure NV3030B display
{
static const uint8_t PROGMEM
  nv3030b[] = {
	  32,
	  0xFD, 2, 0x06, 0x08,                                     // 1: Enter read/write private register, 2 args
	  0x61, 2, 0x07, 0x04,                                     // 2: dvdd setting, 2 args	  
	  0x62, 4, 0x00, 0x44, 0x40, 0x00,                         // 3: bias setting, 4 args
	  0x63, 4, 0x41, 0x07, 0x12, 0x12,                         // 4: vgl setting, 4 args
	  0x64, 1, 0x37,                                           // 5: vgh setting, 1 arg
	  0x65, 3, 0x09, 0x10, 0x21,                               // 6: VSP, 3 args
	  0x66, 3, 0x09, 0x10, 0x21,                               // 7: VSN, 3 args
	  0x67, 2, 0x20, 0x40,                                     // 8: pump clock set, 2 args
	  0x68, 4, 0x90, 0x4C, 0x7C, 0x06,                         // 9: gamma ref 1, 4 args
	  0xB1, 3, 0x0F, 0x02, 0x01,                               // 10: frame rate, 3 args
	  0xB4, 1, 0x01,                                           // 11: display pol control, 1 arg
	  0xB5, 4, 0x02, 0x02, 0x0A, 0x14,                         // 12: blanking porch, 4 args
	  0xB6, 5, 0x04, 0x01, 0x9F, 0x00, 0x02,                   // 13: display function, 5 args
	  0xDF, 1, 0x11,                                           // 14: gofc_gamma_en_sel=1, 1 arg
	  0xE2, 6, 0x03, 0x00, 0x00, 0x26, 0x27, 0x3F,             // 15: gamma positive 3, 6 args
	  0xE5, 6, 0x3F, 0x27, 0x26, 0x00, 0x00, 0x03,             // 16: gamma negative 3, 6 args
	  0xE1, 2, 0x00, 0x57,                                     // 17: gamma positive 2, 2 args
	  0xE4, 2, 0x58, 0x00,                                     // 18: gamma negative 2, 2 args
	  0xE0, 8, 0x01, 0x03, 0x0D, 0x0E, 0x0E, 0x0C, 0x15, 0x19, // 19: gamma positive 1, 8 args
	  0xE3, 8, 0x1A, 0x16, 0x0C, 0x0F, 0x0E, 0x0D, 0x02, 0x01, // 20: gamma negative 1, 8 args
	  0xE6, 2, 0x00, 0xFF,                                     // 21: SRC_CTRL1, 2 args
	  0xE7, 6, 0x01, 0x04, 0x03, 0x03, 0x00, 0x12,             // 22: SRC_CTRL2, 6 args
	  0xE8, 3, 0x00, 0x70, 0x00,                               // 23: SRC_CTRL3, 3 args
	  0xEC, 1, 0x52,                                           // 24: Gate driver timing, 1 arg
	  0xF1, 3, 0x01, 0x01, 0x02,                               // 25: tearing effect, 3 args
	  0xF6, 4, 0x09, 0x10, 0x00, 0x00,                         // 26: interface control, 4 args
	  0xFD, 2, 0xFA, 0xFC,                                     // 27: exit read/write private register, 2 args
	  NV3030B_PIXFMT, 1, 0x55,                                 // 28: set 16bit/pixel format
	  NV3030B_MADCTL, 1, TFT_MAD_BGR,                          // 29: set BGR order
	  NV3030B_INVON, 0,                                        // 30: Inversion on, no args, no delay
	  NV3030B_SLPOUT, TFT_INIT_DELAY, 120,                     // 31: Exit sleep, no args, delay 120ms
	  NV3030B_DISPON, TFT_INIT_DELAY, 10                       // 32: Display on, no args, delay 10ms
    };

  commandList(nv3030b);

  #ifdef TFT_BL
  // Turn on the back-light LED
  digitalWrite(TFT_BL, HIGH);
  pinMode(TFT_BL, OUTPUT);
  #endif
}
// End of NV3030B display configuration