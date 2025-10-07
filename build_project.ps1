# PowerShell script to build PhantomDB
Write-Host "Building PhantomDB..." -ForegroundColor Green

# Remove existing build directory if it exists
if (Test-Path "build") {
    Write-Host "Removing existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force build
}

# Create build directory
Write-Host "Creating build directory..." -ForegroundColor Yellow
New-Item -ItemType Directory -Name "build" | Out-Null

# Change to build directory
Set-Location build

# Configure with CMake
Write-Host "Configuring with CMake..." -ForegroundColor Yellow
& "C:\Program Files\CMake\bin\cmake.exe" ..

# Check if configuration was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# Build the project
Write-Host "Building project..." -ForegroundColor Yellow
& "C:\Program Files\CMake\bin\cmake.exe" --build .

# Check if build was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host "Build completed successfully!" -ForegroundColor Green
Set-Location ..