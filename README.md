# ESPNOW Demo Project

This project demonstrates wireless communication between two ESP32 boards using ESP-NOW protocol:
- **Receiver**: ESP32 dev board with built-in LED on GPIO 2
- **Transmitter**: ESP32 dev board with a button on GPIO 4

## Hardware Setup

### Receiver (ESP32 Dev Board)
- Built-in LED (GPIO 2 on most ESP32 dev boards)
- USB connection for programming

### Transmitter (ESP32 Dev Board)
- Button connected to GPIO 4 (pulled to GND when pressed)
- USB connection for programming

## Prerequisites

1. ESP-IDF v5.0 or later: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html
2. Two compatible ESP32 dev boards

## Building and Flashing

### For Receiver (ESP32):
```bash
cd receiver
idf.py set-target esp32
idf.py menuconfig  # Configure if needed
idf.py build
idf.py -p COM3 flash monitor  # Replace COM3 with your port
```

### For Transmitter (ESP32):
```bash
cd transmitter
idf.py set-target esp32
idf.py menuconfig  # Configure if needed
idf.py build
idf.py -p COM4 flash monitor  # Replace COM4 with your port
```

## Finding Your Serial Ports

**Windows Command Prompt:**
```cmd
wmic logicaldisk get name
mode
```

**PowerShell:**
```powershell
Get-SerialPort
```

Or check Device Manager under "Ports (COM & LPT)"

## How It Works

1. The transmitter reads the button state (GPIO 4)
2. When the button is pressed, it sends a packet via ESP-NOW
3. The transmitter also blinks its built-in LED (GPIO 2) to confirm the button press
4. The receiver listens for ESP-NOW packets
5. When a packet is received, it toggles the built-in LED (GPIO 2)

## Pairing

The transmitter will automatically discover and communicate with the receiver. You may need to note the receiver's MAC address and set it in the transmitter code if automatic discovery doesn't work.

To get the MAC address of receiver:
- Flash the receiver and check the serial output for "MAC Address"

## Notes

- Both boards must be powered and running code
- Ensure both boards are on the same WiFi channel (both default to channel 1)
- ESPNOW uses direct communication without connecting to a WiFi network
