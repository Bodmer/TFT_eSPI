/**************************************************************************************
// The following class creates Sprites in RAM, graphics can then be drawn in the Sprite
// and rendered quickly onto the TFT screen. The class inherits the graphics functions
// from the TFT_eSPI class. Some functions are overridden by this class so that the
// graphics are written to the Sprite rather than the TFT.
// Coded by Bodmer, see license file in root folder
***************************************************************************************/
/***************************************************************************************
// Color bytes are swapped when writing to RAM, this introduces a small overhead but
// there is a nett performance gain by using swapped bytes.
***************************************************************************************/

/***************************************************************************************
** Function name:           TFT_eSprite
** Description:             Class constructor
*************************************************************************************x*/
TFT_eSprite::TFT_eSprite(TFT_eSPI *tft)
{
  _tft = tft;     // Pointer to tft class so we can call member functions

  _iwidth    = 0; // Initialise width and height to 0 (it does not exist yet)
  _iheight   = 0;
  _bpp16 = true;
  _iswapBytes = false;   // Do not swap pushImage colour bytes by default

  _created = false;

  _xs = 0;  // window bounds for pushColor
  _ys = 0;
  _xe = 0;
  _ye = 0;

  _xptr = 0; // pushColor coordinate
  _yptr = 0;

  _icursor_y  = _icursor_x    = 0; // Text cursor position
}


/***************************************************************************************
** Function name:           createSprite
** Description:             Create a sprite (bitmap) of defined width and height
*************************************************************************************x*/
// cast returned value to (uint8_t*) for 8 bit or (uint16_t*) for 16 bit colours
void* TFT_eSprite::createSprite(int16_t w, int16_t h)
{

  if ( _created )
  {
    if ( _bpp16 ) return _img;
    return _img8;
  }

  if ( w < 1 || h < 1 ) return NULL;

  _iwidth    = w;
  _iheight   = h;

  _icursor_x = 0;
  _icursor_y = 0;

  // Default scroll rectangle and gap fill colour
  _sx = 0;
  _sy = 0;
  _sw = w;
  _sh = h;
  _scolor = TFT_BLACK;

  // Add one extra "off screen" pixel to point out-of-bounds setWindow() coordinates
  // this means push/writeColor functions do not need additional bounds checks and
  // hence will run faster in normal circumstances.
  if(_bpp16)
  {
    _img = (uint16_t*) calloc(w * h + 1, sizeof(uint16_t));
    if (_img)
    {
      _created = true;
      return _img;
    }
  }
  else
  {
    _img8 = ( uint8_t*) calloc(w * h + 1, sizeof(uint8_t));
    if (_img8)
    {
      _created = true;
      return _img8;
    }
  }
  
  return NULL;
}


/***************************************************************************************
** Function name:           setDepth
** Description:             Set bits per pixel for colour (8 or 16)
*************************************************************************************x*/

void* TFT_eSprite::setColorDepth(int8_t b)
{
  // Can't change an existing sprite's colour depth so delete it
  if (_created)
  {
    if (_bpp16) free(_img);
    else        free(_img8);
  }

  // Now define the new colour depth
  if ( b > 8 ) _bpp16 = true;  // Bytes per pixel
  else         _bpp16 = false;

  // If it existed, re-create the sprite with the new colour depth
  if (_created)
  {
    _created = false;
    return createSprite(_iwidth, _iheight);
  }

  return NULL;
}


/***************************************************************************************
** Function name:           deleteSprite
** Description:             Delete the sprite to free up memory (RAM)
*************************************************************************************x*/
void TFT_eSprite::deleteSprite(void)
{
  if (!_created ) return;

  if (_bpp16) free(_img);
  else        free(_img8);

  _created = false;
}


/***************************************************************************************
** Function name:           pushSprite
** Description:             Push the sprite to the TFT at x, y
*************************************************************************************x*/
void TFT_eSprite::pushSprite(int32_t x, int32_t y)
{
  if (!_created ) return;

  if (_bpp16) _tft->pushImage(x, y, _iwidth, _iheight, _img );
  //if (_bpp16) TFT_eSPI::pushImage(x, y, _iwidth, _iheight, _img );
  else        _tft->pushImage(x, y, _iwidth, _iheight, _img8);
}


/***************************************************************************************
** Function name:           pushSprite
** Description:             Push the sprite to the TFT at x, y with transparent colour
*************************************************************************************x*/
void TFT_eSprite::pushSprite(int32_t x, int32_t y, uint16_t transp)
{
  if (!_created ) return;

  if (_bpp16) _tft->pushImage(x, y, _iwidth, _iheight, _img, transp );
  else
  {
    transp = (uint8_t)((transp & 0xE000)>>8 | (transp & 0x0700)>>6 | (transp & 0x0018)>>3);
    _tft->pushImage(x, y, _iwidth, _iheight, _img8, (uint8_t)transp);
  }
}


