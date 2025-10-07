# Core module CMakeLists.txt
set(CORE_SOURCES
    core.cpp
    database.cpp
)

set(CORE_HEADERS
    core.h
    database.h
)

add_library(core ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(core ${CONAN_LIBS})