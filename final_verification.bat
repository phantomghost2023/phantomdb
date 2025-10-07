@echo off
echo ========================================
echo    PhantomDB Final Verification Script
echo ========================================
echo.

echo Checking project structure...
if exist "src\core" (
    echo ✅ Core module found
) else (
    echo ❌ Core module missing
)

if exist "src\storage" (
    echo ✅ Storage module found
) else (
    echo ❌ Storage module missing
)

if exist "src\query" (
    echo ✅ Query module found
) else (
    echo ❌ Query module missing
)

if exist "src\transaction" (
    echo ✅ Transaction module found
) else (
    echo ❌ Transaction module missing
)

if exist "src\distributed" (
    echo ✅ Distributed module found
) else (
    echo ❌ Distributed module missing
)

echo.
echo Checking build artifacts...

if exist "Release\phantomdb.exe" (
    echo ✅ Main executable found
) else (
    echo ❌ Main executable missing
)

if exist "simple_test.exe" (
    echo ✅ Simple test executable found
) else (
    echo ❌ Simple test executable missing
)

if exist "verify_phantomdb.exe" (
    echo ✅ Verification executable found
) else (
    echo ❌ Verification executable missing
)

echo.
echo Running verification test...
echo.

if exist "verify_phantomdb.exe" (
    echo Executing verify_phantomdb.exe...
    .\verify_phantomdb.exe
    if %ERRORLEVEL% EQU 0 (
        echo.
        echo ✅ PhantomDB verification PASSED
    ) else (
        echo.
        echo ❌ PhantomDB verification FAILED
    )
) else (
    echo ❌ Cannot run verification - executable not found
)

echo.
echo ========================================
echo    PhantomDB Verification Complete
echo ========================================
pause