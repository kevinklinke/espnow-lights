# Project Structure and Overview

## Directory Layout

```
espnow-demo/
├── receiver/                    # ESP32 receiver code
│   ├── main/
│   │   ├── receiver.c           # Main receiver code
│   │   └── CMakeLists.txt       # Component build config
│   ├── CMakeLists.txt           # Project build config
│   └── sdkconfig.defaults       # Default SDK settings
├── transmitter/                 # ESP32 transmitter code
│   ├── main/
│   │   ├── transmitter.c        # Main transmitter code
│   │   └── CMakeLists.txt       # Component build config
│   ├── CMakeLists.txt           # Project build config
│   └── sdkconfig.defaults       # Default SDK settings
├── build_and_flash.bat          # Windows batch helper script
├── build_and_flash.ps1          # Windows PowerShell helper script
├── README.md                    # Main project documentation
├── HARDWARE.md                  # Hardware setup and wiring
├── SETUP_GUIDE.md              # Step-by-step setup instructions
├── QUICK_REFERENCE.md          # Command quick reference
└── TROUBLESHOOTING.md          # Troubleshooting guide
```

## What Each File Does

### Source Code Files

#### `receiver/main/receiver.c`
- **Purpose:** Code that runs on ESP32 receiver board
- **Functionality:**
  - Initializes ESP-NOW receiver
  - Registers callback for incoming messages
  - Toggles LED (GPIO 2) when message received
  - Prints debug information to serial
- **Key Function:** `on_data_recv()` - called when data arrives

#### `transmitter/main/transmitter.c`
- **Purpose:** Code that runs on ESP32 dev board
- **Functionality:**
  - Initializes ESP-NOW transmitter
  - Monitors button (GPIO 4)
  - Sends message when button pressed
  - Handles send confirmations
  - Implements button debouncing
- **Key Function:** `button_task()` - monitors button and sends

### Build Configuration Files

#### `CMakeLists.txt` (root of each project)
- Configures the ESP-IDF build system
- Tells CMake how to build the project
- Used by: `idf.py build`

#### `main/CMakeLists.txt`
- Configures the main component
- Specifies source files and includes
- Used by the root CMakeLists.txt

#### `sdkconfig.defaults`
- Default ESP-IDF configuration
- Enables: WiFi, ESP-NOW, logging
- Applied when `idf.py menuconfig` is run

### Helper Scripts

#### `build_and_flash.ps1` (PowerShell - Recommended)
Commands:
- `receiver-build` - Compile receiver code
- `receiver-flash -p COM3` - Upload to board on COM3
- `receiver-monitor -p COM3` - Watch serial output
- `transmitter-build` - Compile transmitter code
- `transmitter-flash -p COM4` - Upload to board on COM4
- `transmitter-monitor -p COM4` - Watch serial output
- `list-ports` - Show available COM ports
- `clean` - Delete build files

#### `build_and_flash.bat` (Batch - Alternative)
Same commands as PowerShell version, for traditional CMD.exe

### Documentation Files

| File | Purpose |
|------|---------|
| `README.md` | Project overview and basic setup |
| `HARDWARE.md` | Wiring diagrams and hardware setup |
| `SETUP_GUIDE.md` | Detailed step-by-step instructions |
| `QUICK_REFERENCE.md` | Common commands and workflows |
| `TROUBLESHOOTING.md` | Solutions for common problems |

## Code Flow

### Receiver Flow

```
app_main()
├── Initialize NVS (non-volatile storage)
├── Initialize WiFi stack
├── Configure LED GPIO (GPIO 2)
├── Initialize ESP-NOW
├── Register receive callback → on_data_recv()
├── Get and print MAC address
├── Wait in loop
│   └── When data received:
│       ├── Parse message
│       ├── Toggle LED
│       └── Print info to serial
```

### Transmitter Flow

```
app_main()
├── Initialize NVS
├── Initialize WiFi stack
├── Configure button GPIO (GPIO 4) with pullup
├── Initialize ESP-NOW
├── Register send callback → on_data_sent()
├── Add peer (receiver)
├── Get and print MAC address
├── Create button monitoring task → button_task()
├── Wait in loop

button_task() runs in parallel:
├── Read button state
├── Detect button press (HIGH → LOW transition)
│   └── When pressed:
│       ├── Create message
│       ├── Send via ESP-NOW
│       └── Print confirmation to serial
├── Debounce delay (50ms)
└── Repeat
```

## Message Structure

```c
typedef struct {
    uint8_t type;      // Message type (1 = button press)
    uint32_t seq_num;  // Sequence number (counts up)
} message_t;
```

Size: 5 bytes

Can be expanded to include:
- Temperature/humidity
- Button count
- Battery voltage
- Timestamp
- etc.

## How ESP-NOW Works

