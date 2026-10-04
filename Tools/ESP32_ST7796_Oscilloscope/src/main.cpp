#include <Arduino.h>
#include <TFT_eSPI.h>

#include <algorithm>

namespace {
constexpr int channelCount = 2;
constexpr int signalPins[channelCount] = {34, 35};
constexpr uint16_t channelColors[channelCount] = {TFT_GREEN, TFT_CYAN};
constexpr int plotLeft = 42;
constexpr int plotTop = 52;
constexpr int plotWidth = 426;
constexpr int plotHeight = 224;
constexpr int sampleCount = plotWidth - 2;
constexpr uint32_t samplePeriodUs = 500;
constexpr uint16_t backgroundColor = TFT_BLACK;
constexpr uint16_t gridColor = 0x3186;

TFT_eSPI display;
uint16_t samples[channelCount][sampleCount];

void drawStaticUi() {
  display.fillScreen(backgroundColor);
  display.setRotation(1);
  display.setTextColor(TFT_WHITE, backgroundColor);
  display.drawString("ESP32 SCOPE", 8, 8, 2);
  display.drawString("CH1 GPIO34  CH2 GPIO35  0-3.3V", 8, 30, 2);

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
  display.drawString("Time: " +
                         String(sampleCount * samplePeriodUs / 1000) + " ms",
                     plotLeft, 286, 2);
}

void capture() {
  uint32_t nextSample = micros();
  for (int index = 0; index < sampleCount; ++index) {
    while (static_cast<int32_t>(micros() - nextSample) < 0) {
      yield();
    }
    for (int channel = 0; channel < channelCount; ++channel) {
      samples[channel][index] =
          static_cast<uint16_t>(analogRead(signalPins[channel]));
    }
    nextSample += samplePeriodUs;
  }
}

void drawTrace(float frameRate) {
  uint16_t minimum[channelCount] = {4095, 4095};
  uint16_t maximum[channelCount] = {0, 0};
  uint32_t sum[channelCount] = {0, 0};
  for (int channel = 0; channel < channelCount; ++channel) {
    for (int index = 0; index < sampleCount; ++index) {
      const uint16_t sample = samples[channel][index];
      minimum[channel] = std::min(minimum[channel], sample);
      maximum[channel] = std::max(maximum[channel], sample);
      sum[channel] += sample;
    }
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

  for (int channel = 0; channel < channelCount; ++channel) {
    for (int index = 1; index < sampleCount; ++index) {
      const int previousX = plotLeft + 1 + index - 1;
      const int currentX = plotLeft + 1 + index;
      const int previousY = plotTop + plotHeight - 2 -
          static_cast<int>(samples[channel][index - 1]) *
              (plotHeight - 3) / 4095;
      const int currentY = plotTop + plotHeight - 2 -
          static_cast<int>(samples[channel][index]) *
              (plotHeight - 3) / 4095;
      display.drawLine(previousX, previousY, currentX, currentY,
                       channelColors[channel]);
    }
  }

  display.fillRect(8, 300, 464, 18, backgroundColor);
  for (int channel = 0; channel < channelCount; ++channel) {
    const float averageVolts =
        (sum[channel] / static_cast<float>(sampleCount)) * (3.3f / 4095.0f);
    const float minimumVolts = minimum[channel] * (3.3f / 4095.0f);
    const float maximumVolts = maximum[channel] * (3.3f / 4095.0f);
    const String stats = "CH" + String(channel + 1) + " MIN " +
        String(minimumVolts, 2) + " AVG " + String(averageVolts, 2) +
        " MAX " + String(maximumVolts, 2) + "V";
    display.setTextColor(channelColors[channel], backgroundColor);
    display.drawString(stats, channel == 0 ? 8 : 205, 300, 1);
  }
  const String frameRateText = frameRate > 0.0f
      ? "FPS " + String(frameRate, 1)
      : "FPS --";
  display.setTextColor(TFT_YELLOW, backgroundColor);
  display.drawRightString(frameRateText, display.width() - 8, 300, 1);
}
}  // namespace

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  for (const int signalPin : signalPins) {
    analogSetPinAttenuation(signalPin, ADC_11db);
  }

  display.init();
  drawStaticUi();
  Serial.println("ESP32 oscilloscope ready; inputs are GPIO34 and GPIO35 (ADC1).");
}

void loop() {
  static uint32_t previousFrameUs = 0;
  static bool hasPreviousFrame = false;
  const uint32_t frameStartUs = micros();
  float frameRate = 0.0f;
  if (hasPreviousFrame) {
    const uint32_t frameDurationUs = frameStartUs - previousFrameUs;
    if (frameDurationUs > 0) {
      frameRate = 1000000.0f / frameDurationUs;
    }
  }
  previousFrameUs = frameStartUs;
  hasPreviousFrame = true;

  capture();
  drawTrace(frameRate);
}