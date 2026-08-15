#pragma once
#include <FastLED.h>
#include "WLED_Bar.h"
#include "../../shared/StaticVector.h"

#define BRIGHTNESS 255

template <uint8_t TDataPin>
class WLED_SolidBar {
public:
  WLED_SolidBar(int ledCount);
  ~WLED_SolidBar();
  WLED_SolidBar(const WLED_SolidBar&) = delete;
  WLED_SolidBar& operator=(const WLED_SolidBar&) = delete;
  void begin();
  // Update bar animations (clears array and draws bars, but does NOT call show)
  void update();
  // Display the current LED array (calls FastLED.show)
  void display();
  // Start a solid bar (multiple bars allowed simultaneously)
  // barLengthPixels: how many pixels the bar occupies (solid, no fading)
  // startPixelPosition: optional starting position for the bar (default 0)
  void startBar(uint8_t red, uint8_t green, uint8_t blue, unsigned int barLengthPixels = 10, unsigned int speedMs = 20, int startPixelPosition = 0, bool reverse = false);
  
  // Show charging glow effect (call this while button is pressed)
  // chargePixels: number of pixels currently charged (0-20)
  void showChargingGlow(uint8_t red, uint8_t green, uint8_t blue, int chargePixels);
  
  // Clear charging glow
  void clearChargingGlow();

private:
  static constexpr int MAX_BARS = 32;
  StaticVector<Bar, 32> bars_;
  CRGB *ledArray_;
  int ledCount_;
  
  // Helper: create a bar with the given parameters
  Bar createBar(uint8_t red, uint8_t green, uint8_t blue, unsigned int barLengthPixels, unsigned int speedMs, int startPixelPosition, bool reverse);
  // Helper: update bar position, returns false if bar has exited the strip
  bool updateBarPosition(Bar& bar, unsigned long currentTimeMs);
  // Helper: draw a single bar to the LED array
  void drawBar(const Bar& bar);
};

// ============================================================================
// Template implementations
// ============================================================================

template <uint8_t TDataPin>
WLED_SolidBar<TDataPin>::WLED_SolidBar(int ledCount)
    : ledArray_(nullptr), ledCount_(ledCount) {
  ledArray_ = new CRGB[ledCount_];
  FastLED.addLeds<WS2811, TDataPin, GRB>(ledArray_, ledCount_);
  FastLED.setBrightness(BRIGHTNESS);
}

template <uint8_t TDataPin>
WLED_SolidBar<TDataPin>::~WLED_SolidBar() {
  delete[] ledArray_;
}

template <uint8_t TDataPin>
void WLED_SolidBar<TDataPin>::begin() {
  for (int ledIndex = 0; ledIndex < ledCount_; ++ledIndex) ledArray_[ledIndex] = CRGB::Black;
  FastLED.show();
}

template <uint8_t TDataPin>
Bar WLED_SolidBar<TDataPin>::createBar(uint8_t red, uint8_t green, uint8_t blue, unsigned int barLengthPixels, unsigned int speedMs, int startPixelPosition, bool reverse) {
  Bar bar;
  bar.reverse = reverse;
  bar.ledCount = ledCount_;
  bar.pixelPosition = bar.adjustStartPosition(startPixelPosition);
  bar.color = CRGB(red, green, blue);
  bar.barLengthPixels = barLengthPixels;
  bar.speedMs = speedMs;
  bar.lastUpdateMs = millis();
  return bar;
}

template <uint8_t TDataPin>
bool WLED_SolidBar<TDataPin>::updateBarPosition(Bar& bar, unsigned long currentTimeMs) {
  if (currentTimeMs - bar.lastUpdateMs < bar.speedMs) {
    return true;  // Not yet time to update
  }
  
  bar.lastUpdateMs = currentTimeMs;
  bar.pixelPosition = bar.getNextPixelPosition();
  return !bar.hasExited();
}