/***************************************************************************************
** Function name:           readPixel
** Description:             Read 565 colour of a pixel at defined coordinates
*************************************************************************************x*/
uint16_t TFT_eSprite::readPixel(int32_t x, int32_t y)
{
  if (!_created ) return 0;

  if (_bpp16)
  {
    uint16_t color = _img[x + y * _iwidth];
    return (color >> 8) | (color << 8);
  }

  uint16_t color = _img8[x + y * _iwidth];
  if (color != 0)
  {
    uint8_t  blue[] = {0, 11, 21, 31};
    color =   (color & 0xE0)<<8 | (color & 0xC0)<<5
            | (color & 0x1C)<<6 | (color & 0x1C)<<3
            | blue[color & 0x03];
  }

  return color;
}


/***************************************************************************************
** Function name:           pushImage
** Description:             push 565 colour image into a defined area of a sprite 
*************************************************************************************x*/
void  TFT_eSprite::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, uint16_t *data)
{
  if ((x > _iwidth) || (y > _iheight) || (w == 0) || (h == 0) || !_created) return;

  if (_bpp16)
  {
    for (uint32_t yp = y; yp < y + h; yp++)
    {
      for (uint32_t xp = x; xp < x + w; xp++)
      {
        uint16_t color =  *data++;
        if(!_iswapBytes) color = color<<8 | color>>8;
        _img[xp + yp * _iwidth] = color;
      }
    }
  }
  else
  {
    for (uint32_t yp = y; yp < y + h; yp++)
    {
      for (uint32_t xp = x; xp < x + w; xp++)
      {
        uint16_t color = *data++;
        if(_iswapBytes) color = color<<8 | color>>8;
        _img8[xp + yp * _iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
      }
    }
  }
}


/***************************************************************************************
** Function name:           pushImage
** Description:             push 565 colour FLASH (PROGMEM) image into a defined area
*************************************************************************************x*/
void  TFT_eSprite::pushImage(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint16_t *data)
{
  if ((x > _iwidth) || (y > _iheight) || (w == 0) || (h == 0) || !_created) return;

  if (_bpp16)
  {
    for (uint32_t yp = y; yp < y + h; yp++)
    {
      for (uint32_t xp = x; xp < x + w; xp++)
      {
        uint16_t color = pgm_read_word(data++);
        if(!_iswapBytes) color = color<<8 | color>>8;
        _img[xp + yp * _iwidth] = color;
      }
    }
  }
  else
  {
    for (uint32_t yp = y; yp < y + h; yp++)
    {
      for (uint32_t xp = x; xp < x + w; xp++)
      {
        uint16_t color = pgm_read_word(data++);
        if(_iswapBytes) color = color<<8 | color>>8;
        _img8[xp + yp * _iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
      }
    }
  }
}


/***************************************************************************************
** Function name:           setSwapBytes
** Description:             Used by 16 bit pushImage() to swap byte order in colours
***************************************************************************************/
void TFT_eSprite::setSwapBytes(bool swap)
{
  _iswapBytes = swap;
}


/***************************************************************************************
** Function name:           getSwapBytes
** Description:             Return the swap byte order for colours
***************************************************************************************/
bool TFT_eSprite::getSwapBytes(void)
{
  return _iswapBytes;
}


/***************************************************************************************
** Function name:           setWindow
** Description:             Set the bounds of a window for pushColor and writeColor
*************************************************************************************x*/
void TFT_eSprite::setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
  bool duff_coord = false;

  if (x0 > x1) swap_coord(x0, x1);
  if (y0 > y1) swap_coord(y0, y1);

  if (x0 < 0) x0 = 0;
  if (x0 >= _iwidth) duff_coord = true;
  if (x1 < 0) x1 = 0;
  if (x1 >= _iwidth) x1 = _iwidth - 1;

  if (y0 < 0) y0 = 0;
  if (y0 >= _iheight) duff_coord = true;
  if (y1 < 0) y1 = 0;
  if (y1 >= _iheight) y1 = _iheight - 1;

  if (duff_coord)
  { // Point to that extra "off screen" pixel
    _xs = 0;
    _ys = _iheight;
    _xe = 0;
    _ye = _iheight;
  }
  else
  {
    _xs = x0;
    _ys = y0;
    _xe = x1;
    _ye = y1;
  }

  _xptr = _xs;
  _yptr = _ys;
}


/***************************************************************************************
** Function name:           pushColor
** Description:             Send a new pixel to the set window
*************************************************************************************x*/
void TFT_eSprite::pushColor(uint32_t color)
{
  if (!_created ) return;

  // Write the colour to RAM in set window
  if (_bpp16)
    _img [_xptr + _yptr * _iwidth] = (uint16_t) (color >> 8) | (color << 8);

  else
    _img8[_xptr + _yptr * _iwidth] = (uint8_t )((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);

  // Increment x
  _xptr++;

  // Wrap on x and y to start, increment y if needed
  if (_xptr > _xe)
  {
    _xptr = _xs;
    _yptr++;
    if (_yptr > _ye) _yptr = _ys;
  }

}


/***************************************************************************************
** Function name:           pushColor
** Description:             Send a "len" new pixels to the set window
*************************************************************************************x*/
void TFT_eSprite::pushColor(uint32_t color, uint16_t len)
{
  if (!_created ) return;

  uint16_t pixelColor;
  if (_bpp16)
    pixelColor = (uint16_t) (color >> 8) | (color << 8);

  else
    pixelColor = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;

  while(len--) writeColor(pixelColor);
}


/***************************************************************************************
** Function name:           writeColor
** Description:             Write a pixel with pre-formatted colour to the set window
*************************************************************************************x*/
void TFT_eSprite::writeColor(uint16_t color)
{
  if (!_created ) return;

  // Write 16 bit RGB 565 encoded colour to RAM
  if (_bpp16) _img [_xptr + _yptr * _iwidth] = color;

  // Write 8 bit RGB 332 encoded colour to RAM
  else _img8[_xptr + _yptr * _iwidth] = (uint8_t) color;

  // Increment x
  _xptr++;

  // Wrap on x and y to start, increment y if needed
  if (_xptr > _xe)
  {
    _xptr = _xs;
    _yptr++;
    if (_yptr > _ye) _yptr = _ys;
  }
}


/***************************************************************************************
** Function name:           setScrollRect
** Description:             Set scroll area within the sprite and the gap fill colour
*************************************************************************************x*/
void TFT_eSprite::setScrollRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint16_t color)
{
  if ((x >= _iwidth) || (y >= _iheight) || !_created ) return;

  if (x < 0) x = 0;
  if (y < 0) y = 0;

  if ((x + w) > _iwidth ) w = _iwidth  - x;
  if ((y + h) > _iheight) h = _iheight - y;

  if ( w < 1 || h < 1) return;  

  _sx = x;
  _sy = y;
  _sw = w;
  _sh = h;

  _scolor = color;
}


/***************************************************************************************
** Function name:           scroll
** Description:             Scroll dx,dy pixels, positive right,down, negative left,up
*************************************************************************************x*/
void TFT_eSprite::scroll(int16_t dx, int16_t dy)
{
  if (abs(dx) >= _sw || abs(dy) >= _sh)
  {
    fillRect (_sx, _sy, _sw, _sh, _scolor);
    return;
  }

  // Fetch the scroll area width and height set by setScrollRect()
  uint32_t w  = _sw - abs(dx); // line width to copy
  uint32_t h  = _sh - abs(dy); // lines to copy
  int32_t iw  = _iwidth;       // width of sprite

  // Fetch the x,y origin set by setScrollRect()
  uint32_t tx = _sx; // to x
  uint32_t fx = _sx; // from x
  uint32_t ty = _sy; // to y
  uint32_t fy = _sy; // from y

  // Adjust for x delta
  if (dx <= 0) fx -= dx;
  else tx += dx;

  // Adjust for y delta
  if (dy <= 0) fy -= dy;
  else
  { // Scrolling down so start copy from bottom
    ty = ty + _sh - 1; // "To" pointer
    iw = -iw;          // Pointer moves backwards
    fy = ty - dy;      // "From" pointer
  }

  // Calculate "from y" and "to y" pointers in RAM
  uint32_t fyp = fx + fy * _iwidth;
  uint32_t typ = tx + ty * _iwidth;

  // Now move the pixels in RAM
  if (_bpp16)
  {
    while (h--)
    { // move pixel lines (to, from, byte count)
      memmove( _img + typ, _img + fyp, w<<1);
      typ += iw;
      fyp += iw;
    }
  }
  else
  {
    while (h--)
    { // move pixel lines (to, from, byte count)
      memmove( _img8 + typ, _img8 + fyp, w);
      typ += iw;
      fyp += iw;
    }
  }

  // Fill the gap left by the scrolling
  if (dx > 0) fillRect(_sx, _sy, dx, _sh, _scolor);
  if (dx < 0) fillRect(_sx + _sw + dx, _sy, -dx, _sh, _scolor);
  if (dy > 0) fillRect(_sx, _sy, _sw, dy, _scolor);
  if (dy < 0) fillRect(_sx, _sy + _sh + dy, _sw, -dy, _scolor);
}


/***************************************************************************************
** Function name:           fillSprite
** Description:             Fill the whole sprite with defined colour
*************************************************************************************x*/
void TFT_eSprite::fillSprite(uint32_t color)
{
  if (!_created ) return;

  // Use memset if possible as it is super fast
  if(( (uint8_t)color == (uint8_t)(color>>8) ) && _bpp16)
                    memset(_img,  (uint8_t)color, _iwidth * _iheight * 2);
  else if (!_bpp16)
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    memset(_img8, (uint8_t)color, _iwidth * _iheight);
  }

  else fillRect(0, 0, _iwidth, _iheight, color);
}


