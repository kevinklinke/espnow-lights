#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "WLED_SolidBar.h"
#include "../../shared/Message.h"
#include "ReceiverConfig.h"

static const int LED_GPIO = 8;
static const uint8_t LED_DATA_PIN = 1;
static bool led_state = false;

WLED_SolidBar<LED_DATA_PIN> wledBar(200);

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len < sizeof(Message)) {
        Serial.printf("Received data too short: %d bytes\n", len);
        return;
    }

    Message msg;
    memcpy(&msg, incomingData, sizeof(Message));

    if (msg.destination_id != kReceiverId) {
        Serial.printf("Ignoring message for destination %u (this receiver is %u)\n",
                      msg.destination_id, kReceiverId);
        return;
    }

    led_state = !led_state;
    digitalWrite(LED_GPIO, led_state ? LOW : HIGH);

    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    const char *typeName = "UNKNOWN";
    bool reverse = false;
    if (msg.type == MessageType::NORMAL) {
        typeName = "NORMAL";
        reverse = false;
    } else if (msg.type == MessageType::OPPOSITE) {
        typeName = "OPPOSITE";
        reverse = true;
    }

    Serial.printf("Received message from %s - Type: %s, Destination: %u, Seq: %u - LED: %s\n",
                  macStr, typeName, msg.destination_id, msg.seq_num, led_state ? "ON" : "OFF");

    // Generate vibrant random color
    uint8_t randomHue = random(0, 256);
    uint8_t saturation = random(200, 256);  // High saturation = vibrant
    uint8_t brightness = random(200, 256);  // High brightness = vivid
    CRGB barColor = CHSV(randomHue, saturation, brightness);
    
    // Map press duration to bar speed
    unsigned long pressDurationMs = 100; // TODO - put this in the message struct and set from sender
    uint16_t barSpeed = map(constrain(pressDurationMs, 100, 2000), 100, 2000, 40, 1);
    
    Serial.print("Button released after ");
    Serial.print(pressDurationMs);
    Serial.print("ms - Bar speed: ");
    Serial.println(barSpeed);
    
    // Start bar from the selected strip end
    unsigned int barLengthPixels = 20; // TODO - put this in the message struct and set from sender
    int startPosition = reverse ? 199 : 0;
    wledBar.startBar(barColor.red, barColor.green, barColor.blue, barLengthPixels, barSpeed, startPosition, reverse);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, led_state ? LOW : HIGH);

    wledBar.begin();

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

    // Update bars (clears array and draws bars, no show)
    wledBar.update();

    // Display all LEDs once per frame
    wledBar.display();
}
