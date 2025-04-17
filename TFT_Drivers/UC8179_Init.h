
// This is the command sequence that initialises the  UC8179 driver

{

#ifdef TFT_BUSY
    pinMode(TFT_BUSY, INPUT);
#endif

    writecommand(0x01); // POWER SETTING
    writedata(0x07);
    writedata(0x07); // VGH=20V,VGL=-20V
    writedata(0x3f); // VDH=15V
    writedata(0x3f); // VDL=-15V

    // Enhanced display drive(Add 0x06 command)
    writecommand(0x06); // Booster Soft Start
    writedata(0x17);
    writedata(0x17);
    writedata(0x28);
    writedata(0x17);

    writecommand(0x04); // POWER ON
    delay(100);

    CHECK_BUSY();

    writecommand(0X00); // PANNEL SETTING
    writedata(0x1F);    // KW-3f   KWR-2F BWROTP 0f BWOTP 1f

    writecommand(0x61); // tres
    writedata(EPD_WIDTH >> 8);
    writedata(EPD_WIDTH & 0xFF);
    writedata(EPD_HEIGHT >> 8);
    writedata(EPD_HEIGHT & 0xFF);

    writecommand(0X15);
    writedata(0x00);

    writecommand(0X50); // VCOM AND DATA INTERVAL SETTING
    writedata(0x10);
    writedata(0x07);

    writecommand(0X60); // TCON SETTING
    writedata(0x22);
}