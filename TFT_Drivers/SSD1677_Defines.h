// SSD1677_Defines.h
#ifndef __SSD1677_DEFINES_H__
#define __SSD1677_DEFINES_H__

#ifndef EPD_WIDTH
#define EPD_WIDTH 800 // Adjust based on actual display width
#endif

#ifndef EPD_HEIGHT
#define EPD_HEIGHT 480 // Adjust based on actual display height
#endif

#ifndef TFT_WIDTH
#define TFT_WIDTH EPD_WIDTH
#endif

#ifndef TFT_HEIGHT
#define TFT_HEIGHT EPD_HEIGHT
#endif

#define EPD_COLOR_DEPTH 1

#define EPD_NOP 0xFF
#define EPD_PNLSET 0xFF // No direct panel set command for SSD1677
#define EPD_DISPON 0xFF
#define EPD_DISPOFF 0xFF
#define EPD_SLPIN 0x10
#define EPD_SLPOUT 0xFF
#define EPD_PTLIN 0xFF
#define EPD_PTLOUT 0xFF
#define EPD_PTLW 0xFF

#define TFT_SWRST 0x12
#define TFT_CASET 0x44
#define TFT_PASET 0x45
#define TFT_RAMWR 0x24
#define TFT_RAMRD 0xFF
#define TFT_INVON 0xFF
#define TFT_INVOFF 0xFF

#define TFT_INIT_DELAY 0

#ifdef TFT_BUSY
#define CHECK_BUSY()                  \
    do                                \
    {                                 \
        while (digitalRead(TFT_BUSY)) \
            ;                         \
    } while (0)
#else
#define CHECK_BUSY()
#endif

#define EPD_UPDATE()        \
    do                      \
    {                       \
        writecommand(0x22); \
        writedata(0xF7);    \
        writecommand(0x20); \
        CHECK_BUSY();       \
    } while (0)

#define EPD_SLEEP()         \
    do                      \
    {                       \
        writecommand(0x10); \
        writedata(0x01);    \
        delay(100);         \
    } while (0)

#define EPD_INIT()                         \
    do                                     \
    {                                      \
        digitalWrite(TFT_RST, LOW);        \
        delay(10);                         \
        digitalWrite(TFT_RST, HIGH);       \
        delay(10);                         \
        CHECK_BUSY();                      \
        writecommand(0x12);                \
        CHECK_BUSY();                      \
        writecommand(0x18);                \
        writedata(0x80);                   \
        writecommand(0x0C);                \
        writedata(0xAE);                   \
        writedata(0xC7);                   \
        writedata(0xC3);                   \
        writedata(0xC0);                   \
        writedata(0x80);                   \
        writecommand(0x01);                \
        writedata((EPD_WIDTH - 1) % 256);  \
        writedata((EPD_WIDTH - 1) / 256);  \
        writedata(0x02);                   \
        writecommand(0x3C);                \
        writedata(0x01);                   \
        writecommand(0x11);                \
        writedata(0x03);                   \
        writecommand(0x44);                \
        writedata(0x00);                   \
        writedata(0x00);                   \
        writedata((EPD_HEIGHT - 1) % 256); \
        writedata((EPD_HEIGHT - 1) / 256); \
        writecommand(0x45);                \
        writedata(0x00);                   \
        writedata(0x00);                   \
        writedata((EPD_WIDTH - 1) % 256);  \
        writedata((EPD_WIDTH - 1) / 256);  \
        writecommand(0x4E);                \
        writedata(0x00);                   \
        writedata(0x00);                   \
        writecommand(0x4F);                \
        writedata(0x00);                   \
        writedata(0x00);                   \
        CHECK_BUSY();                      \
    } while (0)

#define EPD_WAKEUP() EPD_INIT()

#define EPD_SET_WINDOW(x1, y1, x2, y2) \
    do                                 \
    {                                  \
        writecommand(0x44);            \
        writedata((x1) % 256);         \
        writedata((x1) / 256);         \
        writedata((x2) % 256);         \
        writedata((x2) / 256);         \
        writecommand(0x45);            \
        writedata((y1) % 256);         \
        writedata((y1) / 256);         \
        writedata((y2) % 256);         \
        writedata((y2) / 256);         \
    } while (0)

#define EPD_PUSH_NEW_COLORS(w, h, colors)   \
    do                                      \
    {                                       \
        writecommand(0x24);                 \
        for (int i = 0; i < w * h / 8; i++) \
        {                                   \
            writedata(colors[i]);           \
        }                                   \
    } while (0)

#define EPD_PUSH_NEW_COLORS(w, h, colors)   \
    do                                      \
    {                                       \
        writecommand(0x24);                 \
        for (int i = 0; i < w * h / 8; i++) \
        {                                   \
            writedata(colors[i]);           \
        }                                   \
    } while (0)

#define EPD_PUSH_NEW_COLORS_FLIP(w, h, colors)                         \
    do                                                                 \
    {                                                                  \
        writecommand(0x24);                                            \
        uint16_t bytes_per_row = (w) / 8;                              \
        for (uint16_t row = 0; row < (h); row++)                       \
        {                                                              \
            uint16_t start = row * bytes_per_row;                      \
            for (uint16_t col = 0; col < bytes_per_row; col++)         \
            {                                                          \
                uint8_t b = colors[start + (bytes_per_row - 1 - col)]; \
                b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);             \
                b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);             \
                b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);             \
                writedata(b);                                          \
            }                                                          \
        }                                                              \
    } while (0)

// Macro to push old color data (red RAM or background)
#define EPD_PUSH_OLD_COLORS(w, h, colors) \
    do                                    \
    {                                     \
    } while (0)

#define EPD_PUSH_OLD_COLORS_FLIP(w, h, colors) \
    do                                         \
    {                                          \
    } while (0)

#endif