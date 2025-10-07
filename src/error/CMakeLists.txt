# Error module CMakeLists.txt
set(ERROR_SOURCES
    error_handler.cpp
)

set(ERROR_HEADERS
    error_handler.h
)

# Create the error library
add_library(error ${ERROR_SOURCES} ${ERROR_HEADERS})

# Link with core library
target_link_libraries(error core)

# Include directories
target_include_directories(error PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Installation rules
install(TARGETS error
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${ERROR_HEADERS}
    DESTINATION include/error
)