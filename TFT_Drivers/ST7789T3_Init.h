{
    // Adapted from Waveshare RP2350-Touch-LCD-2 board example for the Pico C SDK
    writecommand(TFT_SLPOUT);
    delay(120);

    writecommand(TFT_COLMOD);     
    writedata(0x05);

    writecommand(0xF0); // ?? not found in the datasheet
    writedata(0xC3);   

    writecommand(0xF0); // ?? not found in the datasheet
    writedata(0x96);   

    writecommand(0xB4); // ?? not found in the datasheet
    writedata(0x01);   

    writecommand(ST7789T3_GCTRL);     
    writedata(0xC6);   

    writecommand(ST7789T3_LCMCTRL);     
    writedata(0x80);   
    writedata(0x45);   

    writecommand(ST7789T3_IDSET);     
    writedata(0x13);

    writecommand(ST7789T3_VDVVRHEN);     
    writedata(0xA7);   

    writecommand(ST7789T3_VCMOFSET);     
    writedata(0x0A);   

    writecommand(ST7789T3_PWCTRL2);     
    writedata(0x40);
    writedata(0x8A);
    writedata(0x00);
    writedata(0x00);
    writedata(0x29);
    writedata(0x19);
    writedata(0xA5);
    writedata(0x33);

    writecommand(ST7789T3_PVGAMCTRL);
    writedata(0xD0);
    writedata(0x08);
    writedata(0x0F);
    writedata(0x06);
    writedata(0x06);
    writedata(0x33);
    writedata(0x30);
    writedata(0x33);
    writedata(0x47);
    writedata(0x17);
    writedata(0x13);
    writedata(0x13);
    writedata(0x2B);
    writedata(0x31);

    writecommand(ST7789T3_NVGAMCTRL);
    writedata(0xD0);
    writedata(0x0A);
    writedata(0x11);
    writedata(0x0B);
    writedata(0x09);
    writedata(0x07);
    writedata(0x2F);
    writedata(0x33);
    writedata(0x47);
    writedata(0x38);
    writedata(0x15);
    writedata(0x16);
    writedata(0x2C);
    writedata(0x32);

    writecommand(0xF0); // ?? not found in the datasheet
    writedata(0x3C);   

    writecommand(0xF0); // ?? not found in the datasheet
    writedata(0x69);   

    delay(120);                

    writecommand(TFT_INVON);     
	
    writecommand(TFT_DISPON); 
}