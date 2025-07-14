EPaper::EPaper() : _sleep(false), TFT_eSprite(this)
{
    setColorDepth(EPD_COLOR_DEPTH);
    createSprite(_width, _height, 1);
}

void EPaper::begin(uint8_t tc)
{
    setBitmapColor(1, 0);
    setTextFont(1);
    setTextColor(TFT_BLACK, TFT_WHITE, true);
    init(tc);
    fillSprite(1);
#ifdef EPD_HORIZONTAL_MIRROR
    EPD_PUSH_OLD_COLORS_FLIP(_width, _height, _img8);
    fillSprite(0);
    EPD_PUSH_NEW_COLORS_FLIP(_width, _height, _img8);
#else
    EPD_PUSH_OLD_COLORS(_width, _height, _img8);
    fillSprite(0);
    EPD_PUSH_NEW_COLORS(_width, _height, _img8);
#endif
    EPD_UPDATE();
    EPD_WAKEUP();
}

void EPaper::update()
{
    wake();
    EPD_SET_WINDOW(0, 0, (_width - 1), (_height - 1));
#ifdef EPD_HORIZONTAL_MIRROR
    EPD_PUSH_NEW_COLORS_FLIP(_width, _height, _img8);
#else
    EPD_PUSH_NEW_COLORS(_width, _height, _img8);
#endif
    EPD_UPDATE();
    sleep();
}

void EPaper::update(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data)
{
    if (_sleep)
    {
        EPD_WAKEUP();
        _sleep = false;
    }
    uint8_t *p = (uint8_t *)data;
    // only support x, y multiple of 8 (floor)
    x = (x / 8) * 8;
    y = (y / 8) * 8;
    pushImage(x, y, w, h, (uint16_t *)p);
    EPD_SET_WINDOW(x, y, (x + w - 1), (y + h - 1));
#ifdef EPD_HORIZONTAL_MIRROR
    EPD_PUSH_NEW_COLORS_FLIP(w, h, p);
#else
    EPD_PUSH_NEW_COLORS(w, h, p);
#endif
    EPD_UPDATE();
    sleep();
}

void EPaper::sleep()
{
    if (_sleep)
        return;
    EPD_SLEEP();
    _sleep = true;
}

void EPaper::wake()
{
    if (!_sleep)
        return;
    EPD_WAKEUP();
    _sleep = false;
}