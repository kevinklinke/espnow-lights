#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "../../shared/Message.h"

static const int BUTTON_GPIO = 4;
static const int BUTTON2_GPIO = 21;
static const int LED_GPIO = 2;
static bool last_button_state = HIGH;
static bool last_button2_state = HIGH;
static uint32_t seq_num = 0;

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
    pinMode(BUTTON2_GPIO, INPUT_PULLUP);
    pinMode(LED_GPIO, OUTPUT);
    digitalWrite(LED_GPIO, LOW);

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
    Serial.println("Button on GPIO 4 - Normal wave");
    Serial.println("Button on GPIO 21 - Opposite-end wave");
}

void loop() {
    bool current_button_state = digitalRead(BUTTON_GPIO);
    bool current_button2_state = digitalRead(BUTTON2_GPIO);

    if (last_button_state == HIGH && current_button_state == LOW) {
        Message msg = { .type = MessageType::NORMAL, .seq_num = seq_num++ };
        esp_err_t result = esp_now_send(receiver_mac, (uint8_t *)&msg, sizeof(msg));

        if (result == ESP_OK) {
            Serial.printf("Button 4 pressed! Sent normal wave #%u\n", msg.seq_num);
        } else {
            Serial.printf("Failed to send normal wave: %d\n", result);
        }

        digitalWrite(LED_GPIO, HIGH);
        delay(100);
        digitalWrite(LED_GPIO, LOW);
    }

    if (last_button2_state == HIGH && current_button2_state == LOW) {
        Message msg = { .type = MessageType::OPPOSITE, .seq_num = seq_num++ };
        esp_err_t result = esp_now_send(receiver_mac, (uint8_t *)&msg, sizeof(msg));

        if (result == ESP_OK) {
            Serial.printf("Button 21 pressed! Sent opposite wave #%u\n", msg.seq_num);
        } else {
            Serial.printf("Failed to send opposite wave: %d\n", result);
        }

        digitalWrite(LED_GPIO, HIGH);
        delay(100);
        digitalWrite(LED_GPIO, LOW);
    }

    Serial.printf("Button 4: %s, Button 21: %s\n",
                  current_button_state == LOW ? "PRESSED" : "RELEASED",
                  current_button2_state == LOW ? "PRESSED" : "RELEASED");

    last_button_state = current_button_state;
    last_button2_state = current_button2_state;
    delay(50);
}
