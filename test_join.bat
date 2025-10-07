@echo off
echo Testing JOIN clause implementation...

REM Create build directory if it doesn't exist
if not exist build mkdir build
cd build

REM Configure the project with CMake
cmake .. -G "Visual Studio 16 2019"

REM Build the project
cmake --build . --config Release

REM Run the JOIN test
echo.
echo Running JOIN parser test...
Release\join_test.exe

echo.
echo JOIN test completed!
cd ..