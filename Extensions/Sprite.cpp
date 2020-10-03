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
***************************************************************************************/
TFT_eSprite::TFT_eSprite(TFT_eSPI *tft)
{
  _tft = tft;     // Pointer to tft class so we can call member functions

  _iwidth    = 0; // Initialise width and height to 0 (it does not exist yet)
  _iheight   = 0;
  _bpp = 16;
  _iswapBytes = false;   // Do not swap pushImage colour bytes by default

  _created = false;

  _xs = 0;  // window bounds for pushColor
  _ys = 0;
  _xe = 0;
  _ye = 0;

  _xptr = 0; // pushColor coordinate
  _yptr = 0;

  _xPivot = 0;
  _yPivot = 0;

  _colorMap = nullptr;

  this->cursor_y = this->cursor_x = 0; // Text cursor position

  this->_psram_enable = true;
}


/***************************************************************************************
** Function name:           createSprite
** Description:             Create a sprite (bitmap) of defined width and height
***************************************************************************************/
// cast returned value to (uint8_t*) for 8 bit or (uint16_t*) for 16 bit colours
void* TFT_eSprite::createSprite(int16_t w, int16_t h, uint8_t frames)
{

  if ( _created ) return _img8_1;

  if ( w < 1 || h < 1 ) return nullptr;

  _iwidth  = _dwidth  = _bitwidth = w;
  _iheight = _dheight = h;

  this->cursor_x = 0;
  this->cursor_y = 0;

  // Default scroll rectangle and gap fill colour
  _sx = 0;
  _sy = 0;
  _sw = w;
  _sh = h;
  _scolor = TFT_BLACK;

  _xPivot = w/2;
  _yPivot = h/2;

  _img8   = (uint8_t*) callocSprite(w, h, frames);
  _img8_1 = _img8;
  _img8_2 = _img8;
  _img    = (uint16_t*) _img8;
  _img4   = _img8;

  if ( (_bpp == 16) && (frames > 1) ) {
    _img8_2 = _img8 + (w * h * 2 + 1);
  }

  // ESP32 only 16bpp check
  //if (esp_ptr_dma_capable(_img8_1)) Serial.println("DMA capable Sprite pointer _img8_1");
  //else Serial.println("Not a DMA capable Sprite pointer _img8_1");
  //if (esp_ptr_dma_capable(_img8_2)) Serial.println("DMA capable Sprite pointer _img8_2");
  //else Serial.println("Not a DMA capable Sprite pointer _img8_2");

  if ( (_bpp == 8) && (frames > 1) ) {
    _img8_2 = _img8 + (w * h + 1);
  }

  if ( (_bpp == 4) && (_colorMap == nullptr)) createPalette(default_4bit_palette);

  // This is to make it clear what pointer size is expected to be used
  // but casting in the user sketch is needed due to the use of void*
  if ( (_bpp == 1) && (frames > 1) )
  {
    w = (w+7) & 0xFFF8;
    _img8_2 = _img8 + ( (w>>3) * h + 1 );
  }

  if (_img8)
  {
    _created = true;
    return _img8_1;
  }

  return nullptr;
}


/***************************************************************************************
** Function name:           getPointer
** Description:             Returns pointer to start of sprite memory area
***************************************************************************************/
void* TFT_eSprite::getPointer(void)
{
  if (!_created) return nullptr;
  return _img8_1;
}


/***************************************************************************************
** Function name:           created
** Description:             Returns true if sprite has been created
***************************************************************************************/
bool TFT_eSprite::created(void)
{
  return _created;
}


/***************************************************************************************
** Function name:           ~TFT_eSprite
** Description:             Class destructor
***************************************************************************************/
TFT_eSprite::~TFT_eSprite(void)
{
  deleteSprite();

#ifdef SMOOTH_FONT
  if(this->fontLoaded) this->unloadFont();
#endif
}


/***************************************************************************************
** Function name:           callocSprite
** Description:             Allocate a memory area for the Sprite and return pointer
***************************************************************************************/

void* TFT_eSprite::callocSprite(int16_t w, int16_t h, uint8_t frames)
{
  // Add one extra "off screen" pixel to point out-of-bounds setWindow() coordinates
  // this means push/writeColor functions do not need additional bounds checks and
  // hence will run faster in normal circumstances.
  uint8_t* ptr8 = nullptr;

  if (frames > 2) frames = 2; // Currently restricted to 2 frame buffers
  if (frames < 1) frames = 1;

  if (_bpp == 16)
  {
#if defined (ESP32) && defined (CONFIG_SPIRAM_SUPPORT)
    if ( psramFound() && this->_psram_enable && !_tft->DMA_Enabled)
    {
      ptr8 = ( uint8_t*) ps_calloc(frames * w * h + frames, sizeof(uint16_t));
      //Serial.println("PSRAM");
    }
    else
#endif
    {
      ptr8 = ( uint8_t*) calloc(frames * w * h + frames, sizeof(uint16_t));
      //Serial.println("Normal RAM");
    }
  }

  else if (_bpp == 8)
  {
#if defined (ESP32) && defined (CONFIG_SPIRAM_SUPPORT)
    if ( psramFound() && this->_psram_enable ) ptr8 = ( uint8_t*) ps_calloc(frames * w * h + frames, sizeof(uint8_t));
    else
#endif
    ptr8 = ( uint8_t*) calloc(frames * w * h + frames, sizeof(uint8_t));
  }

  else if (_bpp == 4)
  {
    w = (w+1) & 0xFFFE; // width needs to be multiple of 2, with an extra "off screen" pixel
    _iwidth = w;
#if defined (ESP32) && defined (CONFIG_SPIRAM_SUPPORT)
    if ( psramFound() && this->_psram_enable ) ptr8 = ( uint8_t*) ps_calloc(((frames * w * h) >> 1) + frames, sizeof(uint8_t));
    else
#endif
    ptr8 = ( uint8_t*) calloc(((frames * w * h) >> 1) + frames, sizeof(uint8_t));
  }

  else // Must be 1 bpp
  {
    //_dwidth   Display width+height in pixels always in rotation 0 orientation
    //_dheight  Not swapped for sprite rotations
    // Note: for 1bpp _iwidth and _iheight are swapped during Sprite rotations

    w =  (w+7) & 0xFFF8; // width should be the multiple of 8 bits to be compatible with epdpaint
    _iwidth = w;         // _iwidth is rounded up to be multiple of 8, so might not be = _dwidth
    _bitwidth = w;

#if defined (ESP32) && defined (CONFIG_SPIRAM_SUPPORT)
    if ( psramFound() && this->_psram_enable ) ptr8 = ( uint8_t*) ps_calloc(frames * (w>>3) * h + frames, sizeof(uint8_t));
    else
#endif
    ptr8 = ( uint8_t*) calloc(frames * (w>>3) * h + frames, sizeof(uint8_t));
  }

  return ptr8;
}

/***************************************************************************************
** Function name:           createPalette (from RAM array)
** Description:             Set a palette for a 4-bit per pixel sprite
***************************************************************************************/

void TFT_eSprite::createPalette(uint16_t colorMap[], uint8_t colors)
{
  if (_colorMap != nullptr)
  {
    free(_colorMap);
  }

  if (colorMap == nullptr)
  {
    // Create a color map using the default FLASH map
    createPalette(default_4bit_palette);
    return;
  }

  // Allocate and clear memory for 16 color map
  _colorMap = (uint16_t *)calloc(16, sizeof(uint16_t));

  if (colors > 16) colors = 16;

  // Copy map colors
  for (uint8_t i = 0; i < colors; i++)
  {
    _colorMap[i] = colorMap[i];
  }
}

/***************************************************************************************
** Function name:           createPalette (from FLASH array)
** Description:             Set a palette for a 4-bit per pixel sprite
***************************************************************************************/

void TFT_eSprite::createPalette(const uint16_t colorMap[], uint8_t colors)
{
  if (colorMap == nullptr)
  {
    // Create a color map using the default FLASH map
    colorMap = default_4bit_palette;
  }

  // Allocate and clear memory for 16 color map
  _colorMap = (uint16_t *)calloc(16, sizeof(uint16_t));

  if (colors > 16) colors = 16;

  // Copy map colors
  for (uint8_t i = 0; i < colors; i++)
  {
    _colorMap[i] = pgm_read_word(colorMap++);
  }
}

/***************************************************************************************
** Function name:           frameBuffer
** Description:             For 1 bpp Sprites, select the frame used for graphics
***************************************************************************************/
// Frames are numbered 1 and 2
void* TFT_eSprite::frameBuffer(int8_t f)
{
  if (!_created) return nullptr;

  if ( f == 2 ) _img8 = _img8_2;
  else          _img8 = _img8_1;

  if (_bpp == 16) _img = (uint16_t*)_img8;

  //if (_bpp == 8) _img8 = _img8;

  if (_bpp == 4) _img4 = _img8;

  return _img8;
}