/***************************************************************************************
** Function name:           setCursor
** Description:             Set the sprite text cursor x,y position
*************************************************************************************x*/
void TFT_eSprite::setCursor(int16_t x, int16_t y)
{
  _icursor_x = x;
  _icursor_y = y;
}


/***************************************************************************************
** Function name:           width
** Description:             Return the width of sprite
*************************************************************************************x*/
// Return the size of the display
int16_t TFT_eSprite::width(void)
{
  if (!_created ) return 0;
  return _iwidth;
}


/***************************************************************************************
** Function name:           height
** Description:             Return the height of sprite
*************************************************************************************x*/
int16_t TFT_eSprite::height(void)
{
  if (!_created ) return 0;
  return _iheight;
}


/***************************************************************************************
** Function name:           drawPixel
** Description:             push a single pixel at an arbitrary position
*************************************************************************************x*/
void TFT_eSprite::drawPixel(uint32_t x, uint32_t y, uint32_t color)
{
  // x and y are unsigned so that -ve coordinates turn into large positive ones
  // this make bounds checking a bit faster
  if ((x >= _iwidth) || (y >= _iheight) || !_created) return;

  if (_bpp16)
  {
    color = (color >> 8) | (color << 8);
    _img[x+y*_iwidth] = (uint16_t) color;
  }
  else
  {
    _img8[x+y*_iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
  }
}


/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
*************************************************************************************x*/
void TFT_eSprite::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
  if (!_created ) return;

  boolean steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap_coord(x0, y0);
    swap_coord(x1, y1);
  }

  if (x0 > x1) {
    swap_coord(x0, x1);
    swap_coord(y0, y1);
  }

  int32_t dx = x1 - x0, dy = abs(y1 - y0);;

  int32_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

  if (y0 < y1) ystep = 1;

  // Split into steep and not steep for FastH/V separation
  if (steep) {
    for (; x0 <= x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;
        if (dlen == 1) drawPixel(y0, xs, color);
        else drawFastVLine(y0, xs, dlen, color);
        dlen = 0; y0 += ystep; xs = x0 + 1;
      }
    }
    if (dlen) drawFastVLine(y0, xs, dlen, color);
  }
  else
  {
    for (; x0 <= x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;
        if (dlen == 1) drawPixel(xs, y0, color);
        else drawFastHLine(xs, y0, dlen, color);
        dlen = 0; y0 += ystep; xs = x0 + 1;
      }
    }
    if (dlen) drawFastHLine(xs, y0, dlen, color);
  }
}


