// Example for drawArc function. This is intended for arc based meters.
// (See arcMeter example)

// Draws arcs without smooth ends, suitable for dynamically changing arc
// angles to avoid residual anti-alias pixels at the arc segment joints.

// The sides of the arc can optionally be smooth or not. Smooth arcs have
// a much better appearance, especially at small sizes.

// Start angle for drawArc must be smaller than end angle

#include <TFT_eSPI.h>       // Include the graphics library
TFT_eSPI tft = TFT_eSPI();  // Create object "tft"

// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup(void) {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

// -------------------------------------------------------------------------
// Main loop
// -------------------------------------------------------------------------
void loop()
{
  static uint32_t count = 0;

  uint16_t fg_color = random(0x10000);
  uint16_t bg_color = TFT_BLACK;       // This is the background colour used for smoothing (anti-aliasing)

  uint16_t x = random(tft.width());  // Position of centre of arc
  uint16_t y = random(tft.height());

  uint8_t radius       = random(20, tft.width() / 4); // Outer arc radius
  uint8_t thickness    = random(1, radius / 4);     // Thickness
  uint8_t inner_radius = radius - thickness;        // Calculate inner radius (can be 0 for circle segment)

  // 0 degrees is at 6 o'clock position
  // Arcs are drawn clockwise from start_angle to end_angle
  // Start angle for drawArc must be smaller than end angle (function will swap them otherwise)
  uint16_t start_angle = random(361); // Start angle must be in range 0 to 360
  uint16_t end_angle   = random(361); // End angle must be in range 0 to 360

  bool smooth = random(2); // true = smooth sides, false = no smooth sides

  tft.drawArc(x, y, radius, inner_radius, start_angle, end_angle, fg_color, bg_color, smooth);

  //tft.drawArc(x, y, radius, inner_radius, start_angle, end_angle, fg_color, bg_color); // always smooth sides if parameter is missing

  // The following function allows arcs to be drawn through the 6 o'clock position by drawing in 2 segments if
  // the start angle is greater than the end angle
  //drawAnyArc(x, y, radius, inner_radius, start_angle, end_angle, fg_color, bg_color, smooth); // smooth sides if parameter is missing

  count++;
  if (count < 30) delay(500); // After 15s draw as fast as possible!
}


// The following function allows arcs to be drawn through the 0 degree position by drawing in 2 segments

// Function prototype with default smooth setting
void drawAnyArc(int32_t x, int32_t y, int32_t r, int32_t ir, int32_t startAngle, int32_t endAngle,
                uint32_t fg_color, uint32_t bg_color,  bool smooth = true);

void drawAnyArc(int32_t x, int32_t y, int32_t r, int32_t ir, int32_t startAngle, int32_t endAngle,
                uint32_t fg_color, uint32_t bg_color,  bool smooth)
{
  if (endAngle > startAngle)
  {
    // Draw arc in single sweep
    tft.drawArc(x, y, r, ir, startAngle, endAngle, fg_color, bg_color);
  }
  else
  {
    // Arc sweeps through 6 o'clock so draw in two parts
    tft.drawArc(x, y, r, ir, startAngle, 360, fg_color, bg_color);
    tft.drawArc(x, y, r, ir, 0, endAngle, fg_color, bg_color);
  }
}
