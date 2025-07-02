#include <Wire.h>

#define USER_SETUP_ID 503

#define UC8179_DRIVER

#define EPAPER_ENABLE

#define TFT_WIDTH 648
#define TFT_HEIGHT 480

#define EPD_WIDTH TFT_WIDTH
#define EPD_HEIGHT TFT_HEIGHT

#define EPD_HORIZONTAL_MIRROR

#ifdef ENABLE_EPAPER_BOARD_PIN_SETUPS
#include "EPaper_Board_Pins_Setups.h"
#else
#define TFT_SCLK D8
#define TFT_MISO D9
#define TFT_MOSI D10
#define TFT_CS D1 // Chip select control pin
#define TFT_DC D3 // Data Command control pin
#define TFT_BUSY D2
#define TFT_RST D0 // Reset pin (could connect to RST pin)
#endif



#define LOAD_GLCD  // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2 // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4 // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6 // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7 // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8 // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
// #define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

#ifdef CONFIG_IDF_TARGET_ESP32S3
#define USE_HSPI_PORT
#endif

#if defined(SEEED_XIAO_M0)
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY 4000000
#elif  defined(ARDUINO_XIAO_ESP32S3) || defined(ARDUINO_XIAO_ESP32S3_PLUS)
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY  4000000
#elif  defined(ARDUINO_XIAO_ESP32C3)
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY  4000000
#elif defined(ARDUINO_XIAO_ESP32C6)
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY  4000000
#elif defined(ARDUINO_SEEED_XIAO_NRF52840) || defined(ARDUINO_SEEED_XIAO_NRF52840_SENSE) || defined(ARDUINO_Seeed_XIAO_nRF52840) || defined(ARDUINO_Seeed_XIAO_nRF52840_Sense)
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY 4000000
#define ltoa itoa
#elif defined(ARDUINO_SEEED_XIAO_RP2040) || defined(ARDUINO_SEEED_XIAO_RP2350)
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY 4000000
#elif defined(ARDUINO_XIAO_RA4M1)
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY 4000000
#elif defined (EFR32MG24B220F1536IM48)
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY 4000000
#else
#pragma message "Unknown board using default SPI settings (1MHz)"
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY 4000000
#endif
