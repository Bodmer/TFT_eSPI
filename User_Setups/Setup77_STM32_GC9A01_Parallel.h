//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc.
//
//   See the User_Setup_Select.h file if you wish to be able to define multiple
//   setups and then easily select which setup file is used by the compiler.
//
//   If this file is edited correctly then all the library example sketches should
//   run without the need to make any more changes for a particular hardware setup!
//   Note that some sketches are designed for a particular TFT pixel width/height

#define USER_SETUP_ID 0x77

// Define to disable all #warnings in library (can be put in User_Setup_Select.h)
//#define DISABLE_ALL_LIBRARY_WARNINGS

// ##################################################################################
//
// Section 1. Call up the right driver file and any options for it
//
// ##################################################################################

// Define STM32 to invoke optimised processor support (only for STM32)
#define STM32


// STM32 8-bit parallel only:
// If STN32 Port A or B pins 0-7 are used for 8-bit parallel data bus bits 0-7
// then this will improve rendering performance by a factor of ~8x
#define STM_PORTA_DATA_BUS
//#define STM_PORTB_DATA_BUS

// Tell the library to use 8-bit parallel mode (otherwise SPI is assumed)
#define TFT_PARALLEL_8_BIT

 
#define GC9A01_DRIVER


 #define TFT_HEIGHT 240 // GC9A01 240 x 240


// If colours are inverted (white shows as black) then uncomment one of the next
// 2 lines try both options, one of the options should correct the inversion.

// #define TFT_INVERSION_ON
#define TFT_INVERSION_OFF


// ##################################################################################
//
// Section 2. Define the pins that are used to interface with the display here
//
// ##################################################################################



// ######       EDIT THE PINs BELOW TO SUIT YOUR STM32 PARALLEL TFT SETUP        ######


// Tell the library to use 8-bit parallel mode (otherwise SPI is assumed)
#define TFT_PARALLEL_8_BIT

// The STM32 and TFT the pins used for testing are:
#define TFT_CS  PC0   // Chip select control pin (library pulls permanently low
#define TFT_DC  PC1   // Data Command control pin 
#define TFT_RST PC2   // Reset pin, toggles on startup

#define TFT_WR  PB2    // Write strobe control pin  
#define TFT_RD  PB4    // Read strobe control pin

#define TFT_D0  PA0   
#define TFT_D1  PA1   
#define TFT_D2  PA2   // Pins can be randomly assigned, this does not affect
#define TFT_D3  PA3   // TFT screen update performance.
#define TFT_D4  PA4 
#define TFT_D5  PA5 
#define TFT_D6  PA6 
#define TFT_D7  PA7 


// ##################################################################################
//
// Section 3. Define the fonts that are to be used here
//
// ##################################################################################


#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT


// ##################################################################################
//
// Section 4. Other options
//
// ##################################################################################






// Comment out the following #define if "SPI Transactions" do not need to be
// supported. When commented out the code size will be smaller and sketches will
// run slightly faster, so leave it commented out unless you need it!

// Transaction support is needed to work with SD library but not needed with TFT_SdFat
// Transaction support is required if other SPI devices are connected.

// #define SUPPORT_TRANSACTIONS
