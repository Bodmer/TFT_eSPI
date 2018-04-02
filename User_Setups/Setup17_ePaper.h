//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded etc
//
//   See the User_Setup_Select.h file if you wish to be able to define multiple
//   setups and then easily select which setup file is used by the compiler.
//
//   If this file is edited correctly then all the library example sketches should
//   run without the need to make any more changes for a particular hardware setup!

// ##################################################################################
//
// Section 0. Call up the right driver file and any options for it
//
// ##################################################################################

#define EPD_DRIVER // ePaper driver

// ##################################################################################
//
// Section 1. Define the pins that are used to interface with the display here
//
// ##################################################################################


// READ THIS  READ THIS  READ THIS  READ THIS  READ THIS  READ THIS
// Install the ePaper library for your own display size and type
// from here:
// https://github.com/Bodmer/EPD_Libraries

//  Note: Pin allocations for the ePaper signals are defined in
//  the ePaper library's epdif.h file. There follows the default
//  pins already included in epdif.h file for the ESP8266:

///////////////////////////////////////////////////////////////////
//   For ESP8266 connect as follows:                             //
//   Display  3.3V to NodeMCU 3V3                                //
//   Display   GND to NodeMCU GND                                //
//                                                               //
//   Display   GPIO   NodeMCU pin                                //
//    BUSY       5      D1                                       //
//    RESET      4      D2                                       //
//    DC         0      D3                                       //
//    CS         2      D4                                       //
//    CLK       14      D5                                       //
//                      D6 (MISO not connected to display)       //
//    DIN       13      D7                                       //
//                                                               //
///////////////////////////////////////////////////////////////////


// ##################################################################################
//
// Section 2. Not used
//
// ##################################################################################


// ##################################################################################
//
// Section 3. Define the fonts that are to be used here
//
// ##################################################################################

// Comment out the #defines below with // to stop that font being loaded
// The ESP8366 and ESP32 have plenty of memory so commenting out fonts is not
// normally necessary. If all fonts are loaded the extra FLASH space required is
// about 17Kbytes. To save FLASH space only enable the fonts you need!

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 below, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT

// ##################################################################################
//
// Section 4. Not used
//
// ##################################################################################


// ##################################################################################
//
// Section 5. Not used
//
// ##################################################################################

