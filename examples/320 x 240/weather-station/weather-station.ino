/**The MIT License (MIT)
  Copyright (c) 2015 by Daniel Eichhorn
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYBR_DATUM HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  See more at http://blog.squix.ch

  Adapted by Bodmer to use the faster TFT_eSPI library:
  https://github.com/Bodmer/TFT_eSPI

  Plus:
  Minor changes to text placement and auto-blanking out old text with background colour padding
  Moon phase text added
  Forecast text lines are automatically split onto two lines at a central space (some are long!)
  Time is printed with colons aligned to tidy display
  Min and max forecast temperatures spaced out
  The ` character has been changed to a degree symbol in the 36 point font
  New smart WU splash startup screen and updated progress messages
  Display does not need to be blanked between updates
  Icons nudged about slightly to add wind direction + speed
  Barometric pressure added
*/

#define SERIAL_MESSAGES

#include <Arduino.h>

#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

// Additional UI functions
#include "GfxUi.h"

// Fonts created by http://oleddisplay.squix.ch/
#include "ArialRoundedMTBold_14.h"
#include "ArialRoundedMTBold_36.h"

// Download helper
#include "WebResource.h"

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// Helps with connecting to internet
#include <WiFiManager.h>

// check settings.h for adapting to your needs
#include "settings.h"
#include <JsonListener.h>
#include <WundergroundClient.h>
#include "TimeClient.h"

// HOSTNAME for OTA update
#define HOSTNAME "ESP8266-OTA-"

/*****************************
   Important: see settings.h to configure your settings!!!
 * ***************************/

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

boolean booted = true;

GfxUi ui = GfxUi(&tft);

WebResource webResource;
TimeClient timeClient(UTC_OFFSET);

// Set to false, if you prefere imperial/inches, Fahrenheit
WundergroundClient wunderground(IS_METRIC);

//declaring prototypes
void configModeCallback (WiFiManager *myWiFiManager);
void downloadCallback(String filename, int16_t bytesDownloaded, int16_t bytesTotal);
ProgressCallback _downloadCallback = downloadCallback;
void downloadResources();
void updateData();
void drawProgress(uint8_t percentage, String text);
void drawTime();
void drawCurrentWeather();
void drawForecast();
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex);
String getMeteoconIcon(String iconText);
void drawAstronomy();
void drawSeparator(uint16_t y);

long lastDownloadUpdate = millis();

void setup() {
#ifdef SERIAL_MESSAGES
  Serial.begin(250000);
#endif
  tft.begin();
  tft.fillScreen(TFT_BLACK);

  tft.setFreeFont(&ArialRoundedMTBold_14);
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawString("Original by: blog.squix.org", 120, 240);
  tft.drawString("Adapted by: Bodmer", 120, 260);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  
  SPIFFS.begin();
  //listFiles();
  //Uncomment if you want to erase SPIFFS and update all internet resources, this takes some time!
  //tft.drawString("Formatting SPIFFS, so wait!", 120, 200); SPIFFS.format();

  if (SPIFFS.exists("/WU.jpg") == true) ui.drawJpeg("/WU.jpg", 0, 10);
  if (SPIFFS.exists("/Earth.jpg") == true) ui.drawJpeg("/Earth.jpg", 0, 320-56); // Image is 56 pixels high
  delay(1000);
  tft.drawString("Connecting to WiFi", 120, 200);
  tft.setTextPadding(240); // Pad next drawString() text to full width to over-write old text

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  // Uncomment for testing wifi manager
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);

  //or use this for auto generated name ESP + ChipID
  wifiManager.autoConnect();

  //Manual Wifi
  //WiFi.begin(WIFI_SSID, WIFI_PWD);

  // OTA Setup
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  WiFi.hostname(hostname);
  ArduinoOTA.setHostname((const char *)hostname.c_str());
  ArduinoOTA.begin();

  // download images from the net. If images already exist don't download
  tft.drawString("Downloading to SPIFFS...", 120, 200);
  tft.drawString(" ", 120, 240);  // Clear line
  tft.drawString(" ", 120, 260);  // Clear line
  downloadResources();
  //listFiles();
  tft.setTextDatum(BC_DATUM);
  tft.setTextPadding(240); // Pad next drawString() text to full width to over-write old text
  tft.drawString(" ", 120, 200);  // Clear line above using set padding width
  tft.drawString("Fetching weather data...", 120, 200);
  //delay(500);

  // load the weather information
  updateData();
}

