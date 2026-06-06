# Quick Reference Guide

## Wrapper scripts
```powershell
.\flash_receiver.ps1 -Port COM3
.\flash_transmitter.ps1 -Port COM4
```

## PlatformIO CLI
```powershell
cd src\receiver
python -m platformio run -t upload --upload-port COM3

cd ..\transmitter
python -m platformio run -t upload --upload-port COM4
```

## Serial monitor
```powershell
python -m platformio device monitor --port COM3
```

## Notes
- Use `python -m platformio` if `pio` is not available.
- Each board is a separate PlatformIO project under `src/`.
