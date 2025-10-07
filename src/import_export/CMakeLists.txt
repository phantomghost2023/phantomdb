# Import/Export module CMakeLists.txt
set(IMPORT_EXPORT_SOURCES
    data_importer.cpp
    data_exporter.cpp
)

set(IMPORT_EXPORT_HEADERS
    data_importer.h
    data_exporter.h
)

# Create the import_export library
add_library(import_export ${IMPORT_EXPORT_SOURCES} ${IMPORT_EXPORT_HEADERS})

# Link with core library
target_link_libraries(import_export core)

# Include directories
target_include_directories(import_export PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Installation rules
install(TARGETS import_export
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${IMPORT_EXPORT_HEADERS}
    DESTINATION include/import_export
)