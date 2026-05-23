#include "drivers/sound.hpp"

#include "drivers/pin.hpp"
#include "drivers/timer.hpp"
#include "state.hpp"

static constexpr uint16_t SAMPLE_INTERVAL_US = 1000;
static constexpr uint16_t WINDOW_MS = 50;

Sound::Sound(const PinDescriptor& input) : input(input) {
  windowStartMillis = millis();
}

void Sound::update() {
  uint32_t nowMicros = micros();

  // Sample periodically
  if (nowMicros - lastSampleMicros >= SAMPLE_INTERVAL_US) {
    lastSampleMicros = nowMicros;
    uint16_t sample = input.read();
    if (sample < signalMin) signalMin = sample;
    if (sample > signalMax) signalMax = sample;
  }

  uint32_t nowMillis = millis();

  // Finish window
  if (nowMillis - windowStartMillis >= WINDOW_MS) {
    peakToPeak = signalMax - signalMin;
    if (peakToPeak >= state.alarmThreshold) { spikeDetected = true; }

    signalMin = 1023;
    signalMax = 0;
    windowStartMillis = nowMillis;
  }
}

uint16_t Sound::amplitude() { return peakToPeak; }

bool Sound::spike() { return spikeDetected; }

void Sound::clearSpike() { spikeDetected = false; }