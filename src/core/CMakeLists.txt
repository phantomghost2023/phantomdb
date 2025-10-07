# CMakeLists.txt for PhantomDB Core Module

# Define the core library
add_library(core
    core.cpp
    database.cpp
    persistence.cpp
    enhanced_persistence.cpp
    utils.cpp
    query_executor.cpp
)

# Link dependencies
target_link_libraries(core PRIVATE storage query transaction)

# Include directories
target_include_directories(core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install targets
install(TARGETS core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Test for enhanced persistence
add_executable(test_enhanced_persistence test_enhanced_persistence.cpp)
target_link_libraries(test_enhanced_persistence core)

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