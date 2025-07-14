// SSD1677_Rotation.h

rotation = m % 4;

switch (rotation) {
    case 0: // Portrait (default)
        writecommand(0x11);
        writedata(0x03);
        _width  = _init_width;
        _height = _init_height;
        break;
    case 1: // Landscape (90°)
        writecommand(0x11);
        writedata(0x01);  // Adjust based on datasheet
        _width  = _init_height;
        _height = _init_width;
        break;
    case 2: // Inverted Portrait (180°)
        writecommand(0x11);
        writedata(0x00);
        _width  = _init_width;
        _height = _init_height;
        break;
    case 3: // Inverted Landscape (270°)
        writecommand(0x11);
        writedata(0x02);  // Adjust based on datasheet
        _width  = _init_height;
        _height = _init_width;
        break;
}