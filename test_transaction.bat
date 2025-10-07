@echo off
echo Building and testing Transaction Manager...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target transaction_test
echo Running Transaction Manager tests...
.\transaction_test.exe
echo Transaction Manager tests complete!
pause