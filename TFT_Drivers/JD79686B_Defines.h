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

#define EPD_NOP 0xFF     // No operation command (not supported)
#define EPD_PNLSET 0x00  // Panel setting (R00H PSR)
#define EPD_DISPON 0x04  // Power on (R04H PON)
#define EPD_DISPOFF 0x02 // Power off (R02H POF)
#define EPD_SLPIN 0x07   // Enter deep sleep (R07H DSLP)
#define EPD_SLPOUT 0xFF  // Exit sleep (not supported, requires wake-up)
#define EPD_PTLIN 0x91   // Partial display in (R91H PTIN)
#define EPD_PTLOUT 0x92  // Partial display out (R92H PTOUT)
#define EPD_PTLW 0x90    // Partial display window setting (R90H PTL)

#define TFT_SWRST 0xFF   // Software reset (not supported)
#define TFT_CASET 0xFF   // Column address setting (not supported)
#define TFT_PASET 0xFF   // Page address setting (not supported)
#define TFT_RAMWR 0x13   // Write RAM (R13H DTM2, red data)
#define TFT_RAMRD 0xFF   // Read RAM (not supported)
#define TFT_INVON 0xFF   // Display inversion on (not supported)
#define TFT_INVOFF 0xFF  // Display inversion off (not supported)
#define TFT_INIT_DELAY 0 // Initialization delay (none)

#ifdef TFT_BUSY
#define CHECK_BUSY()                   \
    do                                 \
    {                                  \
        while (!digitalRead(TFT_BUSY)) \
            ;                          \
    } while (0)
#else
#define CHECK_BUSY()
#endif

#define EPD_UPDATE()        \
    do                      \
    {                       \
        writecommand(0x04); \
        CHECK_BUSY();       \
        writecommand(0x12); \
        CHECK_BUSY();       \
    } while (0)

#define EPD_SLEEP()         \
    do                      \
    {                       \
        writecommand(0x02); \
        CHECK_BUSY();       \
        writecommand(0x07); \
        writedata(0xA5);    \
    } while (0)

#define EPD_WAKEUP()                 \
    do                               \
    {                                \
        digitalWrite(TFT_RST, LOW);  \
        delay(20);                   \
        digitalWrite(TFT_RST, HIGH); \
        delay(20);                   \
        writecommand(0x4D);          \
        writedata(0x55);             \
        writecommand(0xA6);          \
        writedata(0x38);             \
        writecommand(0xB4);          \
        writedata(0x5D);             \
        writecommand(0xB6);          \
        writedata(0x80);             \
        writecommand(0xB7);          \
        writedata(0x00);             \
        writecommand(0xF7);          \
        writedata(0x02);             \
    } while (0)

#define EPD_SET_WINDOW(x1, y1, x2, y2)                  \
    do                                                  \
    {                                                   \
        writecommand(0x91);                             \
        writecommand(0x90);                             \
        writedata((x1 >> 8) & 0xFF); /* x1 / 256 */     \
        writedata(x1 & 0xFF);        /* x1 % 256 */     \
        writedata((x2 >> 8) & 0xFF); /* x2 / 256 */     \
        writedata((x2 & 0xFF) - 1);  /* x2 % 256 - 1 */ \
        writedata((y1 >> 8) & 0xFF); /* y1 / 256 */     \
        writedata(y1 & 0xFF);        /* y1 % 256 */     \
        writedata((y2 >> 8) & 0xFF); /* y2 / 256 */     \
        writedata((y2 & 0xFF) - 1);  /* y2 % 256 - 1 */ \
        writedata(0x01);                                \
    } while (0)

#define EPD_PUSH_NEW_COLORS(w, h, colors)       \
    do                                          \
    {                                           \
        writecommand(0x13);                     \
        for (int i = 0; i < (w) * (h) / 8; i++) \
        {                                       \
            writedata(colors[i]);               \
        }                                       \
    } while (0)

#define EPD_PUSH_NEW_COLORS_FLIP(w, h, colors)                         \
    do                                                                 \
    {                                                                  \
        writecommand(0x13);                                            \
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

#define EPD_PUSH_OLD_COLORS(w, h, colors)       \
    do                                          \
    {                                           \
        writecommand(0x10);                     \
        for (int i = 0; i < (w) * (h) / 8; i++) \
        {                                       \
            writedata(colors[i]);               \
        }                                       \
    } while (0)

#define EPD_PUSH_OLD_COLORS_FLIP(w, h, colors)                         \
    do                                                                 \
    {                                                                  \
        writecommand(0x10);                                            \
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