long lastDrew = 0;
void loop() {
  // Handle OTA update requests
  ArduinoOTA.handle();

  // Check if we should update the clock
  if (millis() - lastDrew > 30000 && wunderground.getSeconds() == "00") {
    drawTime();
    lastDrew = millis();
  }

  // Check if we should update weather information
  if (millis() - lastDownloadUpdate > 1000 * UPDATE_INTERVAL_SECS) {
    updateData();
    lastDownloadUpdate = millis();
  }
}

// Called if WiFi has not been configured yet
void configModeCallback (WiFiManager *myWiFiManager) {
  tft.setTextDatum(BC_DATUM);
  tft.setFreeFont(&ArialRoundedMTBold_14);
  tft.setTextColor(TFT_ORANGE);
  tft.drawString("Wifi Manager", 120, 28);
  tft.drawString("Please connect to AP", 120, 42);
  tft.setTextColor(TFT_WHITE);
  tft.drawString(myWiFiManager->getConfigPortalSSID(), 120, 56);
  tft.setTextColor(TFT_ORANGE);
  tft.drawString("To setup Wifi Configuration", 120, 70);
}

// callback called during download of files. Updates progress bar
void downloadCallback(String filename, int16_t bytesDownloaded, int16_t bytesTotal) {
  Serial.println(String(bytesDownloaded) + " / " + String(bytesTotal));

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(240);

  int percentage = 100 * bytesDownloaded / bytesTotal;
  if (percentage == 0) {
    tft.drawString(filename, 120, 220);
  }
  if (percentage % 5 == 0) {
    tft.setTextDatum(TC_DATUM);
    tft.setTextPadding(tft.textWidth(" 888% "));
    tft.drawString(String(percentage) + "%", 120, 245);
    ui.drawProgressBar(10, 225, 240 - 20, 15, percentage, TFT_WHITE, TFT_BLUE);
  }

}

// Download the bitmaps
void downloadResources() {
  // tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(&ArialRoundedMTBold_14);
  char id[5];

  // Download WU graphic jpeg first and display it, then the Earth view
  webResource.downloadFile((String)"http://i.imgur.com/njl1pMj.jpg", (String)"/WU.jpg", _downloadCallback);
  if (SPIFFS.exists("/WU.jpg") == true) ui.drawJpeg("/WU.jpg", 0, 10);

  webResource.downloadFile((String)"http://i.imgur.com/v4eTLCC.jpg", (String)"/Earth.jpg", _downloadCallback);
  if (SPIFFS.exists("/Earth.jpg") == true) ui.drawJpeg("/Earth.jpg", 0, 320-56);
  
  //webResource.downloadFile((String)"http://i.imgur.com/IY57GSv.jpg", (String)"/Horizon.jpg", _downloadCallback);
  //if (SPIFFS.exists("/Horizon.jpg") == true) ui.drawJpeg("/Horizon.jpg", 0, 320-160);

  //webResource.downloadFile((String)"http://i.imgur.com/jZptbtY.jpg", (String)"/Rainbow.jpg", _downloadCallback);
  //if (SPIFFS.exists("/Rainbow.jpg") == true) ui.drawJpeg("/Rainbow.jpg", 0, 0);

  for (int i = 0; i < 19; i++) {
    sprintf(id, "%02d", i);
    webResource.downloadFile("http://www.squix.org/blog/wunderground/" + wundergroundIcons[i] + ".bmp", wundergroundIcons[i] + ".bmp", _downloadCallback);
  }
  for (int i = 0; i < 19; i++) {
    sprintf(id, "%02d", i);
    webResource.downloadFile("http://www.squix.org/blog/wunderground/mini/" + wundergroundIcons[i] + ".bmp", "/mini/" + wundergroundIcons[i] + ".bmp", _downloadCallback);
  }
  for (int i = 0; i < 24; i++) {
    webResource.downloadFile("http://www.squix.org/blog/moonphase_L" + String(i) + ".bmp", "/moon" + String(i) + ".bmp", _downloadCallback);
  }
}

