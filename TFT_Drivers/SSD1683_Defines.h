#ifndef __SSD1680_DEFINES_H__
#define __SSD1680_DEFINES_H__

// Define screen resolution
#ifndef EPD_WIDTH
#define EPD_WIDTH 128
#endif

#ifndef EPD_HEIGHT
#define EPD_HEIGHT 296
#endif

#ifndef TFT_WIDTH
#define TFT_WIDTH EPD_WIDTH
#endif

#ifndef TFT_HEIGHT
#define TFT_HEIGHT EPD_HEIGHT
#endif

// Define color depth (1 bit for e-ink display)
#define EPD_COLOR_DEPTH 1

// Define no operation command
#define EPD_NOP 0xFF

// Define common command macros
#define EPD_PNLSET 0x01  // Driver output control
#define EPD_DISPON 0xFF  // No direct display on command
#define EPD_DISPOFF 0xFF // No direct display off command
#define EPD_SLPIN 0x10   // Enter deep sleep
#define EPD_SLPOUT 0xFF  // No direct sleep out command
#define EPD_PTLIN 0x3C   // Partial display in (BorderWaveform)
#define EPD_PTLOUT 0xFF  // No direct partial display out command
#define EPD_PTLW 0x4E    // Set RAM address counter

#define TFT_SWRST 0x12  // Software reset
#define TFT_CASET 0x44  // Set RAM X address start/end position
#define TFT_PASET 0x45  // Set RAM Y address start/end position
#define TFT_RAMWR 0x24  // Write to RAM
#define TFT_RAMRD 0xFF  // No direct read RAM command
#define TFT_INVON 0xFF  // No inversion on command
#define TFT_INVOFF 0xFF // No inversion off command

#define TFT_INIT_DELAY 0

// Macro to check BUSY signal
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

// Macro to update display
#define EPD_UPDATE()        \
    do                      \
    {                       \
        writecommand(0x22); \
        writedata(0xF7);    \
        writecommand(0x20); \
        CHECK_BUSY();       \
    } while (0)

// Macro to enter deep sleep
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
        writecommand(0x01);                \
        writedata((EPD_HEIGHT - 1) % 256); \
        writedata((EPD_HEIGHT - 1) / 256); \
        writedata(0x00);                   \
        writecommand(0x11);                \
        writedata(0x01);                   \
        writecommand(0x44);                \
        writedata(0x00);                   \
        writedata(EPD_WIDTH / 8 - 1);      \
        writecommand(0x45);                \
        writedata((EPD_HEIGHT - 1) % 256); \
        writedata((EPD_HEIGHT - 1) / 256); \
        writedata(0x00);                   \
        writedata(0x00);                   \
        writecommand(0x3C);                \
        writedata(0x05);                   \
        writecommand(0x21);                \
        writedata(0x40);                   \
        writedata(0x00);                   \
        writecommand(0x18);                \
        writedata(0x80);                   \
        writecommand(0x4E);                \
        writedata(0x00);                   \
        writecommand(0x4F);                \
        writedata((EPD_HEIGHT - 1) % 256); \
        writedata((EPD_HEIGHT - 1) / 256); \
        CHECK_BUSY();                      \
    } while (0)

// Macro to wake up device
#define EPD_WAKEUP() EPD_INIT()

// Macro to set display window
#define EPD_SET_WINDOW(x1, y1, x2, y2) \
    do                                 \
    {                                  \
    } while (0)

// Macro to push new color data
#define EPD_PUSH_NEW_COLORS(w, h, colors)        \
    do                                           \
    {                                            \
        writecommand(0x24);                      \
        for (uint16_t i = 0; i < w * h / 8; i++) \
        {                                        \
            writedata(colors[i]);                \
        }                                        \
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

// Macro to push old color data
#define EPD_PUSH_OLD_COLORS(w, h, colors)        \
    do                                           \
    {                                            \
        writecommand(0x26);                      \
        for (uint16_t i = 0; i < w * h / 8; i++) \
        {                                        \
            writedata(colors[i]);                \
        }                                        \
    } while (0)

#define EPD_PUSH_OLD_COLORS_FLIP(w, h, colors)                         \
    do                                                                 \
    {                                                                  \
        writecommand(0x26);                                            \
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
    }while (0)

#endif