 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the TFT_eSPI class and is associated with anti-aliased font functions
 

////////////////////////////////////////////////////////////////////////////////////////
// New anti-aliased (smoothed) font functions added below
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           loadFont
** Description:             loads parameters from a new font vlw file
*************************************************************************************x*/
void TFT_eSPI::loadFont(String fontName, fs::FS &ffs)
{
  fontFS = ffs;
  loadFont(fontName, false);
}
/***************************************************************************************
** Function name:           loadFont
** Description:             loads parameters from a new font vlw file
*************************************************************************************x*/
void TFT_eSPI::loadFont(String fontName, bool flash)
{
  /*
    The vlw font format does not appear to be documented anywhere, so some reverse
    engineering has been applied!

    Header of vlw file comprises 6 uint32_t parameters (24 bytes total):
      1. The gCount (number of character glyphs)
      2. A version number (0xB = 11 for the one I am using)
      3. The font size (in points, not pixels)
      4. Deprecated mboxY parameter (typically set to 0)
      5. Ascent in pixels from baseline to top of "d"
      6. Descent in pixels from baseline to bottom of "p"

    Next are gCount sets of values for each glyph, each set comprises 7 int32t parameters (28 bytes):
      1. Glyph Unicode stored as a 32 bit value
      2. Height of bitmap bounding box
      3. Width of bitmap bounding box
      4. gxAdvance for cursor (setWidth in Processing)
      5. dY = distance from cursor baseline to top of glyph bitmap (signed value +ve = up)
      6. dX = distance from cursor to left side of glyph bitmap (signed value -ve = left)
      7. padding value, typically 0

    The bitmaps start next at 24 + (28 * gCount) bytes from the start of the file.
    Each pixel is 1 byte, an 8 bit Alpha value which represents the transparency from
    0xFF foreground colour, 0x00 background. The sketch uses a linear interpolation
    between the foreground and background RGB component colours. e.g.
        pixelRed = ((fgRed * alpha) + (bgRed * (255 - alpha))/255
    To gain a performance advantage fixed point arithmetic is used with rounding and
    division by 256 (shift right 8 bits is faster).

    After the bitmaps is:
       1 byte for font name string length (excludes null)
       a zero terminated character string giving the font name
       1 byte for Postscript name string length
       a zero/one terminated character string giving the font name
       last byte is 0 for non-anti-aliased and 1 for anti-aliased (smoothed)

    Then the font name seen by Java when it's created
    Then the postscript name of the font
    Then a boolean to tell if smoothing is on or not.

    Glyph bitmap example is:
    // Cursor coordinate positions for this and next character are marked by 'C'
    // C<------- gxAdvance ------->C  gxAdvance is how far to move cursor for next glyph cursor position
    // |                           |
    // |                           |   ascent is top of "d", descent is bottom of "p"
    // +-- gdX --+             ascent
    // |         +-- gWidth--+     |   gdX is offset to left edge of glyph bitmap
    // |   +     x@.........@x  +  |   gdX may be negative e.g. italic "y" tail extending to left of
    // |   |     @@.........@@  |  |   cursor position, plot top left corner of bitmap at (cursorX + gdX)
    // |   |     @@.........@@ gdY |   gWidth and gHeight are glyph bitmap dimensions
    // |   |     .@@@.....@@@@  |  |
    // | gHeight ....@@@@@..@@  +  +    <-- baseline
    // |   |     ...........@@     |
    // |   |     ...........@@     |   gdY is the offset to the top edge of the bitmap
    // |   |     .@@.......@@. descent plot top edge of bitmap at (cursorY + yAdvance - gdY)
    // |   +     x..@@@@@@@..x     |   x marks the corner pixels of the bitmap
    // |                           |
    // +---------------------------+   yAdvance is y delta for the next line, font size or (ascent + descent)
    //                                  some fonts can overlay in y direction so may need a user adjust value

  */

  spiffs = flash;

  if(spiffs) fontFS = SPIFFS;

  unloadFont();

  // Avoid a crash on the ESP32 if the file does not exist
  if (fontFS.exists("/" + fontName + ".vlw") == false) {
    Serial.println("Font file " + fontName + " not found!");
    return;
  }

  fontFile = fontFS.open( "/" + fontName + ".vlw", "r");

  if(!fontFile) return;

  fontFile.seek(0, fs::SeekSet);

  gFont.gCount   = (uint16_t)readInt32(); // glyph count in file
                             readInt32(); // vlw encoder version - discard
  gFont.yAdvance = (uint16_t)readInt32(); // Font size in points, not pixels
                             readInt32(); // discard
  gFont.ascent   = (uint16_t)readInt32(); // top of "d"
  gFont.descent  = (uint16_t)readInt32(); // bottom of "p"

  // These next gFont values might be updated when the Metrics are fetched
  gFont.maxAscent  = gFont.ascent;   // Determined from metrics
  gFont.maxDescent = gFont.descent;  // Determined from metrics
  gFont.yAdvance   = gFont.ascent + gFont.descent;
  gFont.spaceWidth = gFont.yAdvance / 4;  // Guess at space width

  fontLoaded = true;

  // Fetch the metrics for each glyph
  loadMetrics(gFont.gCount);

  //fontFile.close();
}


