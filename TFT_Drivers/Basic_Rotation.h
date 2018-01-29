// This is the command sequence that rotates the ST7735/ILI9163/S6D02A1 driver coordinate frame

  rotation = m % 4; // Limit the range of values to 0-3

  writecommand(TFT_MADCTL);
  switch (rotation) {
    case 0:
       writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_COLOR_ORDER);
      _width  = _width_orig;
      _height = _height_orig;
#ifdef CGRAM_OFFSET
      colstart = R0_OFFSET_X;
      rowstart = R0_OFFSET_Y;
#endif
      break;
    case 1:
       writedata(TFT_MAD_MY | TFT_MAD_MV | TFT_COLOR_ORDER);
      _width  = _height_orig;
      _height = _width_orig;
#ifdef CGRAM_OFFSET
      colstart = R1_OFFSET_X;
      rowstart = R1_OFFSET_Y;
#endif
      break;
    case 2:
       writedata(TFT_COLOR_ORDER);
      _width  = _width_orig;
      _height = _height_orig;
#ifdef CGRAM_OFFSET
      colstart = R2_OFFSET_X;
      rowstart = R2_OFFSET_Y;
#endif
      break;
    case 3:
       writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_COLOR_ORDER);
      _width  = _height_orig;
      _height = _width_orig;
#ifdef CGRAM_OFFSET
      colstart = R3_OFFSET_X;
      rowstart = R3_OFFSET_Y;
#endif
      break;
  }
