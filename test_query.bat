@echo off
echo Building and testing Query Processor...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target query_test
echo Running Query Processor tests...
.\query_test.exe
echo Query Processor tests complete!
pause