// Update the internet based information and update screen
void updateData() {
  // booted = true;  // Test only
  // booted = false; // Test only

  if (booted) ui.drawJpeg("/WU.jpg", 0, 10); // May have already drawn this but it does not take long
  else tft.drawCircle(22, 22, 18, TFT_DARKGREY); // Outer ring - optional

  if (booted) drawProgress(20, "Updating time...");
  else fillSegment(22, 22, 0, (int) (20 * 3.6), 16, TFT_NAVY);

  timeClient.updateTime();
  if (booted) drawProgress(50, "Updating conditions...");
  else fillSegment(22, 22, 0, (int) (50 * 3.6), 16, TFT_NAVY);

  wunderground.updateConditions(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  if (booted) drawProgress(70, "Updating forecasts...");
  else fillSegment(22, 22, 0, (int) (70 * 3.6), 16, TFT_NAVY);

  wunderground.updateForecast(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  if (booted) drawProgress(90, "Updating astronomy...");
  else fillSegment(22, 22, 0, (int) (90 * 3.6), 16, TFT_NAVY);

  wunderground.updateAstronomy(WUNDERGRROUND_API_KEY, WUNDERGRROUND_LANGUAGE, WUNDERGROUND_COUNTRY, WUNDERGROUND_CITY);
  // lastUpdate = timeClient.getFormattedTime();
  // readyForWeatherUpdate = false;
  if (booted) drawProgress(100, "Done...");
  else fillSegment(22, 22, 0, 360, 16, TFT_NAVY);

  if (booted) delay(2000);

  if (booted) tft.fillScreen(TFT_BLACK);
  else   fillSegment(22, 22, 0, 360, 22, TFT_BLACK);

  //tft.fillScreen(TFT_CYAN); // For text padding and update graphics over-write checking only
  drawTime();
  drawCurrentWeather();
  drawForecast();
  drawAstronomy();
  booted = false;
}

// Progress bar helper
void drawProgress(uint8_t percentage, String text) {
  tft.setFreeFont(&ArialRoundedMTBold_14);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(240);
  tft.drawString(text, 120, 220);

  ui.drawProgressBar(10, 225, 240 - 20, 15, percentage, TFT_WHITE, TFT_BLUE);

  tft.setTextPadding(0);
}

// draws the clock
void drawTime() {

  tft.setFreeFont(&ArialRoundedMTBold_36);

  String timeNow = timeClient.getHours() + ":" + timeClient.getMinutes();

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" 44:44 "));  // String width + margin
  tft.drawString(timeNow, 120, 53);

  tft.setFreeFont(&ArialRoundedMTBold_14);

  String date = wunderground.getDate();

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" Ddd, 44 Mmm 4444 "));  // String width + margin
  tft.drawString(date, 120, 16);

  drawSeparator(54);

  tft.setTextPadding(0);
}

