param(
    [string]$Port = ""
)

$projectDir = Join-Path $PSScriptRoot "src\transmitter"
Push-Location $projectDir
try {
    if ($Port) {
        Write-Host "Building and uploading transmitter to port $Port..."
        python -m platformio run -t upload --upload-port $Port
    } else {
        Write-Host "Building and uploading transmitter..."
        python -m platformio run -t upload
    }
} finally {
    Pop-Location
}
