@echo off
echo Testing DELETE statement implementation...

REM Try to compile the simple test
echo.
echo Compiling simple DELETE test...
cl /std:c++17 /EHsc /I. simple_delete_test.cpp src/query/sql_parser.cpp src/query/query_planner.cpp src/query/query_optimizer.cpp src/query/query_processor.cpp src/core/core.cpp src/storage/storage_engine.cpp src/storage/wal_manager.cpp src/storage/index_manager.cpp src/storage/garbage_collector.cpp src/transaction/transaction_manager.cpp /link /out:simple_delete_test.exe

if %errorlevel% neq 0 (
    echo.
    echo Compilation failed. Trying with g++...
    
    g++ -std=c++17 -I. simple_delete_test.cpp src/query/sql_parser.cpp src/query/query_planner.cpp src/query/query_optimizer.cpp src/query/query_processor.cpp src/core/core.cpp src/storage/storage_engine.cpp src/storage/wal_manager.cpp src/storage/index_manager.cpp src/storage/garbage_collector.cpp src/transaction/transaction_manager.cpp -o simple_delete_test.exe
    
    if %errorlevel% neq 0 (
        echo.
        echo Both compilation attempts failed.
        exit /b 1
    )
)

echo.
echo Running simple DELETE test...
simple_delete_test.exe

if %errorlevel% neq 0 (
    echo.
    echo Test failed.
    exit /b 1
)

echo.
echo Test completed successfully!