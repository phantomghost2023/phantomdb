@echo off
REM PhantomDB Code Coverage Report Generator (Windows)
REM This script generates code coverage reports using OpenCppCoverage

echo Generating PhantomDB Code Coverage Report...

REM Check if OpenCppCoverage is available
where OpenCppCoverage.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: OpenCppCoverage.exe not found in PATH
    echo Please install OpenCppCoverage from https://github.com/OpenCppCoverage/OpenCppCoverage
    exit /b 1
)

REM Create build directory for coverage
if not exist coverage_build mkdir coverage_build
cd coverage_build

REM Configure with debug information
cmake -DCMAKE_BUILD_TYPE=Debug .. 
if %errorlevel% neq 0 (
    echo Error: CMake configuration failed
    cd ..
    exit /b 1
)

REM Build the project
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo Error: Build failed
    cd ..
    exit /b 1
)

REM Run tests with coverage analysis
echo Running tests with coverage analysis...
OpenCppCoverage.exe ^
    --sources ..\src ^
    --excluded_sources ..\src\tests ^
    --export_type html:..\coverage_report ^
    --export_type cobertura:..\coverage_report\cobertura.xml ^
    --cover_children ^
    -- .

if %errorlevel% neq 0 (
    echo Warning: Some tests may have failed during coverage analysis
)

cd ..

echo.
echo Coverage report generated in coverage_report directory
echo Open coverage_report\index.html in your browser to view the report