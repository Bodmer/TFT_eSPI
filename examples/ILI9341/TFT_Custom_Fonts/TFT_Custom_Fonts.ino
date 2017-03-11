/*
  Example for TFT_eSPI library

  Created by Bodmer 11/03/17

  Make sure LOAD_GFXFF is defined in the used User_Setup file
  within the library folder.

  --------------------------- NOTE ----------------------------------------
  The free font encoding format does not lend itself easily to plotting
  the background without flicker. For values that changes on screen it is
  better to use Fonts 1- 8 which are encoded specifically for rapid
  drawing with background.
  -------------------------------------------------------------------------

  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  ######       TO SELECT YOUR DISPLAY TYPE AND ENABLE FONTS          ######
  #########################################################################
*/

// Note the the tilda symbol ~ does not exist in some fonts at the moment
#define TEXT "MWyz~12" // Text that will be printed on screen in any font

#include "SPI.h"
#include "TFT_eSPI.h"

// Stock font and GFXFF reference handle
#define GFXFF 1
#define FF18 &FreeSans12pt7b

// Custom are fonts added to library TFT_eSPI\Fonts\Custom folder
// a #include must also be added to the "User_Custom_Fonts.h" file
// here in this folder. See example entries.
#define CF_OL24 &Orbitron_Light_24
#define CF_OL32 &Orbitron_Light_32
#define CF_RT24 &Roboto_Thin_24
#define CF_S24  &Satisfy_24
#define CF_Y32  &Yellowtail_32


// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

void setup(void) {

  tft.begin();

  tft.setRotation(1);

}

void loop() {

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Show custom fonts
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  // Where font sizes increase the screen is not cleared as the larger fonts overwrite
  // the smaller one with the background colour.

  // Set text datum to middle centre
  tft.setTextDatum(MC_DATUM);

  // Set text colour to orange with black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.fillScreen(TFT_BLACK);            // Clear screen
  tft.setFreeFont(FF18);                 // Select the font
  tft.drawString("Yellowtail 32", 160, 60, GFXFF);// Print the string name of the font
  tft.setFreeFont(CF_Y32);                 // Select the font
  tft.drawString(TEXT, 160, 120, GFXFF);// Print the string name of the font
  delay(2000);

  tft.fillScreen(TFT_BLACK);            // Clear screen
  tft.setFreeFont(FF18);                 // Select the font
  tft.drawString("Satisfy 24", 160, 60, GFXFF);// Print the string name of the font
  tft.setFreeFont(CF_S24);                 // Select the font
  tft.drawString(TEXT, 160, 120, GFXFF);// Print the string name of the font
  delay(2000);

  tft.fillScreen(TFT_BLACK);            // Clear screen
  tft.setFreeFont(FF18);                 // Select the font
  tft.drawString("Roboto 24", 160, 60, GFXFF);// Print the string name of the font
  tft.setFreeFont(CF_RT24);                 // Select the font
  tft.drawString(TEXT, 160, 120, GFXFF);// Print the string name of the font
  delay(2000);

  tft.fillScreen(TFT_BLACK);            // Clear screen
  tft.setFreeFont(FF18);                 // Select the font
  tft.drawString("Orbitron 32", 160, 60, GFXFF);// Print the string name of the font
  tft.setFreeFont(CF_OL32);                 // Select the font
  tft.drawString(TEXT, 160, 120, GFXFF);// Print the string name of the font
  delay(2000);

  tft.fillScreen(TFT_BLACK);            // Clear screen
  tft.setFreeFont(FF18);                 // Select the font
  tft.drawString("Orbitron 24", 160, 60, GFXFF);// Print the string name of the font
  tft.setFreeFont(CF_OL24);                 // Select the font
  tft.drawString(TEXT, 160, 120, GFXFF);// Print the string name of the font
  delay(2000);

}

