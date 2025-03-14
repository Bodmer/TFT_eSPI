#define SEEED_XIAO_EPAPER_7INCH5
#include "TFT_eSPI.h"
#include "image.h"
EPaper epaper;
void setup()
{

  epaper.begin();
  epaper.fillScreen(TFT_WHITE);
  epaper.update(); // update the display
 
  // epaper.pushImage(0, 0, 800, 480, (uint16_t *)gImage_1);
  // epaper.update();

  epaper.update(0, 0, 800, 480, (uint16_t *)gImage_1);
  
}

void loop()
{
  // put your main code here, to run repeatedly:
}