/*
 Common Paint Example
 Is adapted to work with Resistive touch from TFT 2.4" MCUFRIEND Shield 
 with chipset ST7781 / SPFD5408. This uses shared pins to touch analogic
 values.
 Tested with ESP32-Dev Board.
 
 Need connect pin SD_SCK and set in PIN_SD_SCK
 Need download Adafruit Resistive Touch Library: Adafruit_TouchScreen
 On different boards maybe need adjust isValidPressure()

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */


#include <TFT_eSPI.h>     // Hardware-specific library
#include <SPI.h>
#include <TouchScreen.h>  // Adafruit_TouchScreen

// If User_Setup.h is OK, just need setup this option.
// On Display, Touch Screen shares PIN with SD Card
// Just select one free pin, this example use GPIO 32 on ESP32
#define PIN_SD_SCK 32 
                      
                      
#define YP TFT_WR  // On Uno is pin A1, just use User_Setup.h
#define XM TFT_DC  // On Uno is pin A2, just use User_Setup.h
#define YM TFT_D7  // On Uno is pin D7, just use User_Setup.h
#define XP TFT_D6  // On Uno is pin D6, just use User_Setup.h

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint p;

TFT_eSPI tft = TFT_eSPI(); 

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;
int calibration = 0;

int16_t calibMinX = 0;
int16_t calibMinY = 0;
int16_t calibMaxX = 0;
int16_t calibMaxY = 0;
int16_t userX = 0;
int16_t userY = 0;

void setup(void) {
  Serial.begin(115200);
  Serial.println(F("Paint Time!"));
  //
  uint16_t identifier = tft.readcommand16(0x00, 2); // 16 Bits
  Serial.print(F("LCD driver chip: "));
  Serial.println(identifier, HEX);
  //
  Serial.print("Screen Width: ");  Serial.println(tft.width());
  Serial.print("Screen Height: "); Serial.println(tft.height());
  //
  pinMode(PIN_SD_SCK, OUTPUT);

  tft.begin();
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  if (calibration < 30) {
    doCalibration();
    return;
  }
  //
  if (isValidPressure()) {
//    Serial.print("X = "); Serial.print(p.x);
//    Serial.print("\tY = "); Serial.print(p.y);
//    Serial.print("\tPressure = "); Serial.println(p.z);

    // Calculate correct user touch
    userX = map(p.x, calibMinX, calibMaxX, 0, tft.width());
    userY = map(p.y, calibMinY, calibMaxY, 0, tft.height());
    // Fix limits
    if (userX < 0) userX = 0;
    if (userY < 0) userY = 0;
    if (userX > tft.width())  userX = tft.width();
    if (userY > tft.height()) userY = tft.height();
//    Serial.print("User X: ");   Serial.print(userX);
//    Serial.print("\tUser Y: "); Serial.println(userY);
    
    if (userY > (tft.height()-5)) {
      // Serial.println("Erase");
      // press the bottom of the screen to erase 
      tft.fillRect(0, BOXSIZE, tft.width(), tft.height()-BOXSIZE, TFT_BLACK);
    }

    // Select colors
    if (userY < BOXSIZE) {
       oldcolor = currentcolor;

       if (userX < BOXSIZE) { 
         currentcolor = TFT_RED; 
         tft.drawRect(0, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
       } else if (userX < BOXSIZE*2) {
         currentcolor = TFT_YELLOW;
         tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
       } else if (userX < BOXSIZE*3) {
         currentcolor = TFT_GREEN;
         tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
       } else if (userX < BOXSIZE*4) {
         currentcolor = TFT_CYAN;
         tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
       } else if (userX < BOXSIZE*5) {
         currentcolor = TFT_BLUE;
         tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
       } else if (userX < BOXSIZE*6) {
         currentcolor = TFT_MAGENTA;
         tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, TFT_WHITE);
       }
       if (oldcolor != currentcolor) {
          if (oldcolor == TFT_RED)     tft.fillRect(0,         0, BOXSIZE, BOXSIZE, TFT_RED);
          if (oldcolor == TFT_YELLOW)  tft.fillRect(BOXSIZE,   0, BOXSIZE, BOXSIZE, TFT_YELLOW);
          if (oldcolor == TFT_GREEN)   tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, TFT_GREEN);
          if (oldcolor == TFT_CYAN)    tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, TFT_CYAN);
          if (oldcolor == TFT_BLUE)    tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, TFT_BLUE);
          if (oldcolor == TFT_MAGENTA) tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, TFT_MAGENTA);
       }
    }
    if (((userY-PENRADIUS) > BOXSIZE) && ((userY+PENRADIUS) < tft.height())) {
      tft.fillCircle(userX, userY, PENRADIUS, currentcolor);
    }
  }
}

void drawnPalette() {
  
  tft.fillRect(0        , 0, BOXSIZE, BOXSIZE, TFT_RED);
  tft.fillRect(BOXSIZE  , 0, BOXSIZE, BOXSIZE, TFT_YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, TFT_GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, TFT_CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, TFT_BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, TFT_MAGENTA);
  tft.drawRect(0,         0, BOXSIZE, BOXSIZE, TFT_WHITE);
  currentcolor = TFT_RED;
}

void readTouchPoint() {
  // Switch SD CLK to read touch data
  digitalWrite(PIN_SD_SCK, HIGH);
  p = ts.getPoint(); // read to global
  digitalWrite(PIN_SD_SCK, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);
}

bool isValidPressure()
{
  readTouchPoint();
  // I using ESP 32, and reading on Z is 0 (zero) no touch (normally),
  // and Adafruit library return negative values or positive..
  //if (p.z > 10 && p.z < 1000) { // Original
  if (p.z > 1 || p.z < -1) {
    return true;
  }
  return false;
}

// Simple calibration steps
void doCalibration() {

  if (calibration == 0) {
    // Draw 0,0 circle
    tft.fillCircle(10, 10, 10, TFT_WHITE);
    tft.setCursor(5, 30);
    tft.setTextColor(TFT_WHITE); 
    tft.println("1: TOUCH ON CIRCLE TO CALIBRATE");
    calibration = 1;
    return;
  }
  if (calibration == 1) {
    if (isValidPressure()) {
      calibMinX = p.x;
      calibMinY = p.y;
      //Serial.print("Z:"); Serial.print(p.z);
      Serial.print("[minX:");
      Serial.print(calibMinX);
      Serial.print("][minY:");
      Serial.print(calibMinY);
      Serial.println("]");
      calibration = 2;
      tft.fillScreen(TFT_BLACK);
    }
    return;
  }
  if (calibration == 2) {
     // Draw 0,0 circle
    tft.fillCircle(tft.width() - 10, tft.height() - 10, 10, TFT_WHITE);
    tft.setCursor(5, 30);
    tft.setTextColor(TFT_WHITE); 
    tft.println("2: TOUCH ON CIRCLE TO CALIBRATE");
    calibration = 3;
    delay(3000);// Wait to not get multiple touch on last location, users will press a lot the second point ;P
    return;
  }
  if (calibration == 3) {
    if (isValidPressure()) {
      calibMaxX = p.x;
      calibMaxY = p.y;
      Serial.print("[maxX:");
      Serial.print(calibMaxX);
      Serial.print("][maxY:");
      Serial.print(calibMaxY);
      Serial.println("]");
      Serial.println("Calibration Ended.");
      calibration = 33; //  end
      tft.fillScreen(TFT_BLACK);
      drawnPalette();
    }
    return;
  }
}
