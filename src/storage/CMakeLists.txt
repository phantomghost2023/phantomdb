# CMakeLists.txt for PhantomDB Storage Module

# Define the storage library
add_library(storage
    storage_engine.cpp
    index_manager.cpp
    enhanced_index_manager.cpp
    btree.h
    hash_table.h
    lsm_tree.h
    wal_manager.cpp
    garbage_collector.cpp
)

# Link dependencies
target_link_libraries(storage PRIVATE core)

# Include directories
target_include_directories(storage PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install targets
install(TARGETS storage
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Tests
add_executable(index_test index_test.cpp)
target_link_libraries(index_test storage)

add_executable(btree_test btree_test.cpp)
target_link_libraries(btree_test storage)

add_executable(hash_table_test hash_table_test.cpp)
target_link_libraries(hash_table_test storage)

add_executable(lsm_tree_test lsm_tree_test.cpp)
target_link_libraries(lsm_tree_test storage)

add_executable(wal_test wal_test.cpp)
target_link_libraries(wal_test storage)

add_executable(gc_test gc_test.cpp)
target_link_libraries(gc_test storage)

add_executable(index_auto_test index_auto_test.cpp)
target_link_libraries(index_auto_test storage)

add_executable(lsm_tree_index_test lsm_tree_index_test.cpp)
target_link_libraries(lsm_tree_index_test storage)

add_executable(simple_lsm_test simple_lsm_test.cpp)
target_link_libraries(simple_lsm_test storage)

add_executable(integration_test integration_test.cpp)
target_link_libraries(integration_test storage core)

# Test for enhanced index manager
add_executable(test_enhanced_index_manager test_enhanced_index_manager.cpp)
target_link_libraries(test_enhanced_index_manager storage)