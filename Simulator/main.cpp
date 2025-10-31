#include "TFT_eSPI.h"
#include "render_sdl.h"

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define SCREEN_SCALE 2
#define SCREEN_BG TFT_BLACK

static void drawTest(TFT_eSprite &sprite) {
  // text size
  sprite.setTextSize(3);

  // red
  sprite.fillRect(10, 0, 30, 30, TFT_RED);
  sprite.setTextColor(TFT_RED);
  sprite.drawString("RED", 50, 0);

  // green
  sprite.fillRect(10, 50, 30, 30, TFT_GREEN);
  sprite.setTextColor(TFT_GREEN);
  sprite.drawString("GREEN", 50, 50);

  // blue
  sprite.fillRect(10, 100, 30, 30, TFT_BLUE);
  sprite.setTextColor(TFT_BLUE);
  sprite.drawString("BLUE", 50, 100);

  // white
  sprite.fillRect(10, 150, 30, 30, TFT_WHITE);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("WHITE", 50, 150);
}

int main() {
  SDLRenderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_SCALE);
  if (!renderer.init()) {
    return 1;
  }

  TFT_eSPI tft = TFT_eSPI();
  tft.init();

  TFT_eSprite sprite = TFT_eSprite(&tft);
  sprite.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
  sprite.fillSprite(SCREEN_BG);

  // test
  drawTest(sprite);

  // update display
  renderer.updateDisplay(&sprite, 0, 0);

  // run event loop
  SDLRenderer::runEventLoop();

  // clean up
  sprite.deleteSprite();

  return 0;
}
