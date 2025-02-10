#ifdef CHSCX6X_DRIVER
#include "Touch_Drivers/CHSCX6X.h"
#else
public:
  bool  getTouch(int32_t *x, int32_t *y, uint16_t threshold = 600);
#endif