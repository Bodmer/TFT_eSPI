//  Created by Bodmer 24th Jan 2017 - Tested in Arduino IDE 1.8.5 esp8266 Core 2.4.0

// The latest Arduino IDE versions support UTF-8 encoding of Unicode characters
// within sketches:
// https://playground.arduino.cc/Code/UTF-8

/*
  The library expects strings to be in UTF-8 encoded format:
  https://www.fileformat.info/info/unicode/utf8.htm

  Creating varaibles needs to be done with care when using character arrays:
  char c = 'µ';          // Wrong
  char bad[4] = "5µA";   // Wrong
  char good[] = "5µA";   // Good
  String okay = "5µA";   // Good

  This is because UTF-8 characters outside the basic Latin set occupy more than
  1 byte per character! A 16 bit unicode character occupies 3 bytes!

*/

//====================================================================================
//                                  Libraries
//====================================================================================
// Call up the SPIFFS FLASH filing system this is part of the ESP Core

#include <TFT_eSPI.h>      // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

uint16_t bg = TFT_BLACK;
uint16_t fg = TFT_WHITE;


//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(115200); // Used for messages and the C array generator

  Serial.println("NodeMCU vlw font test!");

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  listFiles(); // Lists the files so you can see what is in the SPIFFS

  tft.begin();
  tft.setRotation(0);  // portrait

  fg = TFT_WHITE;
  bg = TFT_BLACK;
}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{
  tft.setTextColor(fg, bg);

  //----------------------------------------------------------------------------
  // Anti-aliased font test

  String test1 = "Hello World";

  // Load a smooth font from SPIFFS
  tft.loadFont("Final-Frontier-28");

  tft.setRotation(0);

  // Show all characters on screen with 2 second (2000ms) delay between screens
  tft.showFont(2000); // Note: This function moves the cursor position!

  tft.fillScreen(bg);
  tft.setCursor(0,0);

  tft.println(test1);

  // Remove font parameters from memory to recover RAM
  tft.unloadFont();

  delay(2000);

  //----------------------------------------------------------------------------
  // We can have any random mix of characters in the font

  String test2 = "仝倀"; // Unicodes 0x4EDD, 0x5000

  tft.loadFont("Unicode-Test-72");

  tft.setRotation(1);

  // Show all characters on screen with 2 second (2000ms) delay between screens
  tft.showFont(2000); // Note: This function moves the cursor position!

  tft.fillScreen(bg);
  tft.setCursor(0,0);

  tft.setTextColor(TFT_CYAN, bg);
  tft.println(test2);

  tft.setTextColor(TFT_YELLOW, bg);
  tft.println("12:00pm");

  tft.setTextColor(TFT_MAGENTA, bg);
  tft.println("1000Ω");

  // Remove font parameters from memory to recover RAM
  tft.unloadFont();

  delay(2000);

  //----------------------------------------------------------------------------
  // Latin and Hiragana font mix

  String test3 = "こんにちは";
    
  tft.loadFont("Latin-Hiragana-24");

  tft.setRotation(0);
  
  // Show all characters on screen with 2 second (2000ms) delay between screens
  tft.showFont(2000); // Note: This function moves the cursor position!

  tft.fillScreen(bg);
  tft.setTextColor(TFT_GREEN, bg);
  tft.setCursor(0,0);

  tft.println("Konnichiwa");
  tft.println(test3);
  tft.println();
  tft.println("Sayonara");
  tft.println("さようなら"); // Sayonara

  // Remove font parameters from memory to recover RAM
  tft.unloadFont();

  delay(2000);
  //
  //----------------------------------------------------------------------------
}
//====================================================================================

