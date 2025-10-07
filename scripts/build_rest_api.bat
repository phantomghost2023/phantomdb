@echo off
REM Build script for PhantomDB REST API server

echo Building PhantomDB REST API server...

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

REM Change to build directory
cd build

REM Run CMake
cmake .. -G "Visual Studio 16 2019"

REM Build the project
cmake --build . --config Release

echo.
echo Build complete!
echo.
echo To run the REST API server, execute:
echo   cd build
echo   src\api\Release\rest_server.exe
echo.
echo The server will start on port 8080 by default.
echo Press Ctrl+C to stop the server.