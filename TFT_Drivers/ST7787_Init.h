
// This is the command sequence that initialises the ST7787 driver
//
// This setup information uses simple 8 bit SPI writecommand() and writedata() functions
//
// See ST7735_Setup.h file for an alternative format

{
  /* Take the display out of sleep mode. */
  writecommand(ST7787_SLPOUT);
  /*
    Wait for sleep-out command to complete.
    Datasheet says 5 msec is enough before next command, but 120 msec is
    needed before the display is fully out of sleep mode.
  */
  delay(120);
  /*
    Select 16-bit 565 RGB pixel format (mode 5).
    Same for RGB mode (but we don't use it).
  */
  writecommand(ST7787_COLMOD);
  writedata((13 << 4) | 5);
  /* Disable external vsync. */
  writecommand(ST7787_VSYNCOUT);
  /* Turn on the display */
  writecommand(ST7787_DISPON);

  {
    /* Debug: Read ID and status, to see if the basics are working. */
    uint8_t buf[4];
    docommand(ST7787_RDDID, NULL, 0, buf, 3);
    Serial.print("RDDID: "); Serial.print(buf[0], HEX); Serial.print(" "); Serial.print(buf[1], HEX); Serial.print(" "); Serial.print(buf[2], HEX); Serial.print("\r\n");
    docommand(ST7787_RDDST, NULL, 0, buf, 4);
    Serial.print("RDDST: "); Serial.print(buf[0], HEX); Serial.print(" "); Serial.print(buf[1], HEX); Serial.print(" "); Serial.print(buf[2], HEX); Serial.print(" "); Serial.print(buf[3], HEX); Serial.print("\r\n");
    for (;;) { }
  }
}
