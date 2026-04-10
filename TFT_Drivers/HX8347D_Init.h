
// This is the command sequence that initialises the HX8347D driver
//
// This setup information uses simple 8 bit SPI writecommand() and writedata() functions
//
// See ST7735_Setup.h file for an alternative format


// Configure HX8347D display

writecommand(HX8347D_SETSTBA); 
writedata(0x00);  // STBA[15:8]
writedata(0x20);  // STBA[7:0]

writecommand(HX8347D_SETPTBA);
writedata(0x0C);  // PTBA[15:8]
writedata(0xC4);  // PTBA[7:0]

writecommand(HX8347D_SETSOPN);
writedata(0x40);  // OPON_N[7:0]
writecommand(HX8347D_SETSOPI);
writedata(0x38);  // OPON_I[7:0]

writecommand(0xF1);
writedata(0x01);  // OTPS1B
writecommand(0xF2);
writedata(0x10);  // GEN
writecommand(0x27);
writedata(0xA3);

// Gamma
writecommand(HX8347D_SETGAMMA1);
writedata(0x01);
writedata(0x00);
writedata(0x00);
writedata(0x10);
writedata(0x0E);
writedata(0x24);
writedata(0x04);
writedata(0x50);
writedata(0x02);
writedata(0x13);
writedata(0x19);
writedata(0x19);
writedata(0x16);
writecommand(HX8347D_SETGAMMA2);
writedata(0x1B);
writedata(0x31);
writedata(0x2F);
writedata(0x3F);
writedata(0x3F);
writedata(0x3E);
writedata(0x2F);
writedata(0x7B);
writedata(0x09);
writedata(0x06);
writedata(0x06);
writedata(0x0C);
writedata(0x1D);
writedata(0xCC);

// Power Voltage Setting
writecommand(HX8347D_PWRVRH); 
writedata(0x1B);  // VRH=4.65V
writecommand(HX8347D_PWRBT);
writedata(0x01);  // BT (VGH~15V,VGL~-10V,DDVDH~5V)
writecommand(HX8347D_VCOMVMH);
writedata(0x15);  // VMH (VCOM High voltage ~3.2V)
writecommand(HX8347D_VCOMVML);
writedata(0x50);  // VML (VCOM Low voltage -1.2V)
writecommand(HX8347D_VCOMVMF);
writedata(0x88);  // for Flicker adjust, can reload from OTP

//Power ON Setting
writecommand(HX8347D_SETOSC2);
writedata(0x36);  // I/P_RADJ, N/P_RADJ, Normal mode 60Hz
writecommand(HX8347D_SETOSC1);
writedata(0x01);  // OSC_EN=1

writecommand(0x01);
writedata(0x00);  // DP_STB=0

writecommand(HX8347D_PWRCTL);
writedata(0x88);  // GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DDVDH_TRI=0, STB=0
delay(5);
writecommand(HX8347D_PWRCTL);
writedata(0x80);  // GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DDVDH_TRI=0, STB=0
delay(5);
writecommand(HX8347D_PWRCTL);
writedata(0x90);  // GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
delay(5);
writecommand(HX8347D_PWRCTL);
writedata(0xD0);  // GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
delay(5);

// Color selection
writecommand(HX8347D_COLMOD);
writedata(0x05);  // 65k color

// Set panel
writecommand(HX8347D_SETPANEL);
writedata(0x00);  // SS_P, GS_P, REV_P, BGR_P

// Display ON Setting
writecommand(HX8347D_DISPCTL);
writedata(0x38);  // GON=1, DTE=1, D=1000
delay(40);
writecommand(HX8347D_DISPCTL);
writedata(0x3C);  // GON=1, DTE=1, D=1100

writecommand(HX8347D_MADCTL);
writedata(0x08);  // MY=0, MX=0, MV=0, BGR=1

// Set GRAM Area
writecommand(HX8347D_CASET_S2);
writedata(0x00);
writecommand(HX8347D_CASET_S1);
writedata(0x00);
writecommand(HX8347D_CASET_E2);
writedata(0x00);
writecommand(HX8347D_CASET_E1);
writedata(0xEF);
writecommand(HX8347D_PASET_S2);
writedata(0x00);
writecommand(HX8347D_PASET_S1);
writedata(0x00);
writecommand(HX8347D_PASET_E2);
writedata(0x01);
writecommand(HX8347D_PASET_E1);
writedata(0x3F);