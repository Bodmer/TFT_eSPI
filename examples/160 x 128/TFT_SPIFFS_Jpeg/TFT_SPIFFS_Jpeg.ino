/*====================================================================================

  This sketch demonstrates loading images which have been stored as files in the
  built-in FLASH memory on a NodeMCU 1.0 (ESP8266 based, ESP-12E Module) rendering the
  images onto a 160 x 128 pixel TFT screen.

  The images are stored in the SPI FLASH Filing System (SPIFFS), which effectively
  functions like a tiny "hard drive". This filing system is built into the ESP8266
  Core that can be loaded from the IDE "Boards manager" menu option. This is at
  version 2.3.0 at the time of sketch creation.

  The size of the SPIFFS partition can be set in the IDE as 1Mbyte or 3Mbytes. Either
  will work with this sketch. Typically most sketches easily fit within 1 Mbyte so a
  3 Mbyte SPIFS partition can be used, in which case it can contain 100's of Jpeg
  full screem images.

  The Jpeg library can be found here:
  https://github.com/Bodmer/JPEGDecoder
 
  Images in the Jpeg format can be created using Paint or IrfanView or other picture
  editting software.

  Place the images inside the sketch folder, in a folder called "Data".  Then upload
  all the files in the folder using the Arduino IDE "ESP8266 Sketch Data Upload" option
  in the "Tools" menu:
  http://www.esp8266.com/viewtopic.php?f=32&t=10081
  https://github.com/esp8266/arduino-esp8266fs-plugin/releases
  
  This takes some time, but the SPIFFS content is not altered when a new sketch is
  uploaded, so there is no need to upload the same files again!
  Note: If open, you must close the "Serial Monitor" window to upload data to SPIFFS!

  The IDE will not copy the "data" folder with the sketch if you save the sketch under
  another name. It is necessary to manually make a copy and place it in the sketch
  folder.

  This sketch includes example images in the Data folder.

  Saving images, uploading and rendering on the TFT screen couldn't be much easier!

  Created by Bodmer 24th Jan 2017 - Tested in Arduino IDE 1.8.0 esp8266 Core 2.3.0
  ==================================================================================*/

//====================================================================================
//                                  Libraries
//====================================================================================
// Call up the SPIFFS FLASH filing system this is part of the ESP Core
#define FS_NO_GLOBALS
#include <FS.h>

// JPEG decoder library
#include <JPEGDecoder.h>

// SPI library, built into IDE
#include <SPI.h>

// Call up the TFT library
#include <TFT_eSPI.h> // Hardware-specific library for ESP8266
// The TFT control pins are set in the User_Setup.h file <<<<<<<<<<<<<<<<< NOTE!
// that can be found in the "src" folder of the library

// Invoke TFT library
TFT_eSPI tft = TFT_eSPI();

//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(250000); // Used for messages and the C array generator

  delay(10);
  Serial.println("NodeMCU decoder test!");

  tft.begin();
  tft.setRotation(0);  // 0 & 2 Portrait. 1 & 3 landscape
  tft.fillScreen(TFT_BLACK);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");
  listFiles(); // Lists the files so you can see what is in the SPIFFS

}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{
  // Note the / before the SPIFFS file name must be present, this means the file is in
  // the root directory of the SPIFFS, e.g. "/Tiger.jpg" for a file called "Tiger.jpg"

  tft.setRotation(0);  // portrait
  tft.fillScreen(random(0xFFFF));

  drawJpeg("/EagleEye160.jpg", 0, 16);
  delay(2000);

  tft.fillScreen(random(0xFFFF));
  drawJpeg("/tiger160.jpg", 4, 0);
  delay(2000);

  tft.setRotation(1);  // landscape
  //tft.fillScreen(random(0xFFFF));
  drawJpeg("/arduino160.jpg", 0, 0);
  delay(2000);

  tft.fillScreen(TFT_BLACK);
  drawJpeg("/Baboon160.jpg", 0, 4);
  delay(2000);

  tft.fillScreen(random(0xFFFF));
  drawJpeg("/Mouse160.jpg", 0, 11);
  delay(2000);

  // Create arrays from the jpeg images and send them to the serial port for copy and
  // pasting into a sketch (used to make arrays fot the TFT_FLASH_Jpeg sketch)

  //createArray("/EagleEye160.jpg");
  //createArray("/tiger160.jpg");
  //createArray("/Baboon160.jpg");
  //createArray("/Mouse160.jpg");
  //while(1) yield(); // Stay here
}
//====================================================================================

