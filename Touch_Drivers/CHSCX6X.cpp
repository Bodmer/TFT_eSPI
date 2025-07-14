bool TFT_eSPI::getTouch(int32_t *x, int32_t *y, uint16_t threshold)
{

  int32_t _x = 0, _y = 0;
  uint8_t temp[CHSC6X_READ_POINT_LEN] = {0};
  uint8_t read_len = Wire.requestFrom(CHSC6X_I2C_ID, CHSC6X_READ_POINT_LEN);

#ifdef TOUCH_INT
  if (digitalRead(TOUCH_INT) != LOW)
  {
    delay(1);
    if (digitalRead(TOUCH_INT) != LOW)
      return false;
  }
#endif

  if (read_len == CHSC6X_READ_POINT_LEN)
  {
    TOUCH_WIRE.readBytes(temp, read_len);
    if (temp[0] == 0x01)
    {
      _x = temp[2];
      _y = temp[4];
    }
  }

  // rotation correction
  switch (rotation % 4)
  {
  case 1:
    *x = _y;
    *y = _height - _x;
    *x = (*x < 0) ? 0 : (*x > _height) ? _height
                                       : *x;
    *y = (*y < 0) ? 0 : (*y > _width) ? _width
                                      : *y;
    break;
  case 2:
    *x = _width - _x;
    *y = _height - _y;
    *x = (*x < 0) ? 0 : (*x > _width) ? _width
                                      : *x;
    *y = (*y < 0) ? 0 : (*y > _height) ? _height
                                       : *y;
    break;
  case 3:
    *x = _width - _y;
    *y = _x;
    *x = (*x < 0) ? 0 : (*x > _height) ? _height
                                       : *x;
    *y = (*y < 0) ? 0 : (*y > _width) ? _width
                                      : *y;
    break;
  default:
    *x = _x;
    *y = _y;
    *x = (*x < 0) ? 0 : (*x > _width) ? _width
                                      : *x;
    *y = (*y < 0) ? 0 : (*y > _height) ? _height
                                       : *y;
    break;
  }

  // orientation correction
  *x = *x - _xDatum;
  *y = *y - _yDatum;

  return true;
}

bool TFT_eSPI::getTouchRaw(int32_t *x, int32_t *y)
{
#pragma warning "This function is obsolete and is only implemented empty to maintain compatibility"
  return false;
}

uint16_t TFT_eSPI::getTouchRawZ(void)
{
#pragma warning "This function is obsolete and is only implemented empty to maintain compatibility"
  return 0;
}

void TFT_eSPI::convertRawXY(int32_t *x, int32_t *y)
{
#pragma warning "This function is obsolete and is only implemented empty to maintain compatibility"
}
void TFT_eSPI::calibrateTouch(uint16_t *data, uint32_t color_fg, uint32_t color_bg, uint8_t size)
{
#pragma warning "This function is obsolete and is only implemented empty to maintain compatibility"
}

void TFT_eSPI::setTouch(uint16_t *data)
{
#pragma warning "This function is obsolete and is only implemented empty to maintain compatibility"
}