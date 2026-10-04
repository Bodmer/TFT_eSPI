# ESP32 ST7796 Oscilloscope

Starter two-channel oscilloscope project for an ESP32-WROOM-32 development board and a generic SPI ST7796 display. It builds against the TFT_eSPI library in this checkout and keeps its display configuration local to this project.

## Wiring

Connect the display to 3.3V logic and a common ground. The starter pin map is in `include/TFT_User_Setup.h`:

| Display | ESP32-WROOM-32 |
| --- | --- |
| SCK / CLK | GPIO18 |
| SDI / MOSI | GPIO23 |
| SDO / MISO | GPIO19, if the display exposes it |
| CS | GPIO5 |
| DC / RS | GPIO2 |
| RESET | GPIO4 |
| LED / BL | 3.3V, subject to the display module's backlight requirements |
| VCC / GND | Per the module's power requirements / GND |

Connect the first peak detector output to GPIO34 (CH1) and the second to GPIO35 (CH2). Connect both detector grounds to ESP32 GND. Both are input-only ADC1 pins. The displayed voltage conversion is nominal and currently applies no detector-specific scaling or inversion.

## Input limits

This is a learning/demo instrument, not a calibrated or protected oscilloscope. GPIO34 and GPIO35 must stay between 0V and 3.3V and must never receive a negative voltage. Use suitable dividers and input protection for signals outside that range, and verify the voltages with a multimeter before connecting them. Do not connect this circuit directly to mains or other hazardous voltages.

## Build and upload

Open this directory as a PlatformIO project, select the `esp32dev` environment, then build and upload. The default capture is 480 sample pairs at 2 kS/s (about 240 ms per sweep). The display shows both raw ADC traces in separate colors and each channel's approximate min/average/max voltage; the voltage conversion is nominal and depends on ESP32 ADC calibration and input circuitry.