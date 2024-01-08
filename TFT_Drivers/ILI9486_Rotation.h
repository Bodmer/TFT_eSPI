// This is the command sequence that rotates the ILI9486 driver coordinate frame

writecommand(TFT_MADCTL);
rotation = m % 8;
switch (rotation)
{
case 0: // Portrait
  writedata(TFT_MAD_BGR | TFT_MAD_MX | TFT_MAD_MY);
  _width = _init_width;
  _height = _init_height;
  break;
case 1: // Landscape (Portrait + 90cw)
  writedata(TFT_MAD_BGR | TFT_MAD_MV | TFT_MAD_MY);
  _width = _init_height;
  _height = _init_width;
  break;
case 2: // Portrait UP_DOWN (Portrait + 180cw)
  writedata(TFT_MAD_BGR);
  _width = _init_width;
  _height = _init_height;
  break;
case 3: // Landscape UP_DOWN (Portrait + 270cw)
  writedata(TFT_MAD_BGR | TFT_MAD_MV | TFT_MAD_MX);
  _width = _init_height;
  _height = _init_width;
  break;
case 4: // Portrait mirrored
  writedata(TFT_MAD_BGR | TFT_MAD_MY);
  _width = _init_width;
  _height = _init_height;
  break;
case 5: // Landscape mirrored (Portrait mirrored + 90cw)
  writedata(TFT_MAD_BGR | TFT_MAD_MV);
  _width = _init_height;
  _height = _init_width;
  break;
case 6: // Portrait mirrored UP_DOWN (Portrait mirrored + 180cw)
  writedata(TFT_MAD_BGR | TFT_MAD_MX);
  _width = _init_width;
  _height = _init_height;
  break;
case 7: // Landscape mirrored UP_DOWN (Portrait mirrored + 270cw)
  writedata(TFT_MAD_BGR | TFT_MAD_MV | TFT_MAD_MX | TFT_MAD_MY);
  _width = _init_height;
  _height = _init_width;
  break;
}
