# Distributed module CMakeLists.txt

set(DISTRIBUTED_SOURCES
    cluster_manager.cpp
    node_discovery.cpp
    membership_service.cpp
    sharding_strategy.cpp
    load_balancer.cpp
    raft.cpp
    replication_manager.cpp
    conflict_resolver.cpp
    consensus_manager.cpp
    two_phase_commit.cpp
    saga.cpp
    cross_shard_query.cpp
    distributed_transaction_manager.cpp
    node_manager.cpp
    data_rebalancer.cpp
    resource_scaler.cpp
    elastic_scaling_manager.cpp
)

set(DISTRIBUTED_HEADERS
    cluster_manager.h
    node_discovery.h
    membership_service.h
    sharding_strategy.h
    load_balancer.h
    raft.h
    replication_manager.h
    conflict_resolver.h
    consensus_manager.h
    two_phase_commit.h
    saga.h
    cross_shard_query.h
    distributed_transaction_manager.h
    node_manager.h
    data_rebalancer.h
    resource_scaler.h
    elastic_scaling_manager.h
)

add_library(distributed ${DISTRIBUTED_SOURCES} ${DISTRIBUTED_HEADERS})

target_link_libraries(distributed core storage query transaction)

# Add any distributed-specific dependencies here
# target_link_libraries(distributed some_networking_library)

# Add test executable
add_executable(cluster_test cluster_test.cpp)
target_link_libraries(cluster_test distributed)

# Add consensus test executable
add_executable(consensus_test consensus_test.cpp)
target_link_libraries(consensus_test distributed)

# Add distributed transaction test executable
add_executable(distributed_transaction_test distributed_transaction_test.cpp)
target_link_libraries(distributed_transaction_test distributed)

# Add elastic scaling test executable
add_executable(elastic_scaling_test elastic_scaling_test.cpp)
target_link_libraries(elastic_scaling_test distributed)

# Add integration test executable
add_executable(distributed_integration_test integration_test.cpp)
target_link_libraries(distributed_integration_test distributed)

# Add comprehensive test executable
add_executable(distributed_comprehensive_test comprehensive_test.cpp)
target_link_libraries(distributed_comprehensive_test distributed)

# Add performance test executable
add_executable(distributed_performance_test performance_test.cpp)
target_link_libraries(distributed_performance_test distributed)