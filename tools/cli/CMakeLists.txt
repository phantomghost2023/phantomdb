# PhantomDB CLI CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(PhantomDBCLI)

set(CLI_SOURCES
    phantomdb_cli.cpp
    main.cpp
)

set(CLI_HEADERS
    phantomdb_cli.h
)

# Create the CLI executable
add_executable(phantomdb-cli ${CLI_SOURCES} ${CLI_HEADERS})

# Link with core library
target_link_libraries(phantomdb-cli core)

# Include directories
target_include_directories(phantomdb-cli PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Compiler-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(phantomdb-cli PRIVATE _WIN32)
elseif(UNIX)
    # Unix-specific settings
    target_compile_definitions(phantomdb-cli PRIVATE _UNIX)
endif()

# CLI test executable
add_executable(cli_test cli_test.cpp)
target_link_libraries(cli_test phantomdb-cli)

# Installation rules
install(TARGETS phantomdb-cli
    RUNTIME DESTINATION bin
)