// SSD1677_Init.h
// Command sequence to initialize the SSD1677 driver
{
#ifdef TFT_BUSY
    pinMode(TFT_BUSY, INPUT);
#endif

    digitalWrite(TFT_RST, LOW); // Module reset
    delay(10);
    digitalWrite(TFT_RST, HIGH);
    delay(10);

    CHECK_BUSY();
    writecommand(0x12); // SWRESET
    CHECK_BUSY();

    writecommand(0x18);
    writedata(0x80);

    writecommand(0x0C);
    writedata(0xAE);
    writedata(0xC7);
    writedata(0xC3);
    writedata(0xC0);
    writedata(0x80);

    writecommand(0x01); // Driver output control
    writedata((EPD_WIDTH - 1) % 256);
    writedata((EPD_WIDTH - 1) / 256);
    writedata(0x02);

    writecommand(0x3C); // BorderWaveform
    writedata(0x01);

    writecommand(0x11); // Data entry mode
    writedata(0x03);

    writecommand(0x44); // Set RAM-X address start/end position
    writedata(0x00);
    writedata(0x00);
    writedata((EPD_HEIGHT - 1) % 256);
    writedata((EPD_HEIGHT - 1) / 256);

    writecommand(0x45); // Set RAM-Y address start/end position
    writedata(0x00);
    writedata(0x00);
    writedata((EPD_WIDTH - 1) % 256);
    writedata((EPD_WIDTH - 1) / 256);

    writecommand(0x4E); // Set RAM x address counter
    writedata(0x00);
    writedata(0x00);

    writecommand(0x4F); // Set RAM y address counter
    writedata(0x00);
    writedata(0x00);

    CHECK_BUSY();
}