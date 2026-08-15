param(
    [string]$Port = "",
    [int]$ReceiverId = 1,
    [string]$Env = "esp32-c3",
    [int]$LedGpio = 0,
    [int]$DataPin = 0,
    [int]$PixelCount = 0,
    [int]$BarSpeed = 0
)

$projectDir = Join-Path $PSScriptRoot "src\receiver"
$localConfigPath = Join-Path $projectDir "src\ReceiverConfig.local.h"

Push-Location $projectDir
try {
    $configLines = @()
    $configLines += "#pragma once"
    $configLines += "#define RECEIVER_ID $ReceiverId"

    # If the user provided an explicit LED GPIO, include it. Otherwise, if targeting esp32dev, default to GPIO4.
    if ($LedGpio -ne 0) {
        $configLines += "#define LED_GPIO_PIN $LedGpio"
    } elseif ($Env -eq "esp32dev") {
        $configLines += "#define LED_GPIO_PIN 4"
    }

    # Allow configuring LED data pin (used by the LED library). If not provided, default to 5 for esp32dev.
    if ($DataPin -ne 0) {
        $configLines += "#define LED_DATA_PIN $DataPin"
    } elseif ($Env -eq "esp32dev") {
        $configLines += "#define LED_DATA_PIN 5"
    }

    # Allow configuring number of pixels on the strand
    if ($PixelCount -ne 0) {
        $configLines += "#define LED_PIXEL_COUNT $PixelCount"
    } elseif ($Env -eq "esp32dev") {
        # Default for this specific dev board
        $configLines += "#define LED_PIXEL_COUNT 300"
    }

    # Allow configuring default bar speed (ms per step). If non-zero, this will override runtime mapping.
    if ($BarSpeed -ne 0) {
        $configLines += "#define LED_BAR_SPEED_MS $BarSpeed"
    }

    $configLines -join "`n" | Set-Content -Path $localConfigPath -Encoding UTF8

    $envArg = ""
    if ($Env) { $envArg = "-e $Env" }

    if ($Port) {
        Write-Host "Building and uploading receiver $ReceiverId to port $Port (env: $Env)..."
        if ($Env) {
            python -m platformio run -e $Env -t upload --upload-port $Port
        } else {
            python -m platformio run -t upload --upload-port $Port
        }
    } else {
        Write-Host "Building and attempting upload for receiver $ReceiverId (env: $Env) without an explicit port..."
        if ($Env) {
            python -m platformio run -e $Env -t upload
        } else {
            python -m platformio run -t upload
        }
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Upload did not complete automatically; falling back to a build-only run..."
            if ($Env) {
                python -m platformio run -e $Env
            } else {
                python -m platformio run
            }
        }
    }
} finally {
    if (Test-Path $localConfigPath) {
        Remove-Item $localConfigPath -Force
    }

    Pop-Location
}