```
┌─────────────────────┐
│   Transmitter       │
│   (ESP32)           │
│ ┌─────────────────┐ │
│ │  Button GPIO 4  │ │
│ └────────┬────────┘ │
│          │          │
│  ┌───────▼────────┐ │
│  │  ESP-NOW Stack │ │
│  └───────┬────────┘ │
│          │          │
│          │ Broadcast│
│          │ Packet   │
└──────────┼──────────┘
           │
    ┌──────▼──────────────────┐
    │ Wireless Channel (WiFi) │
    └──────┬──────────────────┘
           │
┌──────────▼──────────────────┐
│     Receiver                │
│     (ESP32)                 │
│  ┌───────────────────────┐  │
│  │  ESP-NOW Stack        │  │
│  └───────────────────────┘  │
│          │                  │
│  ┌───────▼──────────────┐   │
│  │  on_data_recv()      │   │
│  │  (Callback)          │   │
│  └───────────────────────┘   │
│          │                  │
│  ┌───────▼──────────────┐   │
│  │  LED GPIO 2 Toggle   │   │
│  └───────────────────────┘   │
└──────────────────────────────┘
```

## Communication Process

### Step 1: Initialization
1. Both boards start
2. WiFi stack initialized (no WiFi network needed)
3. ESP-NOW initialized
4. Transmitter adds receiver as peer
5. Receiver registers callback

### Step 2: Button Press
1. User presses button
2. GPIO 4 reads LOW (0 volts via GND)
3. Button task detects transition
4. Message created: `{type: 1, seq_num: N}`

### Step 3: Send
1. Transmitter sends message via ESP-NOW
2. Message broadcast to all listening peers
3. Send callback confirms delivery

### Step 4: Receive
1. Receiver's WiFi Hardware receives broadcast
2. Triggers receive callback
3. `on_data_recv()` is called
4. Message is parsed
5. LED state toggled
6. Output logged to serial

### Step 5: Acknowledge
- Receiver automatically sends ACK to transmitter
- Transmitter's callback confirms it worked

## GPIO Reference

### ESP32 Dev Board (Receiver)
- **LED (Built-in):** GPIO 2 on most ESP32 dev boards
- **Status:** Check your board's pinout

### ESP32 Dev Board (Transmitter)
- **Button Input:** GPIO 4
- **Pullup:** Internal (set in code)
- **Alternative Pins:** Can use 0, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33

## Known Limitations

1. **Range:** 
   - Typical: 100-250 meters
   - Can be affected by obstacles and interference

2. **Data Size:**
   - Maximum 250 bytes per message
   - Currently using ~5 bytes

3. **Power:**
   - Requires 3.3V
   - Current draw: ~50-150 mA active

4. **Performance:**
   - Latency: ~few milliseconds
   - Can send multiple messages per second

## Expanding the Project

### Add More Buttons
1. Add new GPIO definitions in transmitter.c
2. Add multiple message types
3. Create separate task for each button (or poll multiple)

### Add More Receivers
1. Get MAC address of each receiver
2. Add them as peers in transmitter
3. Broadcast to all or send to specific one

### Add Encryption
In transmitter.c:
```c
peerInfo.encrypt = true;  // Enable encryption
```

### Add Multi-Channel Communication
Set different channels for different pairs:
```c
peerInfo.channel = 2;  // Use channel 2 instead of 1
```

### Track Message History
Expand message structure:
```c
typedef struct {
    uint8_t type;
    uint32_t seq_num;
    uint32_t timestamp;
    int8_t rssi;  // Signal strength
} message_t;
```

## ESP-IDF Basics

### Project Structure
```
idf.py              ← Build system entrypoint
├── CMakeLists.txt  ← Root config
└── main/           ← Component folder
    ├── CMakeLists.txt
    └── *.c files
```

### Build System
- Uses CMake for configuration
- ESP-IDF provides helpers via `idf.py`
- Details in esp-idf documentation

### Common idf.py Commands
- `idf.py build` - Compile
- `idf.py flash` - Upload to board
- `idf.py monitor` - Watch serial
- `idf.py menuconfig` - Configure options
- `idf.py fullclean` - Delete all build files
- `idf.py set-target esp32` - Change target board

## Debugging Tips

1. **Serial Monitor is your friend**
   - Watch both boards simultaneously
   - Look for "Received message" on receiver
   - Look for "Sent message" on transmitter

2. **Check MAC addresses**
   - Printed at startup
   - Receiver: look for "Waiting for messages"
   - Transmitter: look for "Sending to"

3. **GPIO tests**
   - Toggle LED outside callback to test GPIO
   - Read button GPIO before debounce logic

4. **Message structure**
   - Keep simple at first
   - Gradually add fields
   - Print received fields to verify parsing

5. **Timing issues**
   - Add timestamps
   - Check if everything runs when expected
   - Use logs with timestamps enabled
