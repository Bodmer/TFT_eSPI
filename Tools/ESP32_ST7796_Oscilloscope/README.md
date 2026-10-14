# ESP32 ST7796 Oscilloscope

Measurement input starter project for an ESP32-WROOM-32 development board and a generic SPI ST7796 display. It builds against the TFT_eSPI library in this checkout and keeps its display configuration local to this project.

## Wiring

Connect the display to 3.3V logic and a common ground. The starter pin map is in `include/TFT_User_Setup.h`:

| Display | ESP32-WROOM-32 |
| --- | --- |
| SCK / CLK | GPIO18 |
| SDI / MOSI | GPIO23 |
| SDO / MISO | Not connected (GPIO19 is used for ADC VSYNC) |
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
| AD9235 out-of-range output | Not connected; indicator is temporarily fixed to `IN RANGE` |

The out-of-range indicator is temporarily hard-coded to `false` and therefore always displays `IN RANGE`; GPIO12 is used for the ADC capture trigger instead. The display shows Vpeak-peak, Voffset, the selected effective sampling rate, and the square-wave frequency in four right-side boxes. Voltage, time, frequency, and sampling-rate values use SI prefixes. Dashboard measurements and indicators are capped at 10 fps, while the plot plane is independently capped at 60 fps; changed values are redrawn only when needed, and completed traces wait for the next available plot update. Each detector ADC reading is averaged from ten consecutive samples on every main-loop pass. Each potentiometer is sampled ten times and averaged approximately 30 times per second; the calculated gain is additionally smoothed with a 0.1 exponential-filter factor. Peak/valley hold timing is based on an assumed signal frequency of `Sampling_rate / 100` (one hold period per corresponding signal cycle); the hold refreshes if a reading is within 5% of the held extreme. Vpeak-peak and Voffset always show the latest detector-based calculations, including when the frequency estimate is unknown. V/div is based on Vpeak-peak divided across four vertical divisions. T/div uses the configured `plotSampleCount` across eight horizontal divisions. Frame rate counts dashboard and trace refreshes per second; changes to the rate readout itself are excluded. GPIO27 frequency is counted by the ESP32 pulse counter with a 1 µs glitch filter to reject shorter spurious transitions; input high and low phases must each exceed 1 µs. Each frequency-measurement interval lasts `100 * 100 / Sampling_rate` seconds, capped at 10 seconds. When the interval elapses, the measurement function enables a one-shot rising-edge interrupt and waits up to 10 seconds for it. In the ISR, it records the edge timestamp, pauses PCNT, snapshots the counter and high-limit status, clears the counter, resumes counting, disables the GPIO interrupt, and notifies the waiting measurement function. That function uses the captured count and time interval to update the five-window frequency estimate; the captured edge timestamp is the start of the next interval. If no edge arrives before the timeout, the estimate is cleared and the main loop resumes. Plotting a captured trace is independent of whether the frequency estimate is known.

### AD9235 parallel capture

The current diagnostic build configures I2S0 camera-mode DMA to stream ADC samples, with the acquisition-management task pinned to core 0. GPIO32 supplies a fixed 10 MHz ADC clock, and the frequency-scale potentiometer selects the effective plotted sampling rate from 100 SPS through 10 MSPS. Core 0 averages groups of consecutive ADC samples to decimate the 10 MSPS stream to the selected rate, and captures five plot-plane widths (`5 * plotSampleCount`, 1710 samples with the current plot width) before displaying them. At 400 SPS, for example, each burst spans about 4.275 seconds; at 100 SPS it spans about 17.1 seconds. In the configured I2S camera FIFO mode, each 32-bit DMA word carries a sample in bits 23:16; firmware extracts that byte, corresponding to the ADC's upper eight bits (D11 through D4), inverts it (`255 - value`), and stores it in `adcSamples`. A trace is drawn from the captured samples as soon as the burst completes; it does not wait for the separate GPIO27 frequency measurement. For plotting, it searches the start positions that leave a full plot's worth of samples, selects the first sample within 5% of the measured peak-to-valley code span from the offset-derived target, then draws the following `plotSampleCount` points. If no point is within that tolerance, it uses the closest candidate. The offset-derived 8-bit target is `255 - ((peak + valley) / 2 * 255 / 4095)`.

| AD9235 signal | ESP32-WROOM-32 |
| --- | --- |
| AD9235 D4 | GPIO13 (camera D0) |
| AD9235 D5 | GPIO14 (camera D1) |
| AD9235 D6 | GPIO16 (camera D2) |
| AD9235 D7 | GPIO17 (camera D3) |
| AD9235 D8 | GPIO21 (camera D4) |
| AD9235 D9 | GPIO22 (camera D5) |
| AD9235 D10 | GPIO26 (camera D6) |
| AD9235 D11 | GPIO15 (camera D7) |
| Sampling clock output / XCLK | GPIO32 |
| Sampling clock input / PCLK | GPIO39 |
| Software trigger output | GPIO19 |
| VSYNC input (jumper from GPIO19) | GPIO12 |

