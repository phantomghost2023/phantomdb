@echo off
echo Building and testing Isolation Manager...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target isolation_test
echo Running Isolation Manager tests...
.\isolation_test.exe
echo Isolation Manager tests complete!
pause