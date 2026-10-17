# PlatformIO Arduino ESP-NOW Demo

This folder contains the Arduino PlatformIO version of the project for ESP32.

It includes two independent PlatformIO projects:
- `src/receiver` — ESP32 receiver that drives addressable LEDs and reacts to ESP-NOW messages
- `src/transmitter` — ESP32 transmitter that sends an ESP-NOW packet when a button is pressed and blinks its own built-in LED

## Receiver 1 Curtain

Receiver ID 1 models its 600 LEDs as a 10-strand by 60-position matrix, with LEDs
ordered sequentially by strand. Each message addressed to receiver 1 starts a
random-color, five-LED wave that moves down all strands together. Up to 32 waves
can be active; when a new message arrives at capacity, it replaces the oldest
wave. The ambient shimmer is disabled, so LEDs are dark when no wave is present.

The receiver flash script selects the 600-pixel count by default for receiver 1.
An explicit `-PixelCount` override must still match the curtain geometry.

## Build and Flash

From the root project folder, use the wrapper scripts:
```powershell
.\flash_receiver.ps1 -Port COM3
.\flash_transmitter.ps1 -Port COM4
```

Or use the CLI directly:
```powershell
cd src\receiver
python -m platformio run -e esp32-s2 -t upload --upload-port COM3

cd ..\transmitter
python -m platformio run -t upload --upload-port COM4
```

## Notes

- `src/receiver` and `src/transmitter` are separate PlatformIO projects
- All receivers use the ESP32-S2 target (`esp32-s2-saola-1`) in `platformio.ini`; the receiver flash script defaults to `esp32-s2`.
- The transmitter uses the `esp32dev` environment in `platformio.ini`
