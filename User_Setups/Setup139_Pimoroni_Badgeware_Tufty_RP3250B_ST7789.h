// Pimoroni Badgeware Tufty 2350 (RP2350B) (ST7789 on 8 bit parallel with 240x320 TFT).
#define USER_SETUP_ID 139

#define ST7789_DRIVER     // Configure all registers

#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define TFT_PARALLEL_8_BIT

// Badgeware Tufty 2350 pins
#define TFT_CS   27
#define TFT_DC   28
#define TFT_WR   30
#define TFT_RD   31
#define TFT_D0   32
#define TFT_D1   33
#define TFT_D2   34
#define TFT_D3   35
#define TFT_D4   36
#define TFT_D5   37
#define TFT_D6   38
#define TFT_D7   39

// Backlight for tufty
#define TFT_BL   26
#define TFT_BACKLIGHT_ON HIGH

#define RP2040_PIO_CLK_DIV 3

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
// #define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT
