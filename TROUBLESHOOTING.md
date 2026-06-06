# Troubleshooting Guide for ESPNOW Lights

## Build and install issues

### `python` is not recognized
- Install Python 3.11 or later
- Add Python to PATH during installation
- Verify with:
  ```powershell
  python --version
  ```

### `platformio` is not recognized
- Use the CLI through Python:
  ```powershell
  python -m platformio --version
  ```
- If that works, use `python -m platformio` instead of `pio`

## Upload issues

### Upload fails
- Ensure the correct COM port is selected
- Try a different USB cable or USB port
- Close other programs using the serial port

### No serial output
- Confirm the board is powered
- Confirm the board is on the selected COM port
- Use the PlatformIO monitor command:
  ```powershell
  python -m platformio device monitor --port COM3
  ```

## Common commands

- `.\flash_receiver.ps1 -Port COM3`
- `.\flash_transmitter.ps1 -Port COM4`
- `python -m platformio run -t upload --upload-port COM3`
- `python -m platformio device monitor --port COM3`
