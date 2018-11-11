  // This is the command sequence that rotates the ST7789 driver coordinate frame

  writecommand(TFT_MADCTL);
  rotation = m % 4;
  switch (rotation) {
    case 0: // Portrait
#ifdef CGRAM_OFFSET
      writedata(TFT_MAD_BGR);
      colstart = 0;
      rowstart = 0;
#else
      writedata(TFT_MAD_RGB);
#endif
      _width  = _init_width;
      _height = _init_height;
      break;

    case 1: // Landscape (Portrait + 90)
#ifdef CGRAM_OFFSET
      writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
      colstart = 0;
      rowstart = 0;
#else
      writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_RGB);
#endif
      _width  = _init_height;
      _height = _init_width;
      break;

      case 2: // Inverter portrait
#ifdef CGRAM_OFFSET
       writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_BGR);
       colstart = 0;
       rowstart = 80;
#else
      writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_RGB);
#endif
      _width  = _init_width;
      _height = _init_height;
       break;
    case 3: // Inverted landscape
#ifdef CGRAM_OFFSET
      writedata(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_BGR);
      colstart = 80;
      rowstart = 0;
#else
      writedata(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_RGB);
#endif
      _width  = _init_height;
      _height = _init_width;
      break;
  }
