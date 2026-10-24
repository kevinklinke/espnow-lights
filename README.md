# ESPNOW Lights

This project demonstrates ESP-NOW communication between two ESP32 boards using Arduino and PlatformIO.

- **Receiver**: ESP32 dev board that toggles an LED when it receives a packet
- **Transmitter**: ESP32 dev board that sends a packet when a button is pressed
- **WLED transmitter**: separate Wi-Fi button project that selects a WLED preset

## Project Layout

- `src/receiver` — PlatformIO Arduino receiver project
- `src/transmitter` — PlatformIO Arduino transmitter project
- `src/wled-transmitter` — standalone Wi-Fi button project for controlling WLED
- `src/shared` — shared message interface used by both transmitter and receiver
- `flash_receiver.ps1` — build and upload receiver
- `flash_transmitter.ps1` — build and upload transmitter
- `flash_wled_transmitter.ps1` — build and upload the WLED transmitter
- `HARDWARE.md` — hardware wiring and verification
- `SETUP_GUIDE.md` — setup steps and CLI instructions
- `QUICK_REFERENCE.md` — quick command reference
- `STRUCTURE.md` — project structure and code layout
- `TROUBLESHOOTING.md` — issues and fixes

## Prerequisites

1. Python 3.11 or later
2. PlatformIO CLI installed:
   ```powershell
   python -m pip install -U platformio
   ```
3. Two ESP32 dev boards
4. USB cables

## Build and Flash

### Use the wrapper scripts
```powershell
.\flash_receiver.ps1 -Port COM3
.\flash_transmitter.ps1 -Port COM4
.\flash_wled_transmitter.ps1 -Port COM5
```

The WLED transmitter targets ESP32-C3 only. Confirm the connected board is an
ESP32-C3 before uploading. Copy
`src/wled-transmitter/src/config.sample.h` to
`src/wled-transmitter/src/config.local.h` and set the Wi-Fi credentials and
WLED hostname there. The local config is ignored by Git.

### Direct PlatformIO CLI
```powershell
cd src\receiver
python -m platformio run -t upload --upload-port COM3

cd ..\transmitter
python -m platformio run -t upload --upload-port COM4

cd ..\wled-transmitter
python -m platformio run -e esp32-c3 -t upload --upload-port COM5
```

### Serial monitor
```powershell
python -m platformio device monitor --port COM3
```

## How It Works

1. The ESP-NOW transmitter reads buttons on GPIO 4 and GPIO 21
2. GPIO 4 sends a normal forward wave message
3. GPIO 21 sends an opposite-end wave message
4. The receiver listens for ESP-NOW packets
5. The receiver toggles LED on GPIO 2 and starts the light wave from the selected end of the strip
6. Separately, the WLED transmitter connects to Wi-Fi and sends a preset-selection request when its GPIO 4 button is pressed

## Notes

- If `pio` is not available, use `python -m platformio`
- The receiver and transmitter are separate PlatformIO projects inside `src/`
- Use the PlatformIO extension in VS Code if you prefer a graphical workflow
