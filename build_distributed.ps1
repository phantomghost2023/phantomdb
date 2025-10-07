Write-Host "Building PhantomDB Distributed Components..."

# Create build directory if it doesn't exist
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}

# Change to build directory
Set-Location build

# Configure with CMake
cmake .. -G "Visual Studio 16 2019"

# Build the project
cmake --build . --config Release

Write-Host "Build completed!"

# Run the cluster test
Write-Host "Running cluster test..."
.\Release\cluster_test.exe

Set-Location ..