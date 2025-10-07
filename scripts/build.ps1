# PowerShell build script for PhantomDB

Write-Host "Building PhantomDB..." -ForegroundColor Green

# Create build directory if it doesn't exist
if (!(Test-Path "build")) {
    Write-Host "Creating build directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Name "build" | Out-Null
}

# Change to build directory
Set-Location "build"

# Configure with CMake using the Visual Studio CMake
Write-Host "Configuring with CMake..." -ForegroundColor Yellow
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" ..

# Check if configuration was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

# Build the project
Write-Host "Building project..." -ForegroundColor Yellow
& "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build .

# Check if build was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Build completed successfully!" -ForegroundColor Green

# Run tests if requested
if ($args[0] -eq "test") {
    Write-Host "Running tests..." -ForegroundColor Yellow
    ctest --output-on-failure
}