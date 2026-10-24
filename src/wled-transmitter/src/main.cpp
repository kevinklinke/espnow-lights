#include <WiFi.h>
#include <HTTPClient.h>
#include "config.local.h"

const int BUTTON_GPIO = 4;
bool last_button_state = HIGH;

void sendNextPreset() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected; request skipped");
    return;
  }

  HTTPClient http;
  http.setTimeout(2000);

  String wledUrl = String("http://") + WLED_HOSTNAME + "/json/state";
  if (!http.begin(wledUrl)) {
    Serial.println("Could not initialize HTTP");
    return;
  }

  http.addHeader("Content-Type", "application/json");

  int status = http.POST("{\"ps\":\"1~5~\"}");

  Serial.printf("WLED HTTP status: %d\n", status);

  http.end();
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_GPIO, INPUT_PULLUP);

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

void loop() {
  bool current_button_state = digitalRead(BUTTON_GPIO);

  if (last_button_state == HIGH && current_button_state == LOW) {
    Serial.println("Button pressed! Sending WLED preset request");
    sendNextPreset();
  }

  Serial.printf("Button 4: %s\n", current_button_state == LOW ? "PRESSED" : "RELEASED");

  last_button_state = current_button_state;
  delay(50);
}
