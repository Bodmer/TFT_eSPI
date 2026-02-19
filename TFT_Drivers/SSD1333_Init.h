{
  writecommand(0xFD); // COMMANDLOCK
  writedata(0x12);
  writecommand(0xAE); // DISPLAYOFF
  writecommand(0xB3); // CLOCKDIV
  writedata(0xD1);
  writecommand(0xCA); // MUXRATIO
  writedata(0x7F);
  writecommand(0xA2); // DISPLAYOFFSET
  writedata(0x30);
  writecommand(0xA1); // STARTLINE
  writedata(0x00);
  writecommand(0xAD); // SETMASTER
  writedata(0x80);
  writecommand(0xC1); // CONTRASTABC
  writedata(0x55);
  writedata(0x5F);
  writedata(0x90);
  writecommand(0xC7); // CONTRASTMASTER MASTERCURRENT
  writedata(0x0F);
  writecommand(0xB9); // SETLINGRAYSCALE
  writecommand(0xB1); // PHASELEN
  writedata(0x22);
  writecommand(0xBB); // PRECHARGEVOLT
  writedata(0x17);
  writecommand(0xB6); // PRECHARGEPERIOD
  writedata(0x08);
  writecommand(0xBE); // VCOMH
  writedata(0x05);
  writecommand(0xA6); // DISPLAYALLOFF
  writecommand(0xAF); // DISPLAYON
}
