#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "../../shared/Message.h"

#ifndef TRANSMITTER_LED_GPIO
#define TRANSMITTER_LED_GPIO 8
#endif

#ifndef TRANSMITTER_ID
#define TRANSMITTER_ID 1
#endif

#ifndef TRANSMITTER_LED_ACTIVE_LOW
#define TRANSMITTER_LED_ACTIVE_LOW 1
#endif

static const int BUTTON_GPIO = 4;
static const int LED_GPIO = TRANSMITTER_LED_GPIO;
static const uint8_t DESTINATION_ID = static_cast<uint8_t>(TRANSMITTER_ID);
static bool last_button_state = HIGH;
static uint32_t seq_num = 0;

static void setLedState(bool on) {
    digitalWrite(LED_GPIO, TRANSMITTER_LED_ACTIVE_LOW ? (on ? LOW : HIGH) : (on ? HIGH : LOW));
}

static void blinkStartupId(uint8_t id) {
    if (id == 0) {
        return;
    }

    for (uint8_t i = 0; i < id; ++i) {
        setLedState(true);
        delay(180);
        setLedState(false);
        delay(180);
    }

    delay(400);
}

static uint8_t receiver_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (status == ESP_NOW_SEND_SUCCESS) {
        if (mac_addr) {
            Serial.printf("Data sent successfully to %02x:%02x:%02x:%02x:%02x:%02x\n",
                          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        } else {
            Serial.println("Data sent successfully");
        }
    } else {
        Serial.println("Data send failed");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(BUTTON_GPIO, INPUT_PULLUP);
    pinMode(LED_GPIO, OUTPUT);
    setLedState(false);
    blinkStartupId(DESTINATION_ID);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(onDataSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiver_mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }

    Serial.println("ESP-NOW Transmitter Initialized");
    Serial.printf("MAC Address: %s\n", WiFi.macAddress().c_str());
    Serial.printf("Single button on GPIO 4 sends BAR to receiver ID %u\n", DESTINATION_ID);
}

void loop() {
    bool current_button_state = digitalRead(BUTTON_GPIO);

    if (last_button_state == HIGH && current_button_state == LOW) {
        Trigger trig;
        trig.cmd = TriggerCmd::BAR;
        trig.destination_id = DESTINATION_ID;
        trig.priority = 0;
        trig.reserved = 0;
        trig.seq_num = seq_num++;

        esp_err_t result = esp_now_send(receiver_mac, (uint8_t *)&trig, sizeof(trig));

        if (result == ESP_OK) {
            Serial.printf("Button pressed! Sent BAR trigger to dest %u (#%u)\n", trig.destination_id, trig.seq_num);
        } else {
            Serial.printf("Failed to send BAR trigger: %d\n", result);
        }

        setLedState(true);
        delay(100);
        setLedState(false);
    }

    Serial.printf("Button 4: %s\n", current_button_state == LOW ? "PRESSED" : "RELEASED");

    last_button_state = current_button_state;
    delay(50);
}