/***************************************************************************************
** Function name:           drawFastVLine
** Description:             draw a vertical line
*************************************************************************************x*/
void TFT_eSprite::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color)
{

  if ((x < 0) || (x >= _iwidth) || (y >= _iheight) || !_created) return;

  if (y < 0) { h += y; y = 0; }

  if ((y + h) > _iheight) h = _iheight - y;

  if (h < 1) return;

  if (_bpp16)
  {
    color = (color >> 8) | (color << 8);
    int32_t yp = x + _iwidth * y;
    while (h--) {_img[yp] = (uint16_t) color; yp += _iwidth;}
  }
  else
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    while (h--) _img8[x + _iwidth * y++] = (uint8_t) color;
  }
}


/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
*************************************************************************************x*/
void TFT_eSprite::drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color)
{

  if ((y < 0) || (x >= _iwidth) || (y >= _iheight) || !_created) return;

  if (x < 0) { w += x; x = 0; }

  if ((x + w) > _iwidth)  w = _iwidth  - x;

  if (w < 1) return;

  if (_bpp16)
  {
    color = (color >> 8) | (color << 8);
    while (w--) _img[_iwidth * y + x++] = (uint16_t) color;
  }
  else
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    memset(_img8+_iwidth * y + x, (uint8_t)color, w);
  }
}


/***************************************************************************************
** Function name:           fillRect
** Description:             draw a filled rectangle
*************************************************************************************x*/
void TFT_eSprite::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
  if (!_created ) return;

  if (x < 0) { w += x; x = 0; }

  if ((x < 0) || (y < 0) || (x >= _iwidth) || (y >= _iheight)) return;
  if ((x + w) > _iwidth)  w = _iwidth  - x;
  if ((y + h) > _iheight) h = _iheight - y;
  if ((w < 1) || (h < 1)) return;

  int32_t yp = _iwidth * y + x;

  if (_bpp16)
  {
    color = (color >> 8) | (color << 8);
    uint32_t iw = w;
    int32_t ys = yp;
    if(h--)  {while (iw--) _img[yp++] = (uint16_t) color;}
    yp = ys;
    while (h--)
    {
      yp += _iwidth;
      memcpy( _img+yp, _img+ys, w<<1);
    }
  }
  else
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    while (h--)
    {
	  memset(_img8 + yp, (uint8_t)color, w);
      yp += _iwidth;
    }
  }
}


