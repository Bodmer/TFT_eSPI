
#ifndef EPD_WIDTH
#define EPD_WIDTH 800
#endif

#ifndef EPD_HEIGHT
#define EPD_HEIGHT 480
#endif

#ifndef TFT_WIDTH
#define TFT_WIDTH EPD_WIDTH
#endif

#ifndef TFT_HEIGHT
#define TFT_HEIGHT EPD_HEIGHT
#endif

#define EPD_COLOR_DEPTH 1

#define EPD_NOP 0xFF

#define EPD_PNLSET 0x00

#define EPD_DISPON 0x04
#define EPD_DISPOFF 0x03

#define EPD_SLPIN 0x07
#define EPD_SLPOUT 0xFF

#define EPD_PTLIN 0x90  // Partial display in
#define EPD_PTLOUT 0x91 // Partial display out

#define EPD_PTLW 0x13

#define TFT_SWRST 0xFF
#define TFT_CASET 0xFF
#define TFT_PASET 0xFF
#define TFT_RAMWR 0xFF
#define TFT_RAMRD 0xFF
#define TFT_INVON EPD_DISPON
#define TFT_INVOFF EPD_DISPOFF

#define TFT_INIT_DELAY 0

#ifdef TFT_BUSY
#define CHECK_BUSY()               \
    do                             \
    {                              \
        if (digitalRead(TFT_BUSY)) \
            break;                 \
    } while (true)
#else
#define CHECK_BUSY()
#endif

#define EPD_UPDATE()        \
    do                      \
    {                       \
        writecommand(0x12); \
        CHECK_BUSY();       \
    } while (0)

#define EPD_SLEEP()         \
    do                      \
    {                       \
        writecommand(0X50); \
        writedata(0xf7);    \
        writecommand(0x02); \
        CHECK_BUSY();       \
        writecommand(0x07); \
        writedata(0xA5);    \
    } while (0)

#define EPD_WAKEUP()                 \
    do                               \
    {                                \
        digitalWrite(TFT_RST, LOW);  \
        delay(10);                   \
        digitalWrite(TFT_RST, HIGH); \
        delay(10);                   \
        writecommand(0x04);          \
        delay(100);                  \
        CHECK_BUSY();                \
        writecommand(0xE0);          \
        writedata(0x02);             \
        writecommand(0xE5);          \
        writedata(0x6E);             \
    } while (0)

#define EPD_SET_WINDOW(x1, y1, x2, y2) \
    do                                 \
    {                                  \
        writecommand(0x50);            \
        writedata(0xA9);               \
        writedata(0x07);               \
        writecommand(0x91);            \
        writecommand(0x90);            \
        writedata(x1 / 256);           \
        writedata(x1 % 256);           \
        writedata(x2 / 256);           \
        writedata(x2 % 256 - 1);       \
        writedata(y1 / 256);           \
        writedata(y1 % 256);           \
        writedata(y2 / 256);           \
        writedata(y2 % 256 - 1);       \
        writedata(0x01);               \
    } while (0)

#define EPD_PUSH_NEW_COLORS(w, h, colors)   \
    do                                      \
    {                                       \
        writecommand(0x13);                 \
        for (int i = 0; i < w * h / 8; i++) \
        {                                   \
            writedata(colors[i]);           \
        }                                   \
    } while (0)

#define EPD_PUSH_OLD_COLORS(w, h, colors)   \
    do                                      \
    {                                       \
        writecommand(0x10);                 \
        for (int i = 0; i < w * h / 8; i++) \
        {                                   \
            writedata(colors[i]);           \
        }                                   \
    } while (0)