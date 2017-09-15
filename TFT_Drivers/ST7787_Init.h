
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
  /* Initialise to Rotation 0. */
  writecommand(TFT_MADCTL);
  writedata(TFT_MAD_ML | TFT_MAD_RGB);
  /* Disable external vsync. */
  writecommand(ST7787_VSYNCOUT);
  /* Turn on the display */
  writecommand(ST7787_DISPON);
}
