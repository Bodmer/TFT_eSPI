// Change the width and height if required (defined in portrait mode)
// or use the constructor to over-ride defaults
// Based ON: ILI9225_Defines.h for ST7781 / SPFD5408 (MCUFRIEND UNO/Mega Display Shield)
// https://www.crystalfontz.com/controllers/Sitronix/ST7781/
// https://www.rhydolabz.com/documents/SPFD5408A.pdf


#define WRITE_COMMAND_16        // Change TFT_eSPI::writecommand(); to use tft_Write_16(c);

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0x00
#define TFT_SWRST   0x00        // NO Software Reset ?

#define TFT_CASET 0
#define TFT_PASET 0

#define TFT_CASET1     ST7781_HORIZONTAL_WINDOW_ADDR1
#define TFT_CASET2     ST7781_HORIZONTAL_WINDOW_ADDR2

#define TFT_PASET1     ST7781_VERTICAL_WINDOW_ADDR1
#define TFT_PASET2     ST7781_VERTICAL_WINDOW_ADDR2

#define TFT_RAM_ADDR1  ST7781_RAM_ADDR_SET1
#define TFT_RAM_ADDR2  ST7781_RAM_ADDR_SET2

#define TFT_RAMWR      ST7781_RAM_DATA_REG

#define TFT_MAD_BGR 0x10
#define TFT_MAD_RGB 0x00

#ifdef TFT_RGB_ORDER
  #if (TFT_RGB_ORDER == 1)
    #define TFT_MAD_COLOR_ORDER TFT_MAD_RGB
  #else
    #define TFT_MAD_COLOR_ORDER TFT_MAD_BGR
  #endif
#else
  #define TFT_MAD_COLOR_ORDER TFT_MAD_BGR
#endif

// Not used
#define TFT_INVOFF  0x00
#define TFT_INVON   0x00
#define TFT_RAMRD   0x00
#define TFT_IDXRD   0x00

/* ST7781 Registers */
#define ST7781_DRIVER_OUTPUT_CTRL      0x01  // Driver Output Control
#define ST7781_LCD_AC_DRIVING_CTRL     0x02  // LCD AC Driving Control
#define ST7781_ENTRY_MODE              0x03  // Entry Mode
#define ST7781_RESIZE_CTRL             0x04  // Resize Control
#define ST7781_DISP_CTRL1              0x07  // Display Control 1
#define ST7781_DISP_CTRL2              0x08  // Display Control 2
#define ST7781_DISP_CTRL3              0x09  // Display Control 3
#define ST7781_DISP_CTRL4              0x0A  // Display Control 4
#define ST7781_FRAME_MAKER_POS         0x0D  // Frame Maker Position
#define ST7781_POWER_CTRL1             0x10  // Power Control 1
#define ST7781_POWER_CTRL2             0x11  // Power Control 2
#define ST7781_POWER_CTRL3             0x12  // Power Control 3
#define ST7781_POWER_CTRL4             0x13  // Power Control 4
#define ST7781_RAM_ADDR_SET1           0x20  // Horizontal GRAM Address Set
#define ST7781_RAM_ADDR_SET2           0x21  // Vertical GRAM Address Set
#define ST7781_RAM_DATA_REG            0x22  // Read/Write to RAM Data Register
#define ST7781_VCOMH_CTRL              0x29  // VCOMH Control
#define ST7781_FRAME_COLOR_CTRL        0x2B  // Frame Rate and Color Control
#define ST7781_GAMMA_CTRL1             0x30  // Gamma Control 1
#define ST7781_GAMMA_CTRL2             0x31  // Gamma Control 2
#define ST7781_GAMMA_CTRL3             0x32  // Gamma Control 3
#define ST7781_GAMMA_CTRL4             0x35  // Gamma Control 4
#define ST7781_GAMMA_CTRL5             0x36  // Gamma Control 5
#define ST7781_GAMMA_CTRL6             0x37  // Gamma Control 6
#define ST7781_GAMMA_CTRL7             0x38  // Gamma Control 7
#define ST7781_GAMMA_CTRL8             0x39  // Gamma Control 8
#define ST7781_GAMMA_CTRL9             0x3C  // Gamma Control 9
#define ST7781_GAMMA_CTRL10            0x3D  // Gamma Control 10
#define ST7781_HORIZONTAL_WINDOW_ADDR1 0x50  // Horizontal Address Start Position
#define ST7781_HORIZONTAL_WINDOW_ADDR2 0x51  // Horizontal Address End Position
#define ST7781_VERTICAL_WINDOW_ADDR1   0x52  // Vertical Address Start Position
#define ST7781_VERTICAL_WINDOW_ADDR2   0x53  // Vertical Address End Position
#define ST7781_GATE_SCAN_CTRL1         0x60  // Gate Scan Control 1
#define ST7781_GATE_SCAN_CTRL2         0x61  // Gate Scan Control 2
#define ST7781_PARTIAL_POS_IMG1        0x80  // Partial Image 1 Display Position
#define ST7781_PARTIAL_START_ADDR_IMG1 0x81  // Partial Image 1 Start Address
#define ST7781_PARTIAL_END_ADDR_IMG1   0x82  // Partial Image 1 End Address
#define ST7781_PARTIAL_POS_IMG2        0x83  // Partial Image 2 Display Position
#define ST7781_PARTIAL_START_ADDR_IMG2 0x84  // Partial Image 2 Start Address
#define ST7781_PARTIAL_END_ADDR_IMG2   0x85  // Partial Image 2 End Address
#define ST7781_PANEL_IFACE_CTRL1       0x90  // Panel Interface Control 1
#define ST7781_PANEL_IFACE_CTRL2       0x92  // Panel Interface Control 2
#define ST7781_EEPROM_ID_CODE          0xD2  // EEPROM ID Code
#define ST7781_EEPROM_CTRL_STATUS      0xD9  // EEPROM Control Status
#define ST7781_EEPROM_WRITE_COMMAND    0xDF  // EEPROM Wite Command
#define ST7781_EEPROM_ENABLE           0xFA  // EEPROM Enable
#define ST7781_EEPROM_VCOM_OFFSET      0xFE  // EEPROM VCOM Offset
#define ST7781_FA_FE_ENABLE            0xFF  // FAh/FEh Enable


// Delay between some initialisation commands
//#define TFT_INIT_DELAY 0x00 // Not used unless commandlist invoked
#define TFT_INIT_DELAY 0x00
