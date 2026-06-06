# PlatformIO Arduino ESP-NOW Demo

This folder contains the Arduino PlatformIO version of the project for ESP32.

It includes two independent PlatformIO projects:
- `src/receiver` — ESP32 receiver that toggles its built-in LED when a message arrives
- `src/transmitter` — ESP32 transmitter that sends an ESP-NOW packet when a button is pressed and blinks its own built-in LED

## Build and Flash

From the root project folder, use the wrapper scripts:
```powershell
.\flash_receiver.ps1 -Port COM3
.\flash_transmitter.ps1 -Port COM4
```

Or use the CLI directly:
```powershell
cd src\receiver
python -m platformio run -t upload --upload-port COM3

cd ..\transmitter
python -m platformio run -t upload --upload-port COM4
```

## Notes

- `src/receiver` and `src/transmitter` are separate PlatformIO projects
- The receiver uses the `esp32-c3-devkitm-1` environment in `platformio.ini`
- The transmitter uses the `esp32dev` environment in `platformio.ini`
