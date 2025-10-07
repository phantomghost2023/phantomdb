@echo off
echo Building and testing Parser and Planner...
cd /d d:\PhantomGhost\Storage\Media\Media\Projects\MyProjects\PhantomDB
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .
cd build
"C:\Program Files\CMake\bin\cmake.exe" --build . --target parser_test
"C:\Program Files\CMake\bin\cmake.exe" --build . --target planner_test
echo Running Parser tests...
.\parser_test.exe
echo Running Planner tests...
.\planner_test.exe
echo Parser and Planner tests complete!
pause