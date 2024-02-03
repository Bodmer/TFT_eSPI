// Change the width and height if required (defined in portrait mode)
// or use the constructor to over-ride defaults
#define TFT_WIDTH  240
#define TFT_HEIGHT 320


// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80 // Not used unless commandlist invoked

// Note: HX8347D doesn't have NOP command. 0x00 is assigned as display ID register (read only).
// 0x00 still be used here as dummy register to keep the macro usage happy

// Generic commands used by TFT_eSPar.cpp
#define TFT_NOP     0x00
#define TFT_SWRST   0x00

#define TFT_INVOFF  0x20
#define TFT_INVON   0x21

#define TFT_CASET1  HX8347D_CASET_S2
#define TFT_CASET2  HX8347D_CASET_S1
#define TFT_CASET3  HX8347D_CASET_E2
#define TFT_CASET4  HX8347D_CASET_E1
#define TFT_PASET1  HX8347D_PASET_S2
#define TFT_PASET2  HX8347D_PASET_S1
#define TFT_PASET3  HX8347D_PASET_E2
#define TFT_PASET4  HX8347D_PASET_E1
#define TFT_RAMWR   HX8347D_RAMWR
#define TFT_RAMRD   0x00

#define TFT_MADCTL  HX8347D_MADCTL

#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_RGB 0x00
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH  0x04
#define TFT_MAD_SS  0x02
#define TFT_MAD_GS  0x01

#ifdef TFT_RGB_ORDER
  #if (TFT_RGB_ORDER == 1)
    #define TFT_MAD_COLOR_ORDER TFT_MAD_RGB
  #else
    #define TFT_MAD_COLOR_ORDER TFT_MAD_BGR
  #endif
#else
  #define TFT_MAD_COLOR_ORDER TFT_MAD_BGR
#endif

#define HX8347D_RDDID     0x00

#define HX8347D_CASET_S2  0x02
#define HX8347D_CASET_S1  0x03
#define HX8347D_CASET_E2  0x04
#define HX8347D_CASET_E1  0x05
#define HX8347D_PASET_S2  0x06
#define HX8347D_PASET_S1  0x07
#define HX8347D_PASET_E2  0x08
#define HX8347D_PASET_E1  0x09
#define HX8347D_RAMWR     0x22

#define HX8347D_MADCTL    0x16
#define HX8347D_COLMOD    0x17

#define HX8347D_SETOSC2   0x18
#define HX8347D_SETOSC1   0x19

#define HX8347D_PWRBT     0x1A
#define HX8347D_PWRVRH    0x1B
#define HX8347D_PWRCTL    0x1F
#define HX8347D_VCOMVMF   0x23
#define HX8347D_VCOMVMH   0x24
#define HX8347D_VCOMVML   0x25

#define HX8347D_DISPMODE  0x01
#define HX8347D_DISPCTL   0x28

#define HX8347D_SETPANEL  0x36

#define HX8347D_SETSTBA   0xEA
#define HX8347D_SETPTBA   0xEC
#define HX8347D_SETSOPN   0xE8
#define HX8347D_SETSOPI   0xE9

#define HX8347D_SETGAMMA1 0x40
#define HX8347D_SETGAMMA2 0x50