  // 
  // This is the command sequence that initialises the ST7781 / SPFD5408 driver
  // References:
  //  - https://github.com/compihu/SWIFT-Shield
  //  - https://github.com/prenticedavid/MCUFRIEND_kbv
  //      SUPPORT_7781 / ST7781_regValues_CPT24
  //
  // This want full 16 bit command, so writecommand() is changed with define WRITE_COMMAND_16
{
  writecommand(ST7781_DRIVER_OUTPUT_CTRL);    writedata(0x01);writedata(0x00);   // Driver Output Control Register (R01h)
  writecommand(ST7781_LCD_AC_DRIVING_CTRL);   writedata(0x07);writedata(0x00);   // LCD Driving Waveform Control (R02h)
  writecommand(ST7781_ENTRY_MODE);            writedata(0x10);writedata(0x30);   // Entry Mode (R03h)
  writecommand(ST7781_DISP_CTRL2);            writedata(0x03);writedata(0x02);   // Porch
  writecommand(ST7781_DISP_CTRL3);            writedata(0x00);writedata(0x00);   // Scan
  writecommand(ST7781_DISP_CTRL4);            writedata(0x00);writedata(0x08);   // Fmark Off
  writecommand(ST7781_POWER_CTRL1);           writedata(0x00);writedata(0x08);   // Power Control 1 (R10h)
  writecommand(ST7781_POWER_CTRL2);           writedata(0x00);writedata(0x05);   // Power Control 2 (R11h)
  writecommand(ST7781_POWER_CTRL3);           writedata(0x00);writedata(0x00);   // Power Control 3 (R12h)
  writecommand(ST7781_POWER_CTRL4);           writedata(0x00);writedata(0x00);   // Power Control 4 (R13h)
  delay(100);
  writecommand(ST7781_POWER_CTRL1);           writedata(0x12);writedata(0xB0);   // Power Control 1 SAP=1, BT=2, APE=1, AP=3
  delay(50);
  writecommand(ST7781_POWER_CTRL2);           writedata(0x00);writedata(0x07);   // Power Control 2 VC=7
  delay(50);
  writecommand(ST7781_POWER_CTRL3);           writedata(0x00);writedata(0x8C);   // Power Control 3 VCIRE=1, VRH=12
  writecommand(ST7781_POWER_CTRL4);           writedata(0x17);writedata(0x00);   // Power Control 4 VDV=23
  writecommand(ST7781_VCOMH_CTRL);            writedata(0x00);writedata(0x20);   // NVM read data 2 VCM=32
  delay(50);
  writecommand(ST7781_GAMMA_CTRL1);           writedata(0x00);writedata(0x00);   // Gamma Control 1 App Note CPT 2.4
  writecommand(ST7781_GAMMA_CTRL2);           writedata(0x01);writedata(0x06);   // Gamma Control 2
  writecommand(ST7781_GAMMA_CTRL3);           writedata(0x01);writedata(0x01);   // Gamma Control 3
  writecommand(ST7781_GAMMA_CTRL4);           writedata(0x01);writedata(0x06);   // Gamma Control 4
  writecommand(ST7781_GAMMA_CTRL5);           writedata(0x02);writedata(0x03);   // Gamma Control 5
  writecommand(ST7781_GAMMA_CTRL6);           writedata(0x00);writedata(0x00);   // Gamma Control 6
  writecommand(ST7781_GAMMA_CTRL7);           writedata(0x07);writedata(0x07);   // Gamma Control 7
  writecommand(ST7781_GAMMA_CTRL8);           writedata(0x02);writedata(0x04);   // Gamma Control 8
  writecommand(ST7781_GAMMA_CTRL9);           writedata(0x01);writedata(0x06);   // Gamma Control 9
  writecommand(ST7781_GAMMA_CTRL10);          writedata(0x01);writedata(0x03);   // Gamma Control 10
  writecommand(ST7781_GATE_SCAN_CTRL1);       writedata(0xA7);writedata(0x00);   // Driver Output Control (R60h) .kbv was 0xa700
  writecommand(ST7781_GATE_SCAN_CTRL2);       writedata(0x00);writedata(0x01);   // Driver Output Control (R61h)
  writecommand(ST7781_PANEL_IFACE_CTRL1);     writedata(0x00);writedata(0x30);   // Panel Interface Control 1 (R90h)
  // Display On
  writecommand(ST7781_DISP_CTRL1);            writedata(0x01);writedata(0x33);   // Display Control (R07h)
  delay(50);

}