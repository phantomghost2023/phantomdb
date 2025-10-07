# Plugin module CMakeLists.txt
set(PLUGIN_SOURCES
    plugin_manager.cpp
    base_plugin.cpp
)

set(PLUGIN_HEADERS
    plugin_manager.h
    base_plugin.h
)

# Create the plugin library
add_library(plugin ${PLUGIN_SOURCES} ${PLUGIN_HEADERS})

# Link with core library and filesystem library
target_link_libraries(plugin core std::filesystem)

# Include directories
target_include_directories(plugin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Compiler-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(plugin PRIVATE _WIN32)
elseif(UNIX)
    # Unix-specific settings
    target_compile_definitions(plugin PRIVATE _UNIX)
endif()

# Installation rules
install(TARGETS plugin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${PLUGIN_HEADERS}
    DESTINATION include/plugin
)