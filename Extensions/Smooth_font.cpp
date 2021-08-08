 // Coded by Bodmer 10/2/18, see license in root directory.
 // This is part of the TFT_eSPI class and is associated with anti-aliased font functions
 

////////////////////////////////////////////////////////////////////////////////////////
// New anti-aliased (smoothed) font functions added below
////////////////////////////////////////////////////////////////////////////////////////

//syntax highlight enable
#ifndef TFT_ESPI_VERSION
#include "TFT_eSPI.h"
#endif

/***************************************************************************************
** Function name:           loadFont
** Description:             loads parameters from a font vlw array in memory
*************************************************************************************x*/
void TFT_eSPI::loadFont(const uint8_t array[])
{
  if (array == nullptr) return;
  fontPtr = (uint8_t*) array;
  loadFont("", false);
}


void TFT_eSPI::loadFont(const char * partitionName, esp_partition_subtype_t subtype)
{
  const esp_partition_t* fontpart = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, subtype, partitionName);
  if (!fontpart)
  {
    Serial.print("Font partition not found: ");
    Serial.println(partitionName);
    return;
  }
  
  esp_err_t err = spi_flash_mmap(fontpart->address, fontpart->size, SPI_FLASH_MMAP_INST, (const void **)&fontPtr, &font_handle);
//  esp_err_t err = spi_flash_mmap(fontpart->address, 4*1024*1024, SPI_FLASH_MMAP_INST, (const void **)&fontPtr, &font_handle);
  if (err)
  {
    Serial.print("Partition mapping error ");
    Serial.println(err);
    return;
  }

  loadFont("", false);
}


#ifdef FONT_FS_AVAILABLE
/***************************************************************************************
** Function name:           loadFont
** Description:             loads parameters from a font vlw file
*************************************************************************************x*/
void TFT_eSPI::loadFont(String fontName, fs::FS &ffs)
{
  fontFS = ffs;
  loadFont(fontName, false);
}
#endif


/***************************************************************************************
** Function name:           loadFont
** Description:             loads parameters from a font vlw file
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
    //                                 some fonts can overlay in y direction so may need a user adjust value

  */

  if (fontLoaded) unloadFont();

#ifdef FONT_FS_AVAILABLE
  if (fontName == "") fs_font = false;
  else { fontPtr = nullptr; fs_font = true; }

  if (fs_font) {
    spiffs = flash; // true if font is in SPIFFS

    if(spiffs) fontFS = SPIFFS;

    // Avoid a crash on the ESP32 if the file does not exist
    if (fontFS.exists("/" + fontName + ".vlw") == false) {
      Serial.println("Font file " + fontName + " not found!");
      return;
    }

    fontFile = fontFS.open( "/" + fontName + ".vlw", "r");

    if(!fontFile) return;

    fontFile.seek(0, fs::SeekSet);
  }
#else
  // Avoid unused varaible warning
  fontName = fontName;
  flash = flash;
#endif

  gFont.gArray   = (const uint8_t*)fontPtr;

  gFont.gCount   = (uint16_t)readInt32(); // glyph count in file
                             readInt32(); // vlw encoder version - discard
  gFont.yAdvance = (uint16_t)readInt32(); // Font size in points, not pixels
  gFont.maxDescent = (int32_t)readInt32();
  gFont.ascent   = (uint16_t)readInt32(); // top of "d"
  gFont.descent  = (uint16_t)readInt32(); // bottom of "p"

  // These next gFont values might be updated when the Metrics are fetched
  gFont.maxAscent  = gFont.ascent;   // Determined from metrics
  gFont.yAdvance   = gFont.ascent + gFont.descent;

    yield();

    gFont.yAdvance = gFont.maxAscent + gFont.maxDescent;
    gFont.spaceWidth = (gFont.ascent + gFont.descent) * 2/7;  // Guess at space width

  fontLoaded = true;
}


static TFT_eSPI::CharMetrics _cm;

