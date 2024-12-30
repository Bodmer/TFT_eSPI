// Mandelbrot set generator
// Based on the C# version at:http://csharphelper.com/blog/2014/07/draw-a-mandelbrot-set-fractal-in-c/
// Tiling code adapted from SpriteRotatingCube.ino demo in this library
#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();  // Invoke custom library


// Mandelbrot calculation settings
// 1. Define area to show
double yMin = -1.25;
double yMax = 1.2;
double xMin = -2.5;
double xMax = 1.58;
// 2. Define max iterations per pixel
#define MaxIterations 30
// 3. Define max magnitude squared.
const int MAX_MAG_SQUARED = 10;

// Screen settings
// 1. Set screen size to use
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
// 2. Set tile size
#define TILEWIDTH  200
#define TILEHEIGHT 120

#define BACKLIGHT_PIN 5
#define COLOR_DEPTH 16

typedef struct rgb
{
  int r;
  int g;
  int b;
} rgb;

const int colourCount = 60;
uint32_t colours[MaxIterations];

// Sprite for drawing.
TFT_eSprite sprite  = TFT_eSprite(&tft);
// Pointer to start of sprite in RAM
uint16_t* spritePointer;

void setup()
{
  Serial.begin(115200); 
  delay(2000);
  randomSeed ( analogRead ( 0 ) );
  configureScreen();
  defineSprite();
  createColourArray();

  calculateMandelbrot();
}

void configureScreen()
{
  Serial.println("initialise screen");
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen( TFT_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  Serial.println("screen initialised\n");
}

void defineSprite()
{
  // Define sprite colour depth
  sprite.setColorDepth(COLOR_DEPTH);
  // Create the sprite
  spritePointer = (uint16_t*)sprite.createSprite(TILEWIDTH, TILEHEIGHT);

  // Define text datum and text colour for sprite
  sprite.setTextColor(TFT_BLACK);
  sprite.setTextDatum(MC_DATUM);
}

void loop()
{
  // put your main code here, to run repeatedly:
}

void createColourArray()
{
  int steps = MaxIterations/7;
  int offset = 0;
  rgb stages[7];
  stages[0].r = 0; stages[0].g = 0; stages[0].b = 0;
  stages[1].r = 0; stages[1].g = 0; stages[1].b = 255;
  stages[2].r = 0; stages[2].g = 255; stages[2].b = 0;
  stages[3].r = 0; stages[3].g = 0; stages[3].b = 0;
  stages[4].r = 255; stages[4].g = 255; stages[4].b = 255;
  stages[5].r = 0; stages[5].g = 0; stages[5].b = 0;
  stages[6].r = 255; stages[6].g = 255; stages[6].b = 255;

  for (int r = 0; r < 6; r++)
  {
    rgb startCol = stages[r];
    rgb endCol = stages[r + 1];

    double rDelta = (double)(endCol.r - startCol.r) / (double)steps;
    double gDelta = (double)(endCol.g - startCol.g) / (double)steps;
    double bDelta = (double)(endCol.b - startCol.b) / (double)steps;

    double newRed = startCol.r;
    double newGreen = startCol.g;
    double newBlue = startCol.b;
    for (int i = 0; i < steps; i++)
    {
      colours[offset] = tft.color565((int)newRed, (int)newGreen, (int)newBlue);
      newRed += rDelta;
      newGreen += gDelta;
      newBlue += bDelta;
      offset++;
    }
  }
}

void calculateMandelbrot()
{
  Serial.print("Calculate Mandelbrot set\n");
  Serial.print("("); Serial.print(xMin); Serial.print(", "); Serial.print(yMin); Serial.print(") -> (");
  Serial.print(xMax); Serial.print(", "); Serial.print(yMax); Serial.println(")");
  double xOffset = xMin;
  double yOffset = yMin;
  double xTiles = (double)SCREEN_WIDTH / (double)TILEWIDTH;
  double yTiles = (double)SCREEN_HEIGHT / (double)TILEHEIGHT;
  double xTileRealWidth = (xMax - xMin) / xTiles;
  double yTileRealHeight = (yMax - yMin) / yTiles;
  Serial.printf("drawing set, based on sprites sized (%dx%d) in screen (%dx%d) - tiles (%fx%f) - tile set space (%fx%f) \n ",TILEWIDTH,TILEHEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT,xTiles, yTiles,xTileRealWidth, yTileRealHeight);

  int startTime = millis();
  for(int x = 0; x < SCREEN_WIDTH; x+= TILEWIDTH)
  {
    for(int y = 0; y < SCREEN_HEIGHT; y+= TILEHEIGHT)
    {
      CalculateMandelbrotTile(xOffset, xOffset + xTileRealWidth, yOffset, yOffset + yTileRealHeight, x, y);
      yOffset += yTileRealHeight;
    }
    xOffset += xTileRealWidth;
    yOffset = yMin;
  }
  int endTime = millis();

  Serial.print("Calculation complete in: "); Serial.print(endTime - startTime); Serial.println("ms");
}

void CalculateMandelbrotTile(double xStart, double xEnd, double yStart, double yEnd, int spritePositionX, int spritePositionY)
{
  double dReaC = (xEnd - xStart) / (TILEWIDTH - 1);
  double dImaC = (yEnd - yStart) / (TILEHEIGHT - 1);
  double xStep = (xEnd - xStart) / (double)TILEWIDTH;
  double yStep = (yEnd - yStart) / (double)TILEHEIGHT;
  
  double Zr = 0;
  double Zim = 0;
  double Z2r = 0;
  double Z2im = 0;

  double xPos = xStart;
  double ReaC = xStart;
  long startTime = millis();

  for(int32_t x = 0; x < TILEWIDTH; x++)
  {
    double yPos = yStart;
    double ImaC = yStart;
    for (int32_t y = 0; y < TILEHEIGHT; y++)
    {
      double ReaZ = Zr;
      double ImaZ = Zim;
      double ReaZ2 = Z2r;
      double ImaZ2 = Z2im;
      int clr = 1;
      while ((ReaZ2 + ImaZ2 < MAX_MAG_SQUARED) && (clr < MaxIterations))
      {
        // Calculate Z(clr).
        ReaZ2 = ReaZ * ReaZ;
        ImaZ2 = ImaZ * ImaZ;
        ImaZ = 2 * ImaZ * ReaZ + ImaC;
        ReaZ = ReaZ2 - ImaZ2 + ReaC;
        clr++;
      }

      uint32_t colour =  colours[clr % MaxIterations];
      // Set the pixel's value.
      sprite.drawPixel(x, y,colour);
      ImaC += dImaC;
    }
    ReaC += dReaC;
  }
  long duration = millis() - startTime;
  Serial.printf("Calc a tile at position: (%d,%d) with data range (%f,%f) -> (%f,%f) duration %dms \n", spritePositionX, spritePositionY, xStart, yStart, xEnd, yEnd, duration);
  
  #ifdef USE_DMA_TO_TFT
  if (tft.dmaBusy()) prime_max++; // Increase processing load until just not busy
    tft.pushImageDMA(spritePositionX, spritePositionY, TILEWIDTH, TILEHEIGHT,  spritePointer);
  #else
  sprite.pushSprite(spritePositionX, spritePositionY); // Blocking write (no DMA) 115fps 
  #endif
  //sprite.fillSprite(TFT_BLACK);
}