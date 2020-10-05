/*  

  // Create a viewport at TFT screen coordinated X,y of width W and height H
  tft.setViewport(X, Y, W, H); // By default the 0,0 coordinate datum is moved to top left
                               // corner of viewport
                               // Note: tft.width() and tft.height() now return viewport size!
  // The above command is identical to:
  tft.setViewport(VP_X, VP_Y, VP_W, VP_H, true); // true parameter is optional

  // To create a viewport that keeps the coordinate datum at top left of TFT, use false parameter
  tft.setViewport(VP_X, VP_Y, VP_W, VP_H, false); // Note: tft.width() and tft.height() return TFT size!

  // To get width of viewport
  uint16_t w = tft.getViewportWidth();  // Always returns width of viewport

  // To get height of viewport
  uint16_t h = tft.getViewportHeight(); // Always returns height of viewport

  // To draw a rectangular frame outside viewport of width W (when W is negative)
  tft.frameViewport(TFT_RED, -W); // Note setting the width to a large negative value will clear the screen
                                  // outside the viewport

  // To draw a rectangular frame inside viewport of width W (when W is positive)
  tft.frameViewport(TFT_RED,  W); // Note setting the width to a large positive value will clear the screen
                                  // inside the viewport

  // To reset the viewport to the normal TFT full screen
  tft.resetViewport();  // Note: Graphics will NOT be drawn to the TFT outside a viewport until
                        // this command is used! ( The exception is using the frameViewport command
                        // detailed above with a negative width.)


*/
