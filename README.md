# TFT_eSPI

An Arduino IDE compatible graphics and fonts library for ESP8266 processors with a driver for the ILI9341 and ST7735 based TFT displays.

The library contains proportional fonts, different sizes can be enabled/disabled at compile time to optimise the use of FLASH memory.  The library has been tested with the NodeMCU (ESP8266 based)

The library is based on the Adafruit GFX and Adafruit driver libraries and the aim is to retain compatibility. Significant additions have been made to the library to boost the speed for ESP8266 processors (it is typically 3 to 10 times faster) and to add new features. The new graphics functions include different size proportional fonts and formatting features. There are a significant number of example sketches to demonstrate the different features.

Configuration of the library font selections, pins used to interface with the TFT and other features is made by editting the User_Setup.h file in the library folder.  Fonts and features can easily be disabled by commenting out lines.

