param(
    [string]$Port = "",
    [string]$Env = "esp32-c3"
)

$projectDir = Join-Path $PSScriptRoot "src\transmitter"
Push-Location $projectDir
try {
    if ($Port) {
        Write-Host "Building and uploading transmitter to port $Port (env: $Env)..."
        python -m platformio run -e $Env -t upload --upload-port $Port
    } else {
        Write-Host "Building and attempting upload for transmitter (env: $Env) without an explicit port..."
        python -m platformio run -e $Env -t upload
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Upload did not complete automatically; falling back to a build-only run..."
            python -m platformio run -e $Env
        }
    }
} finally {
    Pop-Location
}