/***************************************************************************************
** Function name:           loadMetrics
** Description:             Get the metrics for each glyph and store in RAM
*************************************************************************************x*/
//#define SHOW_ASCENT_DESCENT
void TFT_eSPI::loadMetrics(uint16_t gCount)
{
  uint32_t headerPtr = 24;
  uint32_t bitmapPtr = 24 + gCount * 28;

#if defined (ESP32) && defined (CONFIG_SPIRAM_SUPPORT)
  if ( psramFound() )
  {
    gUnicode  = (uint16_t*)ps_malloc( gCount * 2); // Unicode 16 bit Basic Multilingual Plane (0-FFFF)
    gHeight   =  (uint8_t*)ps_malloc( gCount );    // Height of glyph
    gWidth    =  (uint8_t*)ps_malloc( gCount );    // Width of glyph
    gxAdvance =  (uint8_t*)ps_malloc( gCount );    // xAdvance - to move x cursor
    gdY       =  (int16_t*)ps_malloc( gCount * 2); // offset from bitmap top edge from lowest point in any character
    gdX       =   (int8_t*)ps_malloc( gCount );    // offset for bitmap left edge relative to cursor X
    gBitmap   = (uint32_t*)ps_malloc( gCount * 4); // seek pointer to glyph bitmap in the file
  }
  else
#endif
  {
    gUnicode  = (uint16_t*)malloc( gCount * 2); // Unicode 16 bit Basic Multilingual Plane (0-FFFF)
    gHeight   =  (uint8_t*)malloc( gCount );    // Height of glyph
    gWidth    =  (uint8_t*)malloc( gCount );    // Width of glyph
    gxAdvance =  (uint8_t*)malloc( gCount );    // xAdvance - to move x cursor
    gdY       =  (int16_t*)malloc( gCount * 2); // offset from bitmap top edge from lowest point in any character
    gdX       =   (int8_t*)malloc( gCount );    // offset for bitmap left edge relative to cursor X
    gBitmap   = (uint32_t*)malloc( gCount * 4); // seek pointer to glyph bitmap in the file
  }

#ifdef SHOW_ASCENT_DESCENT
  Serial.print("ascent  = "); Serial.println(gFont.ascent);
  Serial.print("descent = "); Serial.println(gFont.descent);
#endif

  uint16_t gNum = 0;
  fontFile.seek(headerPtr, fs::SeekSet);
  while (gNum < gCount)
  {
    gUnicode[gNum]  = (uint16_t)readInt32(); // Unicode code point value
    gHeight[gNum]   =  (uint8_t)readInt32(); // Height of glyph
    gWidth[gNum]    =  (uint8_t)readInt32(); // Width of glyph
    gxAdvance[gNum] =  (uint8_t)readInt32(); // xAdvance - to move x cursor
    gdY[gNum]       =  (int16_t)readInt32(); // y delta from baseline
    gdX[gNum]       =   (int8_t)readInt32(); // x delta from cursor
    readInt32(); // ignored

    //Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", gHeight  = "); Serial.println(gHeight[gNum]);
    //Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", gWidth  = "); Serial.println(gWidth[gNum]);
    //Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", gxAdvance  = "); Serial.println(gxAdvance[gNum]);
    //Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", gdY  = "); Serial.println(gdY[gNum]);

    // Different glyph sets have different ascent values not always based on "d", so we could get
    // the maximum glyph ascent by checking all characters. BUT this method can generate bad values
    // for non-existant glyphs, so we will reply on processing for the value and disable this code for now...
    /*
    if (gdY[gNum] > gFont.maxAscent)
    {
      // Try to avoid UTF coding values and characters that tend to give duff values
      if (((gUnicode[gNum] > 0x20) && (gUnicode[gNum] < 0x7F)) || (gUnicode[gNum] > 0xA0))
      {
        gFont.maxAscent   = gdY[gNum];
#ifdef SHOW_ASCENT_DESCENT
        Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", maxAscent  = "); Serial.println(gFont.maxAscent);
#endif
      }
    }
    */

    // Different glyph sets have different descent values not always based on "p", so get maximum glyph descent
    if (((int16_t)gHeight[gNum] - (int16_t)gdY[gNum]) > gFont.maxDescent)
    {
      // Avoid UTF coding values and characters that tend to give duff values
      if (((gUnicode[gNum] > 0x20) && (gUnicode[gNum] < 0xA0) && (gUnicode[gNum] != 0x7F)) || (gUnicode[gNum] > 0xFF))
      {
        gFont.maxDescent   = gHeight[gNum] - gdY[gNum];
#ifdef SHOW_ASCENT_DESCENT
        Serial.print("Unicode = 0x"); Serial.print(gUnicode[gNum], HEX); Serial.print(", maxDescent = "); Serial.println(gHeight[gNum] - gdY[gNum]);
#endif
      }
    }

    gBitmap[gNum] = bitmapPtr;

    headerPtr += 28;

    bitmapPtr += gWidth[gNum] * gHeight[gNum];

    gNum++;
    yield();
  }

  gFont.yAdvance = gFont.maxAscent + gFont.maxDescent;

  gFont.spaceWidth = (gFont.ascent + gFont.descent) * 2/7;  // Guess at space width
}


