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
| Peak detector output | GPIO35 (ADC1) |
| Inverted peak detector output | GPIO36 (ADC1) |
| Gain potentiometer wiper | GPIO34 (ADC1) |
| Frequency-scale potentiometer wiper | GPIO33 (ADC1) |
| Square-wave frequency input | GPIO27 |
| AD9235 out-of-range output (active high) | GPIO12 |

Wire each potentiometer's other legs to 3.3V and GND. The GPIO12 indicator reads the AD9235 out-of-range output as active-high and shows `OUT OF RANGE` in red or `IN RANGE` in green above the plot. GPIO12 is an ESP32 boot-strapping pin: ensure the ADC output does not drive it high while the ESP32 is resetting/booting (use suitable isolation or buffering if necessary), or the board may fail to boot. Confirm the out-of-range output polarity and voltage against the ADC board documentation before connecting it. The display shows Vpeak-peak, Voffset, the selected sampling rate, and the square-wave frequency in four right-side boxes. Voltage, time, frequency, and sampling-rate values use SI prefixes. Dashboard measurements and indicators are capped at 10 fps, while the plot plane is independently capped at 60 fps; changed values are redrawn only when needed, and completed traces wait for the next available plot update. Each detector ADC reading is averaged from ten consecutive samples on every main-loop pass. Each potentiometer is sampled ten times and averaged approximately 30 times per second; the calculated gain is additionally smoothed with a 0.1 exponential-filter factor. Peak/valley hold timing is based on an assumed signal frequency of `Sampling_rate / 100` (one hold period per corresponding signal cycle); the hold refreshes if a reading is within 5% of the held extreme. Vpeak-peak and Voffset always show the latest detector-based calculations, including when the frequency measurement is unknown. V/div is based on Vpeak-peak divided across four vertical divisions. T/div uses the configured `plotSampleCount` across eight horizontal divisions. Frame rate counts dashboard and trace refreshes per second; changes to the rate readout itself are excluded. GPIO27 frequency is counted by the ESP32 pulse counter with a 1 µs glitch filter to reject shorter spurious transitions; input high and low phases must each exceed 1 µs. Each counting window is `100 * 100 / Sampling_rate` seconds, capped at 10 seconds; the frequency estimate uses five consecutive valid windows. It displays `?` if a window has no pulse and remains unknown while fewer than five valid windows have accumulated. A trace is plotted only when frequency is known.

### AD9235 parallel capture

The firmware also configures I2S0 camera-mode DMA to capture an 8-bit sample burst, with the acquisition-management task pinned to the other ESP32 core. Each burst captures five plot-plane widths (`5 * plotSampleCount`, 1710 samples with the current plot width) before it is displayed. The ADC's upper eight bits (D11 through D4) are inverted (`255 - value`) and stored in `adcSamples`. A trace is drawn from the captured samples as soon as the burst completes; it does not wait for the separate GPIO27 frequency measurement. For plotting, it searches the start positions that leave a full plot's worth of samples, selects the first sample within 5% of the measured peak-to-valley code span from the offset-derived target, then draws the following `plotSampleCount` points. If no point is within that tolerance, it uses the closest candidate. The offset-derived 8-bit target is `255 - ((peak + valley) / 2 * 255 / 4095)`.

| AD9235 signal | ESP32-WROOM-32 |
| --- | --- |
| D4 | GPIO13 |
| D5 | GPIO14 |
| D6 | GPIO16 |
| D7 | GPIO17 |
| D8 | GPIO21 |
| D9 | GPIO22 |
| D10 | GPIO26 |
| D11 | GPIO15 |
| Sampling clock output | GPIO32 |
| Sampling clock input (jumper from GPIO32) | GPIO39 |

GPIO32 generates the sampling clock at the selected rate, up to 10 MHz, using the ESP32's strongest GPIO drive setting. GPIO39 feeds that same clock to I2S0's external sampling-clock input; a short jumper between GPIO32 and GPIO39 keeps the loopback close to the ADC clock connection. The drive setting cannot compensate for a clock input that presents too much load or requires a differential/common-mode clock interface. If the clock amplitude drops when connected, use a suitable clock buffer/driver between GPIO32 and the ADC, and feed GPIO39 from the buffered logic-level clock (or confirm the source split is within the driver's loading limits). The firmware samples on the opposite clock edge; confirm correct edge alignment and bit ordering with a known test pattern and a logic analyzer before relying on the capture. Configure the AD9235 clock pins for the clock format and voltage levels specified by its datasheet. Verify the ADC output levels are safe for ESP32 GPIO inputs, and use appropriate level translation/buffering if required.

This pin remap moves the gain potentiometer from GPIO32 to GPIO34, the peak detector from GPIO34 to GPIO35, the inverted peak detector from GPIO35 to GPIO36, and AD9235 D11 from GPIO36 to GPIO15. GPIO15 is an ESP32 boot-strapping pin. Ensure the ADC's D11 output is low or isolated while the ESP32 is resetting/booting; otherwise, the board may not start reliably. GPIO36 is input-only and remains assigned to the inverted detector, which is an input. The sampling-rate control is averaged, low-pass filtered, and uses hysteresis when selecting a discrete rate to reduce rate changes caused by small ADC fluctuations.

Core 1 snapshots the selected sample rate in `adcCaptureSamplingRate` and then sets the atomic `adcCaptureRequested` flag. Core 0 uses that snapshot to configure the clock before the burst and does not retune or abort the clock if the pot changes during capture; a new rate is applied only on the next capture request. Core 0 performs one DMA capture, copies the samples, increments `adcCaptureSequence` on success, and clears the request flag when finished (also clearing it after a failed or interrupted capture). Core 1 observes the cleared flag to detect completion, plots a successful buffer, and only then requests another capture; this keeps the shared sample buffer stable while it is being drawn. If the capture task cannot be created, setup continues without issuing capture requests and a red `CAPTURE TASK FAILED` indicator appears above the plot.

## Input limits

This is a learning/demo instrument, not a calibrated or protected instrument. All ESP32 ADC/GPIO inputs, including the AD9235 data outputs, must stay within the ESP32's permitted input-voltage range and must never receive a negative voltage. The GPIO27 frequency input must be a 3.3V-compatible square wave with both high and low phases longer than the configured 1 µs glitch filter. Use suitable dividers, level translation, and input protection where required, and verify signal levels before connecting them. Do not connect this circuit directly to mains or other hazardous voltages.

## Build and upload

Open this directory as a PlatformIO project, select the `esp32dev` environment, then build and upload. Detector readings are sampled continuously; each detector value averages ten analog readings per main-loop update. Each potentiometer is sampled ten times and averaged approximately 30 times per second. Dashboard measurements are capped at 10 fps and the plot plane at 60 fps. Each plot update follows an ADC burst of five plot-plane widths, acquired at the selected sample rate and displayed without waiting for the independent frequency measurement. The sample-rate pot selection is filtered and uses hysteresis to reduce oscillation between adjacent rates. Frequency is calculated from rising-edge counts over five consecutive windows; each window lasts `100 * 100 / Sampling_rate` seconds, capped at 10 seconds. Zero or insufficient pulses make the frequency unknown. Counts reaching the pulse-counter limit are reported as a lower-bound frequency. Detector measurements use the project's ADC lookup and calibration formulas; confirm those against the actual analog front end before relying on their absolute values.