/***************************************************************************************
** Function name:           setColorDepth
** Description:             Set bits per pixel for colour (1, 8 or 16)
***************************************************************************************/

void* TFT_eSprite::setColorDepth(int8_t b)
{
  // Do not re-create the sprite if the colour depth does not change
  if (_bpp == b) return _img8_1;

  // Validate the new colour depth
  if ( b > 8 ) _bpp = 16;  // Bytes per pixel
  else if ( b > 4 ) _bpp = 8;
  else if ( b > 1 ) _bpp = 4;
  else _bpp = 1;

  // Can't change an existing sprite's colour depth so delete it
  if (_created) free(_img8_1);

  // If it existed, re-create the sprite with the new colour depth
  if (_created)
  {
    _created = false;
    return createSprite(_dwidth, _dheight);
  }

  return nullptr;
}


/***************************************************************************************
** Function name:           getColorDepth
** Description:             Get bits per pixel for colour (1, 8 or 16)
***************************************************************************************/

int8_t TFT_eSprite::getColorDepth(void)
{
  if (_created) return _bpp;
  else return 0;
}


/***************************************************************************************
** Function name:           setBitmapColor
** Description:             Set the 1bpp foreground foreground and background colour
***************************************************************************************/
void TFT_eSprite::setBitmapColor(uint16_t c, uint16_t b)
{
  if (c == b) b = ~c;
  _tft->bitmap_fg = c;
  _tft->bitmap_bg = b;
}

/***************************************************************************************
** Function name:           setPaletteColor
** Description:             Set the 4bpp palette color at the given index
***************************************************************************************/
void TFT_eSprite::setPaletteColor(uint8_t index, uint16_t color)
{
  if (_colorMap == nullptr || index > 15) return; // out of bounds

  _colorMap[index] = color;
}

/***************************************************************************************
** Function name:           getPaletteColor
** Description:             Return the palette color at 4bpp index, or 0 on error.
***************************************************************************************/
uint16_t TFT_eSprite::getPaletteColor(uint8_t index)
{
  if (_colorMap == nullptr || index > 15) return 0; // out of bounds

  return _colorMap[index];
}

/***************************************************************************************
** Function name:           deleteSprite
** Description:             Delete the sprite to free up memory (RAM)
***************************************************************************************/
void TFT_eSprite::deleteSprite(void)
{
  if (_colorMap != nullptr)
  {
    free(_colorMap);
	_colorMap = nullptr;
  }

  if (_created)
  {
    free(_img8_1);
    _img8 = nullptr;
	_created = false;
  }
}


/***************************************************************************************
** Function name:           setPivot
** Description:             Set the pivot point in this Sprite
***************************************************************************************/
void TFT_eSprite::setPivot(int16_t x, int16_t y)
{
  _xPivot = x;
  _yPivot = y;
}


/***************************************************************************************
** Function name:           getPivotX
** Description:             Get the x pivot position
***************************************************************************************/
int16_t TFT_eSprite::getPivotX(void)
{
  return _xPivot;
}


/***************************************************************************************
** Function name:           getPivotY
** Description:             Get the y pivot position
***************************************************************************************/
int16_t TFT_eSprite::getPivotY(void)
{
  return _yPivot;
}