/***************************************************************************************
** Function name:           write
** Description:             draw characters piped through serial stream
*************************************************************************************x*/
size_t TFT_eSprite::write(uint8_t utf8)
{
  if (utf8 == '\r') return 1;

#ifdef SMOOTH_FONT
  if(fontLoaded)
  {
    uint16_t unicode = decodeUTF8(utf8);
    if (unicode < 32 && utf8 != '\n') return 0;

    fontFile = SPIFFS.open( _gFontFilename, "r" );

    if(!fontFile)
    {
      fontLoaded = false;
      return 0;
    }

    drawGlyph(unicode);
    fontFile.close();
    return 0;
  }
#endif

  if (!_created ) return 0;


  uint8_t uniCode = utf8;        // Work with a copy
  if (utf8 == '\n') uniCode+=22; // Make it a valid space character to stop errors
  else if (utf8 < 32) return 0;

  uint16_t width = 0;
  uint16_t height = 0;

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  //Serial.print((uint8_t) uniCode); // Debug line sends all printed TFT text to serial port
  //Serial.println(uniCode, HEX); // Debug line sends all printed TFT text to serial port
  //delay(5);                     // Debug optional wait for serial port to flush through
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  if(!gfxFont) {
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef LOAD_FONT2
  if (textfont == 2)
  {
    if (utf8 > 127) return 0;
    // This is 20us faster than using the fontdata structure (0.443ms per character instead of 0.465ms)
    width = pgm_read_byte(widtbl_f16 + uniCode-32);
    height = chr_hgt_f16;
    // Font 2 is rendered in whole byte widths so we must allow for this
    width = (width + 6) / 8;  // Width in whole bytes for font 2, should be + 7 but must allow for font width change
    width = width * 8;        // Width converted back to pixles
  }
  #ifdef LOAD_RLE
  else
  #endif
#endif

#ifdef LOAD_RLE
  {
    if ((textfont>2) && (textfont<9))
    {
      if (utf8 > 127) return 0;
      // Uses the fontinfo struct array to avoid lots of 'if' or 'switch' statements
      // A tad slower than above but this is not significant and is more convenient for the RLE fonts
      width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[textfont].widthtbl ) ) + uniCode-32 );
      height= pgm_read_byte( &fontdata[textfont].height );
    }
  }
#endif

#ifdef LOAD_GLCD
  if (textfont==1)
  {
      width =  6;
      height = 8;
  }
#else
  if (textfont==1) return 0;
#endif

  height = height * textsize;

  if (utf8 == '\n') 
  {
    _icursor_y += height;
    _icursor_x  = 0;
  }
  else
  {
    if (textwrapX && (_icursor_x + width * textsize > _iwidth))
    {
      _icursor_y += height;
      _icursor_x = 0;
    }
    if (textwrapY && (_icursor_y >= _iheight)) _icursor_y = 0;
    _icursor_x += drawChar(uniCode, _icursor_x, _icursor_y, textfont);
  }

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  } // Custom GFX font
  else
  {

    if(utf8 == '\n') {
      _icursor_x  = 0;
      _icursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else {
      if (uniCode > (uint8_t)pgm_read_byte(&gfxFont->last )) return 0;
      if (uniCode < (uint8_t)pgm_read_byte(&gfxFont->first)) return 0;

      uint8_t   c2    = uniCode - pgm_read_byte(&gfxFont->first);
      GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
      uint8_t   w     = pgm_read_byte(&glyph->width),
                h     = pgm_read_byte(&glyph->height);
      if((w > 0) && (h > 0)) { // Is there an associated bitmap?
        int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);
        if(textwrapX && ((_icursor_x + textsize * (xo + w)) > _iwidth)) {
          // Drawing character would go off right edge; wrap to new line
          _icursor_x  = 0;
          _icursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        }
        if (textwrapY && (_icursor_y >= _iheight)) _icursor_y = 0;
        drawChar(_icursor_x, _icursor_y, uniCode, textcolor, textbgcolor, textsize);
      }
      _icursor_x += pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
    }
  }
