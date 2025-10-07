@echo off
echo Compiling simple Concurrency Control test...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target simple_concurrency_test
echo Running simple Concurrency Control test...
.\simple_concurrency_test.exe
echo Test complete!
pause