// This header file contains a list of user setup files and defines which one the
// compliler uses when the IDE performs a Verify/Compile or Upload.
//
// Users can create configurations for different Espressiff boards and TFT displays.
// This makes selecting between hardware setups easy by "uncommenting" one line.

// The advantage of this hardware configuration method is that the examples provided
// with the library should work with different setups immediately without any other
// changes being needed. It also improves the poratability of users sketches to other
// hardware configurations and compatible libraries.
//
// Create a shortcut to this file on your desktop to permit quick access for editting.
// Re-compile and upload after making and saving any changes to this file.

// Only ONE line should be uncommented

#include <User_Setup.h>           // Default setup
//#include <User_Setup1_ILI9341.h>  // Setup file configured for my ILI9341
//#include <User_Setup2_ST7735.h>   // Setup file configured for my ST7735
//#include <User_Setup3_Template.h> // Setup file template for copying/editting