#endif // LOAD_GFXFF
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  return 1;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a single character in the Adafruit GLCD or freefont
*************************************************************************************x*/
void TFT_eSprite::drawChar(int32_t x, int32_t y, unsigned char c, uint32_t color, uint32_t bg, uint8_t size)
{
  if (!_created ) return;

  if ((x >= _iwidth)            || // Clip right
      (y >= _iheight)           || // Clip bottom
      ((x + 6 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
    return;

#ifdef LOAD_GLCD
//>>>>>>>>>>>>>>>>>>
#ifdef LOAD_GFXFF
  if(!gfxFont) { // 'Classic' built-in font
#endif
//>>>>>>>>>>>>>>>>>>

  boolean fillbg = (bg != color);

  if ((size==1) && fillbg)
  {
    uint8_t column[6];
    uint8_t mask = 0x1;

    for (int8_t i = 0; i < 5; i++ ) column[i] = pgm_read_byte(font + (c * 5) + i);
    column[5] = 0;

    int8_t j, k;
    for (j = 0; j < 8; j++) {
      for (k = 0; k < 5; k++ ) {
        if (column[k] & mask) {
          drawPixel(x + k, y + j, color);
        }
        else {
          drawPixel(x + k, y + j, bg);
        }
      }

      mask <<= 1;

      drawPixel(x + k, y + j, bg);
    }
  }
  else
  {
    for (int8_t i = 0; i < 6; i++ ) {
      uint8_t line;
      if (i == 5)
        line = 0x0;
      else
        line = pgm_read_byte(font + (c * 5) + i);

      if (size == 1) // default size
      {
        for (int8_t j = 0; j < 8; j++) {
          if (line & 0x1) drawPixel(x + i, y + j, color);
          line >>= 1;
        }
      }
      else {  // big size
        for (int8_t j = 0; j < 8; j++) {
          if (line & 0x1) fillRect(x + (i * size), y + (j * size), size, size, color);
          else if (fillbg) fillRect(x + i * size, y + j * size, size, size, bg);
          line >>= 1;
        }
      }
    }
  }

//>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LOAD_GFXFF
  } else { // Custom font
#endif
//>>>>>>>>>>>>>>>>>>>>>>>>>>>
#endif // LOAD_GLCD

#ifdef LOAD_GFXFF
    // Filter out bad characters not present in font
    if ((c >= (uint8_t)pgm_read_byte(&gfxFont->first)) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last )))
    {
//>>>>>>>>>>>>>>>>>>>>>>>>>>>

      c -= pgm_read_byte(&gfxFont->first);
      GFXglyph *glyph  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c]);
      uint8_t  *bitmap = (uint8_t *)pgm_read_dword(&gfxFont->bitmap);

      uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
      uint8_t  w  = pgm_read_byte(&glyph->width),
               h  = pgm_read_byte(&glyph->height),
               xa = pgm_read_byte(&glyph->xAdvance);
      int8_t   xo = pgm_read_byte(&glyph->xOffset),
               yo = pgm_read_byte(&glyph->yOffset);
      uint8_t  xx, yy, bits, bit=0;
      int16_t  xo16 = 0, yo16 = 0;

      if(size > 1) {
        xo16 = xo;
        yo16 = yo;
      }

      uint16_t hpc = 0; // Horizontal foreground pixel count
      for(yy=0; yy<h; yy++) {
        for(xx=0; xx<w; xx++) {
          if(bit == 0) {
            bits = pgm_read_byte(&bitmap[bo++]);
            bit  = 0x80;
          }
          if(bits & bit) hpc++;
          else {
            if (hpc) {
              if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
              else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
              hpc=0;
            }
          }
          bit >>= 1;
        }
        // Draw pixels for this line as we are about to increment yy
        if (hpc) {
          if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
          else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
          hpc=0;
        }
      }
    }
#endif


#ifdef LOAD_GLCD
  #ifdef LOAD_GFXFF
  } // End classic vs custom font
  #endif
#endif

}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a unicode onto the screen
*************************************************************************************x*/
int16_t TFT_eSprite::drawChar(unsigned int uniCode, int x, int y)
{
  return drawChar(uniCode, x, y, textfont);
}

int16_t TFT_eSprite::drawChar(unsigned int uniCode, int x, int y, int font)
{
  if (!_created ) return 0;

  if (font==1)
  {
#ifdef LOAD_GLCD
  #ifndef LOAD_GFXFF
    drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
    return 6 * textsize;
  #endif
#else
  #ifndef LOAD_GFXFF
    return 0;
  #endif
#endif

#ifdef LOAD_GFXFF
    drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
    if(!gfxFont) { // 'Classic' built-in font
    #ifdef LOAD_GLCD
      return 6 * textsize;
    #else
      return 0;
    #endif
    }
    else
    {
      if((uniCode >= pgm_read_byte(&gfxFont->first)) && (uniCode <= pgm_read_byte(&gfxFont->last) ))
      {
        uint8_t   c2    = uniCode - pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
        return pgm_read_byte(&glyph->xAdvance) * textsize;
      }
      else
      {
        return 0;
      }
    }
#endif
  }

  if ((font>1) && (font<9) && ((uniCode < 32) || (uniCode > 127))) return 0;

  int width  = 0;
  int height = 0;
  uint32_t flash_address = 0;
  uniCode -= 32;

#ifdef LOAD_FONT2
  if (font == 2)
  {
    // This is faster than using the fontdata structure
    flash_address = pgm_read_dword(&chrtbl_f16[uniCode]);
    width = pgm_read_byte(widtbl_f16 + uniCode);
    height = chr_hgt_f16;
  }
  #ifdef LOAD_RLE
  else
  #endif
#endif

#ifdef LOAD_RLE
  {
    if ((font>2) && (font<9))
    {
      // This is slower than above but is more convenient for the RLE fonts
      flash_address = pgm_read_dword( pgm_read_dword( &(fontdata[font].chartbl ) ) + uniCode*sizeof(void *) );
      width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[font].widthtbl ) ) + uniCode );
      height= pgm_read_byte( &fontdata[font].height );
    }
  }
#endif

  int w = width;
  int pX      = 0;
  int pY      = y;
  uint8_t line = 0;

