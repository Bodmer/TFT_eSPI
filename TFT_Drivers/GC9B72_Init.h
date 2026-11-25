
// This is the command sequence that initialises the GC9B72 driver

{
  writecommand(0xFE);
  writecommand(0xEF);

  writecommand(0x80);
  writedata(0x19);

  writecommand(0x82);
  writedata(0x09);

  writecommand(0x83);
  writedata(0x03);

  writecommand(0x88);
  writedata(0x00);

  writecommand(0x89);
  writedata(0x38);

  writecommand(0x8A);
  writedata(0x40);

  writecommand(0x8B);
  writedata(0x0A);

  writecommand(0x8C);
  writedata(0x00);

  writecommand(0x81);
  writedata(0xFF);

  writecommand(0x84);
  writedata(0xFF);

  writecommand(0x85);
  writedata(0xFF);

  writecommand(0x86);
  writedata(0xFF);

  writecommand(0x87);
  writedata(0xFF);

  writecommand(0x8E);
  writedata(0xFF);

  writecommand(0x8F);
  writedata(0xFF);

  writecommand(0x98);
  writedata(0x3E);

  writecommand(0x99);
  writedata(0x3E);

  writecommand(0x7D);
  writedata(0x72);

  writecommand(0x70);
  writedata(0x02);
  writedata(0x03);
  writedata(0x03);
  writedata(0x06);
  writedata(0x03);
  writedata(0x03);
  writedata(0x09);
  writedata(0x07);
  writedata(0x09);
  writedata(0x03);

  writecommand(0x90);
  writedata(0x06);
  writedata(0x06);
  writedata(0x01);
  writedata(0x01);

  writecommand(0x93);
  writedata(0x02);
  writedata(0xFF);
  writedata(0x00);

  writecommand(0xCB);
  writedata(0x02);

  writecommand(0xFB);
  writedata(0x00);
  writedata(0x00);

  writecommand(0xF6);
  writedata(0xC0);

  writecommand(0x6C);
  writedata(0x00);
  writedata(0x00);
  writedata(0x22);
  writedata(0x00);
  writedata(0xCC);
  writedata(0x04);
  writedata(0x58);

  writecommand(0xAA);
  writedata(0x0B);
  writedata(0x00);

  writecommand(0xEC);
  writedata(0x07);

  writecommand(0xF9);
  writedata(0x40);

  writecommand(0xEB);
  writedata(0x01);
  writedata(0x67);

  writecommand(0x74);
  writedata(0x01);
  writedata(0x60);
  writedata(0x00);
  writedata(0x00);
  writedata(0x00);
  writedata(0x00);

  writecommand(0xB5);
  writedata(0x14);
  writedata(0x14);
  writedata(0x14);

  writecommand(0x6E);
  writedata(0x0B);
  writedata(0x0B);
  writedata(0x09);
  writedata(0x09);
  writedata(0x13);
  writedata(0x13);
  writedata(0x11);
  writedata(0x11);
  writedata(0x16);
  writedata(0x15);
  writedata(0x01);
  writedata(0x04);
  writedata(0x00);
  writedata(0x0D);
  writedata(0x1D);
  writedata(0x00);
  writedata(0x00);
  writedata(0x1D);
  writedata(0x0D);
  writedata(0x00);
  writedata(0x04);
  writedata(0x08);
  writedata(0x15);
  writedata(0x16);
  writedata(0x12);
  writedata(0x12);
  writedata(0x14);
  writedata(0x14);
  writedata(0x0A);
  writedata(0x0A);
  writedata(0x0C);
  writedata(0x0C);

  writecommand(0x60);
  writedata(0x38);
  writedata(0x1C);
  writedata(0x13);
  writedata(0x56);

  writecommand(0x61);
  writedata(0xF8);
  writedata(0x0A);
  writedata(0x13);
  writedata(0x56);

  writecommand(0x62);
  writedata(0xF8);
  writedata(0x0B);
  writedata(0x13);
  writedata(0x56);

  writecommand(0x63);
  writedata(0x38);
  writedata(0x1C);
  writedata(0x13);
  writedata(0x56);

  writecommand(0x64);
  writedata(0x38);
  writedata(0x20);
  writedata(0x72);
  writedata(0xF8);
  writedata(0x13);
  writedata(0x56);

  writecommand(0x65);
  writedata(0x78);
  writedata(0x1A);
  writedata(0x70);
  writedata(0x0B);
  writedata(0x56);
  writedata(0x13);

  writecommand(0x66);
  writedata(0x38);
  writedata(0x24);
  writedata(0x72);
  writedata(0xFC);
  writedata(0x13);
  writedata(0x56);

  writecommand(0x68);
  writedata(0xB3);
  writedata(0x08);
  writedata(0x0E);
  writedata(0x08);
  writedata(0x0E);
  writedata(0x0A);
  writedata(0x0A);

  writecommand(0x69);
  writedata(0xB3);
  writedata(0x08);
  writedata(0x0E);
  writedata(0x08);
  writedata(0x0E);
  writedata(0x0A);
  writedata(0x0A);

  writecommand(0x6A);
  writedata(0x00);
  writedata(0x00);

  writecommand(0x3A); //COLMOD: Pixel Format Set
  writedata(0x05);

  writecommand(0x36); //Memory Access Control
  writedata(0x00);

  writecommand(0x7C);
  writedata(0xB6);
  writedata(0x29);

  writecommand(0xAC);
  writedata(0x40);

  writecommand(0xC3);
  writedata(0x1A);

  writecommand(0xC4);
  writedata(0x24);

  writecommand(0xC9);
  writedata(0x2F);

  writecommand(0xF0);
  writedata(0x11);
  writedata(0x17);
  writedata(0x08);
  writedata(0x06);
  writedata(0x05);
  writedata(0x38);

  writecommand(0xF1);
  writedata(0x4D);
  writedata(0x72);
  writedata(0x72);
  writedata(0x2D);
  writedata(0x34);
  writedata(0x8F);

  writecommand(0xF2);
  writedata(0x11);
  writedata(0x17);
  writedata(0x08);
  writedata(0x06);
  writedata(0x05);
  writedata(0x38);

  writecommand(0xF3);
  writedata(0x4D);
  writedata(0x72);
  writedata(0x72);
  writedata(0x2D);
  writedata(0x34);
  writedata(0x8F);

  writecommand(0xB4);
  writedata(0x0A);

  writecommand(0x35); //Tearing Effect Line ON
  writedata(0x00);

  writecommand(0xFE);

  writecommand(0xEE);

  writecommand(0x11); //Sleep Out Mode
  delay(120);

  writecommand(0x29); //Display ON
  delay(20);
}