template <uint8_t TDataPin>
void WLED_SolidBar<TDataPin>::drawBar(const Bar& bar) {
  int visibleBarLength = bar.getVisibleLength();
  if (visibleBarLength == 0) {
    return;
  }

  int pixelStart = bar.getVisibleStartPixel();
  int pixelEnd = bar.getVisibleEndPixel();
  int leadingEdgePixel = bar.getLeadingEdgePixel();

  for (int pixelIndex = pixelStart; pixelIndex <= pixelEnd; ++pixelIndex) {
    int distanceFromLeadingEdge = abs(pixelIndex - leadingEdgePixel);
    int remainingDistance = visibleBarLength - distanceFromLeadingEdge;
    uint8_t brightnessPercent = (remainingDistance * remainingDistance * 255) / (visibleBarLength * visibleBarLength);
    
    CRGB fadedColor = bar.color;
    fadedColor.nscale8(brightnessPercent);
    ledArray_[pixelIndex] = fadedColor;
  }
}

template <uint8_t TDataPin>
void WLED_SolidBar<TDataPin>::startBar(uint8_t red, uint8_t green, uint8_t blue, unsigned int barLengthPixels, unsigned int speedMs, int startPixelPosition, bool reverse) {
  Bar newBar = createBar(red, green, blue, barLengthPixels, speedMs, startPixelPosition, reverse);
  
  if (bars_.full()) {
    bars_.erase(0);  // Remove oldest bar (FIFO)
  }
  bars_.push_back(newBar);
}

template <uint8_t TDataPin>
void WLED_SolidBar<TDataPin>::update() {
  unsigned long currentTimeMs = millis();

  // Clear all LEDs
  for (int ledIndex = 0; ledIndex < ledCount_; ++ledIndex) {
    ledArray_[ledIndex] = CRGB::Black;
  }

  // Update and draw bars (iterate in reverse to safely erase)
  for (int barIndex = (int)bars_.size() - 1; barIndex >= 0; --barIndex) {
    if (!updateBarPosition(bars_[barIndex], currentTimeMs)) {
      bars_.erase(barIndex);
    } else {
      drawBar(bars_[barIndex]);
    }
  }
}

template <uint8_t TDataPin>
void WLED_SolidBar<TDataPin>::display() {
  FastLED.show();
}

template <uint8_t TDataPin>
void WLED_SolidBar<TDataPin>::showChargingGlow(uint8_t red, uint8_t green, uint8_t blue, int chargePixels) {
  // Add bouncing/shaky effect to the leading edge
  // Random jitter of ±1 pixel
  int edgeBounce = random(-1, 2);  // Random bounce between -1 and +1
  int displayPixels = constrain(chargePixels + edgeBounce, 1, 20);  // Ensure it stays within bounds
  
  // Create a gradient glow effect from the start of the strip
  CRGB glowColor(red, green, blue);
  
  for (int pixelIndex = 0; pixelIndex < displayPixels && pixelIndex < ledCount_; ++pixelIndex) {
    // Gradient: brightest at front, dimmer toward back
    int distanceFromFront = displayPixels - pixelIndex;
    uint8_t brightness = map(distanceFromFront, 0, displayPixels, 255, 50);
    
    // Add randomness for "shaky" effect - vary brightness by ±40%
    int8_t shakiness = random(-102, 103);  // ±40% of 255 is roughly ±102
    int finalBrightness = constrain((int)brightness + shakiness, 20, 255);
    
    CRGB fadedColor = glowColor;
    fadedColor.nscale8(finalBrightness);
    ledArray_[pixelIndex] = fadedColor;
  }
  
  FastLED.show();
}

template <uint8_t TDataPin>
void WLED_SolidBar<TDataPin>::clearChargingGlow() {
  for (int ledIndex = 0; ledIndex < ledCount_; ++ledIndex) {
    ledArray_[ledIndex] = CRGB::Black;
  }
}
