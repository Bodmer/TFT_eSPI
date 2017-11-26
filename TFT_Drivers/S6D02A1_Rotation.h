
// This is the command sequence that rotates the S6D02A1 driver coordinate frame

  rotation = m % 4;

  writecommand(TFT_MADCTL);
  switch (rotation) {
    case 0:
      writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_BGR);
      _width  = _width_orig;
      _height = _height_orig;
      break;
    case 1:
      writedata(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_BGR);
      _width  = _height_orig;
      _height = _width_orig;
      break;
    case 2:
      writedata(TFT_MAD_BGR);
      _width  = _width_orig;
      _height = _height_orig;
      break;
    case 3:
      writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
      _width  = _height_orig;
      _height = _width_orig;
      break;
  }
