# Setup Guide for ESPNOW Lights

## Prerequisites

Before you begin, make sure you have:

1. Python 3.11 or later
2. PlatformIO CLI installed:
   ```powershell
   python -m pip install -U platformio
   ```
3. Two ESP32 dev boards
4. USB cables
5. A Windows PC or other host that supports PlatformIO

## Step-by-Step Setup

### 1. Verify Python and PlatformIO
```powershell
python --version
python -m platformio --version
```

### 2. Connect the boards
Use [HARDWARE.md](HARDWARE.md) for wiring the receiver and transmitter.

### 3. Build and upload the receiver
```powershell
.\flash_receiver.ps1 -Port COM3
```

### 4. Build and upload the transmitter
```powershell
.\flash_transmitter.ps1 -Port COM4
```

### 5. Monitor the serial output
```powershell
python -m platformio device monitor --port COM3
```

### 6. Test the system
Press the button on the transmitter. The receiver LED should toggle each time a packet is received.

## Manual PlatformIO CLI Commands

### Receiver
```powershell
cd src\receiver
python -m platformio run -t upload --upload-port COM3
```

### Transmitter
```powershell
cd src\transmitter
python -m platformio run -t upload --upload-port COM4
```

### Serial monitor
```powershell
python -m platformio device monitor --port COM3
```

## Notes

- If `python -m platformio` works, you do not need to install a separate `pio` command.
- Use the correct COM port for your board.
- If upload fails, try a different USB cable or port.
