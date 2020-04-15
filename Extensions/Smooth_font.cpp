 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the TFT_eSPI class and is associated with anti-aliased font functions
 

////////////////////////////////////////////////////////////////////////////////////////
// New anti-aliased (smoothed) font functions added below
////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************************
** Function name:           loadFont
** Description:             loads parameters from a font vlw array in memory
*************************************************************************************x*/
void TFT_eSPI::loadFont(const uint8_t array[])
{
  loadFont("", false);
}

#ifdef FONT_FS_AVAILABLE
/***************************************************************************************
** Function name:           loadFont
** Description:             loads parameters from a font vlw file
*************************************************************************************x*/
void TFT_eSPI::loadFont(String fontName, fs::FS &ffs)
{
  loadFont(fontName, false);
}
#endif

/***************************************************************************************
** Function name:           loadFont
** Description:             loads parameters from a font vlw file
*************************************************************************************x*/
void TFT_eSPI::loadFont(String fontName, bool flash)
{
  sf->loadFont(fontName, flash);
  fontLoaded = true;
}


/***************************************************************************************
** Function name:           deleteMetrics
** Description:             Delete the old glyph metrics and free up the memory
*************************************************************************************x*/
void TFT_eSPI::unloadFont( void )
{
 fontLoaded = false;
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
      cursor_x += sf->gFont.spaceWidth;
      return;
    }

    if (code == '\n') {
      cursor_x = 0;
      cursor_y += sf->gFont.yAdvance;
      if (cursor_y >= _height) cursor_y = 0;
      return;
    }
  }

  uint16_t gNum = 0;
  bool found = sf->getUnicodeIndex(code, &gNum);
  
  uint16_t fg = textcolor;
  uint16_t bg = textbgcolor;

  if (found)
  {

    if (textwrapX && (cursor_x + sf->gWidth[gNum] + sf->gdX[gNum] > _width))
    {
      cursor_y += sf->gFont.yAdvance;
      cursor_x = 0;
    }
    if (textwrapY && ((cursor_y + sf->gFont.yAdvance) >= _height)) cursor_y = 0;
    if (cursor_x == 0) cursor_x -= sf->gdX[gNum];

    uint8_t* pbuffer = nullptr;
    const uint8_t* gPtr = (const uint8_t*) sf->gFont.gArray;

#ifdef FONT_FS_AVAILABLE
    if (sf->fs_font)
    {
      sf->fontFile.seek(sf->gBitmap[gNum], fs::SeekSet); // This is taking >30ms for a significant position shift
      pbuffer =  (uint8_t*)malloc(sf->gWidth[gNum]);
    }
#endif

    int16_t  xs = 0;
    uint32_t dl = 0;
    uint8_t pixel;

    int16_t cy = cursor_y + sf->gFont.maxAscent - sf->gdY[gNum];
    int16_t cx = cursor_x + sf->gdX[gNum];

    startWrite(); // Avoid slow ESP32 transaction overhead for every pixel

    for (int y = 0; y < sf->gHeight[gNum]; y++)
    {
#ifdef FONT_FS_AVAILABLE
      if (sf->fs_font) {
        if (sf->spiffs)
        {
          sf->fontFile.read(pbuffer, sf->gWidth[gNum]);
          //Serial.println("SPIFFS");
        }
        else
        {
          endWrite();    // Release SPI for SD card transaction
          sf->fontFile.read(pbuffer,sf-> gWidth[gNum]);
          startWrite();  // Re-start SPI for TFT transaction
          //Serial.println("Not SPIFFS");
        }
      }
#endif
      for (int x = 0; x < sf->gWidth[gNum]; x++)
      {
#ifdef FONT_FS_AVAILABLE
        if (sf->fs_font) pixel = pbuffer[x];
        else
#endif
        pixel = pgm_read_byte(gPtr + sf->gBitmap[gNum] + x + sf->gWidth[gNum] * y);

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

    if (pbuffer) free(pbuffer);
    cursor_x += sf->gxAdvance[gNum];
    endWrite();
  }
  else
  {
    // Not a Unicode in font so draw a rectangle and move on cursor
    drawRect(cursor_x, cursor_y + sf->gFont.maxAscent - sf->gFont.ascent, sf->gFont.spaceWidth, sf->gFont.ascent, fg);
    cursor_x += sf->gFont.spaceWidth + 1;
  }
}

/***************************************************************************************
** Function name:           showFont
** Description:             Page through all characters in font, td ms between screens
*************************************************************************************x*/
void TFT_eSPI::showFont(uint32_t td)
{
  if( !fontLoaded || sf == nullptr) return;

  int16_t cursorX = width(); // Force start of new page to initialise cursor
  int16_t cursorY = height();// for the first character
  uint32_t timeDelay = 0;    // No delay before first page

  fillScreen(textbgcolor);
  
  for (uint16_t i = 0; i < sf->gFont.gCount; i++)
  {
    // Check if this will need a new screen
    if (cursorX + sf->gdX[i] + sf->gWidth[i] >= width())  {
      cursorX = -sf->gdX[i];

      cursorY += sf->gFont.yAdvance;
      if (cursorY + sf->gFont.maxAscent + sf->gFont.descent >= height()) {
        cursorX = -sf->gdX[i];
        cursorY = 0;
        delay(timeDelay);
        timeDelay = td;
        fillScreen(textbgcolor);
      }
    }

    setCursor(cursorX, cursorY);
    drawGlyph(sf->gUnicode[i]);
    cursorX += sf->gxAdvance[i];
    //cursorX +=  printToSprite( cursorX, cursorY, i );
    yield();
  }

  delay(timeDelay);
  fillScreen(textbgcolor);
  //fontFile.close();
}