/***************************************************************************************
** Function name:           pushRotated - Fast fixed point integer maths version
** Description:             Push rotated Sprite to TFT screen
***************************************************************************************/
#define FP_SCALE 10
bool TFT_eSprite::pushRotated(int16_t angle, int32_t transp)
{
  if ( !_created) return false;

  // Bounding box parameters
  int16_t min_x;
  int16_t min_y;
  int16_t max_x;
  int16_t max_y;

  // Get the bounding box of this rotated source Sprite relative to Sprite pivot
  if ( !getRotatedBounds(angle, &min_x, &min_y, &max_x, &max_y) ) return false;

  uint16_t sline_buffer[max_x - min_x + 1];

  int32_t xt = min_x - _tft->_xpivot;
  int32_t yt = min_y - _tft->_ypivot;
  uint32_t xe = _dwidth << FP_SCALE;
  uint32_t ye = _dheight << FP_SCALE;
  uint16_t tpcolor = transp;  // convert to unsigned
  if (_bpp == 4) tpcolor = _colorMap[transp & 0x0F];
  tpcolor = tpcolor>>8 | tpcolor<<8; // Working with swapped color bytes
  _tft->startWrite(); // Avoid transaction overhead for every tft pixel

  // Scan destination bounding box and fetch transformed pixels from source Sprite
  for (int32_t y = min_y; y <= max_y; y++, yt++) {
    int32_t x = min_x;
    uint32_t xs = (_cosra * xt - (_sinra * yt - (_xPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));
    uint32_t ys = (_sinra * xt + (_cosra * yt + (_yPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));

    while ((xs >= xe || ys >= ye) && x < max_x) { x++; xs += _cosra; ys += _sinra; }
    if (x == max_x) continue;

    uint32_t pixel_count = 0;
    do {
      uint16_t rp;
      int32_t xp = xs >> FP_SCALE;
      int32_t yp = ys >> FP_SCALE;
      if (_bpp == 16) {rp = _img[xp + yp * _iwidth]; }
      else { rp = readPixel(xp, yp); rp = rp>>8 | rp<<8; }
      if (tpcolor == rp) {
        if (pixel_count) {
          // TFT window is already clipped, so this is faster than pushImage()
          _tft->setWindow(x - pixel_count, y, x, y);
          _tft->pushPixels(sline_buffer, pixel_count);
          pixel_count = 0;
        }
      }
      else {
        sline_buffer[pixel_count++] = rp;
      }
    } while (++x < max_x && (xs += _cosra) < xe && (ys += _sinra) < ye);
    if (pixel_count) {
      // TFT window is already clipped, so this is faster than pushImage()
      _tft->setWindow(x - pixel_count, y, x, y);
      _tft->pushPixels(sline_buffer, pixel_count);
    }
  }

  _tft->endWrite(); // End transaction

  return true;
}


/***************************************************************************************
** Function name:           pushRotated - Fast fixed point integer maths version
** Description:             Push a rotated copy of the Sprite to another Sprite
***************************************************************************************/
// Not compatible with 4bpp
bool TFT_eSprite::pushRotated(TFT_eSprite *spr, int16_t angle, int32_t transp)
{
  if ( !_created  || _bpp == 4) return false; // Check this Sprite is created
  if ( !spr->_created  || spr->_bpp == 4) return false;  // Ckeck destination Sprite is created

  // Bounding box parameters
  int16_t min_x;
  int16_t min_y;
  int16_t max_x;
  int16_t max_y;

  // Get the bounding box of this rotated source Sprite
  if ( !getRotatedBounds(spr, angle, &min_x, &min_y, &max_x, &max_y) ) return false;

  uint16_t sline_buffer[max_x - min_x + 1];

  int32_t xt = min_x - spr->_xPivot;
  int32_t yt = min_y - spr->_yPivot;
  uint32_t xe = _dwidth << FP_SCALE;
  uint32_t ye = _dheight << FP_SCALE;
  uint16_t tpcolor = transp>>8 | transp<<8;  // convert to unsigned swapped bytes

  bool oldSwapBytes = spr->getSwapBytes();
  spr->setSwapBytes(false);

  // Scan destination bounding box and fetch transformed pixels from source Sprite
  for (int32_t y = min_y; y <= max_y; y++, yt++) {
    int32_t x = min_x;
    uint32_t xs = (_cosra * xt - (_sinra * yt - (_xPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));
    uint32_t ys = (_sinra * xt + (_cosra * yt + (_yPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));

    while ((xs >= xe || ys >= ye) && x < max_x) { x++; xs += _cosra; ys += _sinra; }
    if (x == max_x) continue;

    uint32_t pixel_count = 0;
    do {
      uint16_t rp;
      int32_t xp = xs >> FP_SCALE;
      int32_t yp = ys >> FP_SCALE;
      if (_bpp == 16) rp = _img[xp + yp * _iwidth];
      else { rp = readPixel(xp, yp); rp = rp>>8 | rp<<8; }
      if (tpcolor == rp) {
        if (pixel_count) {
          spr->pushImage(x - pixel_count, y, pixel_count, 1, sline_buffer);
          pixel_count = 0;
        }
      }
      else {
        sline_buffer[pixel_count++] = rp;
      }
    } while (++x < max_x && (xs += _cosra) < xe && (ys += _sinra) < ye);
    if (pixel_count) spr->pushImage(x - pixel_count, y, pixel_count, 1, sline_buffer);
  }
  spr->setSwapBytes(oldSwapBytes);
  return true;
}


/***************************************************************************************
** Function name:           getRotatedBounds
** Description:             Get TFT bounding box of a rotated Sprite wrt pivot
***************************************************************************************/
bool TFT_eSprite::getRotatedBounds(int16_t angle, int16_t *min_x, int16_t *min_y,
                                                  int16_t *max_x, int16_t *max_y)
{
  // Get the bounding box of this rotated source Sprite relative to Sprite pivot
  getRotatedBounds(angle, width(), height(), _xPivot, _yPivot, min_x, min_y, max_x, max_y);

  // Move bounding box so source Sprite pivot coincides with TFT pivot
  *min_x += _tft->_xpivot;
  *max_x += _tft->_xpivot;
  *min_y += _tft->_ypivot;
  *max_y += _tft->_ypivot;

  // Return if bounding box is outside of TFT area
  if (*min_x > _tft->width()) return false;
  if (*min_y > _tft->height()) return false;
  if (*max_x < 0) return false;
  if (*max_y < 0) return false;

  // Clip bounding box to be within TFT area
  if (*min_x < 0) *min_x = 0;
  if (*min_y < 0) *min_y = 0;
  if (*max_x > _tft->width())  *max_x = _tft->width();
  if (*max_y > _tft->height()) *max_y = _tft->height();

  return true;
}


/***************************************************************************************
** Function name:           getRotatedBounds
** Description:             Get destination Sprite bounding box of a rotated Sprite wrt pivot
***************************************************************************************/
bool TFT_eSprite::getRotatedBounds(TFT_eSprite *spr, int16_t angle, int16_t *min_x, int16_t *min_y,
                                                                    int16_t *max_x, int16_t *max_y)
{
  // Get the bounding box of this rotated source Sprite relative to Sprite pivot
  getRotatedBounds(angle, width(), height(), _xPivot, _yPivot, min_x, min_y, max_x, max_y);

  // Move bounding box so source Sprite pivot coincides with destination Sprite pivot
  *min_x += spr->_xPivot;
  *max_x += spr->_xPivot;
  *min_y += spr->_yPivot;
  *max_y += spr->_yPivot;

  // Test only to show bounding box
  // spr->fillSprite(TFT_BLACK);
  // spr->drawRect(min_x, min_y, max_x - min_x + 1, max_y - min_y + 1, TFT_GREEN);

  // Return if bounding box is completely outside of destination Sprite
  if (*min_x > spr->width()) return true;
  if (*min_y > spr->height()) return true;
  if (*max_x < 0) return true;
  if (*max_y < 0) return true;

  // Clip bounding box if it is partially within destination Sprite
  if (*min_x < 0) min_x = 0;
  if (*min_y < 0) min_y = 0;
  if (*max_x > spr->width())  *max_x = spr->width();
  if (*max_y > spr->height()) *max_y = spr->height();

  return true;
}


/***************************************************************************************
** Function name:           rotatedBounds
** Description:             Get bounding box of a rotated Sprite wrt pivot
***************************************************************************************/
void TFT_eSprite::getRotatedBounds(int16_t angle, int16_t w, int16_t h, int16_t xp, int16_t yp,
                                   int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y)
{
  // Trig values for the rotation
  float radAngle = -angle * 0.0174532925; // Convert degrees to radians
  float sina = sin(radAngle);
  float cosa = cos(radAngle);

  w -= xp; // w is now right edge coordinate relative to xp
  h -= yp; // h is now bottom edge coordinate relative to yp

  // Calculate new corner coordinates
  int16_t x0 = -xp * cosa - yp * sina;
  int16_t y0 =  xp * sina - yp * cosa;

  int16_t x1 =  w * cosa - yp * sina;
  int16_t y1 = -w * sina - yp * cosa;

  int16_t x2 =  h * sina + w * cosa;
  int16_t y2 =  h * cosa - w * sina;

  int16_t x3 =  h * sina - xp * cosa;
  int16_t y3 =  h * cosa + xp * sina;

  // Find bounding box extremes, enlarge box to accomodate rounding errors
  *min_x = x0-2;
  if (x1 < *min_x) *min_x = x1-2;
  if (x2 < *min_x) *min_x = x2-2;
  if (x3 < *min_x) *min_x = x3-2;

  *max_x = x0+2;
  if (x1 > *max_x) *max_x = x1+2;
  if (x2 > *max_x) *max_x = x2+2;
  if (x3 > *max_x) *max_x = x3+2;

  *min_y = y0-2;
  if (y1 < *min_y) *min_y = y1-2;
  if (y2 < *min_y) *min_y = y2-2;
  if (y3 < *min_y) *min_y = y3-2;

  *max_y = y0+2;
  if (y1 > *max_y) *max_y = y1+2;
  if (y2 > *max_y) *max_y = y2+2;
  if (y3 > *max_y) *max_y = y3+2;

  _sinra = round(sina * (1<<FP_SCALE));
  _cosra = round(cosa * (1<<FP_SCALE));
}


/***************************************************************************************
** Function name:           pushSprite
** Description:             Push the sprite to the TFT at x, y
***************************************************************************************/
void TFT_eSprite::pushSprite(int32_t x, int32_t y)
{
  if (!_created) return;

  if (_bpp == 16)
  {
    bool oldSwapBytes = _tft->getSwapBytes();
    _tft->setSwapBytes(false);
    _tft->pushImage(x, y, _dwidth, _dheight, _img );
    _tft->setSwapBytes(oldSwapBytes);
  }
  else if (_bpp == 4)
  {
    _tft->pushImage(x, y, _dwidth, _dheight, _img4, false, _colorMap);
  }
  else _tft->pushImage(x, y, _dwidth, _dheight, _img8, (bool)(_bpp == 8));
}


/***************************************************************************************
** Function name:           pushSprite
** Description:             Push the sprite to the TFT at x, y with transparent colour
***************************************************************************************/
void TFT_eSprite::pushSprite(int32_t x, int32_t y, uint16_t transp)
{
  if (!_created) return;

  if (_bpp == 16)
  {
    bool oldSwapBytes = _tft->getSwapBytes();
    _tft->setSwapBytes(false);
    _tft->pushImage(x, y, _dwidth, _dheight, _img, transp );
    _tft->setSwapBytes(oldSwapBytes);
  }
  else if (_bpp == 8)
  {
    transp = (uint8_t)((transp & 0xE000)>>8 | (transp & 0x0700)>>6 | (transp & 0x0018)>>3);
    _tft->pushImage(x, y, _dwidth, _dheight, _img8, (uint8_t)transp, (bool)true);
  }
  else if (_bpp == 4)
  {
    _tft->pushImage(x, y, _dwidth, _dheight, _img4, (uint8_t)(transp & 0x0F), false, _colorMap);
  }
  else _tft->pushImage(x, y, _dwidth, _dheight, _img8, 0, (bool)false);
}


/***************************************************************************************
** Function name:           pushToSprite
** Description:             Push the sprite to another sprite at x, y
***************************************************************************************/
// Note: The following sprite to sprite colour depths are currently supported:
//    Source    Destination
//    16bpp  -> 16bpp
//    16bpp  ->  8bpp
//     4bpp  ->  4bpp (note: color translation depends on the 2 sprites pallet colors)
//     1bpp  ->  1bpp (note: color translation depends on the 2 sprites bitmap colors)

bool TFT_eSprite::pushToSprite(TFT_eSprite *dspr, int32_t x, int32_t y)
{
  if (!_created) return false;
  if (!dspr->created()) return false;

  // Check destination sprite compatibility
  int8_t ds_bpp = dspr->getColorDepth();
  if (_bpp == 16 && ds_bpp != 16 && ds_bpp !=  8) return false;
  if (_bpp ==  8) return false;
  if (_bpp ==  4 && ds_bpp !=  4) return false;
  if (_bpp ==  1 && ds_bpp !=  1) return false;

  bool oldSwapBytes = dspr->getSwapBytes();
  dspr->setSwapBytes(false);
  dspr->pushImage(x, y, _dwidth, _dheight, _img );
  dspr->setSwapBytes(oldSwapBytes);

  return true;
}


/***************************************************************************************
** Function name:           pushToSprite
** Description:             Push the sprite to another sprite at x, y with transparent colour
***************************************************************************************/
/* >>>>>>  Using a transparent color is not supported at the moment  <<<<<<
void TFT_eSprite::pushToSprite(TFT_eSprite *spr, int32_t x, int32_t y, uint16_t transp)
{
  if (!_created) return;

  if (_bpp == 16)
  {
    bool oldSwapBytes = spr->getSwapBytes();
    spr->setSwapBytes(false);
    spr->pushImage(x, y, _dwidth, _dheight, _img, transp );
    spr->setSwapBytes(oldSwapBytes);
  }
  else if (_bpp == 8)
  {
    transp = (uint8_t)((transp & 0xE000)>>8 | (transp & 0x0700)>>6 | (transp & 0x0018)>>3);
    spr->pushImage(x, y, _dwidth, _dheight, _img8, (uint8_t)transp, (bool)true);
  }
  else if (_bpp == 4)
  {
    spr->pushImage(x, y, _dwidth, _dheight, _img4, (uint8_t)(transp & 0x0F), false, _colorMap);
  }
  else spr->pushImage(x, y, _dwidth, _dheight, _img8, 0, (bool)false);
}
*/

/***************************************************************************************
** Function name:           pushSprite
** Description:             Push a cropped sprite to the TFT at tx, ty
***************************************************************************************/
bool TFT_eSprite::pushSprite(int32_t tx, int32_t ty, int32_t sx, int32_t sy, int32_t sw, int32_t sh)
{
  if (!_created) return false;

  // Perform window boundary checks and crop if needed
  setWindow(sx, sy, sx + sw - 1, sy + sh - 1);

  /* These global variables are now populated for the sprite
  _xs = x start coordinate
  _ys = y start coordinate
  _xe = x end coordinate (inclusive)
  _ye = y end coordinate (inclusive)
  */

  // Calculate new sprite window bounding box width and height
  sw = _xe - _xs + 1;
  sh = _ye - _ys + 1;

  if (_ys >= _iheight) return false;

  if (_bpp == 16)
  {
    bool oldSwapBytes = _tft->getSwapBytes();
    _tft->setSwapBytes(false);

    // Check if a faster block copy to screen is possible
    if ( sx == 0 && sw == _dwidth)
      _tft->pushImage(tx, ty, sw, sh, _img + _iwidth * _ys );
    else // Render line by line
      while (sh--)
        _tft->pushImage(tx, ty++, sw, 1, _img + _xs + _iwidth * _ys++ );

    _tft->setSwapBytes(oldSwapBytes);
  }
  else if (_bpp == 8)
  {
    // Check if a faster block copy to screen is possible
    if ( sx == 0 && sw == _dwidth)
      _tft->pushImage(tx, ty, sw, sh, _img8 + _iwidth * _ys, true );
    else // Render line by line
    while (sh--)
      _tft->pushImage(tx, ty++, sw, 1, _img8 + _xs + _iwidth * _ys++, true );
  }
  else if (_bpp == 4)
  {
    // Check if a faster block copy to screen is possible
    if ( sx == 0 && sw == _dwidth)
      _tft->pushImage(tx, ty, sw, sh, _img4 + (_iwidth>>1) * _ys, false, _colorMap );
    else // Render line by line
    {
      uint32_t ds = _xs&1; // Odd x start pixel

      uint32_t de = 0;     // Odd x end pixel
      if ((sw > ds) && (_xe&1)) de = 1;

      uint32_t dm = 0;     // Midsection pixel count
      if (sw > (ds+de)) dm = sw - ds - de;
      sw--;

      uint32_t yp = (_xs + ds + _iwidth * _ys)>>1;
      _tft->startWrite();
      while (sh--)
      {
        if (ds) _tft->drawPixel(tx, ty, readPixel(_xs, _ys) );
        if (dm) _tft->pushImage(tx + ds, ty, dm, 1, _img4 + yp, false, _colorMap );
        if (de) _tft->drawPixel(tx + sw, ty, readPixel(_xe, _ys) );
        _ys++;
        ty++;
        yp += (_iwidth>>1);
      }
      _tft->endWrite();
    }
  }
  else // 1bpp
  {
        // Check if a faster block copy to screen is possible
    if ( sx == 0 && sw == _dwidth)
      _tft->pushImage(tx, ty, sw, sh, _img8 + (_iwidth>>3) * _ys, false );
    else // Render line by line
    {
      _tft->startWrite();
      _tft->setWindow(tx, ty, tx+sw-1, ty+sh-1);
      while (sh--)
      {
        for (uint32_t dx = _xs; dx < _xs + sw; dx++) _tft->pushColor(readPixel(dx, _ys));
        ty++;
        _ys++;
      }
      _tft->endWrite();
    }
  }

  return true;
}


/***************************************************************************************
** Function name:           readPixelValue
** Description:             Read the color map index of a pixel at defined coordinates
***************************************************************************************/
uint16_t TFT_eSprite::readPixelValue(int32_t x, int32_t y)
{
  if ((x < 0) || (x >= _iwidth) || (y < 0) || (y >= _iheight) || !_created) return 0xFF;

  if (_bpp == 16)
  {
    // Return the pixel colour
    return readPixel(x, y);
  }

  if (_bpp == 8)
  {
    // Return the pixel byte value
    return _img8[x + y * _iwidth];
  }

  if (_bpp == 4)
  {
    if (x >= _dwidth) return 0xFF;
    if ((x & 0x01) == 0)
      return _img4[((x+y*_iwidth)>>1)] >> 4;   // even index = bits 7 .. 4
    else
      return _img4[((x+y*_iwidth)>>1)] & 0x0F; // odd index = bits 3 .. 0.
  }

  if (_bpp == 1)
  {
    // Note: _dwidth and _dheight bounds not checked (rounded up -iwidth and _iheight used)
    if (_rotation == 1)
    {
      uint16_t tx = x;
      x = _dwidth - y - 1;
      y = tx;
    }
    else if (_rotation == 2)
    {
      x = _dwidth - x - 1;
      y = _dheight - y - 1;
    }
    else if (_rotation == 3)
    {
      uint16_t tx = x;
      x = y;
      y = _dheight - tx - 1;
    }
    // Return 1 or 0
    return (_img8[(x + y * _bitwidth)>>3] >> (7-(x & 0x7))) & 0x01;
  }

  return 0;
}

/***************************************************************************************
** Function name:           readPixel
** Description:             Read 565 colour of a pixel at defined coordinates
***************************************************************************************/
uint16_t TFT_eSprite::readPixel(int32_t x, int32_t y)
{
  if ((x < 0) || (x >= _iwidth) || (y < 0) || (y >= _iheight) || !_created) return 0xFFFF;

  if (_bpp == 16)
  {
    uint16_t color = _img[x + y * _iwidth];
    return (color >> 8) | (color << 8);
  }

  if (_bpp == 8)
  {
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

  if (_bpp == 4)
  {
    if (x >= _dwidth) return 0xFFFF;
    uint16_t color;
    if ((x & 0x01) == 0)
      color = _colorMap[_img4[((x+y*_iwidth)>>1)] >> 4];   // even index = bits 7 .. 4
    else
      color = _colorMap[_img4[((x+y*_iwidth)>>1)] & 0x0F]; // odd index = bits 3 .. 0.
    return color;
  }

  // Note: Must be 1bpp
  // _dwidth and _dheight bounds not checked (rounded up -iwidth and _iheight used)
  if (_rotation == 1)
  {
    uint16_t tx = x;
    x = _dwidth - y - 1;
    y = tx;
  }
  else if (_rotation == 2)
  {
    x = _dwidth - x - 1;
    y = _dheight - y - 1;
  }
  else if (_rotation == 3)
  {
    uint16_t tx = x;
    x = y;
    y = _dheight - tx - 1;
  }

  uint16_t color = (_img8[(x + y * _bitwidth)>>3] << (x & 0x7)) & 0x80;

  if (color) return _tft->bitmap_fg;
  else       return _tft->bitmap_bg;
}


/***************************************************************************************
** Function name:           pushImage
** Description:             push image into a defined area of a sprite
***************************************************************************************/
void  TFT_eSprite::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data)
{
  if (data == nullptr || !_created) return;
  if ((x >= _iwidth) || (y >= _iheight) || (w == 0) || (h == 0)) return;
  if ((x + w < 0) || (y + h < 0)) return;

  int32_t xo = 0;
  int32_t yo = 0;

  int32_t xs = x;
  int32_t ys = y;

  int32_t ws = w;
  int32_t hs = h;

  if (x < 0) { xo = -x; ws += x; xs = 0; }
  if (y < 0) { yo = -y; hs += y; ys = 0; }

  if (xs + ws >= (int32_t)_iwidth)  ws = _iwidth  - xs;
  if (ys + hs >= (int32_t)_iheight) hs = _iheight - ys;

  if (_bpp == 16) // Plot a 16 bpp image into a 16 bpp Sprite
  {
    // Pointer within original image
    uint8_t *ptro = (uint8_t *)data + ((xo + yo * w) << 1);
    // Pointer within sprite image
    uint8_t *ptrs = (uint8_t *)_img + ((xs + ys * _iwidth) << 1);

    if(_iswapBytes)
    {
      while (hs--)
      {
        // Fast copy with a 1 byte shift
        memcpy(ptrs+1, ptro, (ws<<1) - 1);
        // Now correct just the even numbered bytes
        for (int32_t xp = 0; xp < (ws<<1); xp+=2)
        {
          ptrs[xp] = ptro[xp+1];;
        }
        ptro += w<<1;
        ptrs += _iwidth<<1;
      }
    }
    else
    {
      while (hs--)
      {
        memcpy(ptrs, ptro, ws<<1);
        ptro += w << 1;
        ptrs += _iwidth << 1;
      }
    }
  }
  else if (_bpp == 8) // Plot a 16 bpp image into a 8 bpp Sprite
  {
    for (int32_t yp = yo; yp < yo + hs; yp++)
    {
      x = xs;
      for (int32_t xp = xo; xp < xo + ws; xp++)
      {
        uint16_t color = data[xp + yp * w];
        // When data source is a sprite, the bytes are already swapped
        if(!_iswapBytes) _img8[x + ys * _iwidth] = (uint8_t)((color & 0xE0) | (color & 0x07)<<2 | (color & 0x1800)>>11);
        else _img8[x + ys * _iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
        x++;
      }
      ys++;
    }
  }
  else if (_bpp == 4)
  {
    // The image is assumed to be 4 bit, where each byte corresponds to two pixels.
    // much faster when aligned to a byte boundary, because the alternative is slower, requiring
    // tedious bit operations.

    const uint8_t *dataBuf = (uint8_t *)data;
    int sWidth = (_iwidth >> 1);

    if ((xs & 0x01) == 0 && (xo & 0x01) == 0 && (ws & 0x01) == 0)
    {
      if ((ws & 0x01) == 0) // use memcpy for better perf.
      {
        xs = (xs >> 1) + ys * sWidth;
        ws = (ws >> 1);
        xo = (xo >> 1) + yo * (w>>1);
        while (hs--)
        {
          memcpy(_img4 + xs, dataBuf + xo, ws);
          xo += (w >> 1);
          xs += sWidth;
        }
      }
    }
    else  // not optimized
    {
      // _dwidth and _dheight bounds not checked (rounded up -iwidth and _iheight used)
      for (int32_t yp = yo; yp < yo + hs; yp++)
      {
        x = xs;
        for (int32_t xp = xo; xp < xo + ws; xp++)
        {
          uint32_t color;
          if ((xp & 0x01) == 0)
            color = (dataBuf[((xp+yp*w)>>1)] & 0xF0) >> 4; // even index = bits 7 .. 4
          else
            color = dataBuf[((xp-1+yp*w)>>1)] & 0x0F;  // odd index = bits 3 .. 0.
          drawPixel(x, ys, color);
          x++;
        }
        ys++;
      }
    }
  }

  else // 1bpp
  {
    // Move coordinate rotation to support fn
    if (_rotation == 1)
    {
      int32_t tx = x;
      x = _dwidth - y - 1;
      y = tx;
    }
    else if (_rotation == 2)
    {
      x = _dwidth - x - 1;
      y = _dheight - y - 1;
    }
    else if (_rotation == 3)
    {
      int32_t tx = x;
      x = y;
      y = _dheight - tx - 1;
    }
    // Plot a 1bpp image into a 1bpp Sprite
    uint8_t* pdata = (uint8_t* ) data;
    uint32_t ww =  (w+7) & 0xFFF8;
    for (int32_t yp = 0; yp<h; yp++)
    {
      uint32_t yw = (yp * ww)>>3;
      uint32_t yyp = y + yp;
      for (int32_t xp = 0; xp<w; xp++)
      {
        uint16_t readPixel = (pdata[(xp>>3) + yw] & (0x80 >> (xp & 0x7)) );
        drawPixel(x+xp, yyp, readPixel);
      }
    }
  }
}


/***************************************************************************************
** Function name:           pushImage
** Description:             push 565 colour FLASH (PROGMEM) image into a defined area
***************************************************************************************/
void  TFT_eSprite::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data)
{
#ifdef ESP32
  pushImage(x, y, w, h, (uint16_t*) data);
#else
  // Partitioned memory FLASH processor
  if ((x >= _iwidth) || (y >= _iheight) || (w == 0) || (h == 0) || !_created) return;
  if ((x + w < 0) || (y + h < 0)) return;

  int32_t  xo = 0;
  int32_t  yo = 0;

  int32_t  xs = x;
  int32_t  ys = y;

  int32_t ws = w;
  int32_t hs = h;

  if (x < 0) { xo = -x; ws += x; xs = 0; }
  if (y < 0) { yo = -y; hs += y; ys = 0; }

  if (xs + ws >= (int32_t)_iwidth)  ws = _iwidth  - xs;
  if (ys + hs >= (int32_t)_iheight) hs = _iheight - ys;

  if (_bpp == 16) // Plot a 16 bpp image into a 16 bpp Sprite
  {
    for (int32_t yp = yo; yp < yo + hs; yp++)
    {
      x = xs;
      for (int32_t xp = xo; xp < xo + ws; xp++)
      {
        uint16_t color = pgm_read_word(data + xp + yp * w);
        if(_iswapBytes) color = color<<8 | color>>8;
        _img[x + ys * _iwidth] = color;
        x++;
      }
      ys++;
    }
  }

  else if (_bpp == 8) // Plot a 16 bpp image into a 8 bpp Sprite
  {
    for (int32_t yp = yo; yp < yo + hs; yp++)
    {
      x = xs;
      for (int32_t xp = xo; xp < xo + ws; xp++)
      {
        uint16_t color = pgm_read_word(data + xp + yp * w);
        if(_iswapBytes) color = color<<8 | color>>8;
        _img8[x + ys * _iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
        x++;
      }
      ys++;
    }
  }

  else if (_bpp == 4)
  {
    #ifdef TFT_eSPI_DEBUG
    Serial.println("TFT_eSprite::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data) not implemented");
    #endif
    return;
  }

  else // 1bpp
  {
    // _dwidth and _dheight bounds not checked (rounded up -iwidth and _iheight used)
    // Move coordinate rotation to support fn
    if (_rotation == 1)
    {
      int32_t tx = x;
      x = _dwidth - y - 1;
      y = tx;
    }
    else if (_rotation == 2)
    {
      x = _dwidth - x - 1;
      y = _dheight - y - 1;
    }
    else if (_rotation == 3)
    {
      int32_t tx = x;
      x = y;
      y = _dheight - tx - 1;
    }
    // Plot a 1bpp image into a 1bpp Sprite
    const uint8_t* pdata = (const uint8_t* ) data;
    uint32_t ww =  (w+7) & 0xFFF8;
    for (int32_t yp = 0; yp<h; yp++)
    {
      for (uint32_t xp = 0; xp<ww; xp+=8)
      {
        uint8_t pbyte = pgm_read_byte(pdata++);
        for (uint8_t xc = 0; xc < 8; xc++)
        {
          if (xp+xc<(uint32_t)w) drawPixel(x+xp+xc, y+yp, (pbyte<<xc) & 0x80);
        }
      }
    }
  }
#endif // if ESP32 else ESP8266 check
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
** Description:             Set the bounds of a window in the sprite
***************************************************************************************/
void TFT_eSprite::setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
{
  if (x0 > x1) swap_coord(x0, x1);
  if (y0 > y1) swap_coord(y0, y1);

  if ((x0 >= _dwidth) || (x1 < 0) || (y0 >= _dheight) || (y1 < 0))
  { // Point to that extra "off screen" pixel
    _xs = 0;
    _ys = _dheight;
    _xe = 0;
    _ye = _dheight;
  }
  else
  {
    if (x0 < 0) x0 = 0;
    if (x1 >= _dwidth) x1 = _dwidth - 1;
    if (y0 < 0) y0 = 0;
    if (y1 >= _dheight) y1 = _dheight - 1;

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
***************************************************************************************/
void TFT_eSprite::pushColor(uint32_t color)
{
  if (!_created ) return;

  // Write the colour to RAM in set window
  if (_bpp == 16)
    _img [_xptr + _yptr * _iwidth] = (uint16_t) (color >> 8) | (color << 8);

  else  if (_bpp == 8)
    _img8[_xptr + _yptr * _iwidth] = (uint8_t )((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);

  else if (_bpp == 4)
  {
    uint8_t c = (uint8_t)color & 0x0F;
    if ((_xptr & 0x01) == 0) {
      _img4[(_xptr + _yptr * _iwidth)>>1] = (c << 4) | (_img4[(_xptr + _yptr * _iwidth)>>1] & 0x0F);  // new color is in bits 7 .. 4
    }
    else {
      _img4[(_xptr + _yptr * _iwidth)>>1] = (_img4[(_xptr + _yptr * _iwidth)>>1] & 0xF0) | c; // new color is the low bits
    }
  }

  else drawPixel(_xptr, _yptr, color);

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
***************************************************************************************/
void TFT_eSprite::pushColor(uint32_t color, uint16_t len)
{
  if (!_created ) return;

  uint16_t pixelColor;

  if (_bpp == 16)
    pixelColor = (uint16_t) (color >> 8) | (color << 8);

  else  if (_bpp == 8)
    pixelColor = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;

  else pixelColor = (uint16_t) color; // for 1bpp or 4bpp

  while(len--) writeColor(pixelColor);
}


/***************************************************************************************
** Function name:           writeColor
** Description:             Write a pixel with pre-formatted colour to the set window
***************************************************************************************/
void TFT_eSprite::writeColor(uint16_t color)
{
  if (!_created ) return;

  // Write 16 bit RGB 565 encoded colour to RAM
  if (_bpp == 16) _img [_xptr + _yptr * _iwidth] = color;

  // Write 8 bit RGB 332 encoded colour to RAM
  else if (_bpp == 8) _img8[_xptr + _yptr * _iwidth] = (uint8_t) color;

  else if (_bpp == 4)
  {
    uint8_t c = (uint8_t)color & 0x0F;
    if ((_xptr & 0x01) == 0)
      _img4[(_xptr + _yptr * _iwidth)>>1] = (c << 4) | (_img4[(_xptr + _yptr * _iwidth)>>1] & 0x0F);  // new color is in bits 7 .. 4
    else
      _img4[(_xptr + _yptr * _iwidth)>>1] = (_img4[(_xptr + _yptr * _iwidth)>>1] & 0xF0) | c; // new color is the low bits (x is odd)
  }

  else drawPixel(_xptr, _yptr, color);

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
***************************************************************************************/
void TFT_eSprite::setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
  if ((x >= _iwidth) || (y >= _iheight) || !_created ) return;

  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }

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
***************************************************************************************/
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
  int32_t iw  = _iwidth;       // rounded up width of sprite

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
  if (_bpp == 16)
  {
    while (h--)
    { // move pixel lines (to, from, byte count)
      memmove( _img + typ, _img + fyp, w<<1);
      typ += iw;
      fyp += iw;
    }
  }
  else if (_bpp == 8)
  {
    while (h--)
    { // move pixel lines (to, from, byte count)
      memmove( _img8 + typ, _img8 + fyp, w);
      typ += iw;
      fyp += iw;
    }
  }
  else if (_bpp == 4)
  {
    // could optimize for scrolling by even # pixels using memove (later)
    if (dx >  0) { tx += w; fx += w; } // Start from right edge
    while (h--)
    { // move pixels one by one
      for (uint16_t xp = 0; xp < w; xp++)
      {
        if (dx <= 0) drawPixel(tx + xp, ty, readPixelValue(fx + xp, fy));
        if (dx >  0) drawPixel(tx - xp, ty, readPixelValue(fx - xp, fy));
      }
      if (dy <= 0)  { ty++; fy++; }
      else  { ty--; fy--; }
    }
  }
  else if (_bpp == 1 )
  {
    if (dx >  0) { tx += w; fx += w; } // Start from right edge
    while (h--)
    { // move pixels one by one
      for (uint16_t xp = 0; xp < w; xp++)
      {
        if (dx <= 0) drawPixel(tx + xp, ty, readPixelValue(fx + xp, fy));
        if (dx >  0) drawPixel(tx - xp, ty, readPixelValue(fx - xp, fy));
      }
      if (dy <= 0)  { ty++; fy++; }
      else  { ty--; fy--; }
    }
  }
  else return; // Not 1, 4, 8 or 16 bpp

  // Fill the gap left by the scrolling
  if (dx > 0) fillRect(_sx, _sy, dx, _sh, _scolor);
  if (dx < 0) fillRect(_sx + _sw + dx, _sy, -dx, _sh, _scolor);
  if (dy > 0) fillRect(_sx, _sy, _sw, dy, _scolor);
  if (dy < 0) fillRect(_sx, _sy + _sh + dy, _sw, -dy, _scolor);
}


/***************************************************************************************
** Function name:           fillSprite
** Description:             Fill the whole sprite with defined colour
***************************************************************************************/
void TFT_eSprite::fillSprite(uint32_t color)
{
  if (!_created ) return;

  // Use memset if possible as it is super fast
  if(( (uint8_t)color == (uint8_t)(color>>8) ) && _bpp == 16)
                    memset(_img,  (uint8_t)color, _iwidth * _iheight * 2);
  else if (_bpp == 8)
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    memset(_img8, (uint8_t)color, _iwidth * _iheight);
  }
  else if (_bpp == 4)
  {
    uint8_t c = ((color & 0x0F) | (((color & 0x0F) << 4) & 0xF0));
    memset(_img4, c, (_iwidth * _iheight) >> 1);
  }
  else if (_bpp == 1)
  {
    if(color) memset(_img8, 0xFF, (_iwidth>>3) * _iheight + 1);
    else      memset(_img8, 0x00, (_iwidth>>3) * _iheight + 1);
  }

  else fillRect(0, 0, _iwidth, _iheight, color);
}


/***************************************************************************************
** Function name:           setCursor
** Description:             Set the sprite text cursor x,y position
***************************************************************************************/
// Not needed - using TFT_eSPI class function and this->cursor_x/y
//void TFT_eSprite::setCursor(int16_t x, int16_t y)
//{
//  this->cursor_x = x;
//  this->cursor_y = y;
//}


/***************************************************************************************
** Function name:           width
** Description:             Return the width of sprite
***************************************************************************************/
// Return the size of the display
int16_t TFT_eSprite::width(void)
{
  if (!_created ) return 0;

  if (_bpp > 1) return _dwidth;

  if (_rotation == 1 || _rotation == 3) return _dheight;

  return _dwidth;
}


/***************************************************************************************
** Function name:           height
** Description:             Return the height of sprite
***************************************************************************************/
int16_t TFT_eSprite::height(void)
{
  if (!_created ) return 0;

  if (_bpp > 4) return _dheight;

  if (_rotation == 1 || _rotation == 3) return _dwidth;

  return _dheight;
}


/***************************************************************************************
** Function name:           setRotation
** Description:             Rotate coordinate frame for 1bpp sprite
***************************************************************************************/
// Does nothing for 8 and 16 bpp sprites. TODO allow rotation of these sprites
void TFT_eSprite::setRotation(uint8_t rotation)
{
  if (_bpp != 1) return;

  _rotation = rotation;
  if (rotation == 0 && _iwidth > _iheight) swap_coord(_iwidth, _iheight);
  if (rotation == 1 && _iwidth < _iheight) swap_coord(_iwidth, _iheight);
  if (rotation == 2 && _iwidth > _iheight) swap_coord(_iwidth, _iheight);
  if (rotation == 3 && _iwidth < _iheight) swap_coord(_iwidth, _iheight);
}


/***************************************************************************************
** Function name:           getRotation
** Description:             Get rotation for 1bpp sprite
***************************************************************************************/

uint8_t TFT_eSprite::getRotation(void)
{
  return _rotation;
}


/***************************************************************************************
** Function name:           drawPixel
** Description:             push a single pixel at an arbitrary position
***************************************************************************************/
void TFT_eSprite::drawPixel(int32_t x, int32_t y, uint32_t color)
{
  // Range checking
  if ((x < 0) || (y < 0) || !_created) return;
  if ((x >= _iwidth) || (y >= _iheight)) return;

  if (_bpp == 16)
  {
    color = (color >> 8) | (color << 8);
    _img[x+y*_iwidth] = (uint16_t) color;
  }
  else if (_bpp == 8)
  {
    _img8[x+y*_iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
  }
  else if (_bpp == 4)
  {
    uint8_t c = color & 0x0F;
    int index = (x+y*_iwidth)>>1;;
    if ((x & 0x01) == 0) {
      _img4[index] = (uint8_t)((c << 4) | (_img4[index] & 0x0F));
    }
    else {
      _img4[index] =  (uint8_t)(c | (_img4[index] & 0xF0));
    }
  }
  else // 1 bpp
  {
    if (_rotation == 1)
    {
      uint16_t tx = x;
      x = _dwidth - y - 1;
      y = tx;
    }
    else if (_rotation == 2)
    {
      x = _dwidth - x - 1;
      y = _dheight - y - 1;
    }
    else if (_rotation == 3)
    {
      uint16_t tx = x;
      x = y;
      y = _dheight - tx - 1;
    }

    if (color) _img8[(x + y * _bitwidth)>>3] |=  (0x80 >> (x & 0x7));
    else       _img8[(x + y * _bitwidth)>>3] &= ~(0x80 >> (x & 0x7));
  }
}


/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
***************************************************************************************/
void TFT_eSprite::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
  if (!_created ) return;

  bool steep = abs(y1 - y0) > abs(x1 - x0);
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
***************************************************************************************/
void TFT_eSprite::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color)
{

  if ((x < 0) || (x >= _iwidth) || (y >= _iheight) || !_created) return;

  if (y < 0) { h += y; y = 0; }

  if ((y + h) > _iheight) h = _iheight - y;

  if (h < 1) return;

  if (_bpp == 16)
  {
    color = (color >> 8) | (color << 8);
    int32_t yp = x + _iwidth * y;
    while (h--) {_img[yp] = (uint16_t) color; yp += _iwidth;}
  }
  else if (_bpp == 8)
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    while (h--) _img8[x + _iwidth * y++] = (uint8_t) color;
  }
  else if (_bpp == 4)
  {
    if ((x & 0x01) == 0)
    {
      uint8_t c = (uint8_t) (color & 0xF) << 4;
      while (h--) {
        _img4[(x + _iwidth * y)>>1] = (uint8_t) (c | (_img4[(x + _iwidth * y)>>1] & 0x0F));
        y++;
      }
    }
    else {
      uint8_t c = (uint8_t)color & 0xF;
      while (h--) {
        _img4[(x + _iwidth * y)>>1] = (uint8_t) (c | (_img4[(x + _iwidth * y)>>1] & 0xF0)); // x is odd; new color goes into the low bits.
        y++;
      }
    }
  }
  else
  {
    while (h--)
    {
      drawPixel(x, y, color);
      y++;
    }
  }
}


/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
***************************************************************************************/
void TFT_eSprite::drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color)
{

  if ((y < 0) || (x >= _iwidth) || (y >= _iheight) || !_created) return;

  if (x < 0) { w += x; x = 0; }

  if ((x + w) > _iwidth)  w = _iwidth  - x;

  if (w < 1) return;

  if (_bpp == 16)
  {
    color = (color >> 8) | (color << 8);
    while (w--) _img[_iwidth * y + x++] = (uint16_t) color;
  }
  else if (_bpp == 8)
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    memset(_img8+_iwidth * y + x, (uint8_t)color, w);
  }
  else if (_bpp == 4)
  {
    uint8_t c = (uint8_t)color & 0x0F;
    uint8_t c2 = (c | ((c << 4) & 0xF0));
    if ((x & 0x01) == 1)
    {
      drawPixel(x, y, color);
      x++; w--;
      if (w < 1)
        return;
    }

    if (((w + x) & 0x01) == 1)
    {
      // handle the extra one at the other end
      drawPixel(x + w - 1, y, color);
      w--;
      if (w < 1) return;
    }
    memset(_img4 + ((_iwidth * y + x) >> 1), c2, (w >> 1));
  }
  else {
    while (w--)
    {
      drawPixel(x, y, color);
      x++;
    }
  }
}


/***************************************************************************************
** Function name:           fillRect
** Description:             draw a filled rectangle
***************************************************************************************/
void TFT_eSprite::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
  if (!_created ) return;

  if ((x >= _iwidth) || (y >= _iheight)) return;

  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }

  if ((x + w) > _iwidth)  w = _iwidth  - x;
  if ((y + h) > _iheight) h = _iheight - y;

  if ((w < 1) || (h < 1)) return;

  int32_t yp = _iwidth * y + x;

  if (_bpp == 16)
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
  else if (_bpp == 8)
  {
    color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
    while (h--)
    {
      memset(_img8 + yp, (uint8_t)color, w);
      yp += _iwidth;
    }
  }
  else if (_bpp == 4)
  {
    uint8_t c1 = (uint8_t)color & 0x0F;
    uint8_t c2 = c1 | ((c1 << 4) & 0xF0);
    if ((x & 0x01) == 0 && (w & 0x01) == 0)
    {
      yp = (yp >> 1);
      while (h--)
      {
        memset(_img4 + yp, c2, (w>>1));
        yp += (_iwidth >> 1);
      }
    }
    else if ((x & 0x01) == 0)
    {
      // same as above but you have a hangover on the right.
      yp = (yp >> 1);
      while (h--)
      {
        if (w > 1)
          memset(_img4 + yp, c2, (w-1)>>1);
        // handle the rightmost pixel by calling drawPixel
        drawPixel(x+w-1, y+h, c1);
        yp += (_iwidth >> 1);
      }
    }
    else if ((w & 0x01) == 1)
    {
      yp = (yp + 1) >> 1;
      while (h--) {
        drawPixel(x, y+h, color & 0x0F);
        if (w > 1)
          memset(_img4 + yp, c2, (w-1)>>1);
        // same as above but you have a hangover on the left instead
        yp += (_iwidth >> 1);
      }
    }
    else
    {
      yp = (yp + 1) >> 1;
      while (h--) {
        drawPixel(x, y+h, color & 0x0F);
        if (w > 1) drawPixel(x+w-1, y+h, color & 0x0F);
        if (w > 2)
          memset(_img4 + yp, c2, (w-2)>>1);
        // maximal hacking, single pixels on left and right.
        yp += (_iwidth >> 1);
      }
    }
  }
  else
  {
    while (h--)
    {
      int32_t ww = w;
      int32_t xx = x;
      while (ww--) drawPixel(xx++, y, color);
      y++;
    }
  }
}


/***************************************************************************************
** Function name:           write
** Description:             draw characters piped through serial stream
***************************************************************************************/
size_t TFT_eSprite::write(uint8_t utf8)
{
  uint16_t uniCode = decodeUTF8(utf8);

  if (!uniCode) return 1;

  if (utf8 == '\r') return 1;

#ifdef SMOOTH_FONT
  if(this->fontLoaded)
  {
    if (uniCode < 32 && utf8 != '\n') return 1;

    //Serial.print("Decoded Unicode = 0x");Serial.println(unicode,HEX);

    drawGlyph(uniCode);

    return 1;
  }
#endif

  if (!_created ) return 1;

  if (uniCode == '\n') uniCode+=22; // Make it a valid space character to stop errors
  else if (uniCode < 32) return 1;

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
    if (utf8 > 127) return 1;

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
      if (utf8 > 127) return 1;
      // Uses the fontinfo struct array to avoid lots of 'if' or 'switch' statements
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
  if (textfont==1) return 1;
#endif

  height = height * textsize;

  if (utf8 == '\n')
  {
    this->cursor_y += height;
    this->cursor_x  = 0;
  }
  else
  {
    if (textwrapX && (this->cursor_x + width * textsize > _iwidth))
    {
      this->cursor_y += height;
      this->cursor_x = 0;
    }
    if (textwrapY && (this->cursor_y >= _iheight)) this->cursor_y = 0;
    this->cursor_x += drawChar(uniCode, this->cursor_x, this->cursor_y, textfont);
  }

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  } // Custom GFX font
  else
  {
    if(utf8 == '\n') {
      this->cursor_x  = 0;
      this->cursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else {
      if (uniCode > pgm_read_word(&gfxFont->last )) return 1;
      if (uniCode < pgm_read_word(&gfxFont->first)) return 1;

      uint8_t   c2    = uniCode - pgm_read_word(&gfxFont->first);
      GFXglyph *glyph = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c2]);
      uint8_t   w     = pgm_read_byte(&glyph->width),
                h     = pgm_read_byte(&glyph->height);
      if((w > 0) && (h > 0)) { // Is there an associated bitmap?
        int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);
        if(textwrapX && ((this->cursor_x + textsize * (xo + w)) > _iwidth)) {
          // Drawing character would go off right edge; wrap to new line
          this->cursor_x  = 0;
          this->cursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
        }
        if (textwrapY && (this->cursor_y >= _iheight)) this->cursor_y = 0;
        drawChar(this->cursor_x, this->cursor_y, uniCode, textcolor, textbgcolor, textsize);
      }
      this->cursor_x += pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
    }
  }
#endif // LOAD_GFXFF
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  return 1;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a single character in the Adafruit GLCD or freefont
***************************************************************************************/
void TFT_eSprite::drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size)
{
  if (!_created ) return;

  if ((x >= _iwidth)            || // Clip right
      (y >= _iheight)           || // Clip bottom
      ((x + 6 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
    return;

  if (c < 32) return;
#ifdef LOAD_GLCD
//>>>>>>>>>>>>>>>>>>
#ifdef LOAD_GFXFF
  if(!gfxFont) { // 'Classic' built-in font
#endif
//>>>>>>>>>>>>>>>>>>

  bool fillbg = (bg != color);

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
    if ((c >= pgm_read_word(&gfxFont->first)) && (c <= pgm_read_word(&gfxFont->last )))
    {
//>>>>>>>>>>>>>>>>>>>>>>>>>>>

      c -= pgm_read_word(&gfxFont->first);
      GFXglyph *glyph  = &(((GFXglyph *)pgm_read_dword(&gfxFont->glyph))[c]);
      uint8_t  *bitmap = (uint8_t *)pgm_read_dword(&gfxFont->bitmap);

      uint32_t bo = pgm_read_word(&glyph->bitmapOffset);
      uint8_t  w  = pgm_read_byte(&glyph->width),
               h  = pgm_read_byte(&glyph->height);
               //xa = pgm_read_byte(&glyph->xAdvance);
      int8_t   xo = pgm_read_byte(&glyph->xOffset),
               yo = pgm_read_byte(&glyph->yOffset);
      uint8_t  xx, yy, bits=0, bit=0;
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
***************************************************************************************/
  // Any UTF-8 decoding must be done before calling drawChar()
int16_t TFT_eSprite::drawChar(uint16_t uniCode, int32_t x, int32_t y)
{
  return drawChar(uniCode, x, y, textfont);
}

  // Any UTF-8 decoding must be done before calling drawChar()
int16_t TFT_eSprite::drawChar(uint16_t uniCode, int32_t x, int32_t y, uint8_t font)
{
  if (!_created ) return 0;

  if (!uniCode) return 0;

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
      if((uniCode >= pgm_read_word(&gfxFont->first)) && (uniCode <= pgm_read_word(&gfxFont->last) ))
      {
        uint16_t   c2    = uniCode - pgm_read_word(&gfxFont->first);
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

  int32_t width  = 0;
  int32_t height = 0;
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
      flash_address = pgm_read_dword( (const void*) (pgm_read_dword( &(fontdata[font].chartbl ) ) + uniCode*sizeof(void *)) );
      width = pgm_read_byte( (uint8_t *)pgm_read_dword( &(fontdata[font].widthtbl ) ) + uniCode );
      height= pgm_read_byte( &fontdata[font].height );
    }
  }
#endif

  int32_t w = width;
  int32_t pX      = 0;
  int32_t pY      = y;
  uint8_t line = 0;

#ifdef LOAD_FONT2 // chop out code if we do not need it
  if (font == 2) {
    w = w + 6; // Should be + 7 but we need to compensate for width increment
    w = w / 8;
    if (x + width * textsize >= _iwidth) return width * textsize ;

    for (int32_t i = 0; i < height; i++)
    {
      if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize, textbgcolor);

      for (int32_t k = 0; k < w; k++)
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
    int16_t color = textcolor;
    if (_bpp == 16) color = (textcolor >> 8) | (textcolor << 8);
    else if (_bpp == 8) color = ((textcolor & 0xE000)>>8 | (textcolor & 0x0700)>>6 | (textcolor & 0x0018)>>3);
    int32_t px = 0, py = pY; // To hold character block start and end column and row values
    int32_t pc = 0; // Pixel count
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
***************************************************************************************/
void TFT_eSprite::drawGlyph(uint16_t code)
{
  if (code < 0x21)
  {
    if (code == 0x20) {
      if (_created) this->cursor_x += this->gFont.spaceWidth;
      else this->cursor_x += this->gFont.spaceWidth;
      return;
    }

    if (code == '\n') {
      if (_created)
      {
        this->cursor_x = 0;
        this->cursor_y += this->gFont.yAdvance;
        if (this->cursor_y >= _height) this->cursor_y = 0;
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
  bool found = this->getUnicodeIndex(code, &gNum);

  uint16_t fg = this->textcolor;
  uint16_t bg = this->textbgcolor;

  if (found)
  {

    bool newSprite = !_created;

    if (newSprite)
    {
      createSprite(this->gWidth[gNum], this->gFont.yAdvance);
      if(bg) fillSprite(bg);
      this->cursor_x = -this->gdX[gNum];
      this->cursor_y = 0;
    }
    else
    {
      if( this->textwrapX && ((this->cursor_x + this->gWidth[gNum] + this->gdX[gNum]) > _iwidth)) {
        this->cursor_y += this->gFont.yAdvance;
        this->cursor_x = 0;
      }

      if( this->textwrapY && ((this->cursor_y + this->gFont.yAdvance) > _iheight)) this->cursor_y = 0;

      if ( this->cursor_x == 0) this->cursor_x -= this->gdX[gNum];

    }

    uint8_t* pbuffer = nullptr;
    const uint8_t* gPtr = (const uint8_t*) this->gFont.gArray;

#ifdef FONT_FS_AVAILABLE
    if (this->fs_font) {
      this->fontFile.seek(this->gBitmap[gNum], fs::SeekSet); // This is slow for a significant position shift!
      pbuffer =  (uint8_t*)malloc(this->gWidth[gNum]);
    }
#endif

    int16_t  xs = 0;
    uint16_t dl = 0;
    uint8_t pixel = 0;

    for (int32_t y = 0; y < this->gHeight[gNum]; y++)
    {
#ifdef FONT_FS_AVAILABLE
      if (this->fs_font) {
        this->fontFile.read(pbuffer, this->gWidth[gNum]);
      }
#endif
      for (int32_t x = 0; x < this->gWidth[gNum]; x++)
      {
#ifdef FONT_FS_AVAILABLE
        if (this->fs_font) {
          pixel = pbuffer[x];
        }
        else
#endif
        pixel = pgm_read_byte(gPtr + this->gBitmap[gNum] + x + this->gWidth[gNum] * y);

        if (pixel)
        {
          if (pixel != 0xFF)
          {
            if (dl) { drawFastHLine( xs, y + this->cursor_y + this->gFont.maxAscent - this->gdY[gNum], dl, fg); dl = 0; }
            if (_bpp != 1) drawPixel(x + this->cursor_x + this->gdX[gNum], y + this->cursor_y + this->gFont.maxAscent - this->gdY[gNum], alphaBlend(pixel, fg, bg));
            else if (pixel>127) drawPixel(x + this->cursor_x + this->gdX[gNum], y + this->cursor_y + this->gFont.maxAscent - this->gdY[gNum], fg);
          }
          else
          {
            if (dl==0) xs = x + this->cursor_x + this->gdX[gNum];
            dl++;
          }
        }
        else
        {
          if (dl) { drawFastHLine( xs, y + this->cursor_y + this->gFont.maxAscent - this->gdY[gNum], dl, fg); dl = 0; }
        }
      }
      if (dl) { drawFastHLine( xs, y + this->cursor_y + this->gFont.maxAscent - this->gdY[gNum], dl, fg); dl = 0; }
    }

    if (pbuffer) free(pbuffer);

    if (newSprite)
    {
      pushSprite(this->cursor_x + this->gdX[gNum], this->cursor_y, bg);
      deleteSprite();
      this->cursor_x += this->gxAdvance[gNum];
    }
    else this->cursor_x += this->gxAdvance[gNum];
  }
  else
  {
    // Not a Unicode in font so draw a rectangle and move on cursor
    drawRect(this->cursor_x, this->cursor_y + this->gFont.maxAscent - this->gFont.ascent, this->gFont.spaceWidth, this->gFont.ascent, fg);
    this->cursor_x += this->gFont.spaceWidth + 1;
  }
}


/***************************************************************************************
** Function name:           printToSprite
** Description:             Write a string to the sprite cursor position
***************************************************************************************/
void TFT_eSprite::printToSprite(String string)
{
  if(!this->fontLoaded) return;
  uint16_t len = string.length();
  char cbuffer[len + 1];              // Add 1 for the null
  string.toCharArray(cbuffer, len + 1); // Add 1 for the null, otherwise characters get dropped
  printToSprite(cbuffer, len);
  //printToSprite((char*)string.c_str(), string.length());
}


/***************************************************************************************
** Function name:           printToSprite
** Description:             Write a string to the sprite cursor position
***************************************************************************************/
void TFT_eSprite::printToSprite(char *cbuffer, uint16_t len) //String string)
{
  if(!this->fontLoaded) return;

  uint16_t n = 0;
  bool newSprite = !_created;

  if (newSprite)
  {
    int16_t sWidth = 1;
    uint16_t index = 0;

    while (n < len)
    {
      uint16_t unicode = decodeUTF8((uint8_t*)cbuffer, &n, len - n);
      if (this->getUnicodeIndex(unicode, &index))
      {
        if (n == 0) sWidth -= this->gdX[index];
        if (n == len-1) sWidth += ( this->gWidth[index] + this->gdX[index]);
        else sWidth += this->gxAdvance[index];
      }
      else sWidth += this->gFont.spaceWidth + 1;
    }

    createSprite(sWidth, this->gFont.yAdvance);

    if (this->textbgcolor != TFT_BLACK) fillSprite(this->textbgcolor);
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
  { // The sprite had to be created so place at TFT cursor
    pushSprite(_tft->cursor_x, _tft->cursor_y);
    deleteSprite();
  }
}


/***************************************************************************************
** Function name:           printToSprite
** Description:             Print character in a Sprite, create sprite if needed
***************************************************************************************/
int16_t TFT_eSprite::printToSprite(int16_t x, int16_t y, uint16_t index)
{
  bool newSprite = !_created;
  int16_t sWidth = this->gWidth[index];

  if (newSprite)
  {
    createSprite(sWidth, this->gFont.yAdvance);

    if (this->textbgcolor != TFT_BLACK) fillSprite(this->textbgcolor);

    drawGlyph(this->gUnicode[index]);

    pushSprite(x + this->gdX[index], y, this->textbgcolor);
    deleteSprite();
  }

  else drawGlyph(this->gUnicode[index]);

  return this->gxAdvance[index];
}
#endif
