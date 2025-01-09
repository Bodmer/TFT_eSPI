#include <TFT_eSPI.h> // Include the graphics library

TFT_eSPI tft = TFT_eSPI(); // Create an instance of the library

void setup() {
  pinMode(LCD_CONTROL_PIN, OUTPUT);
  digitalWrite(LCD_CONTROL_PIN, LOW);

  // Initialize the backlight pin
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH); // Turn on the backlight (HIGH = ON)

  // Initialize the display
  tft.init();
  tft.setRotation(1); // Set rotation for landscape/portrait orientation (0-3)

  // Clear the screen with a background color
  tft.fillScreen(TFT_BLACK);

  // Set text color and background color
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Set text size
  tft.setTextSize(2);

  // Display some text
  tft.setCursor(10, 10); // Set the cursor position
  tft.println("Hello, World!");

  // Display additional text
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(10, 50);
  tft.println("ST7789 Screen!");

  // Draw some additional decorative elements (optional)
  tft.drawRect(1, 1, tft.width() - 1, tft.height() - 1, TFT_RED); // Red rectangle around text
}

void loop() {
  // Nothing to do here for this example
}
