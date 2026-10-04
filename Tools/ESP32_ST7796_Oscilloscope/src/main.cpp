#include <Arduino.h>
#include <TFT_eSPI.h>
#include <driver/pcnt.h>
#include <esp_err.h>

namespace {
constexpr int peakDetectorPin = 34;
constexpr int invertedPeakDetectorPin = 35;
constexpr int gainPotPin = 32;
constexpr int frequencyScalePotPin = 33;
constexpr int frequencyInputPin = 27;
constexpr int adcResolution = 4095;
constexpr float adcReferenceVolts = 3.3f;
constexpr uint32_t adcUpdatePeriodMs = 100;
constexpr uint32_t frequencyWindowMs = 250;
constexpr int16_t frequencyCounterLimit = 30000;
constexpr uint16_t backgroundColor = TFT_BLACK;

// possible sampling rates i decided on for the external ADC
const int sampling_rates[] = {100,200,300,400,500,600,700,800,900,1000,2000,3000,4000,5000,6000,7000,8000,9000,10000,20000,30000,40000,50000,60000,70000,80000,90000,100000,200000,300000,400000,500000,600000,700000,800000,900000,1000000,2000000,3000000,4000000,5000000,6000000,7000000,8000000,9000000,10000000};
int Sampling_rate = 0;

TFT_eSPI display;
float measuredFrequencyHz = 0.0f;
bool frequencyOverRange = false;
uint32_t lastFrequencyWindowMs = 0;

void drawStaticUi() {
  display.setRotation(1);
  display.fillScreen(backgroundColor);
  display.setTextColor(TFT_WHITE, backgroundColor);
  display.drawString("MEASUREMENTS", 12, 10, 4);
  display.setTextColor(TFT_LIGHTGREY, backgroundColor);
  display.drawString("Raw ADC values; scaling is not configured", 12, 44, 2);

  display.setTextColor(TFT_GREEN, backgroundColor);
  display.drawString("PEAK DETECTOR  GPIO34", 12, 82, 2);
  display.setTextColor(TFT_CYAN, backgroundColor);
  display.drawString("INVERTED PEAK  GPIO35", 12, 122, 2);
  display.setTextColor(TFT_ORANGE, backgroundColor);
  display.drawString("GAIN POT       GPIO32", 12, 162, 2);
  display.setTextColor(TFT_YELLOW, backgroundColor);
  display.drawString("FREQ SCALE POT GPIO33", 12, 202, 2);

  display.drawFastHLine(12, 244, display.width() - 24, TFT_DARKGREY);
  display.setTextColor(TFT_WHITE, backgroundColor);
  display.drawString("SQUARE-WAVE INPUT  GPIO27", 12, 254, 2);
}

void configureFrequencyCounter() {
  pcnt_config_t config = {};
  config.pulse_gpio_num = frequencyInputPin;
  config.ctrl_gpio_num = PCNT_PIN_NOT_USED;
  config.lctrl_mode = PCNT_MODE_KEEP;
  config.hctrl_mode = PCNT_MODE_KEEP;
  config.pos_mode = PCNT_COUNT_INC;
  config.neg_mode = PCNT_COUNT_DIS;
  config.counter_h_lim = frequencyCounterLimit;
  config.counter_l_lim = 0;
  config.unit = PCNT_UNIT_0;
  config.channel = PCNT_CHANNEL_0;

  ESP_ERROR_CHECK(pcnt_unit_config(&config));
  ESP_ERROR_CHECK(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_H_LIM));
  ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_0));
  ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT_0));
  lastFrequencyWindowMs = millis();
}

void updateFrequencyMeasurement(uint32_t nowMs) {
  const uint32_t elapsedMs = nowMs - lastFrequencyWindowMs;
  if (elapsedMs < frequencyWindowMs) {
    return;
  }

  ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT_0));
  int16_t pulseCount = 0;
  uint32_t eventStatus = 0;
  ESP_ERROR_CHECK(pcnt_get_counter_value(PCNT_UNIT_0, &pulseCount));
  ESP_ERROR_CHECK(pcnt_get_event_status(PCNT_UNIT_0, &eventStatus));
  ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_0));
  ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT_0));

  frequencyOverRange = (eventStatus & PCNT_EVT_H_LIM) != 0;
  if (!frequencyOverRange) {
    measuredFrequencyHz =
        pulseCount * (1000.0f / static_cast<float>(elapsedMs));
  }
  lastFrequencyWindowMs = nowMs;
}

void drawAdcReading(int y, int pin) {
  const int raw = analogRead(pin);
  const float volts = raw * (adcReferenceVolts / adcResolution);
  const String reading =
      "RAW " + String(raw) + "   " + String(volts, 2) + " V";

  display.fillRect(270, y - 2, display.width() - 282, 24, backgroundColor);
  display.setTextColor(TFT_WHITE, backgroundColor);
  display.drawRightString(reading, display.width() - 12, y, 2);
}

void drawFrequencyReading() {
  display.fillRect(12, 282, display.width() - 24, 26, backgroundColor);
  display.setTextColor(TFT_WHITE, backgroundColor);
  if (frequencyOverRange) {
    display.drawString(">120 kHz (OVER RANGE)", 12, 282, 2);
  } else {
    display.drawString(String(measuredFrequencyHz, 1) + " Hz  (250 ms gate)",
                       12, 282, 2);
  }
}

void updateMeasurements() {
  drawAdcReading(82, peakDetectorPin);
  drawAdcReading(122, invertedPeakDetectorPin);
  drawAdcReading(162, gainPotPin);
  drawAdcReading(202, frequencyScalePotPin);
  drawFrequencyReading();
}
}  // namespace

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetPinAttenuation(peakDetectorPin, ADC_11db);
  analogSetPinAttenuation(invertedPeakDetectorPin, ADC_11db);
  analogSetPinAttenuation(gainPotPin, ADC_11db);
  analogSetPinAttenuation(frequencyScalePotPin, ADC_11db);
  pinMode(frequencyInputPin, INPUT);

  display.init();
  drawStaticUi();
  configureFrequencyCounter();
  updateMeasurements();
  Serial.println(
      "Measurement inputs ready: ADC GPIO34/35/32/33; frequency counter GPIO27.");
}

void loop() {
  static uint32_t lastAdcUpdateMs = 0;
  const uint32_t nowMs = millis();
  updateFrequencyMeasurement(nowMs);

  if (nowMs - lastAdcUpdateMs >= adcUpdatePeriodMs) {
    updateMeasurements();
    lastAdcUpdateMs = nowMs;
  }
  delay(5);
}
