#pragma once

#include <FastLED.h>

struct Bar {
  int pixelPosition;
  CRGB color;
  unsigned int barLengthPixels;
  unsigned int speedMs;
  unsigned long lastUpdateMs;
  int ledCount;
  bool reverse;

  int adjustStartPosition(int startPixelPosition) const;
  int getNextPixelPosition() const;
  bool hasExited() const;
  int getVisibleLength() const;
  int getStartPixel() const;
  int getEndPixel() const;
  int getLeadingEdgePixel() const;

  int getBarStartPixel() const;
  int getBarEndPixel() const;
  int getVisibleStartPixel() const;
  int getVisibleEndPixel() const;
};
