        ////////////////////////////////////////////////////
        //  TFT_eSPI driver functions for K210 processor  //
        //  based on https://github.com/fukuen/TFT_eSPI   //
        ////////////////////////////////////////////////////

#ifndef _TFT_eSPI_K210H_
#define _TFT_eSPI_K210H_

// Processor ID reported by getSetup()
#define PROCESSOR_ID 0x0210

// Include processor specific header
#include <stdint.h>
#include "gpio.h"
#include "fpioa.h"
#include "kendryte-standalone-sdk/lib/drivers/include/spi.h"
#include "kendryte-standalone-sdk/lib/drivers/include/sysctl.h"
#include "dmac.h"
#include "sleep.h"
#include <User_Setups/Setup210_Maixduino.h>

#if defined(MAIX_AMIGO) || defined(MAIX_CUBE)
  #include <Wire.h>
  #include <AXP173.h>
#elif defined(M5STICK_V)
  #include <Wire.h>
  #include <AXP192.h>
#endif

#define RST_PIN 6
#define DCX_PIN 7
#define SPI_NUM SPI_DEVICE_0
#define SS_PIN SPI_CHIP_SELECT_3
#define DMA_CH DMAC_CHANNEL3

static int8_t g_gpio_rst;
static uint8_t g_gpio_dcx;
static spi_chip_select_t g_ss;
static dmac_channel_number_t g_dma_ch;
static spi_device_num_t g_spi_num;

void tft_io_init(void)
{
#if defined(MAIX_AMIGO) || defined(MAIX_CUBE)
  AXP173 Axp = AXP173();
  Wire.begin((uint8_t) SDA, (uint8_t) SCL, 400000);
  Axp.begin(true); //Wire is already enabled
#elif defined(M5STICK_V)
  sysctl_set_power_mode(SYSCTL_POWER_BANK3,SYSCTL_POWER_V33);
  AXP192 Axp = AXP192();
  Wire.begin((uint8_t) SDA, (uint8_t) SCL, 400000);
  Axp.begin(true); //Wire is already enabled
#endif
  g_spi_num = SPI_NUM;
  g_dma_ch = DMA_CH;
  g_ss = SS_PIN;
  /* Init SPI IO map and function settings */
  fpioa_set_function(TFT_CS, (fpioa_function_t)(FUNC_SPI0_SS0 + SS_PIN));
#if defined (M5STICK_V)
  fpioa_set_function(TFT_SCLK, FUNC_SPI0_SCLK);
#else
  fpioa_set_function(TFT_WR,   FUNC_SPI0_SCLK);
#endif
  sysctl_set_spi0_dvp_data(1);
  
  //tft_hard_init(uint8_t spi, uint8_t ss, uint8_t rst, uint8_t dcx, uint32_t freq, int8_t rst_pin, int8_t dcx_pin, uint8_t dma_ch){
  fpioa_set_function(TFT_DC, (fpioa_function_t)(FUNC_GPIO0 + DCX_PIN));
  g_gpio_dcx = DCX_PIN;
  gpio_set_drive_mode(g_gpio_dcx, GPIO_DM_OUTPUT);
  gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH);

  fpioa_set_function(TFT_RST, (fpioa_function_t)(FUNC_GPIO0 + RST_PIN));
  g_gpio_rst = RST_PIN;
  gpio_set_drive_mode(g_gpio_rst, GPIO_DM_OUTPUT);
  gpio_set_pin(g_gpio_rst, GPIO_PV_HIGH);

  gpio_set_pin(g_gpio_rst, GPIO_PV_LOW);
#if defined (M5STICK_V)
  spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
#else
  spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
#endif
  spi_set_clk_rate(g_spi_num, SPI_FREQUENCY);
  gpio_set_pin(g_gpio_rst, GPIO_PV_HIGH);
}

void tft_write_command(uint8_t cmd)
{
    gpio_set_pin(g_gpio_dcx, GPIO_PV_LOW);
#if defined (M5STICK_V)
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
#else
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(g_spi_num, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
#endif
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, (uint8_t *)(&cmd), 1, SPI_TRANS_CHAR);
}

void tft_write_byte(uint8_t *data_buf, uint32_t length)
{
    gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH);
#if defined (M5STICK_V)
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
#else
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(g_spi_num, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
#endif
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, data_buf, length, SPI_TRANS_CHAR);
}

void tft_write_half(uint16_t *data_buf, uint32_t length)
{
    gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH);
#if defined (M5STICK_V)
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 16, 0);
#else
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 16, 0);
    spi_init_non_standard(g_spi_num, 16, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
#endif
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, data_buf, length, SPI_TRANS_SHORT);
}

void tft_write_word(uint32_t *data_buf, uint32_t length)
{
    gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH);
