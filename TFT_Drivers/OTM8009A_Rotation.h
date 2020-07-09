  // This is the command sequence that rotates the OTM8009A driver coordinate frame

  rotation = m % 4;
  writecommand(TFT_MADCTL);
  writecommand(TFT_NOP);
  switch (rotation) {
   case 0: // Portrait
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      writedata(0x00);
      writedata(0x00);
     break;
   case 1: // Landscape (Portrait + 90)
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      writedata((1<<5)|(1<<6));
      writedata(0x00);
     break;
   case 2: // Inverter portrait
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;
      writedata((1<<7)|(1<<6));
      writedata(0x00);
     break;
   case 3: // Inverted landscape
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
      
      writedata((1<<7)|(1<<5));
      writedata(0x00);
     break;
  }