#pragma once
#include <FastLED.h>

#define BRIGHTNESS 200

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
  struct Bar {
    bool active;
    int pixelPosition;
    uint8_t red, green, blue;
    unsigned int barLengthPixels;
    unsigned int speedMs;
    unsigned long lastUpdateMs;
    bool reverse;
  };

  static const int MAX_BARS = 32;
  Bar bars_[MAX_BARS];
  CRGB *ledArray_;
  int ledCount_;
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

  for (int barIndex = 0; barIndex < MAX_BARS; ++barIndex) {
    bars_[barIndex].active = false;
  }
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
void WLED_SolidBar<TDataPin>::startBar(uint8_t red, uint8_t green, uint8_t blue, unsigned int barLengthPixels, unsigned int speedMs, int startPixelPosition, bool reverse) {
  // Find free slot
  for (int barIndex = 0; barIndex < MAX_BARS; ++barIndex) {
    if (!bars_[barIndex].active) {
      bars_[barIndex].active = true;
      bars_[barIndex].pixelPosition = reverse ? startPixelPosition + 1 : startPixelPosition - 1; // -1 so first update moves it to startPixelPosition
      bars_[barIndex].red = red;
      bars_[barIndex].green = green;
      bars_[barIndex].blue = blue;
      bars_[barIndex].barLengthPixels = barLengthPixels;
      bars_[barIndex].speedMs = speedMs;
      bars_[barIndex].lastUpdateMs = millis();
      bars_[barIndex].reverse = reverse;
      return;
    }
  }
  // No free slot: replace the oldest bar
  unsigned long oldestTimeMs = ULONG_MAX;
  int selectedBarIndex = 0;
  for (int barIndex = 0; barIndex < MAX_BARS; ++barIndex) {
    if (bars_[barIndex].lastUpdateMs < oldestTimeMs) {
      oldestTimeMs = bars_[barIndex].lastUpdateMs;
      selectedBarIndex = barIndex;
    }
  }
  bars_[selectedBarIndex].active = true;
  bars_[selectedBarIndex].pixelPosition = reverse ? startPixelPosition + 1 : startPixelPosition - 1;
  bars_[selectedBarIndex].red = red;
  bars_[selectedBarIndex].green = green;
  bars_[selectedBarIndex].blue = blue;
  bars_[selectedBarIndex].barLengthPixels = barLengthPixels;
  bars_[selectedBarIndex].speedMs = speedMs;
  bars_[selectedBarIndex].lastUpdateMs = millis();
  bars_[selectedBarIndex].reverse = reverse;
}

template <uint8_t TDataPin>
void WLED_SolidBar<TDataPin>::update() {
  unsigned long currentTimeMs = millis();

  // Clear all LEDs (no fading, just simple solid bars)
  for (int ledIndex = 0; ledIndex < ledCount_; ++ledIndex) {
    ledArray_[ledIndex] = CRGB::Black;
  }

  bool anyBarActive = false;
  for (int barIndex = 0; barIndex < MAX_BARS; ++barIndex) {
    if (!bars_[barIndex].active) continue;

    // Update bar position
    if (currentTimeMs - bars_[barIndex].lastUpdateMs >= bars_[barIndex].speedMs) {
      bars_[barIndex].lastUpdateMs = currentTimeMs;
      if (bars_[barIndex].reverse) {
        bars_[barIndex].pixelPosition -= 1;
        if (bars_[barIndex].pixelPosition <= -(int)bars_[barIndex].barLengthPixels) {
          bars_[barIndex].active = false;
          continue;
        }
      } else {
        bars_[barIndex].pixelPosition += 1;
      
      // Check if bar has completely exited the strip
      // Bar must pass beyond the end by barLengthPixels to ensure all pixels clear
        if (bars_[barIndex].pixelPosition >= ledCount_ + (int)bars_[barIndex].barLengthPixels) {
          bars_[barIndex].active = false;
          continue;
        }
      }
    }

    // Draw solid bar, starting with 1 pixel and growing to full barLengthPixels
    // visibleBarLength starts at 1 and grows until it reaches barLengthPixels
    int visibleBarLength = bars_[barIndex].reverse
                           ? max(0, min(ledCount_ - bars_[barIndex].pixelPosition, (int)bars_[barIndex].barLengthPixels))
                           : max(0, min(bars_[barIndex].pixelPosition + 1, (int)bars_[barIndex].barLengthPixels));
    
    if (visibleBarLength == 0) {
      anyBarActive = true;
      continue;
    }

    CRGB barColor(bars_[barIndex].red, bars_[barIndex].green, bars_[barIndex].blue);
    
    int barStartPixel = bars_[barIndex].reverse
                        ? bars_[barIndex].pixelPosition
                        : bars_[barIndex].pixelPosition - visibleBarLength + 1;
    int barEndPixel = bars_[barIndex].reverse
                      ? bars_[barIndex].pixelPosition + visibleBarLength - 1
                      : bars_[barIndex].pixelPosition;

    int leadingEdgePixel = bars_[barIndex].reverse ? barStartPixel : barEndPixel;

    for (int pixelIndex = barStartPixel;
         pixelIndex <= barEndPixel && pixelIndex < ledCount_ && pixelIndex >= 0;
         ++pixelIndex) {
      // Calculate distance from the leading edge.
      // For normal bars the leading edge is at barEndPixel.
      // For reverse bars the leading edge is at barStartPixel.
      int distanceFromLeadingEdge = abs(pixelIndex - leadingEdgePixel);
      
      // Quadratic fade: full brightness at leading edge, dim at trailing edge
      // Formula: brightness = 255 * ((remainingDistance) / maxDistance)^2
      int remainingDistance = visibleBarLength - distanceFromLeadingEdge;
      uint8_t brightnessPercent = (remainingDistance * remainingDistance * 255) / (visibleBarLength * visibleBarLength);
      
      CRGB fadedColor = barColor;
      fadedColor.nscale8(brightnessPercent);
      ledArray_[pixelIndex] = fadedColor;
    }
    
    anyBarActive = true;
  }

  if (anyBarActive) {
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
    // Gradient: brightest at the front (most recently "charged"), dimmer toward the back
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
