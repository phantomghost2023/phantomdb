# PowerShell script to build PhantomDB reliably
Write-Host "Building PhantomDB reliably..." -ForegroundColor Green

# Remove existing build directory if it exists
if (Test-Path "build") {
    Write-Host "Removing existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
}

# Clean up any existing CMake cache files
Remove-Item -Force CMakeCache.txt -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force CMakeFiles -ErrorAction SilentlyContinue

# Configure with CMake using explicit paths
Write-Host "Configuring with CMake..." -ForegroundColor Yellow
$cmakePath = "C:\Program Files\CMake\bin\cmake.exe"
& $cmakePath -S . -B build -G "Visual Studio 17 2022"

# Check if configuration was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

Write-Host "CMake configuration completed successfully!" -ForegroundColor Green
Write-Host "You can now build specific targets using:" -ForegroundColor Yellow
Write-Host "  cd build" -ForegroundColor Yellow
Write-Host "  cmake --build . --target <target_name>" -ForegroundColor Yellow
Write-Host "Or build everything with:" -ForegroundColor Yellow
Write-Host "  cmake --build . --config Release" -ForegroundColor Yellow

Write-Host "Build setup completed successfully!" -ForegroundColor Green