// draws current weather information
void drawCurrentWeather() {
  // Weather Icon
  String weatherIcon = getMeteoconIcon(wunderground.getTodayIcon());
  //uint32_t dt = millis();
  ui.drawBmp(weatherIcon + ".bmp", 0, 59);
  //Serial.print("Icon draw time = "); Serial.println(millis()-dt);

  // Weather Text

  String weatherText = wunderground.getWeatherText();
  //weatherText = "Heavy Thunderstorms with Small Hail"; // Test line splitting with longest(?) string

  tft.setFreeFont(&ArialRoundedMTBold_14);

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);

  int splitPoint = 0;
  int xpos = 230;
  splitPoint =  splitIndex(weatherText);
  if (splitPoint > 16) xpos = 235;

  tft.setTextPadding(tft.textWidth("Heavy Thunderstorms"));  // Max anticipated string width
  if (splitPoint) tft.drawString(weatherText.substring(0, splitPoint), xpos, 72);
  tft.setTextPadding(tft.textWidth(" with Small Hail"));  // Max anticipated string width + margin
  tft.drawString(weatherText.substring(splitPoint), xpos, 87);

  tft.setFreeFont(&ArialRoundedMTBold_36);

  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);

  // Font ASCII code 96 (0x60) modified to make "`" a degree symbol
  tft.setTextPadding(tft.textWidth("-88`")); // Max width of vales

  weatherText = wunderground.getCurrentTemp();
  if (weatherText.indexOf(".")) weatherText = weatherText.substring(0, weatherText.indexOf(".")); // Make it integer temperature
  if (weatherText == "") weatherText = "?";  // Handle null return
  tft.drawString(weatherText + "`", 221, 100);

  tft.setFreeFont(&ArialRoundedMTBold_14);

  tft.setTextDatum(TL_DATUM);
  tft.setTextPadding(0);
  if (IS_METRIC) tft.drawString("C ", 221, 100);
  else  tft.drawString("F ", 221, 100);

  //tft.drawString(wunderground.getPressure(), 180, 30);
  
  weatherText = ""; //wunderground.getWindDir() + " ";
  weatherText += String((int)(wunderground.getWindSpeed().toInt() * WIND_SPEED_SCALING)) + WIND_SPEED_UNITS;

  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(tft.textWidth(" 888 mph")); // Max string length?
  tft.drawString(weatherText, 128, 136);

  weatherText = wunderground.getPressure();

  tft.setTextDatum(TR_DATUM);
  tft.setTextPadding(tft.textWidth(" 8888mb")); // Max string length?
  tft.drawString(weatherText, 230, 136);
  
  weatherText = wunderground.getWindDir();

  int windAngle = 0;
  String compassCardinal = "";
  switch (weatherText.length()) {
    case 1:
      compassCardinal = "N E S W "; // Not used, see default below
      windAngle = 90 * compassCardinal.indexOf(weatherText) / 2;
      break;
    case 2:
      compassCardinal = "NE SE SW NW";
      windAngle = 45 + 90 * compassCardinal.indexOf(weatherText) / 3;
      break;
    case 3:
      compassCardinal = "NNE ENE ESE SSE SSW WSW WNW NNW";
      windAngle = 22 + 45 * compassCardinal.indexOf(weatherText) / 4; // 22 should be 22.5 but accuracy is not needed!
      break;
    default:
      if (weatherText == "Variable") windAngle = -1;
      else {
        //                 v23456v23456v23456v23456 character ruler
        compassCardinal = "North East  South West"; // Possible strings
        windAngle = 90 * compassCardinal.indexOf(weatherText) / 6;
      }
      break;
  }

  tft.fillCircle(128, 110, 23, TFT_BLACK); // Erase old plot, radius + 1 to delete stray pixels
  tft.drawCircle(128, 110, 22, TFT_DARKGREY);    // Outer ring - optional
  if ( windAngle >= 0 ) fillSegment(128, 110, windAngle - 15, 30, 22, TFT_GREEN); // Might replace this with a bigger rotating arrow
  tft.drawCircle(128, 110, 6, TFT_RED);

  drawSeparator(153);

  tft.setTextDatum(TL_DATUM); // Reset datum to normal
  tft.setTextPadding(0); // Reset padding width to none
}

// draws the three forecast columns
void drawForecast() {
  drawForecastDetail(10, 171, 0);
  drawForecastDetail(95, 171, 2);
  drawForecastDetail(180, 171, 4);
  drawSeparator(171 + 69);
}

// helper for the forecast columns
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex) {
  tft.setFreeFont(&ArialRoundedMTBold_14);

  String day = wunderground.getForecastTitle(dayIndex).substring(0, 3);
  day.toUpperCase();

  tft.setTextDatum(BC_DATUM);

  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("WWW"));
  tft.drawString(day, x + 25, y);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("-88   -88"));
  tft.drawString(wunderground.getForecastHighTemp(dayIndex) + "   " + wunderground.getForecastLowTemp(dayIndex), x + 25, y + 14);

  String weatherIcon = getMeteoconIcon(wunderground.getForecastIcon(dayIndex));
  ui.drawBmp("/mini/" + weatherIcon + ".bmp", x, y + 15);

  tft.setTextPadding(0); // Reset padding width to none
}

