param(
    [string]$Port = ""
)

$envName = "esp32-c3"
$projectDir = Join-Path $PSScriptRoot "src\wled-transmitter"
$localConfigPath = Join-Path $projectDir "src\config.local.h"

if (-not (Test-Path $localConfigPath)) {
    throw "Missing local WLED configuration. Copy 'src\config.sample.h' to 'src\config.local.h' and set WIFI_SSID, WIFI_PASSWORD, and WLED_HOSTNAME before flashing."
}

Push-Location $projectDir
try {
    Write-Warning "Confirm the attached board is an ESP32-C3 before uploading."

    if ($Port) {
        Write-Host "Building and uploading WLED transmitter to port $Port (env: $envName)..."
        python -m platformio run -e $envName -t upload --upload-port $Port
    } else {
        Write-Host "Building and uploading WLED transmitter (env: $envName) using the detected port..."
        python -m platformio run -e $envName -t upload
    }

    if ($LASTEXITCODE -ne 0) {
        throw "WLED transmitter build or upload failed with exit code $LASTEXITCODE."
    }
} finally {
    Pop-Location
}
