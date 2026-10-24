#pragma once

#include <Arduino.h>

class WledPresetController {
public:
  void begin();
  void selectRandomPreset();

private:
  static const unsigned long PRESET_CACHE_TTL_MS = 30000;
  static const size_t MAX_PRESETS = 250;

  uint16_t presetIds[MAX_PRESETS] = {};
  size_t presetCount = 0;
  uint32_t lastPresetRefreshAttempt = 0;
  bool presetCacheValid = false;
  uint16_t currentPresetId = 0;
  bool currentPresetKnown = false;

  bool getWledJson(const char* path, String& response);
  bool refreshPresetCache();
  String makeWledUrl(const char* path) const;
};
