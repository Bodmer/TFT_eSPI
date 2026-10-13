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

Wire each potentiometer's other legs to 3.3V and GND. The GPIO12 indicator reads the AD9235 out-of-range output as active-high and shows `OUT OF RANGE` in red or `IN RANGE` in green above the plot. GPIO12 is an ESP32 boot-strapping pin: ensure the ADC output does not drive it high while the ESP32 is resetting/booting (use suitable isolation or buffering if necessary), or the board may fail to boot. Confirm the out-of-range output polarity and voltage against the ADC board documentation before connecting it. The display shows Vpeak-peak, Voffset, the selected effective sampling rate, and the square-wave frequency in four right-side boxes. Voltage, time, frequency, and sampling-rate values use SI prefixes. Dashboard measurements and indicators are capped at 10 fps, while the plot plane is independently capped at 60 fps; changed values are redrawn only when needed, and completed traces wait for the next available plot update. Each detector ADC reading is averaged from ten consecutive samples on every main-loop pass. Each potentiometer is sampled ten times and averaged approximately 30 times per second; the calculated gain is additionally smoothed with a 0.1 exponential-filter factor. Peak/valley hold timing is based on an assumed signal frequency of `Sampling_rate / 100` (one hold period per corresponding signal cycle); the hold refreshes if a reading is within 5% of the held extreme. Vpeak-peak and Voffset always show the latest detector-based calculations, including when the frequency measurement is unknown. V/div is based on Vpeak-peak divided across four vertical divisions. T/div uses the configured `plotSampleCount` across eight horizontal divisions. Frame rate counts dashboard and trace refreshes per second; changes to the rate readout itself are excluded. GPIO27 frequency is counted by the ESP32 pulse counter with a 1 µs glitch filter to reject shorter spurious transitions; input high and low phases must each exceed 1 µs. Each frequency-measurement interval lasts `100 * 100 / Sampling_rate` seconds, capped at 10 seconds. When the interval elapses, the measurement function enables a one-shot rising-edge interrupt and waits up to 10 seconds for it. In the ISR, it records the edge timestamp, pauses PCNT, snapshots the counter and high-limit status, clears the counter, resumes counting, disables the GPIO interrupt, and notifies the waiting measurement function. That function uses the captured count and time interval to update the five-window frequency estimate; the captured edge timestamp is the start of the next interval. If no edge arrives before the timeout, the estimate is cleared and the main loop resumes. Plotting a captured trace is independent of whether the frequency estimate is known.

### AD9235 parallel capture

The current diagnostic build configures I2S0 camera-mode DMA to stream ADC samples, with the acquisition-management task pinned to core 0. The ADC clock is temporarily reduced to 1 MSPS to isolate DMA operation at a lower input clock; the sampling-rate control is capped at 1 MSPS and selects the effective plotted rate. Core 0 averages groups of consecutive ADC samples to decimate the 1 MSPS stream to that rate, and captures five plot-plane widths (`5 * plotSampleCount`, 1710 samples with the current plot width) before displaying them. At 400 SPS, for example, each burst spans about 4.275 seconds; at 100 SPS it spans about 17.1 seconds. In the configured I2S camera FIFO mode, each 32-bit DMA word carries a sample in bits 23:16; firmware extracts that byte, corresponding to the ADC's upper eight bits (D11 through D4), inverts it (`255 - value`), and stores it in `adcSamples`. A trace is drawn from the captured samples as soon as the burst completes; it does not wait for the separate GPIO27 frequency measurement. For plotting, it searches the start positions that leave a full plot's worth of samples, selects the first sample within 5% of the measured peak-to-valley code span from the offset-derived target, then draws the following `plotSampleCount` points. If no point is within that tolerance, it uses the closest candidate. The offset-derived 8-bit target is `255 - ((peak + valley) / 2 * 255 / 4095)`.

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

GPIO32 generates the fixed 1 MHz diagnostic sampling clock continuously from setup onward. Connect this clock to the AD9235 clock input and to GPIO39. For the routing A/B test, each acquisition alternates GPIO39 between I2S0's camera-mode word-select (WS) input and bit-clock (BCK) input; the report labels each capture with its selected route. The ESP32 camera-mode peripheral normally uses WS as PCLK, while BCK is being tested as an alternative. I2S0 camera-mode DMA receives the byte lanes D4..D11 as D0..D7. DMA starts internally for each requested acquisition and streams until firmware has decimated enough samples for the selected effective sample rate. This uses ten ADC signal connections total (eight data lines, clock output, and clock feedback), with no GPIO1/GPIO3 trigger wiring. GPIO0, GPIO22, GPIO34/35/36 and the supplied example's other data pins are not used because they conflict with the existing display or measurement connections. The display's GPIO2 DC connection and UART0 on GPIO1/GPIO3 are unchanged. Return the clock constant and corresponding rate cap to 10 MHz after the low-rate diagnostic confirms DMA progress.

