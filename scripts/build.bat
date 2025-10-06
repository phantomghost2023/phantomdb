@echo off
REM Build script for PhantomDB on Windows

echo Building PhantomDB...

REM Create build directory if it doesn't exist
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

REM Change to build directory
cd build

REM Install dependencies with Conan
echo Installing dependencies...
conan install .. --build=missing

REM Configure with CMake
echo Configuring with CMake...
cmake ..

REM Build the project
echo Building project...
cmake --build .

echo Build completed successfully!

REM Run tests if requested
if "%1"=="test" (
    echo Running tests...
    ctest --output-on-failure
)