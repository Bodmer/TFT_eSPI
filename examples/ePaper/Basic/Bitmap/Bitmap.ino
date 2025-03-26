#include "TFT_eSPI.h"
#include "image.h"

#ifdef EPAPER_ENABLE  // Only compile this code if the EPAPER_ENABLE is defined in User_Setup.h
EPaper epaper;
#endif
void setup()
{
#ifdef EPAPER_ENABLE
  Serial.begin(115200);
  delay(2000);
  Serial.println("Start");
  epaper.begin();

  epaper.fillScreen(TFT_WHITE);
  epaper.update(); // update the display
  
  // epaper.pushImage(0, 0, 800, 480, (uint16_t *)gImage_1);
  // epaper.update();
  Serial.println("update");
  epaper.update(0, 0, TFT_WIDTH, TFT_HEIGHT, (uint16_t *)gImage_1);
#endif
}

void loop()
{
  // put your main code here, to run repeatedly:
}
