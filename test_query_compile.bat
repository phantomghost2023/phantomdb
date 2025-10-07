@echo off
echo Compiling simple QueryProcessor test...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target simple_query_test
echo Running simple QueryProcessor test...
.\simple_query_test.exe
echo Test complete!
pause