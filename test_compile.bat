@echo off
echo Compiling simple LSM-tree test...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target simple_lsm_test
echo Running simple LSM-tree test...
.\simple_lsm_test.exe
echo Test complete!
pause