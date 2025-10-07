# Core module CMakeLists.txt

# Define the core library
set(CORE_SOURCES
    database.cpp
    utils.cpp
    persistence.cpp
    query_executor.cpp
)

add_library(core ${CORE_SOURCES})

# Link with required libraries
target_link_libraries(core)

# Include directories
target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)

# Try to find nlohmann_json package for query executor (make it optional)
find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
    target_link_libraries(core nlohmann_json::nlohmann_json)
    target_compile_definitions(core PRIVATE HAS_NLOHMANN_JSON=1)
else()
    # If nlohmann_json is not found, we'll disable JSON functionality
    target_compile_definitions(core PRIVATE HAS_NLOHMANN_JSON=0)
endif()

find_package(spdlog QUIET)
if(spdlog_FOUND)
    target_link_libraries(core spdlog::spdlog)
endif()

# Create test executables
add_executable(test_enhanced_database test_enhanced_database.cpp)
target_link_libraries(test_enhanced_database core)

add_executable(test_persistence test_persistence.cpp)
target_link_libraries(test_persistence core)

# Only build query executor test if json is available
if(nlohmann_json_FOUND)
    add_executable(test_query_executor test_query_executor.cpp)
    target_link_libraries(test_query_executor core)
endif()

add_executable(comprehensive_test comprehensive_test.cpp)
target_link_libraries(comprehensive_test core)

# Coverage test executable
add_executable(coverage_test coverage_test.cpp)
target_link_libraries(coverage_test core)