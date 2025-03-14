#include "TFT_eSPI.h"

#ifdef GDEM075T7_DRIVER
#include "TFT_Drivers/GDEM075T7_Defines.h"
#endif

class EPaper : public TFT_eSprite
{
public:
    explicit EPaper();

    void begin(uint8_t tc = TAB_COLOUR);

    void update();
    void update(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);

    void sleep();
    void wake();

private:
    bool _sleep;
};

#include "EPaper.cpp"
