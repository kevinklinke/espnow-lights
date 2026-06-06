# ESPNOW Hardware Connection Guide

## ESP32 Dev Board (Receiver)

### Required Components
- ESP32 dev board
- Micro USB cable
- (Optional) LED if you want to verify LED functionality separately

### Pin Configuration
- Built-in LED: **GPIO 2** on most ESP32 dev boards
- USB for programming: Standard Micro USB port

### Connection Setup
1. Connect Micro USB cable to the receiver ESP32
2. Connect the other end to your computer
3. The board will power up (you may see a tiny LED indicator)

### Verification
- Check Device Manager (Windows) for a COM port
- You should see "USB to UART Bridge" or similar

---

## ESP32 Dev Board (Transmitter)

### Required Components
- ESP32 Dev Board (typically 30-pin or 36-pin)
- Button (momentary switch)
- 10kΩ resistor (pull-up)
- Jumper wires
- Micro USB cable

### Pin Configuration
- Button Input: **GPIO 4**
- USB for programming: Standard Micro USB port
- Button will use internal pull-up (no external resistor needed)

### Button Wiring Diagram
```
                          ESP32 Dev Board
                          ┌──────────────────┐
                    3.3V  │ 3V3              │
                     ┌────┤ GND   GPIO 4     ├──────┐
                     │    └──────────────────┘      │
                     │                               │
                 [Resistor 10k]              [Button]
                     │                               │
                     └───────────────────────────────┘
                            GND

Alternative (without external resistor - uses internal pull-up):
                    GPIO 4  │  
                          [Button]
                            │
                           GND
```

### Connection Setup
1. **Method 1 (Recommended - Internal Pull-up)**:
   - Connect button between GPIO 4 and GND
   - The code enables the internal pull-up resistor
   
2. **Method 2 (External Pull-up)**:
   - Connect 10kΩ resistor between 3.3V and GPIO 4
   - Connect button between GPIO 4 and GND

3. Connect Micro USB cable for programming

### Verification
- Check Device Manager for a COM port
- You should see "USB to UART Bridge" or similar

---

## Finding COM Ports in Windows

### Method 1: Device Manager
1. Right-click **Start** → **Device Manager**
2. Look for **Ports (COM & LPT)**
3. Expand the section
4. You should see entries like "USB-SERIAL CH340" or "Silicon Labs CP210x"
5. Note the COM port number (e.g., COM3, COM4)

### Method 2: PowerShell
```powershell
# Run in PowerShell
Get-CimInstance Win32_PnPEntity | Where-Object {$_.Name -match 'COM\d+'} | Select-Object Name
```

### Method 3: Using the Helper Script
```powershell
.\build_and_flash.ps1 list-ports
```

### Method 4: CMD
```cmd
wmic logicaldisk get name
```

---

## Common Board Pinouts

### ESP32 Dev Board
- Built-in LED typically on GPIO 2
- Check your specific board's pinout
- Usually marked as "LED", "STATUS", or similar

### Standard ESP32 (30-pin)
```
        USB
        ┌──────────┐
    EN  │1  ....  30│GND
    IO0 │2  ESP32  29│IO25
   IO35 │3  DEV BD 28│IO26
   IO34 │4         27│IO27
   IO32 │5         26│IO14
   IO35 │6         25│IO12
   IO36 │7         24│IO13
    3V3 │8         23│IO9  ← Button here
    GND │9         22│IO10
    IO23│10        21│IO11
    IO22│11        20│D2
    IO1 │12        19│D3
    IO3 │13        18│CLK
    IO16│14        17│CMD
    IO17│15        16│D0
    IO5 │16        15│D1
    IO18│17        14│IO8
    IO19│18        13│IO7
    IO21│19        12│IO6
    GND │20        11│EN
        └──────────┘
```

---

## Troubleshooting

### Port Not Found
- Try different USB ports on your computer
- Try a different USB cable
- Check if drivers are installed (usually automatic on Windows 10/11)
- Restart your computer

### Board Not Responding
- Press the EN (Enable) button on the board
- Check if the correct board is selected in ESP-IDF: `idf.py set-target esp32`

### Button Not Working
- Check the wiring (should be GPIO 4 and GND)
- Verify the button is making contact
- Test with a continuity meter

### LED Not Lighting
- Verify GPIO 2 is correct for your ESP32 receiver board
- Check polarity if using external LED
- Built-in LED may be active-low (inverted logic)
