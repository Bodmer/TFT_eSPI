
#if defined(USE_XIAO_EPAPER_DRIVER_BOARD)
#define TFT_SCLK D8
#define TFT_MISO D9
#define TFT_MOSI D10
#define TFT_CS D1
#define TFT_DC D3
#define TFT_BUSY D2
#define TFT_RST D0
#elif defined(USE_XIAO_EPAPER_DISPLAY_BOARD)
#define TFT_SCLK D8
#define TFT_MISO -1
#define TFT_MOSI D10
#define TFT_CS 44  // D7
#define TFT_DC 10  // D16
#define TFT_BUSY 4 // D3
#define TFT_RST 38 // D11
#elif defined(USE_RETERMINAL_E1001_BOARD)
#define TFT_SCLK D8
#define TFT_MISO -1
#define TFT_MOSI D10
#define TFT_CS 10   // D16
#define TFT_DC 11   // D19
#define TFT_BUSY 13 // D17
#define TFT_RST 12  // D18
#endif