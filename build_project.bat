@echo off
echo Building PhantomDB project...

REM Create build directory
if not exist "build" mkdir build
cd build

REM Run CMake
cmake ..

REM Build the project
cmake --build . --config Release

echo Build completed.
pause