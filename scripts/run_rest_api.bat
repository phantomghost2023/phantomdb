@echo off
REM Run script for PhantomDB REST API server

echo Starting PhantomDB REST API server...

REM Check if build directory exists
if not exist "build" (
    echo Build directory not found. Please run build_rest_api.bat first.
    exit /b 1
)

REM Change to build directory
cd build

REM Check if the executable exists
if not exist "src\api\Release\rest_server.exe" (
    if not exist "src\api\rest_server" (
        echo REST API server executable not found. Please run build_rest_api.bat first.
        exit /b 1
    )
)

REM Run the REST API server
echo Starting REST API server on port 8080...
echo Press Ctrl+C to stop the server.
echo.

if exist "src\api\Release\rest_server.exe" (
    src\api\Release\rest_server.exe
) else (
    src\api\rest_server
)