/***************************************************************************************
** Function name:           deleteMetrics
** Description:             Delete the old glyph metrics and free up the memory
*************************************************************************************x*/
void TFT_eSPI::unloadFont( void )
{
  if (gUnicode)
  {
    free(gUnicode);
    gUnicode = NULL;
  }

  if (gHeight)
  {
    free(gHeight);
    gHeight = NULL;
  }

  if (gWidth)
  {
    free(gWidth);
    gWidth = NULL;
  }

  if (gxAdvance)
  {
    free(gxAdvance);
    gxAdvance = NULL;
  }

  if (gdY)
  {
    free(gdY);
    gdY = NULL;
  }

  if (gdX)
  {
    free(gdX);
    gdX = NULL;
  }

  if (gBitmap)
  {
    free(gBitmap);
    gBitmap = NULL;
  }

  if(fontFile) fontFile.close();
  fontLoaded = false;
}


/***************************************************************************************
** Function name:           decodeUTF8
** Description:             Line buffer UTF-8 decoder with fall-back to extended ASCII
*************************************************************************************x*/
/* Function moved to TFT_eSPI.cpp
#define DECODE_UTF8
uint16_t TFT_eSPI::decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining)
{
  byte c = buf[(*index)++];
  //Serial.print("Byte from string = 0x"); Serial.println(c, HEX);

#ifdef DECODE_UTF8
  // 7 bit Unicode
  if ((c & 0x80) == 0x00) return c;

  // 11 bit Unicode
  if (((c & 0xE0) == 0xC0) && (remaining > 1))
    return ((c & 0x1F)<<6) | (buf[(*index)++]&0x3F);

  // 16 bit Unicode
  if (((c & 0xF0) == 0xE0) && (remaining > 2))
  {
    c = ((c & 0x0F)<<12) | ((buf[(*index)++]&0x3F)<<6);
    return  c | ((buf[(*index)++]&0x3F));
  }

  // 21 bit Unicode not supported so fall-back to extended ASCII
  // if ((c & 0xF8) == 0xF0) return c;
#endif

  return c; // fall-back to extended ASCII
}
*/

