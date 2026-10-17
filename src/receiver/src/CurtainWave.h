#pragma once

#include <Arduino.h>
#include <FastLED.h>

static constexpr int CURTAIN_STRAND_COUNT = 10;
static constexpr int CURTAIN_LEDS_PER_STRAND = 60;
static constexpr int CURTAIN_PIXEL_COUNT = CURTAIN_STRAND_COUNT * CURTAIN_LEDS_PER_STRAND;
static constexpr int CURTAIN_WAVE_LENGTH = 5;
static constexpr int CURTAIN_MAX_WAVES = 32;

template <uint8_t TDataPin>
class CurtainWave {
public:
  explicit CurtainWave(unsigned int speedMs)
      : speedMs_(speedMs) {
    FastLED.addLeds<WS2811, TDataPin, GRB>(&leds_[0][0], CURTAIN_PIXEL_COUNT);
    FastLED.setBrightness(255);
  }

  void begin() {
    for (int strand = 0; strand < CURTAIN_STRAND_COUNT; ++strand) {
      for (int position = 0; position < CURTAIN_LEDS_PER_STRAND; ++position) {
        leds_[strand][position] = CRGB::Black;
      }
    }
    FastLED.show();
  }

  void startWave(const CRGB& color) {
    const uint32_t now = millis();
    portENTER_CRITICAL(&waveMux_);

    int slot = -1;
    uint64_t oldestOrder = UINT64_MAX;
    for (int i = 0; i < CURTAIN_MAX_WAVES; ++i) {
      if (!waves_[i].active) {
        slot = i;
        break;
      }
      if (waves_[i].order < oldestOrder) {
        oldestOrder = waves_[i].order;
        slot = i;
      }
    }

    Wave& wave = waves_[slot];
    wave.active = true;
    wave.headPosition = 0;
    wave.color = color;
    wave.lastUpdateMs = now;
    wave.order = nextOrder_++;

    portEXIT_CRITICAL(&waveMux_);
  }

  void update() {
    const uint32_t now = millis();
    Wave frameWaves[CURTAIN_MAX_WAVES] = {};
    int frameWaveCount = 0;

    portENTER_CRITICAL(&waveMux_);
    for (int i = 0; i < CURTAIN_MAX_WAVES; ++i) {
      Wave& wave = waves_[i];
      if (!wave.active) {
        continue;
      }

      if (now - wave.lastUpdateMs >= speedMs_) {
        wave.lastUpdateMs = now;
        ++wave.headPosition;
        if (wave.headPosition >= CURTAIN_LEDS_PER_STRAND + CURTAIN_WAVE_LENGTH - 1) {
          wave.active = false;
          continue;
        }
      }

      frameWaves[frameWaveCount++] = wave;
    }
    portEXIT_CRITICAL(&waveMux_);

    for (int strand = 0; strand < CURTAIN_STRAND_COUNT; ++strand) {
      for (int position = 0; position < CURTAIN_LEDS_PER_STRAND; ++position) {
        leds_[strand][position] = CRGB::Black;
      }
    }

    for (int i = 0; i < frameWaveCount; ++i) {
      const Wave& wave = frameWaves[i];
      const int firstPosition = max(0, wave.headPosition - CURTAIN_WAVE_LENGTH + 1);
      const int lastPosition = min(wave.headPosition, CURTAIN_LEDS_PER_STRAND - 1);

      for (int strand = 0; strand < CURTAIN_STRAND_COUNT; ++strand) {
        for (int position = firstPosition; position <= lastPosition; ++position) {
          leds_[strand][position] += wave.color;
        }
      }
    }

    FastLED.show();
  }

private:
  struct Wave {
    bool active = false;
    int headPosition = 0;
    CRGB color = CRGB::Black;
    uint32_t lastUpdateMs = 0;
    uint64_t order = 0;
  };

  CRGB leds_[CURTAIN_STRAND_COUNT][CURTAIN_LEDS_PER_STRAND] = {};
  Wave waves_[CURTAIN_MAX_WAVES] = {};
  unsigned int speedMs_;
  uint64_t nextOrder_ = 0;
  portMUX_TYPE waveMux_ = portMUX_INITIALIZER_UNLOCKED;
};