#ifdef LOAD_FONT2 // chop out code if we do not need it
  if (font == 2) {
    w = w + 6; // Should be + 7 but we need to compensate for width increment
    w = w / 8;
    if (x + width * textsize >= _iwidth) return width * textsize ;

    for (int i = 0; i < height; i++)
    {
      if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize, textbgcolor);

      for (int k = 0; k < w; k++)
      {
        line = pgm_read_byte((uint8_t *)flash_address + w * i + k);
        if (line) {
          if (textsize == 1) {
            pX = x + k * 8;
            if (line & 0x80) drawPixel(pX, pY, textcolor);
            if (line & 0x40) drawPixel(pX + 1, pY, textcolor);
            if (line & 0x20) drawPixel(pX + 2, pY, textcolor);
            if (line & 0x10) drawPixel(pX + 3, pY, textcolor);
            if (line & 0x08) drawPixel(pX + 4, pY, textcolor);
            if (line & 0x04) drawPixel(pX + 5, pY, textcolor);
            if (line & 0x02) drawPixel(pX + 6, pY, textcolor);
            if (line & 0x01) drawPixel(pX + 7, pY, textcolor);
          }
          else {
            pX = x + k * 8 * textsize;
            if (line & 0x80) fillRect(pX, pY, textsize, textsize, textcolor);
            if (line & 0x40) fillRect(pX + textsize, pY, textsize, textsize, textcolor);
            if (line & 0x20) fillRect(pX + 2 * textsize, pY, textsize, textsize, textcolor);
            if (line & 0x10) fillRect(pX + 3 * textsize, pY, textsize, textsize, textcolor);
            if (line & 0x08) fillRect(pX + 4 * textsize, pY, textsize, textsize, textcolor);
            if (line & 0x04) fillRect(pX + 5 * textsize, pY, textsize, textsize, textcolor);
            if (line & 0x02) fillRect(pX + 6 * textsize, pY, textsize, textsize, textcolor);
            if (line & 0x01) fillRect(pX + 7 * textsize, pY, textsize, textsize, textcolor);
          }
        }
      }
      pY += textsize;
    }
  }

  #ifdef LOAD_RLE
  else
  #endif
#endif  //FONT2

#ifdef LOAD_RLE  //674 bytes of code
  // Font is not 2 and hence is RLE encoded
  {
    w *= height; // Now w is total number of pixels in the character

    if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize * height, textbgcolor);
    int16_t color;
    if (_bpp16) color = (textcolor >> 8) | (textcolor << 8);
    else color = ((textcolor & 0xE000)>>8 | (textcolor & 0x0700)>>6 | (textcolor & 0x0018)>>3);
    int px = 0, py = pY; // To hold character block start and end column and row values
    int pc = 0; // Pixel count
    uint8_t np = textsize * textsize; // Number of pixels in a drawn pixel
    uint8_t tnp = 0; // Temporary copy of np for while loop
    uint8_t ts = textsize - 1; // Temporary copy of textsize
    // 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
    // w is total number of pixels to plot to fill character block
    while (pc < w)
    {
      line = pgm_read_byte((uint8_t *)flash_address);
      flash_address++; // 20 bytes smaller by incrementing here
      if (line & 0x80) {
        line &= 0x7F;
        line++;
        if (ts) {
          px = x + textsize * (pc % width); // Keep these px and py calculations outside the loop as they are slow
          py = y + textsize * (pc / width);
        }
        else {
          px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
          py = y + pc / width;
        }
        while (line--) {
          pc++;
          setWindow(px, py, px + ts, py + ts);
          if (ts) { tnp = np; while (tnp--) writeColor(color); }
          else    writeColor(color);

          px += textsize;

          if (px >= (x + width * textsize))
          {
            px = x;
            py += textsize;
          }
        }
      }
      else {
        line++;
        pc += line;
      }
    }
  }
  // End of RLE font rendering
#endif
  return width * textsize;    // x +
}

