@echo off
echo ========================================
echo      PhantomDB - Distributed Database
echo ========================================
echo.

if exist "Release\phantomdb.exe" (
    echo Starting PhantomDB...
    echo.
    "Release\phantomdb.exe"
) else (
    echo Error: PhantomDB executable not found!
    echo Please build the project first using CMake.
    echo.
    pause
    exit /b 1
)

echo.
echo PhantomDB execution completed.
pause