// SSD1681 screen rotation sequence

rotation = m % 4;

writecommand(0x11); // Data entry mode
switch (rotation)
{
case 0:            // Portrait mode (0 degrees)
  writedata(0x01); // Y decrement, X increment
  _width = EPD_WIDTH;
  _height = EPD_HEIGHT;
  break;
case 2:            // Inverted portrait mode (180 degrees)
  writedata(0x02); // Y increment, X decrement
  _width = EPD_WIDTH;
  _height = EPD_HEIGHT;
  break;
case 1: // Landscape mode (90 degrees)
case 3: // Inverted landscape mode (270 degrees)
  // SSD1681 does not directly support 90° and 270° rotation, not implemented
  writedata(0x01); // Default to 0 degrees
  _width = EPD_WIDTH;
  _height = EPD_HEIGHT;
  break;
}