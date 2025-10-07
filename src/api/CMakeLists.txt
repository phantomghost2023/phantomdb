# API module CMakeLists.txt

# Add subdirectory for observability
add_subdirectory(../observability observability)

# Define the API library
set(API_SOURCES
    database_manager.cpp
    simple_rest_api.cpp
)

# Only include rest_api.cpp if Poco is available
find_package(Poco QUIET COMPONENTS Net Util)
if(Poco_FOUND)
    list(APPEND API_SOURCES rest_api.cpp)
endif()

add_library(api ${API_SOURCES})

# Link with required libraries
target_link_libraries(api 
    core 
    storage 
    query 
    transaction 
    distributed
    observability
)

# Include directories
target_include_directories(api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)

# Find and link required packages (optional)
find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
    target_link_libraries(api nlohmann_json::nlohmann_json)
endif()

find_package(spdlog QUIET)
if(spdlog_FOUND)
    target_link_libraries(api spdlog::spdlog)
endif()

if(Poco_FOUND)
    target_link_libraries(api Poco::Net Poco::Util)
endif()

# Create executable for REST API server (only if dependencies are found)
# Make dependencies optional - use simple implementation if Poco is not available
if(nlohmann_json_FOUND AND spdlog_FOUND AND Poco_FOUND)
    add_executable(rest_server rest_server_main.cpp)
    target_link_libraries(rest_server api)
    
    # Create test executable
    add_executable(rest_api_test rest_api_test.cpp)
    target_link_libraries(rest_api_test api)
else()
    # Create a simple REST server executable without Poco dependencies
    add_executable(simple_rest_server simple_rest_server_main.cpp)
    target_link_libraries(simple_rest_server api)
endif()