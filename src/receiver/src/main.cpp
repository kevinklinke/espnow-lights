#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "ReceiverConfig.h"
#include "CurtainWave.h"
#if RECEIVER_ID != 1
#include "WLED_SolidBar.h"
#endif
#include "../../shared/Message.h"

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

// LED pixel count can be overridden via ReceiverConfig.local.h
#ifndef LED_PIXEL_COUNT
#if RECEIVER_ID == 1
#define LED_PIXEL_COUNT CURTAIN_PIXEL_COUNT
#else
#define LED_PIXEL_COUNT 200
#endif
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

// Optional compile-time override for bar speed (ms per step). If 0, runtime mapping from press duration is used.
#ifndef LED_BAR_SPEED_MS
#define LED_BAR_SPEED_MS 12
#endif

#if RECEIVER_ID == 1
static_assert(LED_PIXEL_COUNT == CURTAIN_PIXEL_COUNT,
              "Receiver 1 curtain geometry requires LED_PIXEL_COUNT to be 600");
CurtainWave<LED_DATA_PIN> curtainWave(LED_BAR_SPEED_MS);
#else
WLED_SolidBar<LED_DATA_PIN> wledBar(LED_PIXEL_COUNT);
static CRGB ambientBuffer[LED_PIXEL_COUNT] = {};

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
#endif

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

#if RECEIVER_ID == 1
    const CRGB waveColor = CHSV(random(0, 256), random(200, 256), random(200, 256));
    curtainWave.startWave(waveColor);
    Serial.printf("Started curtain wave for receiver %u, seq=%u\n", trig.destination_id, trig.seq_num);
    return;
#else

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
#endif
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_GPIO, OUTPUT);
    setReceiverLed(false);

    // Startup ID is always shown on the first LED pixel so it works on all receivers.
#if RECEIVER_ID == 1
    curtainWave.begin();
#else
    wledBar.begin();
#endif
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

#if RECEIVER_ID == 1
    curtainWave.update();
#else
    // Keep the legacy linear-strip effects for other receiver IDs, but leave
    // the background black instead of generating the ambient shimmer.
    wledBar.applyAmbient(ambientBuffer);
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
#endif
}
