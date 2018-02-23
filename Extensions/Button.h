/***************************************************************************************
// The following button class has been ported over from the Adafruit_GFX library so
// should be compatible.
// A slightly different implementation in this TFT_eSPI library allows the button
// legends to be in any font
***************************************************************************************/

class TFT_eSPI_Button {

 public:
  TFT_eSPI_Button(void);
  // "Classic" initButton() uses center & size
  void     initButton(TFT_eSPI *gfx, int16_t x, int16_t y,
  uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
  uint16_t textcolor, char *label, uint8_t textsize);

  // New/alt initButton() uses upper-left corner & size
  void     initButtonUL(TFT_eSPI *gfx, int16_t x1, int16_t y1,
  uint16_t w, uint16_t h, uint16_t outline, uint16_t fill,
  uint16_t textcolor, char *label, uint8_t textsize);
  void     drawButton(boolean inverted = false);
  boolean  contains(int16_t x, int16_t y);

  void     press(boolean p);
  boolean  isPressed();
  boolean  justPressed();
  boolean  justReleased();

 private:
  TFT_eSPI *_gfx;
  int16_t  _x1, _y1; // Coordinates of top-left corner
  uint16_t _w, _h;
  uint8_t  _textsize;
  uint16_t _outlinecolor, _fillcolor, _textcolor;
  char     _label[10];

  boolean  currstate, laststate;
};
