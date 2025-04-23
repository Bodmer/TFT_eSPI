// JD79686B_Init.h
{
#ifdef TFT_BUSY
    pinMode(TFT_BUSY, INPUT);
#endif
    digitalWrite(TFT_RST, LOW);
    delay(20);
    digitalWrite(TFT_RST, HIGH);
    delay(20);
    writecommand(0x4D);
    writedata(0x55);
    writecommand(0xA6);
    writedata(0x38);
    writecommand(0xB4);
    writedata(0x5D);
    writecommand(0xB6);
    writedata(0x80);
    writecommand(0xB7);
    writedata(0x00);
    writecommand(0xF7);
    writedata(0x02);
    writecommand(0X00); // PANNEL SETTING
    writedata(0xF7);
    writedata(0x0D);
}