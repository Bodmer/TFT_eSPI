  // This is the command sequence that rotates the OTM8009A driver coordinate frame
  rotation = m % 4;
  writecommand16(TFT_MADCTL);

  switch (rotation) {
   case 0: // Portrait
      writedata16(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_RGB);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;      
     break;
   case 1: // Landscape (Portrait + 90)
      writedata16(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_RGB);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;      
     break;
   case 2: // Inverter portrait
      writedata16(TFT_MAD_RGB);
      _width  = TFT_WIDTH;
      _height = TFT_HEIGHT;      
     break;
   case 3: // Inverted landscape
      writedata16(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_RGB);
      _width  = TFT_HEIGHT;
      _height = TFT_WIDTH;
     break;
  }