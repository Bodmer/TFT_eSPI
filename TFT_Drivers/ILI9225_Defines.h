// Change the width and height if required (defined in portrait mode)
// or use the constructor to over-ride defaults
#define TFT_WIDTH  176
#define TFT_HEIGHT 220

/* ILI9225 LCD Registers */
#define ILI9225_DRIVER_OUTPUT_CTRL      (0x01u)  // Driver Output Control
#define ILI9225_LCD_AC_DRIVING_CTRL     (0x02u)  // LCD AC Driving Control
#define ILI9225_ENTRY_MODE              (0x03u)  // Entry Mode
#define ILI9225_DISP_CTRL1              (0x07u)  // Display Control 1
#define ILI9225_BLANK_PERIOD_CTRL1      (0x08u)  // Blank Period Control
#define ILI9225_FRAME_CYCLE_CTRL        (0x0Bu)  // Frame Cycle Control
#define ILI9225_INTERFACE_CTRL          (0x0Cu)  // Interface Control
#define ILI9225_OSC_CTRL                (0x0Fu)  // Osc Control
#define ILI9225_POWER_CTRL1             (0x10u)  // Power Control 1
#define ILI9225_POWER_CTRL2             (0x11u)  // Power Control 2
#define ILI9225_POWER_CTRL3             (0x12u)  // Power Control 3
#define ILI9225_POWER_CTRL4             (0x13u)  // Power Control 4
#define ILI9225_POWER_CTRL5             (0x14u)  // Power Control 5
#define ILI9225_VCI_RECYCLING           (0x15u)  // VCI Recycling
#define ILI9225_RAM_ADDR_SET1           (0x20u)  // Horizontal GRAM Address Set
#define ILI9225_RAM_ADDR_SET2           (0x21u)  // Vertical GRAM Address Set
#define ILI9225_GRAM_DATA_REG           (0x22u)  // GRAM Data Register
#define ILI9225_GATE_SCAN_CTRL          (0x30u)  // Gate Scan Control Register
#define ILI9225_VERTICAL_SCROLL_CTRL1   (0x31u)  // Vertical Scroll Control 1 Register
#define ILI9225_VERTICAL_SCROLL_CTRL2   (0x32u)  // Vertical Scroll Control 2 Register
#define ILI9225_VERTICAL_SCROLL_CTRL3   (0x33u)  // Vertical Scroll Control 3 Register
#define ILI9225_PARTIAL_DRIVING_POS1    (0x34u)  // Partial Driving Position 1 Register
#define ILI9225_PARTIAL_DRIVING_POS2    (0x35u)  // Partial Driving Position 2 Register
#define ILI9225_HORIZONTAL_WINDOW_ADDR1 (0x36u)  // Horizontal Address Start Position
#define ILI9225_HORIZONTAL_WINDOW_ADDR2 (0x37u)  // Horizontal Address End Position
#define ILI9225_VERTICAL_WINDOW_ADDR1   (0x38u)  // Vertical Address Start Position
#define ILI9225_VERTICAL_WINDOW_ADDR2   (0x39u)  // Vertical Address End Position
#define ILI9225_GAMMA_CTRL1             (0x50u)  // Gamma Control 1
#define ILI9225_GAMMA_CTRL2             (0x51u)  // Gamma Control 2
#define ILI9225_GAMMA_CTRL3             (0x52u)  // Gamma Control 3
#define ILI9225_GAMMA_CTRL4             (0x53u)  // Gamma Control 4
#define ILI9225_GAMMA_CTRL5             (0x54u)  // Gamma Control 5
#define ILI9225_GAMMA_CTRL6             (0x55u)  // Gamma Control 6
#define ILI9225_GAMMA_CTRL7             (0x56u)  // Gamma Control 7
#define ILI9225_GAMMA_CTRL8             (0x57u)  // Gamma Control 8
#define ILI9225_GAMMA_CTRL9             (0x58u)  // Gamma Control 9
#define ILI9225_GAMMA_CTRL10            (0x59u)  // Gamma Control 10

#define TFT_INVOFF  0x20
#define TFT_INVON   0x21
#define TFT_SWRST   0x28

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80 // Not used unless commandlist invoked

// autoincrement modes (register ILI9225_ENTRY_MODE, bit 5..3 )
enum autoIncMode_t { R2L_BottomUp, BottomUp_R2L, L2R_BottomUp, BottomUp_L2R, R2L_TopDown, TopDown_R2L, L2R_TopDown, TopDown_L2R };

        // correspondig modes if orientation changed:
        const autoIncMode_t modeTab [3][8] = {
        //          { R2L_BottomUp, BottomUp_R2L, L2R_BottomUp, BottomUp_L2R, R2L_TopDown,  TopDown_R2L,  L2R_TopDown,  TopDown_L2R }//
        /* 90° */   { BottomUp_L2R, L2R_BottomUp, TopDown_L2R,  L2R_TopDown,  BottomUp_R2L, R2L_BottomUp, TopDown_R2L,  R2L_TopDown },   
        /*180° */   { L2R_TopDown , TopDown_L2R,  R2L_TopDown,  TopDown_R2L,  L2R_BottomUp, BottomUp_L2R, R2L_BottomUp, BottomUp_R2L}, 
        /*270° */   { TopDown_R2L , R2L_TopDown,  BottomUp_R2L, R2L_BottomUp, TopDown_L2R,  L2R_TopDown,  BottomUp_L2R, L2R_BottomUp}
                };

#define TFT_CASET   0x20
#define TFT_PASET   0x21
#define TFT_RAMWR   0x22
#define TFT_RAMRD   0x23
