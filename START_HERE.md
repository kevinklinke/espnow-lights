# ESPNOW Lights - Start Here

This project uses Arduino and PlatformIO to demonstrate ESP-NOW communication between two ESP32 dev boards.

## Quick Start

### 1. Install prerequisites
- Python 3.11 or later
- PlatformIO CLI:
  ```powershell
  python -m pip install -U platformio
  ```
- Two ESP32 dev boards
- USB cables

### 2. Connect your boards
See [HARDWARE.md](HARDWARE.md) for wiring details.

### 3. Build and upload
```powershell
.\flash_receiver.ps1 -Port COM3
.\flash_transmitter.ps1 -Port COM4
```

### 4. Monitor serial output
```powershell
python -m platformio device monitor --port COM3
```

### 5. Test it
Press the transmitter button and verify the receiver LED toggles.

---

## Files Overview

- `src/receiver` — receiver PlatformIO project
- `src/transmitter` — transmitter PlatformIO project
- `flash_receiver.ps1` — build and upload receiver
- `flash_transmitter.ps1` — build and upload transmitter
- `HARDWARE.md` — wiring information
- `SETUP_GUIDE.md` — detailed setup steps
- `QUICK_REFERENCE.md` — command quick reference
- `STRUCTURE.md` — code and directory structure
- `TROUBLESHOOTING.md` — common issues and fixes

---

## If You Want More Detail
Read `SETUP_GUIDE.md` for complete setup instructions, or `README.md` for the full project overview.
