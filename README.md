# TFT_eSPI

An Arduino IDE compatible graphics and fonts library for ESP8266 and ESP32 processors with a driver for ILI9341, ILI9163, ST7735 and S6D02A1 based TFT displays that support SPI.

The library also supports TFT displays designed for the Raspberry Pi that are based on a ILI9486 driver chip with a 480 x 320 pixel screen. This display must be of the Waveshare design and use a 16 bit serial interface based on the 74HC04, 74HC4040 and 2 x 74HC4094 logic chips. A modification to these displays is possible (see mod image in Tools folder) to make many graphics functions much faster (e.g. 23ms to clear the screen, 1.2ms to draw a 72 pixel high numeral).

The library supports SPI overlap so the TFT screen can share MOSI, MISO and SCLK pins with the program FLASH.

The library contains proportional fonts, different sizes can be enabled/disabled at compile time to optimise the use of FLASH memory.  The library has been tested with the NodeMCU (ESP8266 based) and an ESP32 demo board.

The library is based on the Adafruit GFX and Adafruit driver libraries and the aim is to retain compatibility. Significant additions have been made to the library to boost the speed for ESP8266/ESP32 processors (it is typically 3 to 10 times faster) and to add new features. The new graphics functions include different size proportional fonts and formatting features. There are a significant number of example sketches to demonstrate the different features.

Configuration of the library font selections, pins used to interface with the TFT and other features is made by editting the User_Setup.h file in the library folder.  Fonts and features can easily be disabled by commenting out lines.