Keep the clock/data wiring short and connect all grounds. GPIO39 and all data inputs must stay within the ESP32's permitted voltage range and never go below ground; use suitable high-speed buffering/level translation if required. Verify the actual clock and ADC output voltages at the ESP32 pins before connecting them.

This pin remap moves the gain potentiometer from GPIO32 to GPIO34, the peak detector from GPIO34 to GPIO35, the inverted peak detector from GPIO35 to GPIO36, and AD9235 D11 from GPIO36 to GPIO15. GPIO15 is an ESP32 boot-strapping pin. Ensure the ADC's D11 output is low or isolated while the ESP32 is resetting/booting; otherwise, the board may not start reliably. GPIO36 is input-only and remains assigned to the inverted detector, which is an input. The sampling-rate control is averaged, low-pass filtered, and uses hysteresis when selecting a discrete rate to reduce rate changes caused by small ADC fluctuations.

Core 1 snapshots the selected effective sample rate in `adcCaptureSamplingRate` and then sets the atomic `adcCaptureRequested` flag. Core 0 starts the circular I2S camera-mode DMA buffer and averages/decimates completed blocks into the shared plot buffer. The GPIO32 clock remains on at 1 MHz while idle, during capture, and after success or failure; the capture task controls DMA but never starts, stops, or retunes the clock. When the requested number of output samples has been produced or capture fails, core 0 stops DMA before clearing the request flag. On success it increments `adcCaptureSequence`. Core 1 observes the cleared flag, plots a successful buffer, and only then requests another burst; this keeps the shared sample buffer stable while it is being drawn. If the capture task cannot be created, setup continues without issuing capture requests and a red `CAPTURE TASK FAILED` indicator appears above the plot.

Capture diagnostics are controlled by `captureDebugEnabled` in `src/main.cpp` (disabled by default). When enabled, the line below the plot shows `Sc:` the last completed DMA sample count versus expected, `Sf:` capture state (`RUN` while in progress, `YES` if it failed, `NO` if it succeeded, and `--` before the first request is processed), and `Cr:` whether core 1 issued a capture request. The count is published after DMA has halted; there is no per-descriptor debug counter update during acquisition. During a normal capture the task does not print to Serial or read diagnostic registers. The short I2S ISR ORs interrupt status bits and increments an event count so timeout/error reports retain events that were acknowledged by the ISR; it does not inspect or move sample data. Immediately after starting RX/DMA, the task snapshots the initial engine/link/interrupt/descriptor state. On timeout or DMA error it snapshots the live RX/link state and interrupt masks before halting DMA, then gathers descriptor contents and remaining registers after halt and prints the report. A capture that exceeds twice its estimated sample duration plus one second (minimum two seconds) is halted and reported to Serial with the partial sample count, ISR event mask/count, and I2S state. The start and timeout snapshots are separate, allowing comparison of engine state at start versus at failure.

## Input limits

This is a learning/demo instrument, not a calibrated or protected instrument. All ESP32 ADC/GPIO inputs, including the AD9235 data outputs, must stay within the ESP32's permitted input-voltage range and must never receive a negative voltage. The GPIO27 frequency input must be a 3.3V-compatible square wave with both high and low phases longer than the configured 1 µs glitch filter. Use suitable dividers, level translation, and input protection where required, and verify signal levels before connecting them. Do not connect this circuit directly to mains or other hazardous voltages.

## Build and upload

Open this directory as a PlatformIO project, select the `esp32dev` environment, then build and upload. Detector readings are sampled continuously; each detector value averages ten analog readings per main-loop update. Each potentiometer is sampled ten times and averaged approximately 30 times per second. Dashboard measurements are capped at 10 fps and the plot plane at 60 fps. Each plot update follows an ADC burst of five plot-plane widths, currently captured at 1 MSPS and decimated to the selected effective sample rate; display updates do not wait for the independent frequency measurement. The 1 MSPS setting is temporary for isolating DMA operation and should be changed back to 10 MSPS after the test. The sample-rate pot selection is filtered and uses hysteresis to reduce oscillation between adjacent rates. Frequency is calculated from rising-edge counts over five consecutive windows; each window lasts `100 * 100 / Sampling_rate` seconds, capped at 10 seconds. Zero or insufficient pulses make the frequency unknown. Counts reaching the pulse-counter limit are reported as a lower-bound frequency. Detector measurements use the project's ADC lookup and calibration formulas; confirm those against the actual analog front end before relying on their absolute values.