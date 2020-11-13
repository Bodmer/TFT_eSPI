//Use Maixduino Core and https://github.com/sipeed/Maixduino/pull/99
// or https://github.com/UT2UH/Maixduino with missing boards added

#if defined (MAIX_AMIGO)
  // ILI9486 320 x 480 display in 8-bit parallel mode
  #define ILI9486_DRIVER   // Configure all registers
  #define TFT_WIDTH        320
  #define TFT_HEIGHT       480

  #define TFT_CS            36  // Chip select control pin
  #define TFT_RST           37  // Reset pin
  #define TFT_DC            38  // Data Command control pin
  #define TFT_WR            39  // Write strobe control pin
  #define TFT_LED           -1  // Define as not used

  #define TOUCH_W          320
  #define TOUCH_H          280
  #define CST_DEVICE_ADDR 0x38
  #define CST_INT           33

  #define SPI_FREQUENCY     20000000

#elif defined (MAIX_CUBE)
  // ST7789 240 x 240 1.3" IPS display in 8-bit parallel mode
  #define ST7789_2_DRIVER   // Configure all registers
  #define TFT_WIDTH        240
  #define TFT_HEIGHT       240

  #define TFT_MISO          26
  #define TFT_MOSI          28
  #define TFT_SCLK          27

  #define TFT_CS            36
  #define TFT_RST           37
  #define TFT_DC            38
  #define TFT_WR            39
  #define TFT_LED           17
  #define HAS_BACKLIGHT

  #define TOUCH_INT         -1

  #define SPI_FREQUENCY     20000000

#elif defined (MAIX_GO)
  // ST7789 240 x 320 display in 8-bit parallel mode
  #define ST7789_2_DRIVER    // Configure all registers
  #define TFT_WIDTH  240
  #define TFT_HEIGHT 320

// If colours are inverted (white shows as black) then uncomment one of the next
// 2 lines try both options, one of the options should correct the inversion.
  // #define TFT_INVERSION_ON
  #define TFT_INVERSION_OFF

  #define TFT_CS            36
  #define TFT_RST           37
  #define TFT_DC            38
  #define TFT_WR            39
  #define TFT_LED           -1

  #define TOUCH_INT         -1

  #define SPI_FREQUENCY     20000000

#elif defined(MAIX_DUINO)
  // ST7789 240 x 320 display in 8-bit parallel mode
  #define ST7789_2_DRIVER    // Configure all registers
  #define TFT_WIDTH  240
  #define TFT_HEIGHT 320
  #define TFT_INVERSION_OFF

  #define TFT_CS            36
  #define TFT_RST           37
  #define TFT_DC            38
  #define TFT_WR            39
  #define TFT_LED           17
  #define HAS_BACKLIGHT

  #define TOUCH_INT         -1

  #define SPI_FREQUENCY     20000000

#elif defined (MAIX_DOCK_M1) || defined (MAIX_DOCK_M1W)
  // ST7789 240 x 320 display in 8-bit parallel mode
  #define ST7789_2_DRIVER    // Configure all registers
  #define TFT_WIDTH  240
  #define TFT_HEIGHT 320
  #define TFT_INVERSION_OFF

  #define TFT_CS            36
  #define TFT_RST           37
  #define TFT_DC            38
  #define TFT_WR            39
  #define TFT_LED           -1

  #define TOUCH_INT         -1

  #define SPI_FREQUENCY     20000000

#elif defined (MAIX_BIT) || defined (MAIX_BIT_MIC)
  // ST7789 240 x 320 display in 8-bit parallel mode
  #define ST7789_2_DRIVER    // Configure all registers
  #define TFT_WIDTH  240
  #define TFT_HEIGHT 320
  #define TFT_INVERSION_OFF

  #define TFT_CS            36
  #define TFT_RST           37
  #define TFT_DC            38
  #define TFT_WR            39
  #define TFT_LED           -1

  #define TOUCH_INT         -1

  #define SPI_FREQUENCY     20000000

#elif defined (AIOT_DAAN)
  // ST7789 240 x 320 display in 8-bit parallel mode
  #define ST7789_2_DRIVER    // Configure all registers
  #define TFT_WIDTH  240
  #define TFT_HEIGHT 320

  #define TFT_CS            36
  #define TFT_RST           37
  #define TFT_DC            38
  #define TFT_WR            39
  #define TFT_LED           -1

  #define TOUCH_INT         -1

  #define SPI_FREQUENCY     20000000

#elif defined (M5STICK_V)
// ST7789 240 x 280 display
  #define ST7789_2_DRIVER    // Configure all registers
  #define TFT_WIDTH  135
  #define TFT_HEIGHT 240
  #define CGRAM_OFFSET      // Library will add offsets required
  
  #define TFT_MISO          -1
  #define TFT_MOSI          18
  #define TFT_SCLK          19
  
  #define TFT_DC            20
  #define TFT_RST           21
  #define TFT_CS            22
  #define TFT_LED           -1

  #define TOUCH_INT         -1

  #define SPI_FREQUENCY       40000000
  #define SPI_READ_FREQUENCY  20000000

#endif

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT
