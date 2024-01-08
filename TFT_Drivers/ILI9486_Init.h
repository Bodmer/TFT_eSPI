
// This is the command sequence that initialises the ILI9486 driver
//
// This setup information uses simple 8-bit SPI writecommand() and writedata() functions
//
// See ST7735_Setup.h file for an alternative format

{
    writecommand(0x01); // SW reset
    delay(120);

    writecommand(0xC0);
    writedata(0x19); // VREG1OUT POSITIVE
    writedata(0x1a); // VREG2OUT NEGATIVE

    writecommand(0xC1);
    writedata(0x45); // VGH,VGL    VGH>=14V.
    writedata(0x00);

    writecommand(0xC2); // Normal mode, increase can change the display quality, while increasing power consumption
    writedata(0x33);

    writecommand(0XC5);
    writedata(0x00);
    writedata(0x28); // VCM_REG[7:0]. <=0X80.

    writecommand(0xB1); // Sets the frame frequency of full color normal mode
    writedata(0xA0);    // 0XB0 =70HZ, <=0XB0.0xA0=62HZ
    writedata(0x11);

    writecommand(0xB4);
    writedata(0x02); // 2 DOT FRAME MODE,F<=70HZ.

    writecommand(0xB6); //
    writedata(0x00);
    writedata(0x42); // 0 GS SS SM ISC[3:0];
    writedata(0x3B);

    writecommand(0xB7);
    writedata(0x07);

    writecommand(0xE0);
    writedata(0x1F);
    writedata(0x25);
    writedata(0x22);
    writedata(0x0B);
    writedata(0x06);
    writedata(0x0A);
    writedata(0x4E);
    writedata(0xC6);
    writedata(0x39);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);

    writecommand(0XE1);
    writedata(0x1F);
    writedata(0x3F);
    writedata(0x3F);
    writedata(0x0F);
    writedata(0x1F);
    writedata(0x0F);
    writedata(0x46);
    writedata(0x49);
    writedata(0x31);
    writedata(0x05);
    writedata(0x09);
    writedata(0x03);
    writedata(0x1C);
    writedata(0x1A);
    writedata(0x00);

    writecommand(0X3A); // Set Interface Pixel Format
#ifdef SPI_18BIT_DRIVER
    writedata(0x66);
#else
    writedata(0x55);
#endif
    delay(200);
    writecommand(0x11); // Exit Sleep

    delay(120);
    writecommand(0x29); // Display on
}
