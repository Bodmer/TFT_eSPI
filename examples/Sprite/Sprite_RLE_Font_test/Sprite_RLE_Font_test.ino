/*
 Display all the fast rendering fonts.

 Make sure all the display driver and pin comnections are correct by
 editting the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
*/

// Create a sprite 160 x 128 pixels (needs 40Kbytes of RAM!)
#define IWIDTH  160
#define IHEIGHT 128

// New background colour
#define TFT_BROWN 0x38E0

// Pause in milliseconds between screens, change to 0 to time font rendering
#define WAIT 500

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

TFT_eSprite img = TFT_eSprite(&tft);

unsigned long targetTime = 0; // Used for testing draw times

void setup(void) {
  tft.init();
  tft.setRotation(0);

  tft.fillScreen(TFT_BLUE);

  img.createSprite(IWIDTH, IHEIGHT);
  img.fillSprite(TFT_BLACK);
}

void loop() {
  targetTime = millis();

  // First we test them with a background colour set
  img.setTextSize(1);
  img.fillSprite(TFT_BLACK);
  img.setTextColor(TFT_GREEN, TFT_BLACK);

  img.drawString(" !\"#$%&'()*+,-./0123456", 0, 0, 2);
  img.drawString("789:;<=>?@ABCDEFGHIJKL", 0, 16, 2);
  img.drawString("MNOPQRSTUVWXYZ[\\]^_`", 0, 32, 2);
  img.drawString("abcdefghijklmnopqrstuvw", 0, 48, 2);

  int xpos = 0;
  xpos += img.drawString("xyz{|}~", 0, 64, 2);
  img.drawChar(127, xpos, 64, 2);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BLACK);
  img.setTextColor(TFT_GREEN, TFT_BLACK);

  img.drawString(" !\"#$%&'()*+,-.", 0, 0, 4);
  img.drawString("/0123456789:;", 0, 26, 4);
  img.drawString("<=>?@ABCDE", 0, 52, 4);
  img.drawString("FGHIJKLMNO", 0, 78, 4);
  img.drawString("PQRSTUVWX", 0, 104, 4);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BLACK);
  img.drawString("YZ[\\]^_`abc", 0, 0, 4);
  img.drawString("defghijklmno", 0, 26, 4);
  img.drawString("pqrstuvwxyz", 0, 52, 4);
  xpos = 0;
  xpos += img.drawString("{|}~", 0, 78, 4);
  img.drawChar(127, xpos, 78, 4);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BLACK);
  img.setTextColor(TFT_BLUE, TFT_BLACK);

  img.drawString("012345", 0, 0, 6);
  img.drawString("6789", 0, 40, 6);
  img.drawString("apm-:.", 0, 80, 6);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BLACK);
  img.setTextColor(TFT_RED, TFT_BLACK);

  img.drawString("0123", 0, 0, 7);
  img.drawString("4567", 0, 60, 7);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BLACK);
  img.drawString("890:.", 0, 0, 7);
  img.drawString("", 0, 60, 7);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BLACK);
  img.setTextColor(TFT_YELLOW, TFT_BLACK);

  img.drawString("01", 0, 0, 8);
  img.pushSprite(0, 0); delay(WAIT);
  
  img.drawString("23", 0, 0, 8);
  img.pushSprite(0, 0); delay(WAIT);

  img.drawString("45", 0, 0, 8);
  img.pushSprite(0, 0); delay(WAIT);
  
  img.drawString("67", 0, 0, 8);
  img.pushSprite(0, 0); delay(WAIT);

  img.drawString("89", 0, 0, 8);
  img.pushSprite(0, 0); delay(WAIT);
 
  img.drawString("0:.", 0, 0, 8);
  img.pushSprite(0, 0); delay(WAIT);

  img.setTextColor(TFT_WHITE);
  img.drawNumber(millis() - targetTime, 0, 100, 4);
  img.pushSprite(0, 0); delay(WAIT);
  delay(4000);

  // Now test them with transparent background
  targetTime = millis();

  img.setTextSize(1);
  img.fillSprite(TFT_BROWN);
  img.setTextColor(TFT_GREEN);

  img.drawString(" !\"#$%&'()*+,-./0123456", 0, 0, 2);
  img.drawString("789:;<=>?@ABCDEFGHIJKL", 0, 16, 2);
  img.drawString("MNOPQRSTUVWXYZ[\\]^_`", 0, 32, 2);
  img.drawString("abcdefghijklmnopqrstuvw", 0, 48, 2);
  xpos = 0;
  xpos += img.drawString("xyz{|}~", 0, 64, 2);
  img.drawChar(127, xpos, 64, 2);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BROWN);
  img.setTextColor(TFT_GREEN);

  img.drawString(" !\"#$%&'()*+,-.", 0, 0, 4);
  img.drawString("/0123456789:;", 0, 26, 4);
  img.drawString("<=>?@ABCDE", 0, 52, 4);
  img.drawString("FGHIJKLMNO", 0, 78, 4);
  img.drawString("PQRSTUVWX", 0, 104, 4);

  img.pushSprite(0, 0); delay(WAIT);
  img.fillSprite(TFT_BROWN);
  img.drawString("YZ[\\]^_`abc", 0, 0, 4);
  img.drawString("defghijklmno", 0, 26, 4);
  img.drawString("pqrstuvwxyz", 0, 52, 4);
  xpos = 0;
  xpos += img.drawString("{|}~", 0, 78, 4);
  img.drawChar(127, xpos, 78, 4);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BROWN);
  img.setTextColor(TFT_BLUE);

  img.drawString("012345", 0, 0, 6);
  img.drawString("6789", 0, 40, 6);
  img.drawString("apm-:.", 0, 80, 6);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BROWN);
  img.setTextColor(TFT_RED);

  img.drawString("0123", 0, 0, 7);
  img.drawString("4567", 0, 60, 7);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BROWN);
  img.drawString("890:.", 0, 0, 7);
  img.drawString("", 0, 60, 7);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BROWN);
  img.setTextColor(TFT_YELLOW);

  img.drawString("0123", 0, 0, 8);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BROWN);
  img.drawString("4567", 0, 0, 8);
  img.pushSprite(0, 0); delay(WAIT);

  img.fillSprite(TFT_BROWN);
  img.drawString("890:.", 0, 0, 8);
  img.pushSprite(0, 0); delay(WAIT);

  img.setTextColor(TFT_WHITE);

  img.drawNumber(millis() - targetTime, 0, 100, 4);
  img.pushSprite(0, 0); delay(WAIT);
  delay(4000);;
}

