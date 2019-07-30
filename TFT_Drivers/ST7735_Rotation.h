
// This is the command sequence that rotates the ST7735 driver coordinate frame

  rotation = m % 4; // Limit the range of values to 0-3

  writecommand(TFT_MADCTL);
  switch (rotation) {
    case 0:
     if (tabcolor == INITR_BLACKTAB) {
       writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_RGB);
     } else if(tabcolor == INITR_GREENTAB2) {
       writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_RGB);
       colstart = 2;
       rowstart = 1;
     } else if(tabcolor == INITR_GREENTAB3) {
       writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_BGR);
       colstart = 2;
       rowstart = 3;
     } else if(tabcolor == INITR_GREENTAB128) {
       writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_MH | TFT_MAD_BGR);
       colstart = 0;
       rowstart = 32;
     } else if(tabcolor == INITR_GREENTAB160x80) {
       writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_MH | TFT_MAD_BGR);
       colstart = 26;
       rowstart = 1;
     } else if(tabcolor == INITR_REDTAB160x80) {
       writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_MH | TFT_MAD_BGR);
       colstart = 24;
       rowstart = 0;
     } else if(tabcolor == INITB) {
       writedata(TFT_MAD_MX | TFT_MAD_RGB);
     } else {
       writedata(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_BGR);
     }
      _width  = _init_width;
      _height = _init_height;
      break;
    case 1:
     if (tabcolor == INITR_BLACKTAB) {
       writedata(TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_RGB);
     } else if(tabcolor == INITR_GREENTAB2) {
       writedata(TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_RGB);
       colstart = 1;
       rowstart = 2;
     } else if(tabcolor == INITR_GREENTAB3) {
       writedata(TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_BGR);
       colstart = 3;
       rowstart = 2;
     } else if(tabcolor == INITR_GREENTAB128) {
       writedata(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_BGR);
       colstart = 32;
       rowstart = 0;
     } else if(tabcolor == INITR_GREENTAB160x80) {
       writedata(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_BGR);
       colstart = 1;
       rowstart = 26;
     } else if(tabcolor == INITR_REDTAB160x80) {
       writedata(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_BGR);
       colstart = 0;
       rowstart = 24;
     } else if(tabcolor == INITB) {
       writedata(TFT_MAD_MV | TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_RGB);
     } else {
       writedata(TFT_MAD_MY | TFT_MAD_MV | TFT_MAD_BGR);
     }
      _width  = _init_height;
      _height = _init_width;
      break;
    case 2:
     if (tabcolor == INITR_BLACKTAB) {
       writedata(TFT_MAD_RGB);
     } else if(tabcolor == INITR_GREENTAB2) {
       writedata(TFT_MAD_RGB);
       colstart = 2;
       rowstart = 1;
     } else if(tabcolor == INITR_GREENTAB3) {
       writedata(TFT_MAD_BGR);
       colstart = 2;
       rowstart = 1;
     } else if(tabcolor == INITR_GREENTAB128) {
       writedata(TFT_MAD_BGR);
       colstart = 0;
       rowstart = 0;
     } else if(tabcolor == INITR_GREENTAB160x80) {
       writedata(TFT_MAD_BGR);
       colstart = 26;
       rowstart = 1;
     } else if(tabcolor == INITR_REDTAB160x80) {
       writedata(TFT_MAD_BGR);
       colstart = 24;
       rowstart = 0;
     } else if(tabcolor == INITB) {
       writedata(TFT_MAD_MY | TFT_MAD_RGB);
     } else {
       writedata(TFT_MAD_BGR);
     }
      _width  = _init_width;
      _height = _init_height;
      break;
    case 3:
     if (tabcolor == INITR_BLACKTAB) {
       writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_RGB);
     } else if(tabcolor == INITR_GREENTAB2) {
       writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_RGB);
       colstart = 1;
       rowstart = 2;
     } else if(tabcolor == INITR_GREENTAB3) {
       writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
       colstart = 1;
       rowstart = 2;
     } else if(tabcolor == INITR_GREENTAB128) {
       writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
       colstart = 0;
       rowstart = 0;
     } else if(tabcolor == INITR_GREENTAB160x80) {
       writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
       colstart = 1;
       rowstart = 26;
     } else if(tabcolor == INITR_REDTAB160x80) {
       writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
       colstart = 0;
       rowstart = 24;
     } else if(tabcolor == INITB) {
       writedata(TFT_MAD_MV | TFT_MAD_RGB);
     } else {
       writedata(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_BGR);
     }
      _width  = _init_height;
      _height = _init_width;
      break;

  // These next rotations are for bottum up BMP drawing
  /*  case 4:
      writedata(ST7735_TFT_MAD_MX | ST7735_TFT_MAD_MY | ST7735_TFT_MAD_BGR);
      _width  = _init_width;
      _height = _init_height;
      break;
    case 5:
      writedata(ST7735_TFT_MAD_MV | ST7735_TFT_MAD_MX | ST7735_TFT_MAD_BGR);
      _width  = _init_height;
      _height = _init_width;
      break;
    case 6:
      writedata(ST7735_TFT_MAD_BGR);
      _width  = _init_width;
      _height = _init_height;
      break;
    case 7:
      writedata(ST7735_TFT_MAD_MY | ST7735_TFT_MAD_MV | ST7735_TFT_MAD_BGR);
      _width  = _init_height;
      _height = _init_width;
      break;
  */
  }
