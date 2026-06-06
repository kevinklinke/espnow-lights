# ESPNOW Lights

This project demonstrates ESP-NOW communication between two ESP32 boards using Arduino and PlatformIO.

- **Receiver**: ESP32 dev board that toggles an LED when it receives a packet
- **Transmitter**: ESP32 dev board that sends a packet when a button is pressed

## Project Layout

- `src/receiver` — PlatformIO Arduino receiver project
- `src/transmitter` — PlatformIO Arduino transmitter project
- `flash_receiver.ps1` — build and upload receiver
- `flash_transmitter.ps1` — build and upload transmitter
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
```

### Direct PlatformIO CLI
```powershell
cd src\receiver
python -m platformio run -t upload --upload-port COM3

cd ..\transmitter
python -m platformio run -t upload --upload-port COM4
```

### Serial monitor
```powershell
python -m platformio device monitor --port COM3
```

## How It Works

1. Transmitter reads button state on GPIO 4
2. When pressed, it sends an ESP-NOW packet
3. Receiver listens for ESP-NOW packets
4. Receiver toggles LED on GPIO 2

## Notes

- If `pio` is not available, use `python -m platformio`
- The receiver and transmitter are separate PlatformIO projects inside `src/`
- Use the PlatformIO extension in VS Code if you prefer a graphical workflow
