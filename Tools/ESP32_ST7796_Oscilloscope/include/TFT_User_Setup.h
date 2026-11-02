#define USER_SETUP_INFO "ESP32-WROOM-32 with generic ST7796 SPI display"

#define ST7796_DRIVER

#define TFT_MISO -1
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4

#define TFT_WIDTH  320
#define TFT_HEIGHT 480

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4

#define SPI_FREQUENCY 80000000