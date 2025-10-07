# Tests CMakeLists.txt
# Add Google Test using FetchContent
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Core tests
add_executable(core_tests
    core_tests.cpp
)

target_link_libraries(core_tests 
    core
    storage
    query
    transaction
    GTest::gtest
    GTest::gtest_main
)

# Subquery parsing tests
add_executable(test_subquery_parsing
    test_subquery_parsing.cpp
)

target_link_libraries(test_subquery_parsing 
    query
    GTest::gtest
)

# Subquery planning tests
add_executable(test_subquery_planning
    test_subquery_planning.cpp
)

target_link_libraries(test_subquery_planning 
    query
    GTest::gtest
)

# Subquery execution tests
add_executable(test_subquery_execution
    test_subquery_execution.cpp
)

target_link_libraries(test_subquery_execution 
    query
    transaction
    GTest::gtest
)

# Comprehensive subquery functionality tests
add_executable(test_subquery_functionality
    test_subquery_functionality.cpp
)

target_link_libraries(test_subquery_functionality 
    query
    transaction
    GTest::gtest
)

# Add test to CTest
add_test(NAME core_tests COMMAND core_tests)
add_test(NAME test_subquery_parsing COMMAND test_subquery_parsing)
add_test(NAME test_subquery_planning COMMAND test_subquery_planning)
add_test(NAME test_subquery_execution COMMAND test_subquery_execution)
add_test(NAME test_subquery_functionality COMMAND test_subquery_functionality)