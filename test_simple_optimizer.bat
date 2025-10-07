@echo off
echo Compiling simple Query Optimizer test...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target simple_optimizer_test
echo Running simple Query Optimizer test...
.\simple_optimizer_test.exe
echo Test complete!
pause