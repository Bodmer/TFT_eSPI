
// This is the command sequence that rotates the S6D02A1 driver coordinate frame

  rotation = m % 4;

  writecommand(TFT_MADCTL);
  switch (rotation) {
    case 0:
      writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_BGR);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 1:
      writedata(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_BGR);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;
    case 2:
      writedata(TFT_MAD_BGR);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 3:
      writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;
  }