#ifdef SMOOTH_FONT
/***************************************************************************************
** Function name:           drawGlyph
** Description:             Write a character to the sprite cursor position
*************************************************************************************x*/
void TFT_eSprite::drawGlyph(uint16_t code)
{
  if (code < 0x21)
  {
    if (code == 0x20) {
      if (_created) _icursor_x += _tft->gFont.spaceWidth;
      else _tft->cursor_x += _tft->gFont.spaceWidth;
      return;
    }

    if (code == '\n') {
      if (_created)
      {
      _icursor_x = 0;
      _icursor_y += _tft->gFont.yAdvance;
      if (_icursor_y >= _height) _icursor_y = 0;
      return;
      }
      else
      {
        cursor_x = 0;
        cursor_y += gFont.yAdvance;
        if (cursor_y >= _height) cursor_y = 0;
        return;
      }
    }
  }

  uint16_t gNum = 0;
  bool found = _tft->getUnicodeIndex(code, &gNum);
  
  uint16_t fg = _tft->textcolor;
  uint16_t bg = _tft->textbgcolor;

  if (found)
  {
    
    bool newSprite = !_created;
  
    if (newSprite)
    {
      createSprite(_tft->gWidth[gNum], _tft->gFont.yAdvance);
      if(bg) fillSprite(bg);
      _icursor_x = -_tft->gdX[gNum];
      _icursor_y = 0;
    }

    fontFile.seek(_tft->gBitmap[gNum], fs::SeekSet); // This is slow for a significant position shift!

    uint8_t pbuffer[_tft->gWidth[gNum]];

    uint16_t xs = 0;
    uint16_t dl = 0;

    for (int y = 0; y < _tft->gHeight[gNum]; y++)
    {
      fontFile.read(pbuffer, _tft->gWidth[gNum]);
      for (int x = 0; x < _tft->gWidth[gNum]; x++)
      {
        uint8_t pixel = pbuffer[x];
        if (pixel)
        {
          if (pixel != 0xFF)
          {
            if (dl) { drawFastHLine( xs, y + _icursor_y + _tft->gFont.maxAscent - _tft->gdY[gNum], dl, fg); dl = 0; }
            drawPixel(x + _icursor_x + _tft->gdX[gNum], y + _icursor_y + _tft->gFont.maxAscent - _tft->gdY[gNum], alphaBlend(pixel, fg, bg));
          }
          else
          {
            if (dl==0) xs = x + _icursor_x + _tft->gdX[gNum];
            dl++;
          }
        }
        else
        {
          if (dl) { drawFastHLine( xs, y + _icursor_y + _tft->gFont.maxAscent - _tft->gdY[gNum], dl, fg); dl = 0; }
        }
      }
      if (dl) { drawFastHLine( xs, y + _icursor_y + _tft->gFont.maxAscent - _tft->gdY[gNum], dl, fg); dl = 0; }
    }

    if (newSprite)
    {
      pushSprite(_tft->cursor_x + _tft->gdX[gNum], _tft->cursor_y, bg);
      deleteSprite();
      _tft->cursor_x += _tft->gxAdvance[gNum];
    }
    else _icursor_x += _tft->gxAdvance[gNum];
  }
  else
  {
    // Not a Unicode in font so draw a rectangle and move on cursor
    drawRect(_icursor_x, _icursor_y + _tft->gFont.maxAscent - _tft->gFont.ascent, _tft->gFont.spaceWidth, _tft->gFont.ascent, fg);
    _icursor_x += _tft->gFont.spaceWidth + 1;
  }
}


/***************************************************************************************
** Function name:           printToSprite
** Description:             Write a string to the sprite cursor position
*************************************************************************************x*/
void TFT_eSprite::printToSprite(String string)
{
  if(!_tft->fontLoaded) return;
  int16_t len = string.length();
  char cbuffer[len + 1];              // Add 1 for the null
  string.toCharArray(cbuffer, len + 1); // Add 1 for the null, otherwise characters get dropped
  printToSprite(cbuffer, len);
}


/***************************************************************************************
** Function name:           printToSprite
** Description:             Write a string to the sprite cursor position
*************************************************************************************x*/
void TFT_eSprite::printToSprite(char *cbuffer, int len) //String string)
{
  if(!_tft->fontLoaded) return;
  
  fontFile = SPIFFS.open( _tft->_gFontFilename, "r" );

  if(!fontFile)
  {
    _tft->fontLoaded = false;
    return;
  }

  uint16_t n = 0;
  bool newSprite = !_created;
  
  if (newSprite)
  {
    int16_t sWidth = 0;
    uint16_t index = 0;

    while (n < len)
    {
      uint16_t unicode = decodeUTF8((uint8_t*)cbuffer, &n, len - n);
      if (_tft->getUnicodeIndex(unicode, &index))
      {
        if (n == 0) sWidth -= _tft->gdX[index];
        if (n == len-1) sWidth += ( _tft->gWidth[index] + _tft->gdX[index]);
        else sWidth += _tft->gxAdvance[index];
      }
      else sWidth += _tft->gFont.spaceWidth + 1;
    }

    createSprite(sWidth, _tft->gFont.yAdvance);
    uint16_t transparent = TFT_BLACK;

    if (_tft->textbgcolor != TFT_BLACK) fillSprite(_tft->textbgcolor);
  }

  n = 0;

  while (n < len)
  {
    uint16_t unicode = decodeUTF8((uint8_t*)cbuffer, &n, len - n);
    //Serial.print("Decoded Unicode = 0x");Serial.println(unicode,HEX);
    //Serial.print("n = ");Serial.println(n);
    drawGlyph(unicode);
  }

  if (newSprite)
  {
    pushSprite(_tft->cursor_x, _tft->cursor_y);
    deleteSprite();
  }
    
  fontFile.close();
}


/***************************************************************************************
** Function name:           printToSprite
** Description:             Print character in a Sprite, create sprite if needed
*************************************************************************************x*/
int16_t TFT_eSprite::printToSprite(int16_t x, int16_t y, uint16_t index)
{
  bool newSprite = !_created;
  int16_t sWidth = _tft->gWidth[index];

  if (newSprite)
  {
    createSprite(sWidth, _tft->gFont.yAdvance);
    uint16_t transparent = TFT_BLACK;
    if (_tft->textbgcolor != TFT_BLACK) fillSprite(_tft->textbgcolor);

    drawGlyph(_tft->gUnicode[index]);

    pushSprite(x + _tft->gdX[index], y, _tft->textbgcolor);
    deleteSprite();
  }

  else drawGlyph(_tft->gUnicode[index]);

  return _tft->gxAdvance[index];
}
#endif
