cmake_minimum_required(VERSION 3.15)
project(PhantomDB VERSION 0.1.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Coverage flags
option(COVERAGE "Enable code coverage reporting" OFF)
if(COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
    endif()
endif()

# Benchmark flags
option(BENCHMARKS "Enable benchmark suite" OFF)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4)
endif()

# Include directories
include_directories(src)

# Try to find nlohmann_json package for query executor
find_package(nlohmann_json QUIET)

# Add subdirectories
add_subdirectory(src/core)
add_subdirectory(src/storage)
add_subdirectory(src/query)
add_subdirectory(src/transaction)
add_subdirectory(src/distributed)
add_subdirectory(src/api)
add_subdirectory(src/clients)
add_subdirectory(src/tools)
add_subdirectory(src/security)
add_subdirectory(src/audit)
add_subdirectory(src/error)
add_subdirectory(src/plugin)
add_subdirectory(src/import_export)

# Add benchmarks if enabled
if(BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

# Main executable
add_executable(phantomdb src/main.cpp)
target_link_libraries(phantomdb core storage query transaction distributed api security audit error plugin import_export)