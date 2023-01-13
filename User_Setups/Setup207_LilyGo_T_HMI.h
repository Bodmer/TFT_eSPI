// ST7789 240 x 240 display with no chip select line
#define USER_SETUP_ID 24



#define ST7789_DRIVER // Configure all registers

// #define TFT_RGB_ORDER TFT_BGR // Colour order Blue-Green-Red

#define TFT_WIDTH 240  // ST7789 172 x 320
#define TFT_HEIGHT 320 // ST7789 240 x 320

#define CGRAM_OFFSET
// #define TFT_RGB_ORDER TFT_RGB  // Colour order Red-Green-Blue
#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

// #define TFT_INVERSION_ON
#define TFT_INVERSION_OFF

#define TFT_PARALLEL_8_BIT

// The ESP32 and TFT the pins used for testing are:
#define TFT_CS 6  // Chip select control pin (library pulls permanently low
#define TFT_DC 7  // Data Command control pin - must use a pin in the range 0-31
#define TFT_RST -1 // Reset pin, toggles on startup

#define TFT_WR 8 // Write strobe control pin - must use a pin in the range 0-31
#define TFT_RD -1 // Read strobe control pin

#define TFT_DATA_PIN_OFFSET_EN /* The ESP32S3 controller is controlled by two registers. \
                                Select data pin numbers higher than 32 to enable this option, \
                                this macro definition added by Micky -20220701 */

#define TFT_D0 48 // Must use pins in the range 0-31 for the data bus
#define TFT_D1 47 // so a single register write sets/clears all bits.
#define TFT_D2 39 // Pins can be randomly assigned, this does not affect
#define TFT_D3 40 // TFT screen update performance.
#define TFT_D4 41
#define TFT_D5 42
#define TFT_D6 45
#define TFT_D7 46

#define TFT_BL 38 // LED back-light (required for M5Stack)

#define LOAD_GLCD  // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2 // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4 // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6 // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7 // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8 // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

// #define SPI_FREQUENCY  27000000
// #define SPI_FREQUENCY  40000000

// #define SPI_READ_FREQUENCY  20000000

// #define SPI_TOUCH_FREQUENCY  2500000

// #define SUPPORT_TRANSACTIONS