// Code partly derived from ILI9341_due library example

// Draws the 3 icons across the middle of the screen and pauses.
// Then draws 300 icons at random locations, clears screen and repeats
//
// This demonstrates drawing icons from FLASH

// Icons are stored in tabs, e.g. Alert.h etc
// more than one icon can be in a header file.

/*
 This sketch demonstrates loading images from arrays stored in program (FLASH) memory.

 Works with TFT_eSPI library here:
 https://github.com/Bodmer/TFT_eSPI

 This sketch does not use/need any fonts at all...

 Arrays containing FLASH images can be created with UTFT library tool:
 (libraries\UTFT\Tools\ImageConverter565.exe)
 Convert to .c format then copy into a new tab

 The number and size of icons is limited by available FLASH memory. The icon array will
 use width x height x 2 bytes of FLASH, i.e. 32 x 32 icon uses ~2048 bytes

*/

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();                   // Invoke custom library with default width and height

// Include the header files that contain the icons
#include "Alert.h"
#include "Close.h"
#include "Info.h"

long count = 0; // Loop count

void setup()
{
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);	// landscape

  tft.fillScreen(TFT_BLACK);

  // Draw the icons
  drawIcon(info,  (tft.width() -  infoWidth)/2 - 50, (tft.height() -  infoHeight)/2, infoWidth,  infoHeight);
  drawIcon(alert, (tft.width() - alertWidth)/2,      (tft.height() - alertHeight)/2, alertWidth, alertHeight);
  drawIcon(closeX, (tft.width() - closeWidth)/2 + 50, (tft.height() - closeHeight)/2, closeWidth, closeHeight);

  // Pause here to admire the icons!
  delay(4000);

}

void loop()
{
  // Loop filling and clearing screen
  drawIcon(info, random(tft.width() - infoWidth), random(tft.height() - infoHeight), infoWidth, infoHeight);
  drawIcon(alert, random(tft.width() - alertWidth), random(tft.height() - alertHeight), alertWidth, alertHeight);
  drawIcon(closeX, random(tft.width() - closeWidth), random(tft.height() - closeHeight), alertWidth, closeHeight);

  // Clear screen after 100 x 3 = 300 icons drawn
  if (100 == count++) {
    count = 1;
    tft.setRotation(2 * random(2)); // Rotate randomly to clear display left>right or right>left to reduce monotony!
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(1);
    Serial.println(millis());
  }
}


//====================================================================================
// This is the function to draw the icon stored as an array in program memory (FLASH)
//====================================================================================

// To speed up rendering we use a 64 pixel buffer
#define BUFF_SIZE 64

// Draw array "icon" of defined width and height at coordinate x,y
// Maximum icon size is 255x255 pixels to avoid integer overflow

void drawIcon(const unsigned short* icon, int16_t x, int16_t y, uint16_t width, uint16_t height) {

  uint16_t  pix_buffer[BUFF_SIZE];   // Pixel buffer (16 bits per pixel)

  // Set up a window the right size to stream pixels into
  tft.setWindow(x, y, x + width - 1, y + height - 1);

  // Work out the number whole buffers to send
  uint16_t nb = ((uint16_t)height * width) / BUFF_SIZE;

  // Fill and send "nb" buffers to TFT
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < BUFF_SIZE; j++) {
      pix_buffer[j] = pgm_read_word(&icon[i * BUFF_SIZE + j]);
    }
    tft.pushColors(pix_buffer, BUFF_SIZE);
  }

  // Work out number of pixels not yet sent
  uint16_t np = ((uint16_t)height * width) % BUFF_SIZE;

  // Send any partial buffer left over
  if (np) {
    for (int i = 0; i < np; i++) pix_buffer[i] = pgm_read_word(&icon[nb * BUFF_SIZE + i]);
    tft.pushColors(pix_buffer, np);
  }
}

