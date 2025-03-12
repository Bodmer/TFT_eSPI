public:

  bool  getTouch(int32_t *x, int32_t *y, uint16_t threshold = 600);

  /*Considering the capacitive screen implementation, the following functions are obsolete and are only implemented empty to maintain compatibility*/
  bool getTouchRaw(int32_t *x, int32_t *y);
  uint16_t getTouchRawZ(void);
  void convertRawXY(int32_t *x, int32_t *y);
  void calibrateTouch(uint16_t *data, uint32_t color_fg, uint32_t color_bg, uint8_t size);
  void setTouch(uint16_t *data);
