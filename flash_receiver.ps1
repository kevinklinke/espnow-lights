param(
    [string]$Port = ""
)

$projectDir = Join-Path $PSScriptRoot "src\receiver"
Push-Location $projectDir
try {
    if ($Port) {
        Write-Host "Building and uploading receiver to port $Port..."
        python -m platformio run -t upload --upload-port $Port
    } else {
        Write-Host "Building and uploading receiver..."
        python -m platformio run -t upload
    }
} finally {
    Pop-Location
}
