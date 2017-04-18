//                            USER DEFINED SETTINGS
//
//   The User_Setup header that will be called up is defined in User_Setup_Select.h
//
//   Set driver type, fonts to be loaded, pins used and SPI control method etc
//   
//   If this file is editted correctly then all the library example sketches should
//   run without the need to make any more changes for a particular hardware setup!

// ##################################################################################
//
// Section 0. Call up the right driver file and any options for it
//
// ##################################################################################

// Only define one driver, the other ones must be commented out
#define ILI9341_DRIVER
//#define ST7735_DRIVER

// For ST7735 and ILI9163 ONLY, define the pixel width and height in portrait orientation
//#define TFT_WIDTH  128
//#define TFT_HEIGHT 160
//#define TFT_HEIGHT 128

// For ST7735 ONLY, define the type of display, originally this was based on the
// colour of the tab on the screen protector film but this is not always true, so try
// out the different options below if the screen does not display graphics correctly,
// e.g. colours wrong, mirror images, or tray pixels at the edges.
// Comment out ALL BUT ONE of these options for a ST7735 display driver, save this
// this User_Setup file, then rebuild and upload the sketch to the board again:

//#define ST7735_INITB
//#define ST7735_GREENTAB
//#define ST7735_GREENTAB2
//#define ST7735_REDTAB
//#define ST7735_BLACKTAB

// ##################################################################################
//
// Section 1. Define the pins that are used to interface with the display here
//
// ##################################################################################

// We must use hardware SPI, a minimum of 3 GPIO pins is needed.
// Typical setup for NodeMCU ESP-12 is :
//
// Display SDO/MISO  to NodeMCU pin D6 (or leave disconnected if not reading TFT)
// Display LED       to NodeMCU pin VIN (or 5V, see below)
// Display SCK       to NodeMCU pin D5
// Display SDI/MOSI  to NodeMCU pin D7
// Display DC (or AO)to NodeMCU pin D3
// Display RESET     to NodeMCU pin D4 (or RST, see below)
// Display CS        to NodeMCU pin D8 (or GND, see below)
// Display GND       to NodeMCU pin GND (0V)
// Display VCC       to NodeMCU 5V or 3.3V
//
// The TFT RESET pin can be connected to the NodeMCU RST pin or 3.3V to free up a control pin
//
// With some displays such as the ILI9341 the TFT CS pin can be connected to GND if no more
// SPI deivces (e.g. an SD Card) are connected, in this case comment out the #define TFT_CS
// line below so it is NOT defined. Other displays such at the ST7735 require the TFT CS pin
// to be toggled during setup, so in these cases the TFT_CS line must be defined and connected.
//
// The NodeMCU D0 pin can be used for RST
//
// See Section 2. below if DC or CS is connected to D0
//
// Note: only some versions of the NodeMCU provide the USB 5V on the VIN pin
// If 5V is not available at a pin you can use 3.3V but backlight brightness
// will be lower.

// ###### EDIT THE PIN NUMBERS IN THE LINES FOLLOWING TO SUIT YOUR SETUP ######

// ModeMCU
#define TFT_CS   PIN_D8  // Chip select control pin D8
#define TFT_DC   PIN_D3  // Data Command control pin
#define TFT_RST  PIN_D4  // Reset pin (could connect to NodeMCU RST, see next line)
//#define TFT_RST  -1  // Set TFT_RST to -1 if the display RESET is connected to NodeMCU RST or 3.3V

// ESP32 Dev board (planned, not supported yet)
//#define TFT_CS   5  // Chip select control pin
//#define TFT_DC   2  // Data Command control pin
//#define TFT_RST  4  // Reset pin (could connect to Arduino RESET pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

// ##################################################################################
//
// Section 2. Define the way the DC and/or CS lines are driven
//
// ##################################################################################

// Normally the library uses direct register access for the DC and CS lines for speed
// If D0 (GPIO16) is used for CS or DC then a different slower method must be used
// Uncomment one line if D0 is used for DC or CS
// DC on D0 = 6% performance penalty at 40MHz SPI running graphics test
// CS on D0 = 2% performance penalty at 40MHz SPI running graphics test

// #define D0_USED_FOR_DC
// #define D0_USED_FOR_CS

// ##################################################################################
//
// Section 3. Define the fonts that are to be used here
//
// ##################################################################################

// Comment out the #defines below with // to stop that font being loaded
// The ESP8366 had plenty of memory so commenting out fonts is not normally necessary
// If all fonts are loaded the extra FLASH space required is about 17Kbytes...
// To save FLASH space only enable the fonts you need!

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// ##################################################################################
//
// Section 4. Not used
//
// ##################################################################################


// ##################################################################################
//
// Section 5. Other options
//
// ##################################################################################

// Define the SPI clock frequency
// With an ILI9341 display 40MHz works OK, 80MHz sometimes fails
// With a ST7735 display more than 27MHz may not work (spurious pixels and lines)

// #define SPI_FREQUENCY   1000000
// #define SPI_FREQUENCY   5000000
// #define SPI_FREQUENCY  10000000
// #define SPI_FREQUENCY  20000000
// #define SPI_FREQUENCY  27000000 // Actually sets it to 26.67MHz = 80/3
 #define SPI_FREQUENCY  40000000 // Maximum to use SPIFFS
// #define SPI_FREQUENCY  80000000


// Comment out the following #define if "SPI Transactions" do not need to be
// supported. Tranaction support is required if other SPI devices are connected.
// When commented out the code size will be smaller and sketches will
// run slightly faster, so leave it commented out unless you need it!
// Transaction support is needed to work with SD library but not needed with TFT_SdFat

// #define SUPPORT_TRANSACTIONS
