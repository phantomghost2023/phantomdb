# Core module CMakeLists.txt
set(CORE_SOURCES
    core.cpp
    database.cpp
    utils.cpp
    persistence.cpp
    query_executor.cpp
)

set(CORE_HEADERS
    core.h
    database.h
    utils.h
    persistence.h
    query_executor.h
)

# Try to find nlohmann_json package
find_package(nlohmann_json QUIET)

if(nlohmann_json_FOUND)
    # If found, use the system package
    add_library(core ${CORE_SOURCES} ${CORE_HEADERS})
    target_link_libraries(core ${CONAN_LIBS} nlohmann_json::nlohmann_json)
else()
    # If not found, check for header-only version in include directory
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann/json.hpp)
        add_library(core ${CORE_SOURCES} ${CORE_HEADERS})
        target_include_directories(core PUBLIC include)
        target_link_libraries(core ${CONAN_LIBS})
    else()
        # Fallback: exclude query_executor if JSON library not available
        list(REMOVE_ITEM CORE_SOURCES query_executor.cpp)
        list(REMOVE_ITEM CORE_HEADERS query_executor.h)
        add_library(core ${CORE_SOURCES} ${CORE_HEADERS})
        target_link_libraries(core ${CONAN_LIBS})
    endif()
endif()

# Test executables
add_executable(test_enhanced_database test_enhanced_database.cpp)
target_link_libraries(test_enhanced_database core)

add_executable(test_persistence test_persistence.cpp)
target_link_libraries(test_persistence core)

# Only build query executor test if JSON library is available
if(nlohmann_json_FOUND OR EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann/json.hpp)
    add_executable(test_query_executor test_query_executor.cpp)
    target_link_libraries(test_query_executor core)
endif()

# Comprehensive test (always build)
add_executable(comprehensive_test comprehensive_test.cpp)
target_link_libraries(comprehensive_test core)