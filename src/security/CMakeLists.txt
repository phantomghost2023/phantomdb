# Security module CMakeLists.txt
set(SECURITY_SOURCES
    rbac.cpp
)

set(SECURITY_HEADERS
    rbac.h
)

add_library(security ${SECURITY_SOURCES} ${SECURITY_HEADERS})
target_link_libraries(security core)

# RBAC test executable
add_executable(rbac_test rbac_test.cpp)
target_link_libraries(rbac_test security core)

# Database RBAC integration test executable
add_executable(db_rbac_test db_rbac_test.cpp)
target_link_libraries(db_rbac_test security core)