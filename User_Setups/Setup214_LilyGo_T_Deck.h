// ST7789 240 x 320 display
//
// The board peripheral power control pin needs to be set to HIGH when using any of the tdeck peripherals, the
// backlight also needs turning on manually before you can run the TFT_eSPI examples. The following will need 
// to be added inside the setup function.
//
// pinMode(TDECK_PERI_GPIO, OUTPUT);
// digitalWrite(TDECK_PERI_GPIO, HIGH);
// pinMode(TFT_BL, OUTPUT);
// digitalWrite(TFT_BL, HIGH);

#define USER_SETUP_ID 214

#define ST7789_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 320
#define CGRAM_OFFSET
#define TFT_RGB_ORDER TFT_BGR
#define TFT_INVERSION_ON

// Lilygo T-Deck setup
#define TFT_MISO 38
#define TFT_MOSI 41
#define TFT_SCLK 40
#define TFT_CS   12
#define TFT_DC   11
#define TFT_RST  -1
#define TFT_BL   42
#define TDECK_PERI_GPIO 10


#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT


#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000