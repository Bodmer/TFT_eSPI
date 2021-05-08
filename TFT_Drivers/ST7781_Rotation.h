
// This is the command sequence that rotates the ST7781 driver coordinate frame
// Based on ILI9225_Rotation.h and MCUFRIEND_kbv = MCUFRIEND_kbv::setRotation()

  rotation = m % 4; // Limit the range of values to 0-3

  switch (rotation) {
    case 0:
      writecommand(ST7781_GATE_SCAN_CTRL1);
      writedata(0xA7);writedata(0x00);
      writecommand(ST7781_DRIVER_OUTPUT_CTRL);
      writedata(0x01);writedata(0x00);
      writecommand(ST7781_ENTRY_MODE);
      writedata(TFT_MAD_COLOR_ORDER);writedata(0x30);
      _width  = _init_width;
      _height = _init_height;
      break;
    case 1:
      writecommand(ST7781_GATE_SCAN_CTRL1);
      writedata(0xA7);writedata(0x00);
      writecommand(ST7781_DRIVER_OUTPUT_CTRL);
      writedata(0x00);writedata(0x00);
      writecommand(ST7781_ENTRY_MODE);
      writedata(TFT_MAD_COLOR_ORDER);writedata(0x38);
      _width  = _init_height;
      _height = _init_width;
      break;
    case 2:
      writecommand(ST7781_GATE_SCAN_CTRL1);
      writedata(0x27);writedata(0x00);
      writecommand(ST7781_DRIVER_OUTPUT_CTRL);
      writedata(0x00);writedata(0x00);
      writecommand(ST7781_ENTRY_MODE);
      writedata(TFT_MAD_COLOR_ORDER);writedata(0x30);
      _width  = _init_width;
      _height = _init_height;
      break;
    case 3:
      writecommand(ST7781_GATE_SCAN_CTRL1);
      writedata(0x27);writedata(0x00);
      writecommand(ST7781_DRIVER_OUTPUT_CTRL);
      writedata(0x01);writedata(0x00);
      writecommand(ST7781_ENTRY_MODE);
      writedata(TFT_MAD_COLOR_ORDER);writedata(0x38);
      _width  = _init_height;
      _height = _init_width;
    break;
  }

  // Finish Window
  setWindow(0, 0, _width - 1, _height - 1);