TFT_eSPI::CharMetrics * TFT_eSPI::getCharMetrics(uint16_t gNum)
{
  CharMetrics * cm = &_cm;

  fontPtr = (uint8_t *)&gFont.gArray[24 + gNum * 28];
  cm->gUnicode  = (uint16_t)readInt32(); // Unicode code point value
  cm->gHeight   =  (uint8_t)readInt32(); // Height of glyph
  cm->gWidth    =  (uint8_t)readInt32(); // Width of glyph
  cm->gxAdvance =  (uint8_t)readInt32(); // xAdvance - to move x cursor
  cm->gdY       =  (int16_t)readInt32(); // y delta from baseline
  cm->gdX       =   (int8_t)readInt32(); // x delta from cursor
  cm->gBitmap   = 24 + gFont.gCount*28 + readInt32();

  return cm;
}


/***************************************************************************************
** Function name:           deleteMetrics
** Description:             Delete the old glyph metrics and free up the memory
*************************************************************************************x*/
void TFT_eSPI::unloadFont( void )
{
  gFont.gArray = nullptr;

#ifdef FONT_FS_AVAILABLE
  if (fs_font && fontFile) fontFile.close();
#endif

  if (font_handle)
  {
    spi_flash_munmap(font_handle);
    font_handle = 0;
  }

  fontLoaded = false;
}


/***************************************************************************************
** Function name:           readInt32
** Description:             Get a 32 bit integer from the font file
*************************************************************************************x*/
uint32_t TFT_eSPI::readInt32(void)
{
  uint32_t val;

#ifdef FONT_FS_AVAILABLE
  if (fs_font) 
  {
    fontFile.seek((uint32_t)fontPtr, fs::SeekSet);
    val |= fontFile.read();
    val |= fontFile.read() << 8;
    val |= fontFile.read() << 16;
    val |= fontFile.read() << 24;
  }
  else
#endif
  {
    val = pgm_read_dword(fontPtr) & 0xFFFFFFFFul;
  }

  fontPtr += 4;
  return val;
}


/***************************************************************************************
** Function name:           getUnicodeIndex
** Description:             Get the font file index of a Unicode character
*************************************************************************************x*/

#ifdef FAST_UNICODE_INDEX
bool TFT_eSPI::getUnicodeIndex(uint16_t unicode, uint16_t *index)
{
  int above = gFont.gCount;
  int bottom = 0;
  int i = above / 2;
  do
  {
    fontPtr = (uint8_t *)&gFont.gArray[24 + i * 28];
    uint32_t code = readInt32(); // Unicode code point value. 32bit to prevent compiler optimisation leading to unaligned access (16 bit)
    if (code == unicode)
    {
      *index = i;
      return true;
    }
    if (code > unicode)
    {
      above = i;
    }
    else
    {
      bottom = i+1;
    }
    i = (above+bottom)/2;
  } while (above > bottom);

  return false;
}

#else

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

#endif

//--------------------------------------------------------------------------------------
void * memcpy_I(void * dst, const void * src, int len)
{
//  return memcpy(dst, src, len);

  if (!len) return dst;

  uint32_t d;
  uint32_t dst1 = (uint32_t)dst;
  uint32_t src1 = (uint32_t)src;

  if (src1 & 3)
  {
    d = *(uint32_t*)(src1 & ~3);
    d >>= 8 * (src1 & 3);
    while (src1 & 3)
    {
      *(uint8_t *)dst1 = d & 0xFF;
      d >>= 8;
      src1++;
      dst1++;
      len--;
      if (!len) return dst;
    }
  }

  while (len > 3)
  {
    d = *(uint32_t *)src1;
    *(uint32_t *)dst1 = d;
    src1 += 4;
    dst1 += 4;
    len -= 4;
  }

  if (!len) return dst;

  d = *(uint32_t *)src1;
  while (len)
  {
    *(uint8_t *)dst1 = d & 0xFF;
    d >>= 8;
    src1++;
    dst1++;
    len--;
  }

  return dst;
}


