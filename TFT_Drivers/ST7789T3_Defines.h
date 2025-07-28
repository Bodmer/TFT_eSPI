#ifndef TFT_WIDTH
  #define TFT_WIDTH  240
#endif
#ifndef TFT_HEIGHT
  #define TFT_HEIGHT 320
#endif

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80 // Not used unless commandlist invoked


// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0x00
#define TFT_SWRST   0x01

#define TFT_SLPIN   0x10
#define TFT_SLPOUT  0x11
#define TFT_NORON   0x13

#define TFT_INVOFF  0x20
#define TFT_INVON   0x21
#define TFT_DISPOFF 0x28
#define TFT_DISPON  0x29
#define TFT_CASET   0x2A
#define TFT_PASET   0x2B
#define TFT_RAMWR   0x2C
#define TFT_RAMRD   0x2E
#define TFT_MADCTL  0x36
#define TFT_COLMOD  0x3A

// Flags for TFT_MADCTL
#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_RGB 0x00
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH  0x04
#define TFT_MAD_SS  0x02
#define TFT_MAD_GS  0x01

#define TFT_MAD_COLOR_ORDER TFT_MAD_BGR

#define ST7789T3_GCTRL		  0xB7      // Gate control
#define ST7789T3_LCMCTRL	  0xC0      // LCM control
#define ST7789T3_IDSET		  0xC1      // ID setting
#define ST7789T3_VDVVRHEN	  0xC2      // VDV and VRH command enable
#define ST7789T3_VCMOFSET	  0xC5      // VCOMS offset set
#define ST7789T3_PVGAMCTRL	0xE0      // Positive voltage gamma control
#define ST7789T3_NVGAMCTRL	0xE1      // Negative voltage gamma control
#define ST7789T3_PWCTRL2    0xE8      // Power control 2
