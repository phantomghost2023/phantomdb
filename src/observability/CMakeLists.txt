# Observability module CMakeLists.txt

# Define the observability library
add_library(observability
    observability.h
    observability.cpp
    init.h
    init.cpp
)

# Link required libraries (without nlohmann_json)
target_link_libraries(observability 
    # nlohmann_json::nlohmann_json  # Removed to avoid dependency
    # spdlog::spdlog  # Removed to avoid dependency
)

# Set include directories
target_include_directories(observability PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the library
install(TARGETS observability
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install header files
install(FILES 
    observability.h
    init.h
    DESTINATION include/phantomdb/observability
)