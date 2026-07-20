#include "WLED_Bar.h"
#include <algorithm>

int Bar::adjustStartPosition(int startPixelPosition) const {
  return startPixelPosition + (reverse ? 1 : -1);
}

int Bar::getNextPixelPosition() const {
  return pixelPosition + (reverse ? -1 : 1);
}

int Bar::getBarStartPixel() const {
  return reverse ? pixelPosition : pixelPosition - (int)barLengthPixels + 1;
}

int Bar::getBarEndPixel() const {
  return reverse ? pixelPosition + (int)barLengthPixels - 1 : pixelPosition;
}

int Bar::getVisibleStartPixel() const {
  return std::max(getBarStartPixel(), 0);
}

int Bar::getVisibleEndPixel() const {
  return std::min(getBarEndPixel(), ledCount - 1);
}

bool Bar::hasExited() const {
  return getVisibleEndPixel() < 0 || getVisibleStartPixel() >= ledCount;
}

int Bar::getVisibleLength() const {
  int visibleStart = getVisibleStartPixel();
  int visibleEnd = getVisibleEndPixel();
  return std::max(0, visibleEnd - visibleStart + 1);
}

int Bar::getStartPixel() const {
  return getBarStartPixel();
}

int Bar::getEndPixel() const {
  return getBarEndPixel();
}

int Bar::getLeadingEdgePixel() const {
  return reverse ? getStartPixel() : getEndPixel();
}