/***************************************************************************************
** Function name:           drawGlyph
** Description:             Write a character to the TFT cursor position
*************************************************************************************x*/
// Expects file to be open
void TFT_eSPI::drawGlyph(uint16_t code)
{
  uint16_t fg = textcolor;
  uint16_t bg = textbgcolor;

  if (code < 0x21)
  {
    if (code == 0x20) {
      //if (fg!=bg) fillRect(cursor_x, cursor_y, gFont.spaceWidth, gFont.yAdvance, bg);
      cursor_x += gFont.spaceWidth;
      return;
    }

    if (code == '\n') {
      cursor_x = 0;
      cursor_y += gFont.yAdvance;
      if (textwrapY && (cursor_y >= height())) cursor_y = 0;
      return;
    }
  }

  uint16_t gNum = 0;
  bool found = getUnicodeIndex(code, &gNum);
  
  if (found)
  {
    CharMetrics * cm = getCharMetrics(gNum);
    if (textwrapX && (cursor_x + cm->gWidth + cm->gdX > width()))
    {
      cursor_y += gFont.yAdvance;
      cursor_x = 0;
    }
    if (textwrapY && ((cursor_y + gFont.yAdvance) >= height())) cursor_y = 0;
    if (cursor_x == 0) cursor_x -= cm->gdX;

    uint8_t* pbuffer = glyph_line_buffer;//(uint8_t*)malloc(cm->gWidth);
    const uint8_t* gPtr = (const uint8_t*) gFont.gArray + cm->gBitmap;

#ifdef FONT_FS_AVAILABLE
    if (fs_font)
    {
      fontFile.seek(cm->gBitmap, fs::SeekSet); // This is taking >30ms for a significant position shift
    }
#endif

    int16_t cy = cursor_y + gFont.maxAscent - cm->gdY;
    int16_t cx = cursor_x + cm->gdX;

    int16_t  xs = cx;
    uint32_t dl = 0;
    uint8_t pixel;

    startWrite(); // Avoid slow ESP32 transaction overhead for every pixel

    //if (fg!=bg) fillRect(cursor_x, cursor_y, gxAdvance[gNum], gFont.yAdvance, bg);

    for (int y = 0; y < cm->gHeight; y++)
    {
#ifdef FONT_FS_AVAILABLE
      if (fs_font) {
        if (spiffs)
        {
          fontFile.read(pbuffer, cm->gWidth);
          //Serial.println("SPIFFS");
        }
        else
        {
          endWrite();    // Release SPI for SD card transaction
          fontFile.read(pbuffer, cm->gWidth);
          startWrite();  // Re-start SPI for TFT transaction
          //Serial.println("Not SPIFFS");
        }
      }
      else
#endif
      {
        memcpy_I(pbuffer, gPtr, cm->gWidth);
        gPtr += cm->gWidth;
      }

      for (int x = 0; x < cm->gWidth; x++)
      {
        pixel = pbuffer[x];

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

    //if (pbuffer) free(pbuffer);
    cursor_x += cm->gxAdvance;
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

  int16_t cursorX = width(); // Force start of new page to initialise cursor
  int16_t cursorY = height();// for the first character
  uint32_t timeDelay = 0;    // No delay before first page

  fillScreen(textbgcolor);
  
  for (uint16_t i = 0; i < gFont.gCount; i++)
  {
    // Check if this will need a new screen
    CharMetrics * cm = getCharMetrics(i);
    if (cursorX + cm->gdX + cm->gWidth >= width())  {
      cursorX = -cm->gdX;

      cursorY += gFont.yAdvance;
      if (cursorY + gFont.maxAscent + gFont.descent >= height()) {
        cursorX = -cm->gdX;
        cursorY = 0;
        delay(timeDelay);
        timeDelay = td;
        fillScreen(textbgcolor);
      }
    }

    setCursor(cursorX, cursorY);
    drawGlyph(cm->gUnicode);
    cursorX += cm->gxAdvance;
    //cursorX +=  printToSprite( cursorX, cursorY, i );
    yield();
  }

  delay(timeDelay);
  fillScreen(textbgcolor);
  //fontFile.close();

}
