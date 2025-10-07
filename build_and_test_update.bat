@echo off
echo Building and testing UPDATE statement support...
echo =================================================

REM Create build directory
if not exist "build" mkdir build
cd build

REM Run CMake
cmake ..

REM Build the project
cmake --build . --config Release

REM Run the UPDATE tests
echo.
echo Running UPDATE parser test...
src\query\Release\update_test.exe

echo.
echo Running UPDATE plan test...
src\query\Release\update_plan_test.exe

echo.
echo Running UPDATE execution test...
src\query\Release\update_execution_test.exe

echo.
echo All UPDATE tests completed.
pause