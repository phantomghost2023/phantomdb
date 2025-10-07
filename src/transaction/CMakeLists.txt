# Transaction module CMakeLists.txt
set(TRANSACTION_SOURCES
    transaction_manager.cpp
    mvcc_manager.cpp
    lock_manager.cpp
    isolation_manager.cpp
)

set(TRANSACTION_HEADERS
    transaction_manager.h
    mvcc_manager.h
    lock_manager.h
    isolation_manager.h
)

add_library(transaction ${TRANSACTION_SOURCES} ${TRANSACTION_HEADERS})
target_link_libraries(transaction core)

# Transaction test executable
add_executable(transaction_test transaction_test.cpp)
target_link_libraries(transaction_test transaction core)

# MVCC test executable
add_executable(mvcc_test mvcc_test.cpp)
target_link_libraries(mvcc_test transaction core)

# Lock manager test executable
add_executable(lock_test lock_test.cpp)
target_link_libraries(lock_test transaction core)

# Isolation manager test executable
add_executable(isolation_test isolation_test.cpp)
target_link_libraries(isolation_test transaction core)

# Simple TransactionManager compilation test
add_executable(simple_transaction_test simple_transaction_test.cpp)
target_link_libraries(simple_transaction_test transaction core)

# Simple concurrency compilation test
add_executable(simple_concurrency_test simple_concurrency_test.cpp)
target_link_libraries(simple_concurrency_test transaction core)

# Simple isolation compilation test
add_executable(simple_isolation_test simple_isolation_test.cpp)
target_link_libraries(simple_isolation_test transaction core)