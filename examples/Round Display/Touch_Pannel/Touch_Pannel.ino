#include <Arduino.h>
#include <TFT_eSPI.h> 
#include <SPI.h>


TFT_eSPI tft = TFT_eSPI();       

void setup()   {
  tft.init();
  tft.fillScreen(TFT_WHITE);
}


void loop() {
  int32_t x = 0;
  int32_t y = 0;
  if(tft.getTouch(&x,&y)){
    tft.fillCircle(x, y, 2, TFT_BLACK);
  }
  
}


