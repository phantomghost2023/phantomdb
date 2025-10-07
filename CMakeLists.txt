cmake_minimum_required(VERSION 3.10)
project(PhantomDB VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(MSVC)
    # Enable multi-processor compilation and treat warnings as errors
    add_compile_options(/W4 /WX)
else()
    # Enable all warnings and treat them as errors
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Add subdirectories
add_subdirectory(src/core)
add_subdirectory(src/storage)
add_subdirectory(src/query)
add_subdirectory(src/transaction)
add_subdirectory(src/api)
add_subdirectory(src/tools)
add_subdirectory(src/testing)
add_subdirectory(src/audit)
add_subdirectory(src/security)
add_subdirectory(src/plugin)
add_subdirectory(src/import_export)
add_subdirectory(src/distributed)

# Main executable
add_executable(phantomdb src/main.cpp)

# Link libraries
target_link_libraries(phantomdb 
    PRIVATE 
    core
    storage
    query
    transaction
    api
    audit
    security
    plugin
    import_export
    distributed
)

# Include directories
target_include_directories(phantomdb PRIVATE src)

# Install targets
install(TARGETS phantomdb
    RUNTIME DESTINATION bin
)

# Enable testing
enable_testing()

# Add test executables (only if GTest is available)
find_package(GTest QUIET)
if(GTest_FOUND)
    add_subdirectory(tests)
endif()