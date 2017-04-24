/**The MIT License (MIT)
Copyright (c) 2015 by Daniel Eichhorn
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

See more at http://blog.squix.ch

Adapted by Bodmer to use the faster TFT_eSPI library:
https://github.com/Bodmer/TFT_eSPI

*/


#include <TFT_eSPI.h> // Hardware-specific library

#define FS_NO_GLOBALS // Avoid conflict with SD library File type definition
#include <FS.h>

// JPEG decoder library
#include <JPEGDecoder.h>

#ifndef _GFX_UI_H
#define _GFX_UI_H

// Maximum of 85 for BUFFPIXEL as 3 x this value is stored in an 8 bit variable!
// 32 is an efficient size for SPIFFS due to SPI hardware pipeline buffer size
// A larger value of 80 is better for SD cards
#define BUFFPIXEL 32

class GfxUi {
  public:
    GfxUi(TFT_eSPI * tft);
    void drawBmp(String filename, uint8_t x, uint16_t y);
    void drawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percentage, uint16_t frameColor, uint16_t barColor);
    void jpegInfo();
    void drawJpeg(const char *filename, int xpos, int ypos);
    void jpegRender(int xpos, int ypos);
    
  private:
    TFT_eSPI * _tft;
    uint16_t read16(fs::File &f);
    uint32_t read32(fs::File &f);

};

#endif

