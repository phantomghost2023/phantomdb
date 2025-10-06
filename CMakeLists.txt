cmake_minimum_required(VERSION 3.15)
project(PhantomDB VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
endif()

# Include directories
include_directories(src)

# Add subdirectories
add_subdirectory(src/core)
add_subdirectory(src/storage)
# TODO: Add other modules as they are implemented
# add_subdirectory(src/query)
# add_subdirectory(src/transaction)
# add_subdirectory(src/distributed)
# add_subdirectory(src/tools)
# add_subdirectory(src/api)
# add_subdirectory(tests)

# Main executable
add_executable(phantomdb src/main.cpp)
target_link_libraries(phantomdb core storage)