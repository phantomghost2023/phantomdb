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
    btree.h
    hash_table.h
    lsm_tree.h
)

add_library(storage ${STORAGE_SOURCES} ${STORAGE_HEADERS})
target_link_libraries(storage core)

# WAL test executable
add_executable(wal_test wal_test.cpp)
target_link_libraries(wal_test storage core)

# Index test executable
add_executable(index_test index_test.cpp)
target_link_libraries(index_test storage core)

# Garbage collector test executable
add_executable(gc_test gc_test.cpp)
target_link_libraries(gc_test storage core)

# Integration test executable
add_executable(integration_test integration_test.cpp)
target_link_libraries(integration_test storage core)

# B-tree test executable
add_executable(btree_test btree_test.cpp)
target_link_libraries(btree_test storage core)

# Hash table test executable
add_executable(hash_table_test hash_table_test.cpp)
target_link_libraries(hash_table_test storage core)

# LSM-tree test executable
add_executable(lsm_tree_test lsm_tree_test.cpp)
target_link_libraries(lsm_tree_test storage core)

# LSM-tree index integration test executable
add_executable(lsm_tree_index_test lsm_tree_index_test.cpp)
target_link_libraries(lsm_tree_index_test storage core)

# Simple LSM-tree compilation test
add_executable(simple_lsm_test simple_lsm_test.cpp)
target_link_libraries(simple_lsm_test storage core)

# Index auto-indexing test executable
add_executable(index_auto_test index_auto_test.cpp)
target_link_libraries(index_auto_test storage core)