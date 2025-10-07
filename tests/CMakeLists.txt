# Tests CMakeLists.txt
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Add Google Test
find_package(GTest REQUIRED)

# Core tests
add_executable(core_tests
    core_tests.cpp
)

target_link_libraries(core_tests 
    core
    storage
    query
    transaction
    ${CONAN_LIBS}
    GTest::GTest
    GTest::Main
)

# Subquery parsing tests
add_executable(test_subquery_parsing
    test_subquery_parsing.cpp
)

target_link_libraries(test_subquery_parsing 
    query
    ${CONAN_LIBS}
)

# Subquery planning tests
add_executable(test_subquery_planning
    test_subquery_planning.cpp
)

target_link_libraries(test_subquery_planning 
    query
    ${CONAN_LIBS}
)

# Subquery execution tests
add_executable(test_subquery_execution
    test_subquery_execution.cpp
)

target_link_libraries(test_subquery_execution 
    query
    transaction
    ${CONAN_LIBS}
)

# Comprehensive subquery functionality tests
add_executable(test_subquery_functionality
    test_subquery_functionality.cpp
)

target_link_libraries(test_subquery_functionality 
    query
    transaction
    ${CONAN_LIBS}
)

# Add test to CTest
add_test(NAME core_tests COMMAND core_tests)
add_test(NAME test_subquery_parsing COMMAND test_subquery_parsing)
add_test(NAME test_subquery_planning COMMAND test_subquery_planning)
add_test(NAME test_subquery_execution COMMAND test_subquery_execution)
add_test(NAME test_subquery_functionality COMMAND test_subquery_functionality)