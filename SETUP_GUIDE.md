# Setup Guide for ESPNOW Demo

## Prerequisites

Before you begin, make sure you have:

1. **ESP-IDF installed**: The official development framework for ESP32
   - Download from: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html
   - Version 5.0 or later recommended
   - Windows: Follow the "Standard Setup of Toolchain for Windows" guide

2. **Required Hardware**:
   - ESP32 Dev Board (receiver)
   - ESP32 Dev Board (transmitter)
   - Push button
   - Two Micro USB cables (for programming)
   - Computer running Windows

3. **Optional but helpful**:
   - USB-to-Serial drivers (usually pre-installed on Windows 10/11)
   - A simple terminal program like PuTTY for serial monitoring

## Step-by-Step Setup

### 1. Install ESP-IDF (Windows)

#### Option A: Using ESP-IDF Installer (Recommended)
```
https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows.html
```

#### Option B: Manual Setup
```
1. Download Python 3.7+ from python.org
2. Clone ESP-IDF:
   git clone --recursive https://github.com/espressif/esp-idf.git
3. Run the installer from esp-idf folder:
   .\install.bat
4. Set up environment:
   .\export.bat
```

### 2. Verify ESP-IDF Installation
```cmd
idf.py --version
```

You should see a version number like `ESP-IDF v5.x.x`

### 3. Hardware Setup

See [HARDWARE.md](HARDWARE.md) for:
- Wiring diagrams
- Pin assignments
- How to find COM ports
- Troubleshooting hardware issues

### 4. Build the Receiver Code

```cmd
# Navigate to project directory
cd c:\Users\kevin\src\espnow-demo

# Build receiver
.\build_and_flash.ps1 receiver-build
```

Or using the batch file:
```cmd
build_and_flash.bat receiver-build
```

### 5. Flash the Receiver

```cmd
# Flash to COM3 (adjust if your port is different)
.\build_and_flash.ps1 receiver-flash -p COM3
```

Or:
```cmd
build_and_flash.bat receiver-flash -p COM3
```

### 6. Build the Transmitter Code

```cmd
.\build_and_flash.ps1 transmitter-build
```

### 7. Flash the Transmitter

```cmd
# Flash to COM4 (adjust if your port is different)
.\build_and_flash.ps1 transmitter-flash -p COM4
```

### 8. Monitor Serial Output

In two separate terminals:

**Terminal 1 - Monitor Receiver:**
```cmd
.\build_and_flash.ps1 receiver-monitor -p COM3
```

**Terminal 2 - Monitor Transmitter:**
```cmd
.\build_and_flash.ps1 transmitter-monitor -p COM4
```

### 9. Test the System

1. Press the button on the transmitter ESP32
2. Watch the receiver logs - you should see "Received message"
3. Check the built-in LED on the receiver ESP32 - it should toggle

Expected output on transmitter:
```
I (timestamp) espnow_transmitter: Button pressed! Sent message #0
I (timestamp) espnow_transmitter: Button pressed! Sent message #1
```

Expected output on receiver:
```
I (timestamp) espnow_receiver: Received message - LED: ON
I (timestamp) espnow_receiver: Received message - LED: OFF
```

## Manual Building Without Helper Scripts

If preferred, you can build manually:

### Build Receiver:
```cmd
cd receiver
idf.py set-target esp32
idf.py build
idf.py -p COM3 flash
idf.py -p COM3 monitor
cd ..
```

### Build Transmitter:
```cmd
cd transmitter
idf.py set-target esp32
idf.py build
idf.py -p COM4 flash
idf.py -p COM4 monitor
cd ..
```

## Configuration

### Change GPIO Pins

If you need to use different GPIO pins:

**Receiver (receiver/main/receiver.c):**
```c
#define LED_GPIO GPIO_NUM_2  // Change this to your LED pin
```

**Transmitter (transmitter/main/transmitter.c):**
```c
#define BUTTON_GPIO GPIO_NUM_9  // Change this to your button pin
```

Then rebuild and reflash.

### Use Specific MAC Address

The transmitter uses broadcast by default. To use a specific MAC address:

1. Run the receiver and note its MAC address from serial output:
   ```
   MAC Address: aa:bb:cc:dd:ee:ff
   ```

2. Update transmitter/main/transmitter.c:
   ```c
   static uint8_t receiver_mac[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
   ```

3. Rebuild and reflash the transmitter

## Troubleshooting

### Build Errors

**Error: "idf.py command not found"**
- Solution: Make sure ESP-IDF is installed and `export.bat` or `install-bat` was run
- Run: Set the IDF_PATH environment variable or re-run the ESP-IDF installer

**Error: "CMake configuration failed"**
- Solution: Clean the build and try again
  ```cmd
  .\build_and_flash.ps1 clean
  ```

### Flashing Issues

**"Failed to connect to COM3"**
- Solution: 
  - Check if the correct port is being used: `.\build_and_flash.ps1 list-ports`
  - Try a different USB port
  - Try a different USB cable
  - Restart the board by pressing EN button

**"Device not responding"**
- Solution: Press the EN button on the board, or press EN + BOOT together, then release BOOT first

### Communication Issues

**Nothing received on receiver**
- Check that both boards are powered
- Verify button wiring (GPIO 4 to GND)
- Both boards should show their MAC address on startup - they will communicate with each other
- Try with broadcast address (already default)

**LED not toggling**
- Verify GPIO 2 is correct for your ESP32 receiver board (check datasheet)
- The LED may be on the wrong GPIO - try GPIO 2 or another board LED pin
- The LED logic may be inverted - modify receiver.c: `gpio_set_level(LED_GPIO, !led_state);`

**Button not working**
- Verify button is connected to GPIO 4
- Check that button is connected to GND (not 3.3V)
- Test that button is wired correctly by checking logs (you may see repeated "Button pressed" messages)

### Serial Monitor Issues

**"Permission denied" when opening serial port**
- Solution: Close any other programs using the COM port (PuTTY, IDE, etc.)

**Garbled characters in serial output**
- Solution: This is usually not a problem - it's just the bootloader at a different baud rate
- Wait a moment and the output will show correctly

## Next Steps

Once the basic demo works, you can:

1. **Add more data** in the message structure
2. **Use specific pairing** instead of broadcast
3. **Add encryption** for secure communication
4. **Increase range** by adjusting transmission power
5. **Add multiple receivers** by pairing multiple devices
6. **Use different channels** for separate communication pairs

See ESP-NOW documentation: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_now.html

## Additional Resources

- ESP-IDF Documentation: https://docs.espressif.com/projects/esp-idf/
- ESP32 Datasheet: https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf
- ESP NOW API: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_now.html
