# API module CMakeLists.txt

# Add subdirectory for observability
add_subdirectory(../observability observability)

# Define the API library
add_library(api
    rest_api.cpp
    database_manager.cpp
)

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

find_package(Poco QUIET COMPONENTS Net Util)
if(Poco_FOUND)
    target_link_libraries(api Poco::Net Poco::Util)
endif()

# Create executable for REST API server (only if dependencies are found)
if(nlohmann_json_FOUND AND spdlog_FOUND AND Poco_FOUND)
    add_executable(rest_server rest_server_main.cpp)
    target_link_libraries(rest_server api)
    
    # Create test executable
    add_executable(rest_api_test rest_api_test.cpp)
    target_link_libraries(rest_api_test api)
endif()