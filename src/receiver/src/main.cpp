#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "WLED_SolidBar.h"
#include "../../shared/Message.h"
#include "ReceiverConfig.h"

// LED pin can be overridden by defining `LED_GPIO_PIN` in ReceiverConfig.local.h
#ifndef LED_GPIO_PIN
#define LED_GPIO_PIN 8
#endif
static const int LED_GPIO = LED_GPIO_PIN;

// LED data pin can be overridden by defining `LED_DATA_PIN` in ReceiverConfig.local.h
#ifndef LED_DATA_PIN
#define LED_DATA_PIN 1
#endif
// Use a different name for the runtime variable to avoid colliding with the macro
static const uint8_t LED_DATA_GPIO = LED_DATA_PIN;
static bool led_state = false;

#ifndef RECEIVER_DEBUG_MODE
#define RECEIVER_DEBUG_MODE true
#endif
static const bool kReceiverDebugMode = RECEIVER_DEBUG_MODE;
static uint32_t debugFlashUntilMs = 0;

// LED pixel count can be overridden via ReceiverConfig.local.h
#ifndef LED_PIXEL_COUNT
#define LED_PIXEL_COUNT 200
#endif

static void setReceiverLed(bool on) {
    digitalWrite(LED_GPIO, on ? LOW : HIGH);
    led_state = on;
}

static void flashFirstPixel(uint8_t red, uint8_t green, uint8_t blue, uint32_t durationMs) {
    if (LED_PIXEL_COUNT == 0) {
        return;
    }

    FastLED[0][0] = CRGB(red, green, blue);
    FastLED.show();
    delay(durationMs);
    FastLED[0][0] = CRGB::Black;
    FastLED.show();
}

static void blinkStartupId(uint8_t id) {
    if (id == 0) {
        return;
    }

    for (uint8_t i = 0; i < id; ++i) {
        flashFirstPixel(255, 255, 255, 120);
        delay(120);
    }

    delay(400);
}

WLED_SolidBar<LED_DATA_PIN> wledBar(LED_PIXEL_COUNT);

// Optional compile-time override for bar speed (ms per step). If 0, runtime mapping from press duration is used.
#ifndef LED_BAR_SPEED_MS
#define LED_BAR_SPEED_MS 12
#endif

// Transient override state
enum TransientType : uint8_t {
    TRANSIENT_NONE = 0,
    TRANSIENT_FLASH = 1,
    TRANSIENT_BAR = 2,
};

struct TransientState {
    bool active = false;
    bool waitingForBars = false;
    uint32_t endMs = 0;
    TransientType type = TRANSIENT_NONE;
    uint8_t intensity = 0;
} transient;

// Ambient buffer (caller-owned)
static CRGB ambientBuffer[LED_PIXEL_COUNT];

void onDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    if (len < sizeof(Trigger)) {
        Serial.printf("Received data too short: %d bytes\n", len);
        return;
    }

    Trigger trig;
    memcpy(&trig, incomingData, sizeof(Trigger));

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    Serial.printf("Trigger from %s - cmd=%u dest=%u prio=%u seq=%u\n",
                  macStr, static_cast<uint8_t>(trig.cmd), trig.destination_id, trig.priority, trig.seq_num);

    if (trig.destination_id != kReceiverId) {
        Serial.printf("Ignoring trigger for destination %u (this receiver is %u)\n", trig.destination_id, kReceiverId);
        return;
    }

    if (kReceiverDebugMode) {
        debugFlashUntilMs = millis() + 150;
    }

    switch (trig.cmd) {
        case TriggerCmd::FLASH: {
            // Start a very fast wipe (bar) that rides over ambient.
            // Use a longer bar so it reads as a bright flash across the strand.
            unsigned int barLengthPixels = LED_PIXEL_COUNT / 4; // shorter flash (quarter-strip)
            if (barLengthPixels < 10) barLengthPixels = 10;
            if (barLengthPixels > 100) barLengthPixels = 100;
            unsigned int steps = LED_PIXEL_COUNT + barLengthPixels + 2;
            const unsigned int desiredMs = 100; // faster wipe
            unsigned int speedMs = desiredMs / steps;
            if (speedMs < 1) speedMs = 1;

            // Pick a lively random color for the flash (transmitters are dumb)
            uint8_t randomHue = random(0, 256);
            uint8_t saturation = random(200, 256);
            uint8_t brightness = random(200, 256);
            CRGB barColor = CHSV(randomHue, saturation, brightness);
            int startPosition = 0;
            bool reverse = false;
            wledBar.startBar(barColor.red, barColor.green, barColor.blue, barLengthPixels, speedMs, startPosition, reverse, true);

            transient.active = true;
            transient.type = TRANSIENT_FLASH;
            transient.waitingForBars = true; // wait until wipe/bars finish, then start 2s timer
            transient.endMs = 0;
            break;
        }

        case TriggerCmd::BAR: {
            // Start a bar wave as a visible transient; receivers decide mapping.
            // Transmitters are dumb — pick a palette based on seq_num so triggers vary.
            uint8_t paletteId = trig.seq_num & 0x03; // 0..3 rotating
            uint8_t hue = paletteId * 64;           // map 0..3 -> distinct hue ranges
            uint8_t saturation = random(200, 256);  // lively saturation
            uint8_t brightness = random(200, 256);  // lively brightness
            CRGB barColor = CHSV(hue, saturation, brightness);
                unsigned int barLengthPixels = LED_PIXEL_COUNT / 3;
                if (barLengthPixels < 10) barLengthPixels = 10;
                if (barLengthPixels > 100) barLengthPixels = 100;
            int startPosition = 0;
            bool reverse = false;
            wledBar.startBar(barColor.red, barColor.green, barColor.blue, barLengthPixels, LED_BAR_SPEED_MS, startPosition, reverse);

            transient.active = true;
            transient.type = TRANSIENT_BAR;
            transient.waitingForBars = true; // wait until bars finish then start 5s timer
            break;
        }

        default:
            Serial.println("Unknown trigger cmd");
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_GPIO, OUTPUT);
    setReceiverLed(false);

    // Startup ID is always shown on the first LED pixel so it works on all receivers.
    wledBar.begin();
    blinkStartupId(kReceiverId);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(onDataRecv);

    Serial.println("ESP-NOW Receiver Initialized");
    Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());
    Serial.println("Waiting for messages...");
}

void loop() {
    delay(2);

    uint32_t now = millis();

    // Build ambient white shimmer (always on). Transient bars will be blended on top.
    // Implement per-pixel, independent twinkles: random pixel spikes that decay over time.
    const uint8_t baseBri = 1; // slightly dimmer ambient base
    // persistent per-pixel state for twinkle amplitude (0..255)
    static uint8_t twinkleLevel[LED_PIXEL_COUNT] = {0};
    const uint8_t decayPerFrame = 1; // how fast twinkles fade (1 = very slow)
    const uint8_t triggerChance = 1; // approx chance (out of 256) per pixel per frame (very rare)

    for (int i = 0; i < LED_PIXEL_COUNT; ++i) {
        // Occasionally trigger a new twinkle on this pixel
        if (random8() < triggerChance) {
            uint8_t peak = random8(160, 255); // bright peak for the spike
            if (peak > twinkleLevel[i]) twinkleLevel[i] = peak;
        }

        // Compute final brightness: tiny base plus current twinkle level
        uint8_t bri = qadd8(baseBri, twinkleLevel[i]);
        ambientBuffer[i] = CRGB(bri, bri, bri);

        // Decay the twinkle for next frame
        twinkleLevel[i] = qsub8(twinkleLevel[i], decayPerFrame);
    }

    if (kReceiverDebugMode && now < debugFlashUntilMs) {
        ambientBuffer[0] = CRGB::White;
    }

    // Apply ambient background into the bar's internal buffer (bars draw on top)
    wledBar.applyAmbient(ambientBuffer);

    // Provide per-pixel twinkle amplitudes to the bar renderer so bars can be
    // brightened by twinkles (preserving bar hue). Then update/draw bars.
    wledBar.setTwinkleLevels(twinkleLevel, LED_PIXEL_COUNT);
    wledBar.update();

    // After update, check transient lifecycle
    if (transient.active) {
        if (transient.waitingForBars) {
            if (!wledBar.hasActiveBars()) {
                // Bars finished — clear transient state immediately; ambient remains on.
                transient.waitingForBars = false;
                transient.active = false;
                transient.type = TRANSIENT_NONE;
                transient.endMs = 0;
            }
        }
    }

    // Display all LEDs once per frame
    wledBar.display();
}
