@echo off
echo Building and testing Concurrency Components...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target mvcc_test
"C:\Program Files\CMake\bin\cmake.exe" --build . --target lock_test
echo Running MVCC tests...
.\mvcc_test.exe
echo Running Lock Manager tests...
.\lock_test.exe
echo Concurrency Component tests complete!
pause