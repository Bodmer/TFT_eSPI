#ifdef CHSCX6X_DRIVER
#include "Touch_Drivers/CHSCX6X.cpp"
#else
bool TFT_eSPI::getTouch(int32_t *x, int32_t *y, uint16_t threshold)
{
    return false;
}

bool TFT_eSPI::getTouchRaw(int32_t *x, int32_t *y)
{
    return false;
}

uint16_t TFT_eSPI::getTouchRawZ(void)
{
    return 0;
}

void TFT_eSPI::convertRawXY(int32_t *x, int32_t *y)
{
}
void TFT_eSPI::calibrateTouch(uint16_t *data, uint32_t color_fg, uint32_t color_bg, uint8_t size)
{
}

void TFT_eSPI::setTouch(uint16_t *data)
{
}
#endif