/***************************************************************************************
** Function name:           decodeUTF8
** Description:             Serial UTF-8 decoder with fall-back to extended ASCII
*************************************************************************************x*/
/* Function moved to TFT_eSPI.cpp
uint16_t TFT_eSPI::decodeUTF8(uint8_t c)
{

#ifdef DECODE_UTF8

  // 7 bit Unicode
  if ((c & 0x80) == 0x00) {
    decoderState = 0;
    return (uint16_t)c;
  }

  if (decoderState == 0)
  {
    // 11 bit Unicode
    if ((c & 0xE0) == 0xC0)
    {
      decoderBuffer = ((c & 0x1F)<<6);
      decoderState = 1;
      return 0;
    }

    // 16 bit Unicode
    if ((c & 0xF0) == 0xE0)
    {
      decoderBuffer = ((c & 0x0F)<<12);
      decoderState = 2;
      return 0;
    }
    // 21 bit Unicode not supported so fall-back to extended ASCII
    if ((c & 0xF8) == 0xF0) return (uint16_t)c;
  }
  else
  {
    if (decoderState == 2)
    {
      decoderBuffer |= ((c & 0x3F)<<6);
      decoderState--;
      return 0;
    }
    else
    {
      decoderBuffer |= (c & 0x3F);
      decoderState = 0;
      return decoderBuffer;
    }
  }
#endif

  decoderState = 0;
  return (uint16_t)c; // fall-back to extended ASCII
}
*/


/***************************************************************************************
** Function name:           alphaBlend
** Description:             Blend foreground and background and return new colour
*************************************************************************************x*/
uint16_t TFT_eSPI::alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc)
{
  // For speed use fixed point maths and rounding to permit a power of 2 division
  uint16_t fgR = ((fgc >> 10) & 0x3E) + 1;
  uint16_t fgG = ((fgc >>  4) & 0x7E) + 1;
  uint16_t fgB = ((fgc <<  1) & 0x3E) + 1;

  uint16_t bgR = ((bgc >> 10) & 0x3E) + 1;
  uint16_t bgG = ((bgc >>  4) & 0x7E) + 1;
  uint16_t bgB = ((bgc <<  1) & 0x3E) + 1;

  // Shift right 1 to drop rounding bit and shift right 8 to divide by 256
  uint16_t r = (((fgR * alpha) + (bgR * (255 - alpha))) >> 9);
  uint16_t g = (((fgG * alpha) + (bgG * (255 - alpha))) >> 9);
  uint16_t b = (((fgB * alpha) + (bgB * (255 - alpha))) >> 9);

  // Combine RGB565 colours into 16 bits
  //return ((r&0x18) << 11) | ((g&0x30) << 5) | ((b&0x18) << 0); // 2 bit greyscale
  //return ((r&0x1E) << 11) | ((g&0x3C) << 5) | ((b&0x1E) << 0); // 4 bit greyscale
  return (r << 11) | (g << 5) | (b << 0);
}


/***************************************************************************************
** Function name:           readInt32
** Description:             Get a 32 bit integer from the font file
*************************************************************************************x*/
uint32_t TFT_eSPI::readInt32(void)
{
  uint32_t val = 0;
  val |= fontFile.read() << 24;
  val |= fontFile.read() << 16;
  val |= fontFile.read() << 8;
  val |= fontFile.read();
  return val;
}


/***************************************************************************************
** Function name:           getUnicodeIndex
** Description:             Get the font file index of a Unicode character
*************************************************************************************x*/
bool TFT_eSPI::getUnicodeIndex(uint16_t unicode, uint16_t *index)
{
  for (uint16_t i = 0; i < gFont.gCount; i++)
  {
    if (gUnicode[i] == unicode)
    {
      *index = i;
      return true;
    }
  }
  return false;
}


