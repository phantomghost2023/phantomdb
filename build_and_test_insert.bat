@echo off
echo Building and testing INSERT statement support...
echo =================================================

REM Create build directory
if not exist "build" mkdir build
cd build

REM Run CMake
cmake ..

REM Build the project
cmake --build . --config Release

REM Run the INSERT tests
echo.
echo Running INSERT parser test...
src\query\Release\insert_test.exe

echo.
echo Running INSERT plan test...
src\query\Release\insert_plan_test.exe

echo.
echo Running INSERT execution test...
src\query\Release\insert_execution_test.exe

echo.
echo All INSERT tests completed.
pause