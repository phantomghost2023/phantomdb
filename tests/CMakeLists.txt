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
    ${CONAN_LIBS}
    GTest::GTest
    GTest::Main
)

# Add test to CTest
add_test(NAME core_tests COMMAND core_tests)