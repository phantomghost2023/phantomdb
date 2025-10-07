@echo off
echo Building and testing Query Optimizer...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target optimizer_test
echo Running Query Optimizer tests...
.\optimizer_test.exe
echo Query Optimizer tests complete!
pause