/*
  This sketch is the same as the Font_Demo_4 example, except the fonts in this
  example are in a FLASH (program memory) array. This means that processors
  such as the STM32 series that are not supported by a SPIFFS library can use
  smooth (anti-aliased) fonts.
*/

/*
  There are four different methods of plotting anti-aliased fonts to the screen.

  This sketch uses method 4, printing "String" or character array types only to screen,
  via a Sprite. The Sprite must NOT have been created already. The printToSprite()
  function automatically creates a sprite of a minimal size to contain the String,
  then plots to screen at the "tft" cursor position. Printing via a sprite draws the
  text faster on the screen. This method minimises flicker but uses RAM for the Sprite,
  the Sprite is automatically deleted after plotting to the TFT.

  Number and float types must be converted to strings to use printToSprite() e.g.:
  spr.printToSprite( (String) number );
  spr.printToSprite( (String) (number * 55 / 1.23) );  // Put calculations within brackets

  The key advantage of this method is that you do not need to calculate the size of sprite
  needed to contain the text, the library does that for you. The library also fills the
  the sprite with text background colour for you.

  printToSprite() has a second purpose, if the sprite has been created already the String
  will be printed into the Sprite at the "sprite" cursor position, which is
  different to the "tft" cursor position. In this case the Sprite is not deleted and
  you must use pushSprite() to plot on the screen. This method is not used in this sketch.
  because in general it is better to use drawString() in an already created sprite.
  printToSprite() will NOT move the tft cursor.
*/

// A processing sketch to create new fonts can be found in the Tools folder of TFT_eSPI
// https://github.com/Bodmer/TFT_eSPI/tree/master/Tools/Create_Smooth_Font/Create_font

// This sketch uses font files created from the Noto family of fonts:
// https://www.google.com/get/noto/

#include "NotoSansBold15.h"
#include "NotoSansBold36.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36

#include <SPI.h>
#include <TFT_eSPI.h>       // Hardware-specific library

TFT_eSPI    tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft); // Sprite class needs to be invoked

void setup(void) {

  Serial.begin(250000);

  tft.begin();

  tft.setRotation(1);

  spr.setColorDepth(16); // 16 bit colour needed to show anti-aliased fonts
}

void loop() {

  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set the font colour and the background colour

  tft.setTextDatum(TC_DATUM); // Top Centre datum

  int xpos = tft.width() / 2; // Half the screen width
  int ypos = 50;


  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Small font
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  spr.loadFont(AA_FONT_SMALL); // Must load the font first into the sprite class
  
  spr.setTextColor(TFT_YELLOW, TFT_BLACK); // Set the sprite font colour and the background colour

  tft.setCursor(xpos - 50, ypos);          // Set the tft cursor position, yes tft position!
  spr.printToSprite("Small 15pt font");    // Prints to tft cursor position, tft cursor NOT moved

  ypos += spr.fontHeight();   // Get the font height and move ypos down

  spr.unloadFont(); // Remove the font from sprite class to recover memory used

  delay(4000);

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Large font
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  tft.fillScreen(TFT_BLACK);

  spr.loadFont(AA_FONT_LARGE); // Load another different font

  spr.setTextColor(TFT_WHITE, TFT_BLUE); // Set the font colour and the background colour

  tft.setCursor(xpos - 90, ypos);  // Set the tft cursor position
  spr.printToSprite("36pt font");  // Text is rendered via a minimally sized sprite

  ypos += spr.fontHeight();  // Get the font height and move ypos down

  // Draw changing numbers - no flicker using this plot method!
  for (int i = 0; i <= 200; i++) {
    tft.setCursor(10, 10);
    // Number is converted to String type by (String) (number)
    spr.printToSprite(" " + (String) (i / 100.0) + " "); // Space padding helps over-write old numbers
    delay (20);
  }

  spr.unloadFont(); // Remove the font to recover memory used

  delay(8000);
}
