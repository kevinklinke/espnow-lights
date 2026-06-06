# ESPNOW Demo - Start Here

Welcome to your ESPNOW learning project! This document will get you started.

## What You've Got

A complete wireless communication demo using the ESP-NOW protocol:
- **Receiver**: ESP32 dev board with a built-in LED that toggles when it receives a message
- **Transmitter**: ESP32 dev board with a button that sends a message when pressed
- **Communication**: Direct wireless link (no WiFi network needed)

## Quick Start (5 minutes)

### If You're In A Hurry

1. **Install ESP-IDF** (if not already done): https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows.html

2. **Connect your boards** to USB (see [HARDWARE.md](HARDWARE.md) for wiring)

3. **Open PowerShell** in the project folder and run:
   ```powershell
   # Find which COM ports your boards are on
   .\build_and_flash.ps1 list-ports
   
   # Build and flash receiver (assuming COM3)
   .\build_and_flash.ps1 receiver-build
   .\build_and_flash.ps1 receiver-flash -p COM3
   
   # Build and flash transmitter (assuming COM4)
   .\build_and_flash.ps1 transmitter-build
   .\build_and_flash.ps1 transmitter-flash -p COM4
   
   # In one window, monitor receiver
   .\build_and_flash.ps1 receiver-monitor -p COM3
   
   # In another window, monitor transmitter
   .\build_and_flash.ps1 transmitter-monitor -p COM4
   ```

4. **Press the button** on the transmitter and watch the receiver LED toggle!

That's it! You're running ESPNOW.

---

## Files Overview

This project includes everything you need:

### Documentation (Start Here)
- **[README.md](README.md)** - Project overview
- **[HARDWARE.md](HARDWARE.md)** - How to wire everything up ⭐ Read this first if you haven't wired yet
- **[SETUP_GUIDE.md](SETUP_GUIDE.md)** - Detailed step-by-step instructions
- **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - Command quick reference
- **[STRUCTURE.md](STRUCTURE.md)** - Code structure and how it works
- **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** - When things go wrong

### Code
- **receiver/main/receiver.c** - Receives messages and toggles LED
- **transmitter/main/transmitter.c** - Reads button and sends messages

### Build Scripts
- **build_and_flash.ps1** - PowerShell helper (Windows) - RECOMMENDED
- **build_and_flash.bat** - Batch file helper (Windows) - Alternative

---

## Prerequisites

You need:
1. **ESP-IDF** installed and working
2. **ESP32** dev board (receiver)
3. **ESP32** dev board (transmitter)
4. **Button** (momentary switch)
5. **USB cables** (2x Micro USB)
6. **Computer** with Windows

Don't have ESP-IDF? Follow: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows.html

---

## Recommended Reading Order

### First Time Setup
1. [HARDWARE.md](HARDWARE.md) - Understand what to wire
2. [SETUP_GUIDE.md](SETUP_GUIDE.md) - Follow the step-by-step guide
3. [QUICK_REFERENCE.md](QUICK_REFERENCE.md) - Copy commands from here

### If Something Breaks
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Find your issue and solution

### When It's Working
- [STRUCTURE.md](STRUCTURE.md) - Understanding the code

### For Deep Dives
- [README.md](README.md) - All the details

---

## What Happens When It Works

### On Receiver (Watch this output):
```
I (1000) espnow_receiver: ESP-NOW Receiver Initialized
I (1010) espnow_receiver: MAC Address: aa:bb:cc:dd:ee:ff
I (1020) espnow_receiver: Waiting for messages...
I (5000) espnow_receiver: Received message - LED: ON
I (5100) espnow_receiver: Received message - LED: OFF
```

### On Transmitter (Watch this output):
```
I (1000) espnow_transmitter: ESP-NOW Transmitter Initialized
I (1010) espnow_transmitter: MAC Address: 11:22:33:44:55:66
I (1020) espnow_transmitter: Button on GPIO 4 - Press to send message
I (5000) espnow_transmitter: Button pressed! Sent message #0
I (5010) espnow_transmitter: Data sent successfully
```

---

## Next Steps After Getting It Working

Once this demo is working, you can:

1. **Add more buttons** - Create multi-button remote control
2. **Add more sensors** - Send temperature, humidity, light levels, etc.
3. **Control more devices** - Switch multiple LEDs or motors
4. **Add security** - Use encryption for real-world applications
5. **Extend range** - Adjust transmission power
6. **Create networks** - Have multiple transmitters/receivers

See [STRUCTURE.md](STRUCTURE.md) for ideas on expanding.

---

## Key Components Explained

### The Button (Transmitter)
- Connected to GPIO 4
- When pressed, it sends a wireless packet
- The transmitter code watches for this and sends data
- The transmitter built-in LED will blink briefly to confirm each press

### The LED (Receiver)
- Connected to GPIO 2 on most ESP32 dev boards
- Toggles (on ↔ off) each time a packet is received
- You'll see it light up when you press the button

### ESP-NOW
- Wireless protocol that doesn't need WiFi
- Direct chip-to-chip communication
- Low power, low latency
- Range: 100-250 meters typically

---

## Troubleshooting - Quick Fixes

| Problem | Quick Fix |
|---------|-----------|
| Can't find COM port | Run `.\build_and_flash.ps1 list-ports` |
| Board won't flash | Press EN button or try different USB cable |
| No serial output | Check you're monitoring the right port |
| Button doesn't work | Check wiring to GPIO 4 and GND |
| LED doesn't toggle | Check wiring to GPIO 2, or check if GPIO is correct for your board |
| No communication | Make sure both boards are powered and running code |

See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for detailed solutions.

---

## Getting Help

If something doesn't work:

1. **Check the error message** - Search [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
2. **Verify your hardware** - Check [HARDWARE.md](HARDWARE.md)
3. **Review your steps** - Follow [SETUP_GUIDE.md](SETUP_GUIDE.md) again
4. **Check official docs** - https://docs.espressif.com/projects/esp-idf/

---

## Let's Get Started!

👉 **Next: Open [HARDWARE.md](HARDWARE.md) to wire up your boards**

Or if already wired: **Open [SETUP_GUIDE.md](SETUP_GUIDE.md) to follow the setup steps**

Good luck! 🚀
