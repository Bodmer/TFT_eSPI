// Define STM32 to invoke STM32 optimised driver
#define STM32

// Define if Port A pins 7-0 are used for data bus bits 7-0
#define STM_PORTA_DATA_BUS
//#define STM_PORTB_DATA_BUS
//#define STM_PORTC_DATA_BUS

// Tell the library to use 8 bit parallel mode (otherwise SPI is assumed)
#define TFT_PARALLEL_8_BIT

// Define the TFT display driver
#define ILI9341_DRIVER
//#define ILI9488_DRIVER


#define TFT_CS PA11 // Chip select control pin
#define TFT_DC PA12 // Data Command control pin
#define TFT_RST PA9 // Reset pin
#define TFT_WR PA8 // Write strobe control pin
#define TFT_RD PA10 // Read pin
#define TFT_D0 PA0  // 8 bit parallel bus to TFT
#define TFT_D1 PA1
#define TFT_D2 PA2
#define TFT_D3 PA3
#define TFT_D4 PA4
#define TFT_D5 PA5
#define TFT_D6 PA6
#define TFT_D7 PA7

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// STM32 support for smooth fonts via program memory (FLASH) arrays
#define SMOOTH_FONT

//---------------- Touch Pad -------------------------------

#define TOUCH_CS PB9

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
#define SPI_TOUCH_FREQUENCY  2500000

// The STM32F103 has 2 free SPI ports 
// so uncomment the following line if you want to use second SPI for Touch:   //SPIClass SPITwo(PB15, PB14, PB13); (MOSI, MISO, CLK)
#define SPI2_for_TOUCH_PORT
