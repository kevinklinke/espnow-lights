#include "WledPresetController.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <esp_system.h>
#include <stdlib.h>

#include "config.local.h"

String WledPresetController::makeWledUrl(const char* path) const {
  return String("http://") + WLED_HOSTNAME + path;
}

bool WledPresetController::getWledJson(const char* path, String& response) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected; WLED request skipped");
    return false;
  }

  HTTPClient http;
  http.setTimeout(2000);

  if (!http.begin(makeWledUrl(path))) {
    Serial.println("Could not initialize HTTP");
    return false;
  }

  int status = http.GET();
  if (status != HTTP_CODE_OK) {
    Serial.printf("WLED GET %s failed with HTTP status: %d\n", path, status);
    http.end();
    return false;
  }

  response = http.getString();
  http.end();
  return true;
}

bool WledPresetController::refreshPresetCache() {
  lastPresetRefreshAttempt = millis();

  String response;
  if (!getWledJson("/presets.json", response)) {
    return false;
  }

  JsonDocument presets;
  DeserializationError error = deserializeJson(presets, response);
  if (error) {
    Serial.printf("Could not parse WLED presets: %s\n", error.c_str());
    return false;
  }
  if (!presets.is<JsonObject>()) {
    Serial.println("WLED presets response is not a JSON object");
    return false;
  }

  size_t newPresetCount = 0;
  for (JsonPair preset : presets.as<JsonObject>()) {
    const char* key = preset.key().c_str();
    char* end = nullptr;
    unsigned long id = strtoul(key, &end, 10);
    if (end == key || *end != '\0' || id == 0 || id > UINT16_MAX ||
        !preset.value().is<JsonObject>() || newPresetCount == MAX_PRESETS) {
      continue;
    }
    presetIds[newPresetCount++] = static_cast<uint16_t>(id);
  }

  presetCount = newPresetCount;
  presetCacheValid = true;
  Serial.printf("Cached %u WLED presets\n", static_cast<unsigned>(presetCount));

  String stateResponse;
  if (getWledJson("/json/state", stateResponse)) {
    JsonDocument state;
    error = deserializeJson(state, stateResponse);
    if (!error && state.is<JsonObject>()) {
      currentPresetId = state["ps"] | 0;
      currentPresetKnown = true;
    } else {
      Serial.println("Could not parse current WLED state");
    }
  }

  return true;
}

void WledPresetController::selectRandomPreset() {
  if (!presetCacheValid || millis() - lastPresetRefreshAttempt >= PRESET_CACHE_TTL_MS) {
    if (!refreshPresetCache()) {
      if (!presetCacheValid) {
        Serial.println("No WLED preset cache available; request skipped");
        return;
      }
      Serial.println("Using expired WLED preset cache");
    }
  }

  if (presetCount == 0) {
    Serial.println("No saved WLED presets found");
    return;
  }

  size_t eligibleCount = 0;
  for (size_t i = 0; i < presetCount; ++i) {
    if (!currentPresetKnown || presetIds[i] != currentPresetId) {
      ++eligibleCount;
    }
  }
  if (eligibleCount == 0) {
    eligibleCount = presetCount;
    currentPresetKnown = false;
  }

  size_t selectedIndex = static_cast<size_t>(random(eligibleCount));
  uint16_t selectedPresetId = 0;
  for (size_t i = 0; i < presetCount; ++i) {
    if (!currentPresetKnown || presetIds[i] != currentPresetId) {
      if (selectedIndex == 0) {
        selectedPresetId = presetIds[i];
        break;
      }
      --selectedIndex;
    }
  }

  HTTPClient http;
  http.setTimeout(2000);
  if (!http.begin(makeWledUrl("/json/state"))) {
    Serial.println("Could not initialize HTTP");
    return;
  }

  http.addHeader("Content-Type", "application/json");
  String payload = String("{\"ps\":") + selectedPresetId + "}";
  int status = http.POST(payload);
  Serial.printf("Selected WLED preset %u; HTTP status: %d\n",
                static_cast<unsigned>(selectedPresetId), status);

  if (status >= HTTP_CODE_OK && status < HTTP_CODE_MULTIPLE_CHOICES) {
    currentPresetId = selectedPresetId;
    currentPresetKnown = true;
  }
  http.end();
}

void WledPresetController::begin() {
  randomSeed(esp_random());

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected");
  Serial.println(WiFi.localIP());
}
