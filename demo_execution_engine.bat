@echo off
echo Phantom-DB Execution Engine Demo
echo =================================

echo.
echo This script demonstrates the execution engine functionality.
echo After building the project, you can run the execution engine tests:
echo.
echo   cd build
echo   .\src\query\Release\execution_engine_test.exe
echo   .\src\query\Release\simple_execution_test.exe
echo.
echo These tests will show:
echo  - Execution node creation and execution
echo  - Query processor integration with execution engine
echo  - Result propagation through execution trees
echo.
pause