/***************************************************************************************
** Function name:           drawGlyph
** Description:             Write a character to the TFT cursor position
*************************************************************************************x*/
// Expects file to be open
void TFT_eSPI::drawGlyph(uint16_t code)
{
  if (code < 0x21)
  {
    if (code == 0x20) {
      cursor_x += gFont.spaceWidth;
      return;
    }

    if (code == '\n') {
      cursor_x = 0;
      cursor_y += gFont.yAdvance;
      if (cursor_y >= _height) cursor_y = 0;
      return;
    }
  }

  uint16_t gNum = 0;
  bool found = getUnicodeIndex(code, &gNum);
  
  uint16_t fg = textcolor;
  uint16_t bg = textbgcolor;

  if (found)
  {

    if (textwrapX && (cursor_x + gWidth[gNum] + gdX[gNum] > _width))
    {
      cursor_y += gFont.yAdvance;
      cursor_x = 0;
    }
    if (textwrapY && ((cursor_y + gFont.yAdvance) >= _height)) cursor_y = 0;
    if (cursor_x == 0) cursor_x -= gdX[gNum];

    fontFile.seek(gBitmap[gNum], fs::SeekSet); // This is taking >30ms for a significant position shift

    uint8_t pbuffer[gWidth[gNum]];

    int16_t xs = 0;
    uint32_t dl = 0;

    int16_t cy = cursor_y + gFont.maxAscent - gdY[gNum];
    int16_t cx = cursor_x + gdX[gNum];

    startWrite(); // Avoid slow ESP32 transaction overhead for every pixel

    for (int y = 0; y < gHeight[gNum]; y++)
    {
      if (spiffs)
      {
        fontFile.read(pbuffer, gWidth[gNum]);
        //Serial.println("SPIFFS");
      }
      else
      {
        endWrite();    // Release SPI for SD card transaction
        fontFile.read(pbuffer, gWidth[gNum]);
        startWrite();  // Re-start SPI for TFT transaction
        //Serial.println("Not SPIFFS");
      }

      for (int x = 0; x < gWidth[gNum]; x++)
      {
        uint8_t pixel = pbuffer[x]; //<//
        if (pixel)
        {
          if (pixel != 0xFF)
          {
            if (dl) {
              if (dl==1) drawPixel(xs, y + cy, fg);
              else drawFastHLine( xs, y + cy, dl, fg);
              dl = 0;
            }
            if (getColor) bg = getColor(x + cx, y + cy);
            drawPixel(x + cx, y + cy, alphaBlend(pixel, fg, bg));
          }
          else
          {
            if (dl==0) xs = x + cx;
            dl++;
          }
        }
        else
        {
          if (dl) { drawFastHLine( xs, y + cy, dl, fg); dl = 0; }
        }
      }
      if (dl) { drawFastHLine( xs, y + cy, dl, fg); dl = 0; }
    }

    cursor_x += gxAdvance[gNum];
    endWrite();
  }
  else
  {
    // Not a Unicode in font so draw a rectangle and move on cursor
    drawRect(cursor_x, cursor_y + gFont.maxAscent - gFont.ascent, gFont.spaceWidth, gFont.ascent, fg);
    cursor_x += gFont.spaceWidth + 1;
  }
}

/***************************************************************************************
** Function name:           showFont
** Description:             Page through all characters in font, td ms between screens
*************************************************************************************x*/
void TFT_eSPI::showFont(uint32_t td)
{
  if(!fontLoaded) return;

  if(!fontFile)
  {
    fontLoaded = false;
    return;
  }

  int16_t cursorX = width(); // Force start of new page to initialise cursor
  int16_t cursorY = height();// for the first character
  uint32_t timeDelay = 0;    // No delay before first page

  fillScreen(textbgcolor);
  
  for (uint16_t i = 0; i < gFont.gCount; i++)
  {
    // Check if this will need a new screen
    if (cursorX + gdX[i] + gWidth[i] >= width())  {
      cursorX = -gdX[i];

      cursorY += gFont.yAdvance;
      if (cursorY + gFont.maxAscent + gFont.descent >= height()) {
        cursorX = -gdX[i];
        cursorY = 0;
        delay(timeDelay);
        timeDelay = td;
        fillScreen(textbgcolor);
      }
    }

    setCursor(cursorX, cursorY);
    drawGlyph(gUnicode[i]);
    cursorX += gxAdvance[i];
    //cursorX +=  printToSprite( cursorX, cursorY, i );
    yield();
  }

  delay(timeDelay);
  fillScreen(textbgcolor);
  //fontFile.close();

}
