@echo off
setlocal enabledelayedexpansion

echo PhantomDB Release Script
echo =======================

:: Check if we're in the right directory
if not exist "CMakeLists.txt" (
    echo Error: CMakeLists.txt not found. Please run this script from the project root.
    exit /b 1
)

:: Get version from CMakeLists.txt
for /f "tokens=*" %%i in ('findstr "project(PhantomDB" CMakeLists.txt') do (
    set line=%%i
    echo !line! | findstr /C:"VERSION" >nul
    if !errorlevel! equ 0 (
        for /f "tokens=3 delims= " %%v in ("!line!") do (
            set VERSION=%%v
            goto :version_found
        )
    )
)

:version_found
echo Current version: %VERSION%

:: Show git status
echo.
echo Git status:
git status --porcelain

:: Check for uncommitted changes
git status --porcelain | findstr /V /C:"" >nul
if !errorlevel! equ 0 (
    echo.
    echo Warning: You have uncommitted changes. Continue? (y/N)
    set /p response=""
    if /i not "!response!"=="y" (
        echo Release cancelled.
        exit /b 1
    )
)

:: Create and push tag
echo.
echo Creating tag v%VERSION%...
git tag -a "v%VERSION%" -m "Release version %VERSION%"
git push origin "v%VERSION%"

echo.
echo Release v%VERSION% created and pushed successfully!

:: Create release notes
echo.
echo Release Notes for v%VERSION%
echo ==========================
echo.
echo ## What's New
echo.
echo ### Enhanced Features
echo - Schema enforcement for data operations
echo - Condition parsing and evaluation utilities
echo - Enhanced data validation with type checking
echo - File-based persistence for data durability
echo - JSON-based query format for simplified interactions
echo.
echo ### Improvements
echo - Refactored database implementation to store column definitions
echo - Improved error handling and validation messages
echo - Enhanced main demo application to showcase new features
echo - Updated documentation with examples for new functionality
echo.
echo ### Testing
echo - Comprehensive test suite for enhanced database functionality
echo - Persistence testing with file I/O operations
echo - Query executor testing with JSON-based queries
echo.
echo ## Getting Started
echo.
echo 1. Clone the repository:
echo    git clone https://github.com/your-org/phantomdb.git
echo.
echo 2. Build the project:
echo    cmake -DCMAKE_BUILD_TYPE=Release .
echo    cmake --build .
echo.
echo 3. Run the demo:
echo    .\Release\phantomdb.exe
echo.
echo For more details, see the README.md file.

pause