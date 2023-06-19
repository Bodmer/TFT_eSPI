
// This is the command sequence that initialises the R61529 driver
//
// This setup information uses simple 8 bit SPI writecommand() and writedata() functions
//
// Thie files R61529_init.h, R61529_Rotation.h and R61529_Defines.h is copied from
// https://github.com/daumemo/IPS_LCD_R61529_FT6236_Arduino_eSPI_Test/blob/master/lib/TFT_eSPI/TFT_Drivers
// Thanks to daumemo who has released it under a MIT License.
//
// Copyright (c) 2020 daumemo

    writecommand(TFT_SLPOUT);
    delay(20);

    writecommand(0xB0);
    writedata(0x04);
    delay(2);

    writecommand(0xB8); //lcd pwm
    writedata(0x02);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);

    writecommand(0xB9); //lcd pwm
    writedata(0x01); // PWMON = 1;
    writedata(0x00); // BDCV = 255;
    writedata(0xff); // pwm freq = 13.7 kHz
    writedata(0x18); // PWMWM = 1; LEDPWME = 1;

    //additional commands:
    writecommand(0xB3); //Frame Memory Access and Interface Setting
    writedata(0x02); // reset start position of a window area address...
    writedata(0x00); //TE pin is used. TE signal is output every frame.
    writedata(0x00); // empty according to the datasheet - does nothing;
    writedata(0x00); // convert 16/18 bits to 24bits data by writing zeroes to LSBs. Sets image data write/read format(?)
    writedata(0x00);  // ???? (not needed?)
    delay(2);

    writecommand(0xB4); //Display Mode
    writedata(0x00); //Uses internal oscillator

    writecommand(0xC0); // Panel Driving Setting;
    writedata(0x03); // Output polarity is inverted. Left/right interchanging scan. Forward scan. BGR mode (depends on other settings). S960 → S1 (depends)
    writedata(0xDF); // Number of lines for driver to drive - 480.
    writedata(0x40); // Scan start position - Gate1. (depend on other param);
    writedata(0x10); // Dot inversion. Dot inversion in not-lit display area. If 0x13 - both will be set to 'column inversion'.
    writedata(0x00); // settings for non-lit display area...
    writedata(0x01); // 3 frame scan interval in non-display area...
    writedata(0x00); // Source output level in retrace period...
    writedata(0x55);//54 . Internal clock divider = 5 (low and high periods).

    writecommand(0xC1); //Display Timing Setting for Normal Mode
    writedata(0x07); // Clock devider = 12. 14MHz/12. Used by display circuit and step-up circuit.
    writedata(0x27); // These bits set the number of clocks in 1 line period. 0x27 - 39 clocks.
    writedata(0x08); // Number of back porch lines. 0x08 - 8 lines.
    writedata(0x08); // Number of front porch lines. 0x08 - 8lines.
    writedata(0x00); // Spacial configuriation mode 1 (?). 1 line inversion mode (?).

    writecommand(0xC4); // Source/Gate Driving Timing Setting
    writedata(0x57); // falling position (stop) of gate driver - 4 clocks... gate start position - 8 clocks...
    writedata(0x00); // nothing to set up according to the datasheet
    writedata(0x05); // Source precharge period (GND) - 5 clocks.
    writedata(0x03); // source precharge period (VCI) - 3 clocks.

    writecommand(0xC6); //DPI polarity control
    writedata(0x04); // VSYNC -Active Low. HSYNC - Active Low. DE pin enable data write in when DE=1. Reads data on the rising edge of the PCLK signal.

    //----Gamma setting start-----
    writecommand(0xC8);
    writedata(0x03);
    writedata(0x12);
    writedata(0x1A);
    writedata(0x24);
    writedata(0x32);
    writedata(0x4B);
    writedata(0x3B);
    writedata(0x29);
    writedata(0x1F);
    writedata(0x18);
    writedata(0x12);
    writedata(0x04);

    writedata(0x03);
    writedata(0x12);
    writedata(0x1A);
    writedata(0x24);
    writedata(0x32);
    writedata(0x4B);
    writedata(0x3B);
    writedata(0x29);
    writedata(0x1F);
    writedata(0x18);
    writedata(0x12);
    writedata(0x04);

    writecommand(0xC9);
    writedata(0x03);
    writedata(0x12);
    writedata(0x1A);
    writedata(0x24);
    writedata(0x32);
    writedata(0x4B);
    writedata(0x3B);
    writedata(0x29);
    writedata(0x1F);
    writedata(0x18);
    writedata(0x12);
    writedata(0x04);

    writedata(0x03);
    writedata(0x12);
    writedata(0x1A);
    writedata(0x24);
    writedata(0x32);
    writedata(0x4B);
    writedata(0x3B);
    writedata(0x29);
    writedata(0x1F);
    writedata(0x18);
    writedata(0x12);
    writedata(0x04);

    writecommand(0xCA);
    writedata(0x03);
    writedata(0x12);
    writedata(0x1A);
    writedata(0x24);
    writedata(0x32);
    writedata(0x4B);
    writedata(0x3B);
    writedata(0x29);
    writedata(0x1F);
    writedata(0x18);
    writedata(0x12);
    writedata(0x04);

    writedata(0x03);
    writedata(0x12);
    writedata(0x1A);
    writedata(0x24);
    writedata(0x32);
    writedata(0x4B);
    writedata(0x3B);
    writedata(0x29);
    writedata(0x1F);
    writedata(0x18);
    writedata(0x12);
    writedata(0x04);
//---Gamma setting end--------
    //old ones:
    writecommand(0xD0);
    writedata(0x99);//DC4~1//A5. Set up clock cycle of the internal step up controller.
    writedata(0x06);//BT // Set Voltage step up factor.
    writedata(0x08);// default according to the datasheet - does nothing.
    writedata(0x20);// VCN step up cycles.
    writedata(0x29);//VC1, VC2// VCI3 voltage = 2.70V;  VCI2 voltage = 3.8V.
    writedata(0x04);// default 
    writedata(0x01);// default 
    writedata(0x00);// default 
    writedata(0x08);// default
    writedata(0x01);// default
    writedata(0x00);// default
    writedata(0x06);// default
    writedata(0x01);// default
    writedata(0x00);// default
    writedata(0x00);// default
    writedata(0x20);// default

    writecommand(0xD1);//VCOM setting
    writedata(0x00);//disable write to VDC[7:0]
    writedata(0x20);//45 38 VPLVL// voltage of γ correction registers for positive polarity
    writedata(0x20);//45 38 VNLVL// voltage of γ correction registers for negative polarity
    writedata(0x15);//32 2A VCOMDC// VNLVL x 0.063

    writecommand(0xE0);//NVM Access Control
    writedata(0x00);//NVM access is disabled
    writedata(0x00);//Erase operation (disabled).
    writedata(0x00);//TE pin works as tearing effect pin. 
  // should be one more writeData(0x00); according to the datasheet.

    writecommand(0xE1); //set_DDB_write_control
    writedata(0x00); 
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);
    writedata(0x00);

    writecommand(0xE2); //NVM Load Control
    writedata(0x00); // does not execute data load from the NVM to each command

    writecommand(TFT_MADCTL);
    writedata(0x00);

    writecommand(0x3A); // set_pixel_format
    writedata(0x55); // 16-Bit/pixel = 55h, 24-bit/pixel = 77h

    writecommand(TFT_CASET);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0x3F);

    writecommand(TFT_PASET);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0xDF);

    delay(120);
    writecommand(TFT_DISPON);

    delay(250);
// End of R61581 display configuration



