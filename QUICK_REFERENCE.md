# Quick Reference Guide

## Using the Helper Scripts

### PowerShell Version (recommended for Windows)
```powershell
cd c:\Users\kevin\src\espnow-demo

# List available COM ports
.\build_and_flash.ps1 list-ports

# Build receiver
.\build_and_flash.ps1 receiver-build

# Flash receiver to COM3
.\build_and_flash.ps1 receiver-flash -p COM3

# Monitor receiver on COM3
.\build_and_flash.ps1 receiver-monitor -p COM3

# Build transmitter
.\build_and_flash.ps1 transmitter-build

# Flash transmitter to COM4
.\build_and_flash.ps1 transmitter-flash -p COM4

# Monitor transmitter on COM4
.\build_and_flash.ps1 transmitter-monitor -p COM4

# Clean all builds
.\build_and_flash.ps1 clean
```

### Batch File Version
```cmd
cd c:\Users\kevin\src\espnow-demo

build_and_flash.bat receiver-build
build_and_flash.bat receiver-flash -p COM3
build_and_flash.bat receiver-monitor -p COM3

build_and_flash.bat transmitter-build
build_and_flash.bat transmitter-flash -p COM4
build_and_flash.bat transmitter-monitor -p COM4

build_and_flash.bat clean
```

## Manual Commands

### ESP-IDF Direct Commands

**Build Receiver (ESP32 dev board):**
```cmd
cd receiver
idf.py set-target esp32
idf.py build
```

**Flash Receiver:**
```cmd
cd receiver
idf.py -p COM3 flash
```

**Monitor Receiver:**
```cmd
cd receiver
idf.py -p COM3 monitor
```

**Build + Flash + Monitor (One Command):**
```cmd
cd receiver
idf.py -p COM3 flash monitor
```

**Same for Transmitter (change to esp32 target):**
```cmd
cd transmitter
idf.py set-target esp32
idf.py build
idf.py -p COM4 flash monitor
```

## Common Debug Steps

### Step 1: Verify Serial Connection
```powershell
.\build_and_flash.ps1 list-ports
```

### Step 2: Check Receiver MAC Address
```cmd
cd receiver
idf.py -p COM3 monitor
```
Look for: `MAC Address: xx:xx:xx:xx:xx:xx`

### Step 3: Check Transmitter MAC Address
```cmd
cd transmitter
idf.py -p COM4 monitor
```
Look for: `MAC Address: xx:xx:xx:xx:xx:xx`

### Step 4: Test Communication
1. Flash receiver to COM3
2. Note its MAC address from serial output
3. Flash transmitter to COM4
4. Press button on transmitter
5. Watch receiver output for "Received message"

## Typical Full Workflow

```powershell
cd c:\Users\kevin\src\espnow-demo

# Step 1: Build receiver
.\build_and_flash.ps1 receiver-build

# Step 2: Flash receiver
.\build_and_flash.ps1 receiver-flash -p COM3

# Step 3: Build transmitter
.\build_and_flash.ps1 transmitter-build

# Step 4: Flash transmitter
.\build_and_flash.ps1 transmitter-flash -p COM4

# Step 5: In one PowerShell window, monitor receiver
.\build_and_flash.ps1 receiver-monitor -p COM3

# Step 6: In another PowerShell window, monitor transmitter
.\build_and_flash.ps1 transmitter-monitor -p COM4

# Step 7: Press the button and watch the logs!
```

## Changing COM Ports

If your devices are on different ports (e.g., COM7 and COM8):

```powershell
.\build_and_flash.ps1 receiver-flash -p COM7
.\build_and_flash.ps1 transmitter-flash -p COM8
.\build_and_flash.ps1 receiver-monitor -p COM7
.\build_and_flash.ps1 transmitter-monitor -p COM8
```

## Creating Aliases (Optional)

Make frequently used commands shorter in PowerShell:

```powershell
# Add to PowerShell profile
function rb { .\build_and_flash.ps1 receiver-build }
function rf { .\build_and_flash.ps1 receiver-flash -p COM3 }
function rm { .\build_and_flash.ps1 receiver-monitor -p COM3 }
function tb { .\build_and_flash.ps1 transmitter-build }
function tf { .\build_and_flash.ps1 transmitter-flash -p COM4 }
function tm { .\build_and_flash.ps1 transmitter-monitor -p COM4 }
```

Then you can just use: `rb`, `rf`, `rm`, `tb`, `tf`, `tm`

## Environment Variables

Set these if needed:

```cmd
# Set COM ports as environment variables
setx DEFAULT_RX_PORT COM3
setx DEFAULT_TX_PORT COM4

# Or in PowerShell
$env:DEFAULT_RX_PORT = "COM3"
$env:DEFAULT_TX_PORT = "COM4"
```

## Useful Log Messages to Watch For

### On Receiver:
```
ESP-NOW Receiver Initialized
MAC Address: xx:xx:xx:xx:xx:xx
Waiting for messages...
Received message from xx:xx:xx:xx:xx:xx - Type: 1, Seq: 0 - LED: ON
```

### On Transmitter:
```
ESP-NOW Transmitter Initialized
MAC Address: yy:yy:yy:yy:yy:yy
Button on GPIO 4 - Press to send message
Button pressed! Sent message #0
Data sent successfully to ff:ff:ff:ff:ff:ff
```

## Troubleshooting Quick Fixes

| Problem | Solution |
|---------|----------|
| Port not found | `.\build_and_flash.ps1 list-ports` to see available ports |
| Build fails | `.\build_and_flash.ps1 clean` then rebuild |
| Can't flash | Press EN button on board or try different USB port |
| No serial output | Check baud rate is 115200 and correct port is selected |
| Button doesn't work | Verify GPIO 4 to GND wiring |
| LED doesn't toggle | Verify GPIO 2 and check if logic is inverted |
| No communication | Verify both boards are powered and have compatible code |

## Expanding the Project

Once this works, check the files for modifications:

- `receiver/main/receiver.c` - LED toggling logic
- `transmitter/main/transmitter.c` - Button reading and sending logic
- Modify message structure to send more data
- Add error handling
- Implement proper state management
- Add multiple buttons or receivers

See [SETUP_GUIDE.md](SETUP_GUIDE.md) for more details.
