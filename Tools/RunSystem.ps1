param(
    [int]$EmulatorCount = 25
)

$rootDir = Split-Path $PSScriptRoot -Parent
$buildDir = Join-Path $rootDir "Builds-Release"

$server = Join-Path $buildDir "Server.exe"
$emulator = Join-Path $buildDir "DeviceClient.exe"

Write-Host "Starting server..."
Start-Process $server

Write-Host "Starting $EmulatorCount emulator(s)..."

for ($i = 1; $i -le $EmulatorCount; $i++) {
    Write-Host "Starting emulator $i..."
    Start-Process $emulator
}

Write-Host "Done."

