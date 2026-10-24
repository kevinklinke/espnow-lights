#include <Arduino.h>
#include "WledPresetController.h"

static const int BUTTON_GPIO = 4;
static bool lastButtonState = HIGH;
static WledPresetController wled;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_GPIO, INPUT_PULLUP);
  wled.begin();
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_GPIO);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("Button pressed! Sending WLED preset request");
    wled.selectRandomPreset();
  }

  Serial.printf("Button 4: %s\n", currentButtonState == LOW ? "PRESSED" : "RELEASED");

  lastButtonState = currentButtonState;
  delay(50);
}
