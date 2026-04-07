// Setup for the ESP32-S3 with ST7789V 240x320 display
// Tested with Freenove ESP32-S3-WROOM (N8R8) and Waveshare 2" SPI LCD
#define USER_SETUP_ID 73

#define ST7789_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

                    // Typical board default pins (tested)
#define TFT_MOSI 11 //     11 or 35 (FSPI D)
#define TFT_SCLK 12 //     12 or 36 (FSPI CLK)
#define TFT_CS   10 //     10 or 34 (FSPI CS0)
#define TFT_MISO -1

// Use pins in range 0-31
#define TFT_DC   14
#define TFT_RST  21

// #define TFT_BL   -1 // Backlight control pin, set to -1 if not used
// #define TFT_BACKLIGHT_ON HIGH

// #define TFT_INVERSION_ON  // Uncomment if colours are inverted
#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

// FSPI port (SPI2) used unless following defined
// On Arduino ESP32 Core 3.x, enable USE_HSPI_PORT and reduce
// SPI_FREQUENCY to 20000000 to avoid crashes and corruption
// #define USE_HSPI_PORT

#define SPI_FREQUENCY  40000000

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT
