# Build and flash script for ESPNOW Demo
# PowerShell version for Windows

param(
    [Parameter(Position = 0)]
    [string]$Command,
    
    [Alias('p')]
    [Parameter(Position = 1)]
    [string]$Port,
    
    [Parameter(Position = 2)]
    [string]$Option1,
    
    [Parameter(Position = 3)]
    [string]$Option2
)

# Initialize ESP-IDF environment if not already done
if (-not (Get-Command idf.py -ErrorAction SilentlyContinue)) {
    $idfPath = $env:IDF_PATH
    if (-not $idfPath) {
        # Try common ESP-IDF installation paths
        $possiblePaths = @(
            "$env:USERPROFILE\.espressif\esp-idf",
            "C:\Espressif\esp-idf",
            "C:\esp-idf"
        )
        
        foreach ($path in $possiblePaths) {
            if (Test-Path "$path\export.ps1") {
                $idfPath = $path
                break
            }
        }
    }
    
    if ($idfPath -and (Test-Path "$idfPath\export.ps1")) {
        Write-Host "Initializing ESP-IDF environment from: $idfPath" -ForegroundColor Yellow
        & "$idfPath\export.ps1"
    } else {
        Write-Host "ERROR: Could not find ESP-IDF installation. Please set IDF_PATH environment variable." -ForegroundColor Red
        exit 1
    }
}

function Show-Help {
    Write-Host "Usage: .\build_and_flash.ps1 [command] [options]"
    Write-Host ""
    Write-Host "Commands:"
    Write-Host "  receiver-build              Build receiver code"
    Write-Host "  receiver-flash -p PORT      Flash receiver (default port: COM3)"
    Write-Host "  transmitter-build           Build transmitter code"
    Write-Host "  transmitter-flash -p PORT   Flash transmitter (default port: COM4)"
    Write-Host "  receiver-monitor -p PORT    Monitor receiver serial output"
    Write-Host "  transmitter-monitor -p PORT Monitor transmitter serial output"
    Write-Host "  clean                       Clean build artifacts"
    Write-Host "  list-ports                  List available COM ports"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build_and_flash.ps1 receiver-build"
    Write-Host "  .\build_and_flash.ps1 receiver-flash -p COM3"
    Write-Host "  .\build_and_flash.ps1 transmitter-build"
    Write-Host "  .\build_and_flash.ps1 transmitter-flash -p COM4"
    Write-Host "  .\build_and_flash.ps1 list-ports"
}

function Get-ComPort {
    param(
        [string]$DefaultPort
    )
    
    if ($Port) {
        return $Port
    }
    
    if ($Option1 -eq "-p" -and $Option2) {
        return $Option2
    }
    return $DefaultPort
}

function List-Ports {
    Write-Host "Available COM ports:"
    $ports = Get-CimInstance Win32_PnPEntity | Where-Object {$_.Name -match 'COM\d+'} | Select-Object -ExpandProperty Name
    if ($ports) {
        $ports | ForEach-Object { Write-Host "  $_" }
    } else {
        Write-Host "  No COM ports found"
    }
}

if ([string]::IsNullOrEmpty($Command)) {
    Show-Help
    exit 1
}

switch ($Command) {
    "receiver-build" {
        Write-Host "Building receiver..."
        Push-Location receiver
        & idf.py build
        $exitCode = $LASTEXITCODE
        Pop-Location
        exit $exitCode
    }
    
    "receiver-flash" {
        $port = Get-ComPort -DefaultPort "COM3"
        Write-Host "Flashing receiver to $port..."
        Push-Location receiver
        & idf.py -p $port flash
        $exitCode = $LASTEXITCODE
        Pop-Location
        exit $exitCode
    }
    
    "receiver-monitor" {
        $port = Get-ComPort -DefaultPort "COM3"
        Write-Host "Monitoring receiver on $port..."
        Push-Location receiver
        & idf.py -p $port monitor
        $exitCode = $LASTEXITCODE
        Pop-Location
        exit $exitCode
    }
    
    "transmitter-build" {
        Write-Host "Building transmitter..."
        Push-Location transmitter
        & idf.py build
        $exitCode = $LASTEXITCODE
        Pop-Location
        exit $exitCode
    }
    
    "transmitter-flash" {
        $port = Get-ComPort -DefaultPort "COM4"
        Write-Host "Flashing transmitter to $port..."
        Push-Location transmitter
        & idf.py -p $port flash
        $exitCode = $LASTEXITCODE
        Pop-Location
        exit $exitCode
    }
    
    "transmitter-monitor" {
        $port = Get-ComPort -DefaultPort "COM4"
        Write-Host "Monitoring transmitter on $port..."
        Push-Location transmitter
        & idf.py -p $port monitor
        $exitCode = $LASTEXITCODE
        Pop-Location
        exit $exitCode
    }
    
    "clean" {
        Write-Host "Cleaning build artifacts..."
        Push-Location receiver
        & idf.py fullclean
        Pop-Location
        Push-Location transmitter
        & idf.py fullclean
        Pop-Location
        exit 0
    }
    
    "list-ports" {
        List-Ports
        exit 0
    }
    
    default {
        Write-Host "Unknown command: $Command"
        Show-Help
        exit 1
    }
}
