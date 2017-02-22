
// This is the command sequence that rotates the ILI9341 driver coordinate frame

  rotation = m % 8; // Limit the range of values to 0-7

  writecommand(TFT_MADCTL);
  switch (rotation) {
    case 0:
      writedata(TFT_MAD_MX | TFT_MAD_BGR);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 1:
      writedata(TFT_MAD_MV | TFT_MAD_BGR);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;
    case 2:
      writedata(TFT_MAD_MY | TFT_MAD_BGR);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 3:
      writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_BGR);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;
  // These next rotations are for bottum up BMP drawing
    case 4:
      writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_BGR);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 5:
      writedata(TFT_MAD_MV | TFT_MAD_MX | TFT_MAD_BGR);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;
    case 6:
      writedata(TFT_MAD_BGR);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 7:
      writedata(TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_BGR);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;

  }