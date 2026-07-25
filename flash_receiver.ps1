param(
    [string]$Port = "",
    [int]$ReceiverId = 1
)

$projectDir = Join-Path $PSScriptRoot "src\receiver"
$localConfigPath = Join-Path $projectDir "src\ReceiverConfig.local.h"

Push-Location $projectDir
try {
    @"
#pragma once
#define RECEIVER_ID $ReceiverId
"@ | Set-Content -Path $localConfigPath -Encoding UTF8

    if ($Port) {
        Write-Host "Building and uploading receiver $ReceiverId to port $Port..."
        python -m platformio run -t upload --upload-port $Port
    } else {
        Write-Host "Building and attempting upload for receiver $ReceiverId without an explicit port..."
        python -m platformio run -t upload
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Upload did not complete automatically; falling back to a build-only run..."
            python -m platformio run
        }
    }
} finally {
    if (Test-Path $localConfigPath) {
        Remove-Item $localConfigPath -Force
    }

    Pop-Location
}
