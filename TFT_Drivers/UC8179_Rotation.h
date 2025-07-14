
// This is the command sequence that rotates the UC8179 driver coordinate frame

  rotation = m % 4;

  writecommand(EPD_PNLSET);
  switch (rotation) {
    case 0: // Portrait
      writedata(0x1F);  // 0b1111
      _width  = _init_width;
      _height = _init_height;
      break;
    case 1: // Landscape (Portrait + 90)
     writedata(0x1B); // 0b1011
      _width  = _init_height;
      _height = _init_width;
      break;
    case 2: // Inverter portrait
      writedata(0x13); // 0b0011
      _width  = _init_width;
      _height = _init_height;
      break;
    case 3: 
      writedata(0x17); // 0b0111
      _width  = _init_height;
      _height = _init_width;
      break;
  }
