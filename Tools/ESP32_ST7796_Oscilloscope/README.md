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

Wire each potentiometer's other legs to 3.3V and GND. The display shows Vpeak-peak, Voffset, the selected sampling rate, and the square-wave frequency in four right-side boxes. Voltage, time, frequency, and sampling-rate values use SI prefixes. The dashboard has no fixed refresh interval: it checks for changed readings each main-loop pass and redraws only values that changed. Trace redraws are driven by completed captures, so they can update faster when capture cadence increases. Each detector ADC reading is averaged from five consecutive samples on every main-loop pass; the held extrema are updated from these averaged values. Their extrema are held for one estimated input period and the hold refreshes if a reading is within 5% of the held extreme. V/div is based on Vpeak-peak divided across four vertical divisions. T/div uses the configured `plotSampleCount` across eight horizontal divisions. Frame rate counts dashboard and trace refreshes per second; changes to the rate readout itself are excluded. GPIO27 frequency is counted by the ESP32 pulse counter. Each counting window corresponds to the time needed for `plotSampleCount` samples at the selected sample rate; the frequency estimate uses five consecutive valid windows. It displays `?` if a window has no pulse and remains unknown while fewer than five valid windows have accumulated. Vpeak-peak and Voffset also display `?` while frequency is unknown because their detector timing depends on the signal period. A trace is plotted only when frequency is known.

### AD9235 parallel capture

The firmware also configures I2S0 camera-mode DMA to capture an 8-bit sample burst, with the acquisition-management task pinned to the other ESP32 core. The ADC's upper eight bits (D11 through D4) are inverted (`255 - value`) and stored in `adcSamples`; the buffer holds twice `plotSampleCount` samples (684 with the current plot width). For plotting, it searches the start positions that leave a full plot's worth of samples, selects the first sample within 5% of the measured peak-to-valley code span from the offset-derived target, then draws the following `plotSampleCount` points. If no point is within that tolerance, it uses the closest candidate. The offset-derived 8-bit target is `255 - ((peak + valley) / 2 * 255 / 4095)`. The additional buffer space makes it more likely that the trigger match still leaves enough samples to fill the plot plane.

| AD9235 signal | ESP32-WROOM-32 |
| --- | --- |
| D4 | GPIO13 |
| D5 | GPIO14 |
| D6 | GPIO16 |
| D7 | GPIO17 |
| D8 | GPIO21 |
| D9 | GPIO22 |
| D10 | GPIO26 |
| D11 | GPIO36 |
| Sampling clock output | GPIO15 |
| Sampling clock input (jumper from GPIO15) | GPIO39 |

GPIO15 generates the sampling clock at the selected rate, up to 10 MHz. GPIO39 feeds that same clock to I2S0's external sampling-clock input. The firmware samples on the opposite clock edge; confirm correct edge alignment and bit ordering with a known test pattern and a logic analyzer before relying on the capture. Configure the AD9235 clock pins for the clock format and voltage levels specified by its datasheet. Verify the ADC output levels are safe for ESP32 GPIO inputs, and use appropriate level translation/buffering if required. GPIO15 is a boot-strapping pin; the clock is enabled by firmware only after startup.

The capture task holds the completed buffer stable until plotting finishes; it then clears the ready flag to request the next burst. Each completed burst increments `adcCaptureSequence`.

## Input limits

This is a learning/demo instrument, not a calibrated or protected instrument. All ESP32 ADC/GPIO inputs, including the AD9235 data outputs, must stay within the ESP32's permitted input-voltage range and must never receive a negative voltage. The GPIO27 frequency input must be a 3.3V-compatible square wave. Use suitable dividers, level translation, and input protection where required, and verify signal levels before connecting them. Do not connect this circuit directly to mains or other hazardous voltages.

## Build and upload

Open this directory as a PlatformIO project, select the `esp32dev` environment, then build and upload. Detector readings are sampled continuously. Each potentiometer is sampled five times and averaged approximately 30 times per second. Dashboard values refresh as soon as their displayed values change, with no fixed rate cap; traces redraw when a capture completes. Frequency is calculated from rising-edge counts over five consecutive windows based on `plotSampleCount / Sampling_rate`; zero or insufficient pulses make the frequency unknown. Counts reaching the pulse-counter limit are reported as a lower-bound frequency. Detector measurements use the project's ADC lookup and calibration formulas; confirm those against the actual analog front end before relying on their absolute values.