// Setup for the ESP32 Wemos D1 R32 (modified) in Parallel mode with ILI9486 display and resistive touch screen
#define USER_SETUP_ID 70

#define ESP32_PARALLEL
#define TFT_PARALLEL_8_BIT
#define ILI9486_DRIVER

#define TFT_WIDTH  320
#define TFT_HEIGHT 480

//Enable analogue touch screen reading
#define TOUCH_ANALOG
// Define which pins are used by touch screen
#define TOUCH_ANALOG_XP 12
#define TOUCH_ANALOG_YM 13
#define TOUCH_ANALOG_XM 15
#define TOUCH_ANALOG_YP 4
// ESP32 WiFi removes access to ADC2 channel so pins 4 and 15 attached to the touchscreen 
// no longer have analog input capability. So pins 35 and 39 is used as ADC1.
#define TOUCH_ANALOG_AXM 35
#define TOUCH_ANALOG_AYP 39

#define TFT_RST 32
#define TFT_CS 33
#define TFT_RD 2
#define TFT_DC 15 //muxed with X- touchscreen
#define TFT_WR 4  //muxed with Y+ touchscreen
#define TFT_D0 12 //muxed with X+ touchscreen
#define TFT_D1 13 //muxed with Y- touchscreen
#define TFT_D2 26
#define TFT_D3 25
#define TFT_D4 17
#define TFT_D5 16
#define TFT_D6 27
#define TFT_D7 14

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT
