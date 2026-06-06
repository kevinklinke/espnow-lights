@echo off
REM Build and flash script for ESPNOW Demo
REM This script helps you build and flash both receiver and transmitter

setlocal enabledelayedexpansion

if "%1"=="" (
    echo Usage: build_and_flash.bat [command] [options]
    echo.
    echo Commands:
    echo   receiver-build          Build receiver code
    echo   receiver-flash [-p PORT]  Flash receiver (default port: COM3)
    echo   transmitter-build       Build transmitter code
    echo   transmitter-flash [-p PORT]  Flash transmitter (default port: COM4)
    echo   receiver-monitor [-p PORT]  Monitor receiver serial output
    echo   transmitter-monitor [-p PORT]  Monitor transmitter serial output
    echo   clean                   Clean build artifacts
    echo.
    echo Examples:
    echo   build_and_flash.bat receiver-build
    echo   build_and_flash.bat receiver-flash -p COM3
    echo   build_and_flash.bat transmitter-build
    echo   build_and_flash.bat transmitter-flash -p COM4
    exit /b 1
)

set COMMAND=%1

if "%COMMAND%"=="receiver-build" (
    echo Building receiver...
    cd receiver
    call idf.py build
    cd ..
    exit /b %ERRORLEVEL%
)

if "%COMMAND%"=="receiver-flash" (
    set PORT=COM3
    if "%2"=="-p" set PORT=%3
    echo Flashing receiver to %PORT%...
    cd receiver
    call idf.py -p %PORT% flash
    cd ..
    exit /b %ERRORLEVEL%
)

if "%COMMAND%"=="receiver-monitor" (
    set PORT=COM3
    if "%2"=="-p" set PORT=%3
    echo Monitoring receiver on %PORT%...
    cd receiver
    call idf.py -p %PORT% monitor
    cd ..
    exit /b %ERRORLEVEL%
)

if "%COMMAND%"=="transmitter-build" (
    echo Building transmitter...
    cd transmitter
    call idf.py build
    cd ..
    exit /b %ERRORLEVEL%
)

if "%COMMAND%"=="transmitter-flash" (
    set PORT=COM4
    if "%2"=="-p" set PORT=%3
    echo Flashing transmitter to %PORT%...
    cd transmitter
    call idf.py -p %PORT% flash
    cd ..
    exit /b %ERRORLEVEL%
)

if "%COMMAND%"=="transmitter-monitor" (
    set PORT=COM4
    if "%2"=="-p" set PORT=%3
    echo Monitoring transmitter on %PORT%...
    cd transmitter
    call idf.py -p %PORT% monitor
    cd ..
    exit /b %ERRORLEVEL%
)

if "%COMMAND%"=="clean" (
    echo Cleaning build artifacts...
    cd receiver
    call idf.py fullclean
    cd ..\transmitter
    call idf.py fullclean
    cd ..
    exit /b %ERRORLEVEL%
)

echo Unknown command: %COMMAND%
exit /b 1
