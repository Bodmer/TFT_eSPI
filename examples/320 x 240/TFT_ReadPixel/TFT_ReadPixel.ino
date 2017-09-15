// Test readPixel() and readRect()

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

unsigned long runTime = 0;


void setup()
{
  //randomSeed(analogRead(A0));
  Serial.begin(115200);

  // Setup the LCD
  tft.init();
}

uint16_t pixel_buf[16*16];

void loop()
{
  runTime = millis();

  tft.fillScreen(ILI9341_BLACK);
  tft.drawPixel(1, 1, tft.color565(255, 0, 0));
  tft.drawPixel(10, 10, tft.color565(0, 255, 0));
  tft.drawPixel(3, 15, tft.color565(0, 0, 255));
  tft.drawPixel(13, 4, tft.color565(30<<3, 20<<2, 10<<3));
  Serial.println("readPixel() test:");
  printRGB(tft.readPixel(0, 0));
  printRGB(tft.readPixel(1, 1));
  printRGB(tft.readPixel(10, 10));
  printRGB(tft.readPixel(3, 15));
  printRGB(tft.readPixel(13, 4));

  tft.fillRect(20, 5, 18, 18, tft.color565(0x22, 0x22, 0x22));
  tft.drawPixel(11, 10, tft.color565(3<<3, 5<<2, 9<<3));
  tft.drawPixel(10, 11, tft.color565(0<<3, 1<<2, 2<<3));
  Serial.println("readRect() test:");
  tft.readRect(10, 10, 2, 2, pixel_buf);
  int i;
  for (i = 0; i < 4; ++i)
    printRGB((pixel_buf[i]<<8)|(pixel_buf[i]>>8));

  tft.readRect(0, 0, 16, 16, pixel_buf);
  tft.pushRect(20+1, 5+1, 16, 16, pixel_buf);

  while(1) yield();
}

void printRGB(uint16_t pixel)
{
  Serial.print("RGB: ");
  Serial.print(pixel >> 11);
  Serial.print(" ");
  Serial.print((pixel >> 5) & 0x3f);
  Serial.print(" ");
  Serial.println(pixel & 0x1f);
}