#if defined (M5STICK_V)
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 32, 0);
#else
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);
    spi_init_non_standard(g_spi_num, 0, 32, 0, SPI_AITM_AS_FRAME_FORMAT);
#endif
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, data_buf, length, SPI_TRANS_INT);
}

void tft_fill_data(uint32_t *data_buf, uint32_t length)
{
    gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH);
#if defined (M5STICK_V)
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 32, 0);
#else
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 32, 0);
    spi_init_non_standard(g_spi_num, 0, 32, 0, SPI_AITM_AS_FRAME_FORMAT);
#endif
    spi_fill_data_dma(g_dma_ch, g_spi_num, g_ss, data_buf, length);
}

void tft_write_cs()
{
#if defined (M5STICK_V)
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
#else
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(g_spi_num, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
#endif
}

void tft_write_a_byte(uint8_t data)
{
//    dmac_wait_done(g_dma_ch);
//    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
//    spi_init_non_standard(g_spi_num, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, (uint8_t *)(&data), 1, SPI_TRANS_CHAR);
}

void tft_write_a_half(uint16_t data)
{
//    dmac_wait_done(g_dma_ch);
#if defined (M5STICK_V)
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 16, 0);
#else
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 16, 0);
    spi_init_non_standard(g_spi_num, 16, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
#endif
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, (uint16_t *)(&data), 1, SPI_TRANS_SHORT);
}

void tft_write_a_word(uint32_t data)
{
    uint8_t tmp[4] = { static_cast<uint8_t>((data >> 24) & 0xff), static_cast<uint8_t>((data >> 16) & 0xff), static_cast<uint8_t>((data >> 8) & 0xff), static_cast<uint8_t>(data & 0xff) };
    dmac_wait_done(g_dma_ch);
#if defined (M5STICK_V)
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
#else
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(g_spi_num, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
#endif
    spi_send_data_normal_dma(g_dma_ch, g_spi_num, g_ss, (uint8_t *)tmp, 4, SPI_TRANS_CHAR);
}

uint8_t tft_read_a_byte(void)
{
    uint8_t tmp;
#if defined (M5STICK_V)
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
#else
    spi_init(g_spi_num, SPI_WORK_MODE_0, SPI_FF_OCTAL, 8, 0);
    spi_init_non_standard(g_spi_num, 8, 0, 0, SPI_AITM_AS_FRAME_FORMAT);
#endif
    spi_receive_data_standard_dma(g_dma_ch, g_dma_ch, g_spi_num, g_ss, NULL, 0, (uint8_t *)(&tmp), 1);
    return tmp;
}


// Processor specific code used by SPI bus transaction startWrite and endWrite functions
#define SET_BUS_WRITE_MODE // Not used
#define SET_BUS_READ_MODE  // Not used

// Code to check if DMA is busy, used by SPI bus transaction startWrite and endWrite functions
#define DMA_BUSY_CHECK // Not used so leave blank

// SUPPORT_TRANSACTIONS is mandatory for K210
#if !defined (SUPPORT_TRANSACTIONS)
  #define SUPPORT_TRANSACTIONS
#endif

// Initialise processor specific SPI functions, used by init()
#define INIT_TFT_DATA_BUS // Not used

// If smooth font is used then it is likely SPIFFS will be needed
#ifdef SMOOTH_FONT
  // Call up the SPIFFS (SPI FLASH Filing System) for the anti-aliased fonts
  // #define FS_NO_GLOBALS
  // #include <FS.h>
  // #include "Maixduino_SPIFFS.h"
  // #define FONT_FS_AVAILABLE
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the DC (TFT Data/Command or Register Select (RS))pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if (TFT_DC >= 0)
  #if defined (RPI_DISPLAY_TYPE)
    #if defined (ILI9486_DRIVER)
      // RPi ILI9486 display needs a slower DC change
      #define DC_C gpio_set_pin(g_gpio_dcx, GPIO_PV_LOW); \
                    gpio_set_pin(g_gpio_dcx, GPIO_PV_LOW)
      #define DC_D gpio_set_pin(g_gpio_dcx, GPIO_PV_LOW); \
                    gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH)
    #else
      // Other RPi displays need a slower C->D change
      #define DC_C gpio_set_pin(g_gpio_dcx, GPIO_PV_LOW)
      #define DC_D gpio_set_pin(g_gpio_dcx, GPIO_PV_LOW); \
                    gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH)
    #endif
  #else
    #define DC_C gpio_set_pin(g_gpio_dcx, GPIO_PV_LOW)
    #define DC_D gpio_set_pin(g_gpio_dcx, GPIO_PV_HIGH)
  #endif
#else
  #define DC_C  // No macro allocated so it generates no code
  #define DC_D  // No macro allocated so it generates no code
#endif


