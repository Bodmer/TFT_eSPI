
// This is the command sequence that rotates the SSD1333 driver coordinate frame

rotation = m % 4; // Limit the range of values to 0-3

uint8_t madctl = 0x64;
uint8_t startline = 0x00;
uint8_t dispoffset = 0x00;

switch (rotation) {
	case 0:
		madctl |= 0x10;
		_width = _init_width;
		_height = _init_height;
		startline = 0x00;
		dispoffset = 0x30;
		#ifdef CGRAM_OFFSET
		colstart = 8;
		rowstart = 0;
		#endif
		break;
	case 1:
		madctl |= 0x13;
		_width = _init_height;
		_height = _init_width;
		startline = 0x00;
		dispoffset = 0x30;
		#ifdef CGRAM_OFFSET
		colstart = 0;
		rowstart = 8;
		#endif
		break;
	case 2:
		madctl |= 0x02;
		_width = _init_width;
		_height = _init_height;
		startline = 0x30;
		dispoffset = 0x80;
		#ifdef CGRAM_OFFSET
		colstart = 8;
		rowstart = 48;
		#endif
		break;
	case 3:
		madctl |= 0x01;
		_width = _init_height;
		_height = _init_width;
		startline = 0x00;
		dispoffset = 0x80;
		#ifdef CGRAM_OFFSET
		colstart = 0;
		rowstart = 8;
		#endif
		break;
}

writecommand(0xA0); // SETREMAP
writedata(madctl);
writecommand(0xA1); // STARTLINE
writedata(startline);
writecommand(0xA2); // DISPLAYOFFSET
writedata(dispoffset);
