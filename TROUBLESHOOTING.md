# Troubleshooting Guide for ESPNOW Demo

## Build Issues

### Error: `idf.py: command not found`

**Cause:** ESP-IDF is not installed or the environment is not set up.

**Solutions:**
1. Install ESP-IDF from: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html
2. Run the ESP-IDF installer (`install.bat` on Windows)
3. In PowerShell, run: `$env:IDF_PATH = "C:\path\to\esp-idf"`
4. Or use the ESP-IDF PowerShell prompt from the Start Menu

### Error: `CMake Error`

**Cause:** Build directory is corrupted or CMake configuration failed.

**Solutions:**
```powershell
# Clean the entire build directory
.\build_and_flash.ps1 clean

# Then rebuild
.\build_and_flash.ps1 receiver-build
.\build_and_flash.ps1 transmitter-build
```

### Error: `python: command not found`

**Cause:** Python is not installed or not in PATH.

**Solutions:**
1. Install Python 3.7+ from python.org
2. Add Python to PATH: https://docs.python.org/3/using/windows.html#installation-steps
3. Or use the ESP-IDF Python environment

### Error: `Target not found` or `Unknown target`

**Cause:** Target not set correctly.

**Solutions:**
```cmd
# Set the correct target before building
cd receiver
idf.py set-target esp32
idf.py build

cd ..\transmitter
idf.py set-target esp32
idf.py build
```

---

## Flashing Issues

### Error: `Failed to connect to COM3`

**Cause:** 
- Board not connected or wrong port
- Another program is using the port
- USB driver not installed
- Board not in download mode

**Solutions:**

1. **Check if board is connected:**
   ```powershell
   .\build_and_flash.ps1 list-ports
   ```

2. **Close other programs** using the port (PuTTY, Arduino IDE, etc.)

3. **Try the right COM port:**
   ```powershell
   .\build_and_flash.ps1 receiver-flash -p COM3
   .\build_and_flash.ps1 receiver-flash -p COM4
   # Try each COM port from list-ports
   ```

4. **Reset the board:**
   - Press the EN (Enable) button on the board
   - Try flashing again

5. **Enter download mode:**
   - Hold BOOT button
   - Press EN button while holding BOOT
   - Release EN button (still holding BOOT)
   - Release BOOT button
   - Try flashing

6. **Install USB drivers:**
   - For CH340 chips: https://www.wch.cn/downloads/CH341SER_ZIP.html
   - For CP210x chips: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
   - Restart computer after installing

### Error: `Timeout waiting for packet header`

**Cause:** Board not in download mode or connection is unstable.

**Solutions:**
1. Press EN button to reset
2. Try a different USB cable
3. Try a different USB port on computer
4. Reduce flashing speed: `idf.py -p COM3 -b 230400 flash`

### Error: `Invalid header: 0xffffffff`

**Cause:** Board is not in download mode or is disconnected.

**Solutions:**
1. Disconnect and reconnect USB cable
2. Manually enter download mode (see above)
3. Check if using the correct board type (esp32)

---

## Serial Monitor Issues

### No Output in Serial Monitor

**Cause:**
- Wrong COM port
- Wrong baud rate
- Board not powered
- Code not running

**Solutions:**

1. **Check connection:**
   ```powershell
   .\build_and_flash.ps1 list-ports
   ```

2. **Monitor with correct port:**
   ```powershell
   .\build_and_flash.ps1 receiver-monitor -p COM3
   ```

3. **Manual baud rate check:**
   - Baud rate should be 115200
   - Common alternatives: 74880 (bootloader output), 9600, 230400

4. **Check if code is running:**
   - Press EN button on board
   - Look for bootloader messages first (these may be garbled)
   - Code output should follow

### Garbled Characters in Output

**Cause:** Baud rate mismatch.

**Solutions:**
1. Baud rate is usually correct at 115200
2. The initial bootloader output at 74880 baud may look garbled - this is normal
3. Wait a moment - application output at 115200 will follow

### "Permission denied" on COM port

**Cause:** Another program has the port open.

**Solutions:**
1. Close all other serial monitors (PuTTY, IDE, etc.)
2. Kill any python processes: `taskkill /F /IM python.exe` (be careful!)
3. Restart the computer

---

## Communication Issues

### Transmitter and Receiver Can't Communicate

**Cause:**
- Receiver not listening
- Transmitter not sending
- Wrong MAC address
- Different channels
- Too far apart

**Solutions:**

1. **Verify both boards are running:**
   - Open two serial monitors
   - One for receiver (look for "Waiting for messages...")
   - One for transmitter (look for "Button on GPIO...")

2. **Verify receiver is initialized:**
   ```
   Look for: ESP-NOW Receiver Initialized
   Look for: MAC Address: xx:xx:xx:xx:xx:xx
   Look for: Waiting for messages...
   ```

3. **Verify transmitter is initialized:**
   ```
   Look for: ESP-NOW Transmitter Initialized
   Look for: MAC Address: yy:yy:yy:yy:yy:yy
   ```

4. **Check the codes:**
   - Receiver should have `esp_now_register_recv_cb(on_data_recv);`
   - Transmitter should have `esp_now_add_peer(&peerInfo);`

5. **Test with broadcast (default):**
   - Transmitter uses broadcast (ff:ff:ff:ff:ff:ff) by default
   - Any device listening should receive

6. **Check channel:**
   - Default channel is 1 (or current WiFi channel)
   - Both boards should be on the same channel

### "Button pressed" but LED doesn't toggle

**Cause:**
- Receiver not receiving messages
- LED on wrong GPIO
- LED logic inverted

