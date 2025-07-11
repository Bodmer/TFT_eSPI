// SSD1683_Rotation.h

rotation = m % 4;

writecommand(0x11);  // Data entry mode
switch (rotation) {
    case 0: // Portrait (default)
        writedata(0x01);
        _width  = _init_width;
        _height = _init_height;
        break;
    case 1: // Landscape (90°)
        writedata(0x03);  // Adjust based on datasheet
        _width  = _init_height;
        _height = _init_width;
        break;
    case 2: // Inverted Portrait (180°)
        writedata(0x00);  // Adjust based on datasheet
        _width  = _init_width;
        _height = _init_height;
        break;
    case 3: // Inverted Landscape (270°)
        writedata(0x02);  // Adjust based on datasheet
        _width  = _init_height;
        _height = _init_width;
        break;
}