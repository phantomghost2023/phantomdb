# Audit module CMakeLists.txt
set(AUDIT_SOURCES
    audit_logger.cpp
)

set(AUDIT_HEADERS
    audit_logger.h
)

add_library(audit ${AUDIT_SOURCES} ${AUDIT_HEADERS})
target_link_libraries(audit core)

# Audit test executable
add_executable(audit_test audit_test.cpp)
target_link_libraries(audit_test audit core)

# Database audit integration test executable
add_executable(db_audit_test db_audit_test.cpp)
target_link_libraries(db_audit_test audit core)