////////////////////////////////////////////////////////////////////////////////////////
// Define the CS (TFT chip select) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if (TFT_CS >= 0)
  #ifdef RPI_DISPLAY_TYPE  // RPi display needs a slower CS change
    #define CS_L digitalWrite(TFT_CS, HIGH); tft_write_cs()
    #define CS_H tft_write_cs(); digitalWrite(TFT_CS, HIGH)
  #else
    #define CS_L tft_write_cs()
    #define CS_H digitalWrite(TFT_CS, HIGH)
  #endif
#else
  #define CS_L  // No macro allocated so it generates no code
  #define CS_H  // No macro allocated so it generates no code
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the WR (TFT Write) pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if !defined TFT_WR || (TFT_WR < 0)
  #define WR_L // No macro allocated so it generates no code
  #define WR_H // No macro allocated so it generates no code
#else
  #define WR_L digitalWrite(TFT_WR, LOW)
  #define WR_H digitalWrite(TFT_WR, HIGH)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Define the touch screen chip select pin drive code
////////////////////////////////////////////////////////////////////////////////////////
#if !defined TOUCH_CS || (TOUCH_CS < 0)
  #define T_CS_L // No macro allocated so it generates no code
  #define T_CS_H // No macro allocated so it generates no code
#else
  #define T_CS_L digitalWrite(TOUCH_CS, LOW)
  #define T_CS_H digitalWrite(TOUCH_CS, HIGH)
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Make sure SPI default pins are assigned if not specified by user or set to -1
////////////////////////////////////////////////////////////////////////////////////////
#ifndef TFT_MISO
  #define TFT_MISO -1
#endif
#ifndef TFT_MOSI
  #define TFT_MOSI -1
#endif
#ifndef TFT_SCLK
  #define TFT_SCLK -1
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Macros to write commands/pixel colour data to an ILI9488 TFT
////////////////////////////////////////////////////////////////////////////////////////
#if  defined (ILI9488_DRIVER) // 16 bit colour converted to 3 bytes for 18 bit RGB

  // Write 8 bits to TFT
  #define tft_Write_8(C)   spi_.transfer(C)

  // Convert 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16(C)  spi_.transfer(((C) & 0xF800)>>8); \
                           spi_.transfer(((C) & 0x07E0)>>3); \
                           spi_.transfer(((C) & 0x001F)<<3)

  // Convert swapped byte 16 bit colour to 18 bit and write in 3 bytes
  #define tft_Write_16S(C) spi_.transfer((C) & 0xF8); \
                           spi_.transfer(((C) & 0xE000)>>11 | ((C) & 0x07)<<5); \
                           spi_.transfer(((C) & 0x1F00)>>5)
  // Write 32 bits to TFT
  #define tft_Write_32(C)  spi_.transfer16((C)>>16); spi_.transfer16((uint16_t)(C))

  // Write two address coordinates
  #define tft_Write_32C(C,D) spi_.transfer16(C); spi_.transfer16(D)

  // Write same value twice
  #define tft_Write_32D(C) spi_.transfer16(C); spi_.transfer16(C)

#else
////////////////////////////////////////////////////////////////////////////////////////
// Macros to write commands/pixel colour data to an Raspberry Pi TFT
////////////////////////////////////////////////////////////////////////////////////////
  #if  defined (RPI_DISPLAY_TYPE)
    // RPi TFT type always needs 16 bit transfers
    #define tft_Write_8(C)     spi_.transfer(0); spi_.transfer(C)

////////////////////////////////////////////////////////////////////////////////////////
// Macros for all other SPI displays
////////////////////////////////////////////////////////////////////////////////////////
  #else
    // K210 low level SPI writes for 8, 16 and 32 bit values
    // Write 8 bits
    #define tft_Write_8(C) tft_write_a_byte(C)

    // Write 16 bits with corrected endianess for 16 bit colours
    #define tft_Write_16(C) tft_write_a_half(C)

    // Write 16 bits
    #define tft_Write_16S(C) tft_write_a_half(((C)>>8 & 0xFF) | ((C)<<8))

    // Write 32 bits
    #define tft_Write_32(C) tft_write_a_word(C)

    // Write two address coordinates
    #define tft_Write_32C(C,D)  tft_write_a_word((C)<<16 | (D))

    // Write same value twice
    #define tft_Write_32D(C) tft_write_a_word((C)<<16 | (C))
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////////////
// Macros to read from display using SPI or software SPI
////////////////////////////////////////////////////////////////////////////////////////
#if defined (TFT_SDA_READ)
  // Macros to support a bit banged function call for K210 and bi-directional SDA pin
  #define TFT_eSPI_ENABLE_8_BIT_READ // Enable tft_Read_8();
  #define SCLK_L digitalWrite(TFT_SCLK, LOW)
  #define SCLK_H digitalWrite(TFT_SCLK, LOW)
#else
  // Use a SPI read transfer
  #define tft_Read_8() tft_read_a_byte()
#endif

#endif // Header end
