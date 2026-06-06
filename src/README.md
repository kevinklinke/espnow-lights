# PlatformIO Arduino ESP-NOW Demo

This folder contains an Arduino-style PlatformIO version of the project for ESP32.

It includes two independent PlatformIO projects:
- `receiver` — ESP32 receiver that toggles its built-in LED when a message arrives
- `transmitter` — ESP32 transmitter that sends an ESP-NOW packet when a button is pressed and blinks its own built-in LED

## Build and Flash

Open `platformio-arduino/receiver` or `platformio-arduino/transmitter` in VS Code and use the PlatformIO toolbar.

Or use the CLI:

```powershell
cd platformio-arduino\receiver
platformio run --target upload
platformio device monitor
```

```powershell
cd platformio-arduino\transmitter
platformio run --target upload
platformio device monitor
```

## Notes

- Both projects use `board = esp32dev`
- Button input is on `GPIO 4`
- Built-in LED is on `GPIO 2`
- The transmitter uses broadcast messaging by default
