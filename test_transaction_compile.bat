@echo off
echo Compiling simple TransactionManager test...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target simple_transaction_test
echo Running simple TransactionManager test...
.\simple_transaction_test.exe
echo Test complete!
pause