// draw moonphase and sunrise/set and moonrise/set
void drawAstronomy() {
  tft.setFreeFont(&ArialRoundedMTBold_14);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" Waxing Crescent "));
  tft.drawString(wunderground.getMoonPhase(), 120, 260 - 2);

  int moonAgeImage = 24 * wunderground.getMoonAge().toInt() / 30.0;
  ui.drawBmp("/moon" + String(moonAgeImage) + ".bmp", 120 - 30, 260);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(0); // Reset padding width to none
  tft.drawString("Sun", 40, 280);

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" 88:88 "));
  int dt = rightOffset(wunderground.getSunriseTime(), ":"); // Draw relative to colon to them aligned
  tft.drawString(wunderground.getSunriseTime(), 40 + dt, 300);

  dt = rightOffset(wunderground.getSunsetTime(), ":");
  tft.drawString(wunderground.getSunsetTime(), 40 + dt, 315);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(0); // Reset padding width to none
  tft.drawString("Moon", 200, 280);

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" 88:88 "));
  dt = rightOffset(wunderground.getMoonriseTime(), ":"); // Draw relative to colon to them aligned
  tft.drawString(wunderground.getMoonriseTime(), 200 + dt, 300);

  dt = rightOffset(wunderground.getMoonsetTime(), ":");
  tft.drawString(wunderground.getMoonsetTime(), 200 + dt, 315);

  tft.setTextPadding(0); // Reset padding width to none
}

// Helper function, should be part of the weather station library and should disappear soon
String getMeteoconIcon(String iconText) {
  if (iconText == "F") return "chanceflurries";
  if (iconText == "Q") return "chancerain";
  if (iconText == "W") return "chancesleet";
  if (iconText == "V") return "chancesnow";
  if (iconText == "S") return "chancetstorms";
  if (iconText == "B") return "clear";
  if (iconText == "Y") return "cloudy";
  if (iconText == "F") return "flurries";
  if (iconText == "M") return "fog";
  if (iconText == "E") return "hazy";
  if (iconText == "Y") return "mostlycloudy";
  if (iconText == "H") return "mostlysunny";
  if (iconText == "H") return "partlycloudy";
  if (iconText == "J") return "partlysunny";
  if (iconText == "W") return "sleet";
  if (iconText == "R") return "rain";
  if (iconText == "W") return "snow";
  if (iconText == "B") return "sunny";
  if (iconText == "0") return "tstorms";


  return "unknown";
}

// if you want separators, uncomment the tft-line
void drawSeparator(uint16_t y) {
  tft.drawFastHLine(10, y, 240 - 2 * 10, 0x4228);
}

// determine the "space" split point in a long string
int splitIndex(String text)
{
  int index = 0;
  while ( (text.indexOf(' ', index) >= 0) && ( index <= text.length() / 2 ) ) {
    index = text.indexOf(' ', index) + 1;
  }
  if (index) index--;
  return index;
}

// Calculate coord delta from start of text String to start of sub String contained within that text
// Can be used to vertically right align text so for example a colon ":" in the time value is always
// plotted at same point on the screen irrespective of different proportional character widths,
// could also be used to align decimal points for neat formatting
int rightOffset(String text, String sub)
{
  int index = text.indexOf(sub);
  return tft.textWidth(text.substring(index));
}

// Calculate coord delta from start of text String to start of sub String contained within that text
// Can be used to vertically left align text so for example a colon ":" in the time value is always
// plotted at same point on the screen irrespective of different proportional character widths,
// could also be used to align decimal points for neat formatting
int leftOffset(String text, String sub)
{
  int index = text.indexOf(sub);
  return tft.textWidth(text.substring(0, index));
}

// Draw a segment of a circle, centred on x,y with defined start_angle and subtended sub_angle
// Angles are defined in a clockwise direction with 0 at top
// Segment has radius r and it is plotted in defined colour
// Can be used for pie charts etc, in this sketch it is used for wind direction
#define DEG2RAD 0.0174532925 // Degrees to Radians conversion factor
#define INC 2 // Minimum segment subtended angle and plotting angle increment (in degrees)
void fillSegment(int x, int y, int start_angle, int sub_angle, int r, unsigned int colour)
{
  // Calculate first pair of coordinates for segment start
  float sx = cos((start_angle - 90) * DEG2RAD);
  float sy = sin((start_angle - 90) * DEG2RAD);
  uint16_t x1 = sx * r + x;
  uint16_t y1 = sy * r + y;

  // Draw colour blocks every INC degrees
  for (int i = start_angle; i < start_angle + sub_angle; i += INC) {

    // Calculate pair of coordinates for segment end
    int x2 = cos((i + 1 - 90) * DEG2RAD) * r + x;
    int y2 = sin((i + 1 - 90) * DEG2RAD) * r + y;

    tft.fillTriangle(x1, y1, x2, y2, x, y, colour);

    // Copy segment end to sgement start for next segment
    x1 = x2;
    y1 = y2;
  }
}
