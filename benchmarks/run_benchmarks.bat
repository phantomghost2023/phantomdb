@echo off
REM Script to run all PhantomDB benchmarks (Windows)

echo Building PhantomDB with benchmarks enabled...

REM Create build directory
if not exist build_benchmarks mkdir build_benchmarks
cd build_benchmarks

REM Configure with benchmarks enabled
cmake -DBENCHMARKS=ON ..

REM Build benchmarks
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo Error: Build failed
    cd ..
    exit /b 1
)

echo Running benchmarks...

REM Create results directory
if not exist ..\benchmark_results mkdir ..\benchmark_results
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set timestamp=%dt:~0,8%_%dt:~8,6%
set results_dir=..\benchmark_results\benchmark_%timestamp%

if not exist %results_dir% mkdir %results_dir%

REM Run all benchmarks
echo Running core benchmarks...
benchmarks\Release\core_benchmarks.exe > %results_dir%\core_benchmarks.txt 2>&1

echo Running storage benchmarks...
benchmarks\Release\storage_benchmarks.exe > %results_dir%\storage_benchmarks.txt 2>&1

echo Running query benchmarks...
benchmarks\Release\query_benchmarks.exe > %results_dir%\query_benchmarks.txt 2>&1

echo Running transaction benchmarks...
benchmarks\Release\transaction_benchmarks.exe > %results_dir%\transaction_benchmarks.txt 2>&1

echo Running distributed benchmarks...
benchmarks\Release\distributed_benchmarks.exe > %results_dir%\distributed_benchmarks.txt 2>&1

REM Run all benchmarks together
echo Running all benchmarks...
benchmarks\Release\run_all_benchmarks.exe > %results_dir%\all_benchmarks.txt 2>&1

cd ..

echo.
echo Benchmark results saved to %results_dir%
echo Summary of results:
echo -------------------
for %%f in (%results_dir%\*.txt) do (
    echo === %%~nxf ===
    findstr /C:"Benchmark:" %results_dir%\%%~nxf | more
    echo.
)