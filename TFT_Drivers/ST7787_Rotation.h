
// This is the command sequence that rotates the ST7787 driver coordinate frame
//
// Rotation 0 is portrait mode. Rotations 1, 2, and 3 are rotated clockwise
// by 90, 180, and 270 degrees.
//
// Vertical refresh direction is set to be the same as data write order.
// So coordinate 0 is refresh first, and coordinate 319 is refreshed last.
// For some reason, counter to datasheet information, MADCTL must be set
// with the ML bit as the inverse of the MY bit to achieve this.

  rotation = m % 4; // Limit the range of values to 0-3

  writecommand(TFT_MADCTL);
  switch (rotation) {
    case 0:
      writedata(TFT_MAD_ML | TFT_MAD_RGB);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 1:
      writedata(TFT_MAD_ML | TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_RGB);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;
    case 2:
      writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_RGB);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      break;
    case 3:
      writedata(TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_RGB);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      break;
  }
