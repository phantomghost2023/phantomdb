@echo off
echo Building and testing DELETE statement support...

REM Create build directory if it doesn't exist
if not exist build mkdir build
cd build

REM Configure the project with CMake
cmake .. -G "Visual Studio 16 2019"

REM Build the project
cmake --build . --config Release

REM Run the DELETE tests
echo.
echo Running DELETE parser test...
Release\delete_test.exe

echo.
echo Running DELETE plan test...
Release\delete_plan_test.exe

echo.
echo Running DELETE execution test...
Release\delete_execution_test.exe

echo.
echo All DELETE tests completed!
cd ..