@echo off
echo Verifying DELETE statement implementation...

REM Check if build directory exists, if not create it
if not exist "build" mkdir "build"
cd "build"

REM Try to build with CMake
echo.
echo Configuring build with CMake...
cmake .. -G "Visual Studio 16 2019"
if %errorlevel% neq 0 (
    echo.
    echo CMake configuration failed. Trying to compile verification script directly...
    
    REM Try to compile our verification script directly with g++
    cd ..
    echo.
    echo Compiling verification script directly...
    g++ -std=c++17 -I. verify_delete_implementation.cpp src/query/sql_parser.cpp src/query/query_planner.cpp src/query/execution_engine.cpp src/query/query_optimizer.cpp src/query/query_processor.cpp src/core/core.cpp src/transaction/transaction_manager.cpp src/storage/storage_engine.cpp src/storage/wal_manager.cpp src/storage/index_manager.cpp src/storage/garbage_collector.cpp -o verify_delete.exe
    
    if %errorlevel% neq 0 (
        echo.
        echo Direct compilation failed. Please check the source code for errors.
        exit /b 1
    )
    
    echo.
    echo Running verification...
    verify_delete.exe
    exit /b 0
)

REM If CMake succeeded, try to build
echo.
echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo.
    echo Build failed.
    exit /b 1
)

echo.
echo Running DELETE tests...
Release\delete_test.exe
if %errorlevel% neq 0 (
    echo.
    echo DELETE parser test failed.
    exit /b 1
)

Release\delete_plan_test.exe
if %errorlevel% neq 0 (
    echo.
    echo DELETE plan test failed.
    exit /b 1
)

Release\delete_execution_test.exe
if %errorlevel% neq 0 (
    echo.
    echo DELETE execution test failed.
    exit /b 1
)

echo.
echo All DELETE tests passed successfully!
cd ..