@echo off
echo Compiling simple Isolation Manager test...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target simple_isolation_test
echo Running simple Isolation Manager test...
.\simple_isolation_test.exe
echo Test complete!
pause