GPIO32 generates a fixed 10 MHz clock continuously from setup onward, using the ESP-IDF default GPIO drive capability. The LEDC clock configuration uses an 80 MHz source with an exact divider/resolution combination for a nominal 50% duty-cycle output; verify the actual waveform on the hardware. Connect the clock to the AD9235 clock input and GPIO39 through a 100 Ω series resistor placed close to GPIO32. Keep the clock connection and its ground return short. Before connecting it to the ADC, scope the signal at the ADC CLK pin with a 10× probe and a short ground spring; confirm ringing/overshoot stays within the ADC's specified input limits. Do not reconnect the clock while the ESP32 resets or if the waveform exceeds those limits. The tested WS route captures the full requested sample buffer; routing GPIO39 to BCK timed out without data, so the firmware now keeps GPIO39 routed to I2S0's camera-mode word-select (WS) input. I2S0 camera-mode DMA receives the byte lanes D4..D11 as D0..D7. Each acquisition starts DMA, then pulses GPIO19 low for 10 microseconds and high again; connect GPIO19 to GPIO12 so the rising edge reaches I2S0 VSYNC. GPIO12 is an ESP32 boot-strapping pin and must remain low while the chip resets/boots. GPIO19 is high-impedance during reset and is driven low by firmware after the boot straps have been sampled; do not connect any external signal that can drive GPIO12 high during startup. Leave the AD9235 out-of-range output disconnected from GPIO12 and disconnect the display's MISO/SDO wire from GPIO19. The interface therefore needs the eight ADC data connections, clock output and feedback, plus this software-trigger jumper.

After each capture, the `VSYNC GPIO12 low/high` diagnostic should read `0/1`, confirming the trigger pulse reaches the input pad. GPIO19 is an output, so its input-pad readback is not used as a trigger diagnostic.

GPIO1 and GPIO3 remain available for UART0 TX/RX and serial monitoring during capture. The GPIO25 DAC output is unchanged. The display's GPIO2 DC connection and other display wiring are unchanged. GPIO0, GPIO22, GPIO34/35/36 and the supplied example's other data pins remain unused because they conflict with existing display or measurement connections. The XCLK is configured for 10 MHz, and the frequency-scale potentiometer selects among the available rates up to that 10 MSPS hardware-clock limit.

Keep the clock/data wiring short and connect all grounds. GPIO39 and all data inputs must stay within the ESP32's permitted voltage range and never go below ground; use suitable high-speed buffering/level translation if required. Verify the actual clock and ADC output voltages at the ESP32 pins before connecting them.

This pin remap moves the gain potentiometer from GPIO32 to GPIO34, the peak detector from GPIO34 to GPIO35, the inverted peak detector from GPIO35 to GPIO36, and AD9235 D11 from GPIO36 to GPIO15. GPIO15 is an ESP32 boot-strapping pin. Ensure the ADC's D11 output is low or isolated while the ESP32 is resetting/booting; otherwise, the board may not start reliably. GPIO36 is input-only and remains assigned to the inverted detector, which is an input. The sampling-rate control is averaged, low-pass filtered, and uses hysteresis when selecting a discrete rate to reduce rate changes caused by small ADC fluctuations.

Core 1 snapshots the selected effective sample rate in `adcCaptureSamplingRate` and then sets the atomic `adcCaptureRequested` flag. Core 0 starts the circular I2S camera-mode DMA buffer, pulses GPIO19/GPIO12 VSYNC, and averages/decimates completed blocks into the shared plot buffer. The GPIO32 clock remains on at 10 MHz while idle, during capture, and after success or failure; the capture task controls DMA but never starts, stops, or retunes the clock. Core 0 then clears the request flag; on success it also increments `adcCaptureSequence`. Core 1 observes the cleared flag, plots a successful buffer, and only then requests another burst; this keeps the shared sample buffer stable while it is being drawn. If the capture task cannot be created, setup continues without issuing capture requests and a red `CAPTURE TASK FAILED` indicator appears above the plot.

`captureDebugEnabled` is the master debug switch and is currently `false`. When enabled, the line below the plot shows `Sc:` the last completed DMA sample count versus expected, `Sf:` capture state, and `Cr:` whether core 1 issued a capture request. `adcCaptureDiagnosticsEnabled` is a separate, currently `false` switch for the verbose I2S/DMA snapshots and Serial reports; those diagnostics run only when both flags are `true`. With the master flag `false`, capture diagnostics do not sample GPIO levels, snapshot I2S registers/descriptors, collect ISR counters, run the extra GPIO39 PCNT monitor, or print capture debug reports. The I2S ISR still reads and clears interrupt status needed to manage DMA and detect errors. UART0 remains available during capture. The task-creation error message remains enabled because it reports an operational failure, not debug output.

## Input limits

This is a learning/demo instrument, not a calibrated or protected instrument. All ESP32 ADC/GPIO inputs, including the AD9235 data outputs, must stay within the ESP32's permitted input-voltage range and must never receive a negative voltage. The GPIO27 frequency input must be a 3.3V-compatible square wave with both high and low phases longer than the configured 1 µs glitch filter. Use suitable dividers, level translation, and input protection where required, and verify signal levels before connecting them. Do not connect this circuit directly to mains or other hazardous voltages.

## Build and upload

Open this directory as a PlatformIO project, select the `esp32dev` environment, then build and upload. Detector readings are sampled continuously; each detector value averages ten analog readings per main-loop update. Each potentiometer is sampled ten times and averaged approximately 30 times per second. Dashboard measurements are capped at 10 fps and the plot plane at 60 fps. Each plot update follows an ADC burst of five plot-plane widths, clocked at 10 MHz and decimated to the effective sample rate selected by the frequency-scale potentiometer (100 SPS to 10 MSPS); display updates do not wait for the independent frequency measurement. The sample-rate pot selection is filtered and uses hysteresis to reduce oscillation between adjacent rates. Frequency is calculated from rising-edge counts over five consecutive windows; each window lasts `100 * 100 / Sampling_rate` seconds, capped at 10 seconds. Zero or insufficient pulses make the frequency unknown. Counts reaching the pulse-counter limit are reported as a lower-bound frequency. Detector measurements use the project's ADC lookup and calibration formulas; confirm those against the actual analog front end before relying on their absolute values.