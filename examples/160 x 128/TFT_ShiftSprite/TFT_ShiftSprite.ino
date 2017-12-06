#include <Arduino.h>
#include <TFT_eSPI.h>

/** TFT_eSPI class for display */
TFT_eSPI tft = TFT_eSPI();
/** Sprite needed for graph scrolling */
TFT_eSprite graph1 = TFT_eSprite(&tft);
/** Sprite needed for graph scrolling */
TFT_eSprite graph2 = TFT_eSprite(&tft);
/** Value for graph1 */
int graph1Val = 1;
/** Value for graph2 */
int graph2Val = 1;

void setup() {
  tft.init();
  tft.fillScreen(TFT_BLACK);
  graph1.createSprite(128, 64);
  graph1.fillSprite(TFT_BLACK);
  graph2.createSprite(128, 64);
  graph2.fillSprite(TFT_BLACK);
}

void loop() {
  graph1.shiftRectLeft(0, 0, 128, 64, 1, TFT_BLACK);
  graph1.drawFastVLine(127,64-graph1Val,graph1Val,TFT_YELLOW);
  graph2.shiftRectRight(0, 0, 128, 64, 1, TFT_BLACK);
  graph2.drawFastVLine(0,64-graph2Val,graph2Val,TFT_RED);

  graph1.pushSprite(0, 32);
  graph2.pushSprite(0, 96);

  graph1Val++;
  graph2Val++;
  if (graph1Val == 65) graph1Val = 1;
  if (graph2Val == 65) graph2Val = 1;
  delay(50);
}
