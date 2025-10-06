# Storage module CMakeLists.txt
set(STORAGE_SOURCES
    storage_engine.cpp
    wal_manager.cpp
    index_manager.cpp
    garbage_collector.cpp
)

set(STORAGE_HEADERS
    storage_engine.h
    wal_manager.h
    index_manager.h
    garbage_collector.h
)

add_library(storage ${STORAGE_SOURCES} ${STORAGE_HEADERS})
target_link_libraries(storage core ${CONAN_LIBS})