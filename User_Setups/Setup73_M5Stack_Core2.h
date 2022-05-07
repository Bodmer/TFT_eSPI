#define USER_SETUP_ID 73

#define ILI9341_DRIVER

#define TFT_SDA_READ
#define M5STACK  // mirror image correction
#define TFT_INVERSION_ON

#define TFT_MISO 38
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS    5
#define TFT_DC   15
#define TFT_RST  -1
#define TOUCH_CS -1  // (suppress warning)

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT

#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  16000000