**Solutions:**

1. **Verify message is sent:**
   - Look for "Sent message #X" on transmitter
   - Look for "Data sent successfully" on transmitter

2. **Verify message is received:**
   - Look for "Received message" on receiver
   - Check MAC address in message

3. **Check GPIO on receiver:**
   - Default is GPIO 2
   - Modify in receiver.c if needed: `#define LED_GPIO GPIO_NUM_XX`

4. **Check LED logic:**
   - LED may be active-low
   - Try inverting: `gpio_set_level(LED_GPIO, !led_state);`

### Lots of Messages Without Pressing Button

**Cause:**
- Button bouncing
- GPIO floating (no pullup)
- Button wired upside down

**Solutions:**

1. **Check button wiring:**
   - Button should connect GPIO 4 to GND when pressed
   - Check wiring diagram in HARDWARE.md

2. **Add debounce delay:**
   - Already implemented (50ms)
   - Increase if still bouncing: change `pdMS_TO_TICKS(50)` to `pdMS_TO_TICKS(100)`

3. **Verify pullup is working:**
   - Code enables internal pullup: `pull_up_en = GPIO_PULLUP_ENABLE`
   - When button not pressed, GPIO should read HIGH (1)
   - When button pressed, GPIO should read LOW (0)

---

## Hardware Issues

### Board Not Powering On

**Cause:**
- Bad USB cable
- Wrong USB port
- Board defect
- Wrong power supply

**Solutions:**

1. **Check USB cable:**
   - Try different USB cable
   - Try USB cable from another device that works

2. **Try different USB port:**
   - Try different port on computer
   - Try different computer if possible

3. **Check for LED indicator:**
   - Some boards have a blue LED when powered
   - If no LED, may need external power supply

4. **Try manual power:**
   - Some boards may need external 3.3V power
   - Do NOT use 5V - will damage the board

### Button Doesn't Work

**Cause:**
- Wrong GPIO
- Button not making contact
- Wiring incorrect
- Button defective

**Solutions:**

1. **Test the button:**
   - Use a continuity tester on your multimeter
   - When pressed, should show continuity
   - When not pressed, should show no continuity

2. **Check wiring:**
   - One button pin to GPIO 4
   - Other button pin to GND
   - No other connections

3. **Verify GPIO in code:**
   - Check transmitter.c: `#define BUTTON_GPIO GPIO_NUM_9`
   - Match your actual GPIO number

4. **Test without code:**
   - Measure voltage on GPIO 4
   - Should be 3.3V when not pressed
   - Should be 0V when pressed

### LED Doesn't Light Up

**Cause:**
- Wrong GPIO
- LED defective
- Wrong logic level
- Wrong pin on board

**Solutions:**

1. **Check GPIO:**
   - Receiver ESP32: Usually GPIO 2 (check board pinout)
   - Try commonly used pins: 2, 4, 5, 12, 13, 14

2. **Test the LED:**
   - Check polarity if external LED
   - Try powered LED to verify it works
   - Use multimeter or continuity tester

3. **Check logic:**
   - LED may be active-LOW (inverted)
   - Modify receiver.c: `gpio_set_level(LED_GPIO, !led_state);`

4. **Verify pin exists:**
   - Check board datasheet or pinout
   - Some boards don't have certain GPIOs exposed

---

## Windows Specific Issues

### PowerShell Script Won't Run

**Cause:** PowerShell execution policy is restricted.

**Solutions:**
```powershell
# Allow scripts in current session
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process

# Or use batch file instead
.\build_and_flash.bat receiver-build
```

### Can't Find Command in PowerShell

**Cause:** Working directory is wrong.

**Solutions:**
```powershell
# Navigate to project folder first
cd C:\Users\kevin\src\espnow-demo

# Then run commands
.\build_and_flash.ps1 receiver-build
```

### Environment Variables Not Taking Effect

**Cause:** Terminal was open before setting variables.

**Solutions:**
1. Close PowerShell/CMD completely
2. Open new PowerShell/CMD window
3. Try again

---

## Advanced Debugging

### Enable Verbose Logging

```cmd
cd receiver
idf.py -vv build
idf.py -p COM3 -vv flash
idf.py -p COM3 monitor -vv
```

The `-vv` flag enables verbose output.

### Use GDB Debugger

```cmd
cd receiver
idf.py -p COM3 gdb
```

Requires JTAG adapter (more advanced).

### Check Log Levels

In code, you can control logging:

```c
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG  // More verbose
ESP_LOGD(TAG, "Debug message");
ESP_LOGI(TAG, "Info message");
ESP_LOGW(TAG, "Warning message");
ESP_LOGE(TAG, "Error message");
```

Or set in sdkconfig:
```
CONFIG_LOG_DEFAULT_LEVEL=4  # DEBUG level
```

---

## Getting Help

If nothing above works:

1. **Check official documentation:**
   - ESP-IDF: https://docs.espressif.com/projects/esp-idf/
   - ESP-NOW: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/network/esp_now.html

2. **Check board documentation:**
   - Find your board's datasheet and schematic
   - Verify GPIO pins are correct
   - Verify LED polarity (if external)

3. **Collect debug information:**
   - Note exact error messages
   - Take photos of board and wiring
   - Note your board models and revisions
   - Note Windows version

4. **Try minimal example:**
   - Simplify your setup (e.g., just LED blink)
   - Add features one at a time
   - This helps isolate the problem

5. **ESP-IDF GitHub Issues:**
   - https://github.com/espressif/esp-idf/issues
   - Search for similar issues
   - Post detailed information if creating new issue
