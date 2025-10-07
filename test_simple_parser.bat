@echo off
echo Compiling simple Parser and Planner test...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target simple_parser_test
echo Running simple Parser and Planner test...
.\simple_parser_test.exe
echo Test complete!
pause