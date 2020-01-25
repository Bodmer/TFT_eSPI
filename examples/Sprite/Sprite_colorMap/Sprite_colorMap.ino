/*

  Sketch to show how a Sprite is created, how to draw pixels
  and text within the Sprite and then push the Sprite onto
  the display screen.

  Example for library:
  https://github.com/Bodmer/TFT_eSPI

  A Sprite is notionally an invisible graphics screen that is
  kept in the processors RAM. Graphics can be drawn into the
  Sprite just as it can be drawn directly to the screen. Once
  the Sprite is completed it can be plotted onto the screen in
  any position. If there is sufficient RAM then the Sprite can
  be the same size as the screen and used as a frame buffer.

  A 16 bit Sprite occupies (2 * width * height) bytes in RAM.

  On a ESP8266 Sprite sizes up to 126 x 160 can be accomodated,
  this size requires 40kBytes of RAM for a 16 bit color depth.
  
  When 8 bit color depth sprites are created they occupy
  (width * height) bytes in RAM, so larger sprites can be
  created, or the RAM required is halved.

*/

// Set delay after plotting the sprite
#define DELAY 1000

// Width and height of sprite
#define WIDTH  128
#define HEIGHT 128

#include <TFT_eSPI.h>                 // Include the graphics library (this includes the sprite functions)

TFT_eSPI    tft = TFT_eSPI();         // Declare object "tft"

TFT_eSprite spr = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object


void setup()
{
  Serial.begin(250000);
  Serial.println();

  delay(500);

  // Initialise the TFT registers
  tft.init();
  
  spr.setColorDepth(4);

  // Create a sprite of defined size
  spr.createSprite(WIDTH, HEIGHT);

  // Clear the TFT screen to blue
  tft.fillScreen(TFT_BLUE);
}

void loop(void)
{
  // Fill the whole sprite with color 5 (Sprite is in memory so not visible yet)
  spr.fillSprite(10);

   // create a color map with known colors
  uint16_t cmap[16];


  cmap[0] = TFT_BLACK;
  cmap[1] = TFT_NAVY;
  cmap[2] = TFT_DARKGREEN;
  cmap[3] = TFT_DARKCYAN;
  cmap[4] = TFT_MAROON;
  cmap[5] = TFT_PURPLE;
  cmap[6] = TFT_OLIVE;
  cmap[7] = TFT_LIGHTGREY;
  cmap[8] = TFT_DARKGREY;
  cmap[9] = TFT_BLUE;
  cmap[10] = TFT_GREEN;
  cmap[11] = TFT_CYAN;
  cmap[12] = TFT_RED;
  cmap[13] = TFT_MAGENTA;
  cmap[14] = TFT_YELLOW;
  cmap[15] = TFT_WHITE;

  spr.setColorMap(cmap, 16);
 
  spr.pushSprite(-40, -40);
  spr.pushSprite(tft.width() / 2 - WIDTH / 2, tft.height() / 2 - HEIGHT / 2, 10);
  spr.pushSprite(tft.width() - WIDTH + 40, tft.height() - HEIGHT + 40);

  // Number of pixels to draw
  uint16_t n = 100;
  // Draw 100 random color pixels at random positions in sprite
  while (n--)
  {
    uint16_t color = random(0x10); // Returns color 0 - 0x0F
    int16_t x = random(WIDTH);        // Random x coordinate
    int16_t y = random(HEIGHT);       // Random y coordinate
    spr.drawPixel( x, y, color);      // Draw pixel in sprite
  }

  spr.pushSprite(-40, -40);
  spr.pushSprite(tft.width() / 2 - WIDTH / 2, tft.height() / 2 - HEIGHT / 2);
  spr.pushSprite(tft.width() - WIDTH + 40, tft.height() - HEIGHT + 40);

  delay(DELAY);

  // Draw some lines
  spr.drawLine(1, 0, WIDTH, HEIGHT-1, 6);
  spr.drawLine(0, 0, WIDTH, HEIGHT, 6);
  spr.drawLine(0, 1, WIDTH-1, HEIGHT, 2);
  spr.drawLine(0, HEIGHT-1, WIDTH-1, 0, 2);
  spr.drawLine(0, HEIGHT, WIDTH, 0, 3);
  spr.drawLine(1, HEIGHT, WIDTH, 1, 3);
  spr.drawLine(4, 0, 4, HEIGHT-1, 11);
  spr.drawLine(0, 16, WIDTH-1, 16, 13);

  // draw some circles with random colors.
  spr.drawCircle(20, 60, 10, 6);
  spr.drawCircle(80, 60, 15, 7);
  spr.drawCircle(50, 108, 5, 9);
  spr.drawCircle(45, 86, 3, 8);
  spr.fillCircle(102, 56, 4, 11);

  spr.fillRect(28, 32, 40, 4, 5);
  //spr.fillRect(27, 42, 40, 14, 6);
  //spr.fillRect(33, 55, 3, 4, 7);
  //spr.fillRect(34, 32, 7, 4, 8);

 // Draw some text with Middle Centre datum
  spr.setTextDatum(MC_DATUM);
  spr.drawString("Sprite", WIDTH / 2, HEIGHT / 2, 1);

  // Now push the sprite to the TFT at position 0,0 on screen
  spr.pushSprite(-40, -40);
  spr.pushSprite(tft.width() / 2 - WIDTH / 2, tft.height() / 2 - HEIGHT / 2);
  spr.pushSprite(tft.width() - WIDTH + 40, tft.height() - HEIGHT + 40);

  delay(DELAY * 4);

  // create a new color map and use it instead

  for (auto i = 0; i < 16; i++)
  {
    cmap[i] = random(0x10000);
  }

  spr.setColorMap(cmap, 16);
  // Now push the sprite to the TFT at position 0,0 on screen

  spr.pushSprite(-40, -40);
  spr.pushSprite(tft.width() / 2 - WIDTH / 2, tft.height() / 2 - HEIGHT / 2);
  spr.pushSprite(tft.width() - WIDTH + 40, tft.height() - HEIGHT + 40);

  delay(DELAY);

    // Fill TFT screen with blue
  tft.fillScreen(TFT_BLUE);

  // Draw a blue rectangle in sprite so when we move it 1 pixel it does not leave a trail
  // on the blue screen background
  cmap[14] = TFT_BLUE;
  spr.setColorMap(cmap, 16);

  spr.drawRect(0, 0, WIDTH, HEIGHT, 14);

  int x = tft.width() / 2  -  WIDTH / 2;
  int y = tft.height() / 2 - HEIGHT / 2;

  uint32_t updateTime = 0;       // time for next update

  while (true)
  {
    // Random movement direction
    int dx = 1; if (random(2)) dx = -1;
    int dy = 1; if (random(2)) dy = -1;

    // Pull it back onto screen if it wanders off
    if (x < -WIDTH/2) dx = 1;
    if (x >= tft.width()-WIDTH/2) dx = -1;
    if (y < -HEIGHT/2) dy = 1;
    if (y >= tft.height()-HEIGHT/2) dy = -1;

    // Draw it 50 time, moving in random direct or staying still
    n = 50;
    int wait = random (50);
    while (n)
    {
      if (updateTime <= millis())
      {
        // Use time delay so sprite does not move fast when not all on screen
        updateTime = millis() + wait;

        // Push the sprite to the TFT screen
          spr.pushSprite(x, y);

        // Change coord for next loop
        x += dx;
        y += dy;
        n--;
        yield(); // Stop watchdog reset
      }
    }
  } // Infinite while, will not exit!
}

