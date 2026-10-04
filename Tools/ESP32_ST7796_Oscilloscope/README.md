# ESP32 ST7796 Oscilloscope

Measurement input starter project for an ESP32-WROOM-32 development board and a generic SPI ST7796 display. It builds against the TFT_eSPI library in this checkout and keeps its display configuration local to this project.

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

Connect all measurement grounds to ESP32 GND. Wire the inputs as follows:

| Measurement | ESP32-WROOM-32 |
| --- | --- |
| Peak detector output | GPIO34 (ADC1) |
| Inverted peak detector output | GPIO35 (ADC1) |
| Gain potentiometer wiper | GPIO32 (ADC1) |
| Frequency-scale potentiometer wiper | GPIO33 (ADC1) |
| Square-wave frequency input | GPIO27 |

Wire each potentiometer's other legs to 3.3V and GND. The display shows Vpeak-peak, Voffset, the selected sampling rate, and the square-wave frequency in four right-side boxes. Voltage, time, frequency, and sampling-rate values use SI prefixes. The graticule is currently a layout guide; trace acquisition and plotting are not active. The two detector ADCs are sampled continuously as quickly as the main loop permits. Their extrema are held for one estimated input period and the hold refreshes if a reading is within 5% of the held extreme. V/div is based on Vpeak-peak divided across four vertical divisions. T/div uses the configured `plotSampleCount` across eight horizontal divisions. Frame rate counts dashboard refreshes that change at least one measurement or scale value per second; changes to the rate readout itself are excluded. GPIO27 frequency is counted by the ESP32 pulse counter. Each counting window corresponds to the time needed for `plotSampleCount` samples at the selected sample rate; the frequency estimate uses five consecutive valid windows. It displays `?` if a window has no pulse and remains unknown while fewer than five valid windows have accumulated. One pulse per window is accepted only when the counter can observe the windows individually.

## Input limits

This is a learning/demo instrument, not a calibrated or protected instrument. All ADC inputs and the GPIO27 square-wave input must stay between 0V and 3.3V and must never receive a negative voltage. The frequency input must be a 3.3V-compatible square wave. Use suitable dividers and input protection for signals outside these ranges, and verify the voltages with a multimeter before connecting them. Do not connect this circuit directly to mains or other hazardous voltages.

## Build and upload

Open this directory as a PlatformIO project, select the `esp32dev` environment, then build and upload. Detector readings are sampled continuously; the potentiometers and display update every 100 ms. Frequency is calculated from rising-edge counts over five consecutive windows based on `plotSampleCount / Sampling_rate`; zero or insufficient pulses make the frequency unknown. Counts reaching the pulse-counter limit are reported as a lower-bound frequency. Detector measurements use the project's ADC lookup and calibration formulas; confirm those against the actual analog front end before relying on their absolute values.