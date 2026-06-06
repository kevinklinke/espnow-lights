# Project Structure and Overview

This project is organized as two separate PlatformIO Arduino projects under `src/`.

## Directory layout

```
espnow-lights/
├── flash_receiver.ps1
├── flash_transmitter.ps1
├── HARDWARE.md
├── SETUP_GUIDE.md
├── QUICK_REFERENCE.md
├── STRUCTURE.md
├── TROUBLESHOOTING.md
├── src/
│   ├── receiver/
│   │   ├── platformio.ini
│   │   └── src/
│   │       └── main.cpp
│   └── transmitter/
│       ├── platformio.ini
│       └── src/
│           └── main.cpp
```

## Receiver

- `src/receiver/platformio.ini` — PlatformIO project configuration for the receiver board
- `src/receiver/src/main.cpp` — Arduino code that initializes ESP-NOW and toggles the LED on packet reception
- `src/shared/message.h` — shared ESP-NOW message payload definition used by both transmitter and receiver

## Transmitter

- `src/transmitter/platformio.ini` — PlatformIO project configuration for the transmitter board
- `src/transmitter/src/main.cpp` — Arduino code that sends ESP-NOW packets when buttons are pressed
- `src/shared/message.h` — shared ESP-NOW message payload definition used by both transmitter and receiver

## Build scripts

- `flash_receiver.ps1` — builds and uploads the receiver project
- `flash_transmitter.ps1` — builds and uploads the transmitter project

## PlatformIO Notes

- Use `python -m platformio run -t upload` to build and upload from the command line
- Each project uses a separate PlatformIO environment defined in its `platformio.ini`
- Use `python -m platformio device monitor --port COMx` to view serial output
