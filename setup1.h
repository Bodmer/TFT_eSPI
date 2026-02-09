// setup1.h - KONFIGURASI LCD TFT 3.5 INCH
// Simpan file ini di folder yang sama dengan sketch Anda
// JANGAN edit User_Setup.h di library
 
#define ESP32_PARALLEL
 
// Pin kontrol sesuai permintaan
#define TFT_CS   33    // LCD_CS
#define TFT_DC   15    // LCD_RS
#define TFT_RST  32    // LCD_RST
 
#define TFT_WR    4    // LCD_WR
#define TFT_RD    2    // LCD_RD
 
// Pin data 8-bit
#define TFT_D0   12    // LCD_D0
#define TFT_D1   13    // LCD_D1
#define TFT_D2   26    // LCD_D2
#define TFT_D3   25    // LCD_D3
#define TFT_D4   21    // LCD_D4
#define TFT_D5    5    // LCD_D5
#define TFT_D6   27    // LCD_D6
#define TFT_D7   14    // LCD_D7
 
// Ukuran layar
#define TFT_WIDTH  480
#define TFT_HEIGHT 320
 
// Font dan fitur
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
 
#define SMOOTH_FONT
 
// Orientasi layar (0-3)
#define TFT_MAD_COLOR_ORDER  TFT_RGB
