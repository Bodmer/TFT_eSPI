
// THIS SKETCH IS A TEMPLATE EXAMPLE ONLY AND IS NOT INTENDED TO BE RUN UNLESS MODIFIED!

// See ReadMe tab for instructions to add a "user setup" configuration file to your sketch

#include <SPI.h>
#include <TFT_eSPI.h>       // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library

void setup(void) {
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0, 4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Hello World");
}

void loop() {
}
