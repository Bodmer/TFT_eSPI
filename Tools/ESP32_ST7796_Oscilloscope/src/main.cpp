#include <Arduino.h>
#include <TFT_eSPI.h>

#include <algorithm>

namespace {
constexpr int signalPin = 34;
constexpr int sampleCount = 480;
constexpr uint32_t samplePeriodUs = 500;
constexpr int plotLeft = 42;
constexpr int plotTop = 52;
constexpr int plotWidth = 426;
constexpr int plotHeight = 224;
constexpr uint16_t backgroundColor = TFT_BLACK;
constexpr uint16_t gridColor = 0x3186;
constexpr uint16_t traceColor = TFT_GREEN;

TFT_eSPI display;
uint16_t samples[sampleCount];

void drawStaticUi() {
  display.fillScreen(backgroundColor);
  display.setRotation(1);
  display.setTextColor(TFT_WHITE, backgroundColor);
  display.drawString("ESP32 SCOPE", 8, 8, 2);
  display.drawString("CH1  GPIO34  0-3.3V", 8, 30, 2);

  for (int division = 0; division <= 8; ++division) {
    const int x = plotLeft + division * (plotWidth - 1) / 8;
    display.drawFastVLine(x, plotTop, plotHeight, gridColor);
  }
  for (int division = 0; division <= 4; ++division) {
    const int y = plotTop + division * (plotHeight - 1) / 4;
    display.drawFastHLine(plotLeft, y, plotWidth, gridColor);
  }
  display.drawRect(plotLeft, plotTop, plotWidth, plotHeight, TFT_WHITE);
  display.drawString("0V", 5, plotTop + plotHeight - 10, 1);
  display.drawString("3.3V", 5, plotTop - 2, 1);
  display.drawString("Time: 240 ms", plotLeft, 286, 2);
}

void capture() {
  uint32_t nextSample = micros();
  for (int index = 0; index < sampleCount; ++index) {
    while (static_cast<int32_t>(micros() - nextSample) < 0) {
      yield();
    }
    samples[index] = static_cast<uint16_t>(analogRead(signalPin));
    nextSample += samplePeriodUs;
  }
}

void drawTrace() {
  uint16_t minimum = 4095;
  uint16_t maximum = 0;
  uint32_t sum = 0;
  for (const uint16_t sample : samples) {
    minimum = std::min(minimum, sample);
    maximum = std::max(maximum, sample);
    sum += sample;
  }

  display.fillRect(plotLeft + 1, plotTop + 1, plotWidth - 2, plotHeight - 2, backgroundColor);
  for (int division = 1; division < 8; ++division) {
    const int x = plotLeft + division * (plotWidth - 1) / 8;
    display.drawFastVLine(x, plotTop + 1, plotHeight - 2, gridColor);
  }
  for (int division = 1; division < 4; ++division) {
    const int y = plotTop + division * (plotHeight - 1) / 4;
    display.drawFastHLine(plotLeft + 1, y, plotWidth - 2, gridColor);
  }

  for (int index = 1; index < sampleCount; ++index) {
    const int previousY = plotTop + plotHeight - 2 -
        static_cast<int>(samples[index - 1]) * (plotHeight - 3) / 4095;
    const int currentY = plotTop + plotHeight - 2 -
        static_cast<int>(samples[index]) * (plotHeight - 3) / 4095;
    display.drawLine(plotLeft + index - 1, previousY,
                     plotLeft + index, currentY, traceColor);
  }

  const float averageVolts = (sum / static_cast<float>(sampleCount)) * (3.3f / 4095.0f);
  const float minimumVolts = minimum * (3.3f / 4095.0f);
  const float maximumVolts = maximum * (3.3f / 4095.0f);
  display.fillRect(8, 300, 460, 18, backgroundColor);
  display.setTextColor(TFT_YELLOW, backgroundColor);
  display.drawString("MIN " + String(minimumVolts, 2) + "V  AVG " +
                     String(averageVolts, 2) + "V  MAX " +
                     String(maximumVolts, 2) + "V", 8, 300, 2);
}
}  // namespace

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetPinAttenuation(signalPin, ADC_11db);

  display.init();
  drawStaticUi();
  Serial.println("ESP32 oscilloscope ready; input is GPIO34 (ADC1).");
}

void loop() {
  capture();
  drawTrace();
}