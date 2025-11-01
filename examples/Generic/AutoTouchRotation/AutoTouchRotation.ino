/*
  This example demonstrates how to automatically apply the correct touch
  calibration data for all four display rotations using the TFT_eSPI library.

  The sketch shows how to:
   - Store four sets of touch calibration data (one for each rotation)
   - Automatically load the correct calibration when tft.setRotation() changes
   - Keep touch alignment consistent with the display orientation

  The display rotates automatically every 10 seconds (0→1→2→3).
  Touch input is immediately available in the new orientation.

  Calibration data is stored in SPIFFS for persistence across power cycles.

  This example is based on the TFT_Button_Label_Datum example and works with
  ESP32 modules using the XPT2046 touch controller.

  Tested on:
   - ESP32-32E with ST7796 display (HSPI)
   - XPT2046 touch controller
*/

#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSPI_Button key;

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

// Touch calibration data for each of the 4 rotations
// Obtain these values using the Touch_calibrate example
uint16_t calData0[5] = { 219, 3560, 230, 3662, 4 }; // rotation 0
uint16_t calData1[5] = { 206, 3677, 204, 3544, 7 }; // rotation 1
uint16_t calData2[5] = { 215, 3566, 229, 3674, 2 }; // rotation 2
uint16_t calData3[5] = { 239, 3651, 218, 3553, 1 }; // rotation 3

// Pointer table for easy access to calibration data sets
uint16_t* calDataSets[4] = { calData0, calData1, calData2, calData3 };

uint8_t currentRotation = 1; // Start in landscape mode
bool touchscreen_calibrated = false;

//---------------------------------------------------------------------------
void setup(void)
{
  Serial.begin(115200);
  
  tft.init();

  // Set the rotation before we calibrate
  tft.setRotation(currentRotation);

  // call screen calibration
  touch_calibrate();

  // Display initial UI
  drawUI();

  Serial.println("\nAutoTouchRotation Example - TFT_eSPI");
  Serial.println("Display rotates every 10 seconds");
  Serial.printf("Current rotation: %d\n", currentRotation);
}

//---------------------------------------------------------------------------
void loop()
{
  static unsigned long lastChange = 0;
  uint16_t t_x = 0, t_y = 0;
  bool pressed = false;

  // Check for touch input
  if (touchscreen_calibrated && tft.getTouch(&t_x, &t_y)) {
    pressed = true;
  }

  // Handle button press/release
  if (pressed && key.contains(t_x, t_y)) {
    key.press(true);
  } else {
    key.press(false);
  }

  // Handle button just pressed
  if (key.justPressed()) {
    key.drawButton(true);
    tft.fillRect(10, 30, 200, 20, TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Button Pressed!", 10, 30);
    Serial.printf("Pressed @ rotation %d\n", currentRotation);
  }

  // Handle button just released
  if (key.justReleased()) {
    key.drawButton();
  }

  // Demo: change rotation every 10 seconds
  if (millis() - lastChange > 10000) {
    currentRotation = (currentRotation + 1) % 4;
    setRotationAndTouch(currentRotation);
    drawUI();
    lastChange = millis();
  }

  delay(20);
}

//---------------------------------------------------------------------------
// Set display rotation and apply matching touch calibration data
//---------------------------------------------------------------------------
void setRotationAndTouch(uint8_t rot)
{
  if (rot > 3) rot = 0;
  
  tft.setRotation(rot);
  tft.setTouch(calDataSets[rot]);
  touchscreen_calibrated = true;

  Serial.printf("Rotation: %d, Touch cal data: { %d, %d, %d, %d, %d }\n",
                rot,
                calDataSets[rot][0], calDataSets[rot][1],
                calDataSets[rot][2], calDataSets[rot][3],
                calDataSets[rot][4]);
}

//---------------------------------------------------------------------------
// Redraw user interface elements
//---------------------------------------------------------------------------
void drawUI(void)
{
  tft.fillScreen(TFT_BLACK);
  
  key.initButtonUL(&tft, 60, 100, 120, 60, TFT_WHITE, TFT_BLUE, TFT_WHITE, "PRESS", 2);
  key.drawButton();
  
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Rotation: " + String(currentRotation), 10, 10);
}

//---------------------------------------------------------------------------
// Touch screen calibration routine
//---------------------------------------------------------------------------
void touch_calibrate(void)
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Calibration data is stored in SPIFFS
  if (SPIFFS.begin()) {
    if (SPIFFS.exists(CALIBRATION_FILE)) {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (!calDataOK) {
    // data not valid so run calibration again
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.println("Set REPEAT_CAL to false to skip this next time");
    }

    // conduct calibration
    delay(2000);
    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    // (Re)set the calibration data
    tft.setTouch(calData);

    // Store calibration data
    if (SPIFFS.begin()) {
      File f = SPIFFS.open(CALIBRATION_FILE, "w");
      if (f) {
        f.write((const unsigned char *)calData, 14);
        f.close();
      }
      SPIFFS.end();
    }
  } else {
    // Use stored calibration data
    tft.setTouch(calData);
  }

  touchscreen_calibrated = true;
}
