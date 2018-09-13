
// This is the command sequence that rotates the ILI9225 driver coordinate frame

  rotation = m % 4; // Limit the range of values to 0-3

//   writecommand(TFT_MADCTL);
  switch (rotation) {
    case 0:
      // writedata(TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_BGR);
      _width  = _init_width;
      _height = _init_height;
      break;
    case 1:
      _width  = _init_height;
      _height = _init_width;
      break;
    case 2:
      _width  = _init_width;
      _height = _init_height;
      break;
    case 3:
      _width  = _init_height;
      _height = _init_width;
      break;
  }
