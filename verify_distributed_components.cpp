/*
 * Simple verification test for distributed components
 * This test verifies that all distributed components compile correctly
 * and can be instantiated without errors.
 */

#include "src/distributed/cluster_manager.h"
#include "src/distributed/node_discovery.h"
#include "src/distributed/membership_service.h"
#include "src/distributed/sharding_strategy.h"
#include "src/distributed/load_balancer.h"
#include <iostream>

using namespace phantomdb::distributed;

int main() {
    std::cout << "Verifying PhantomDB Distributed Components..." << std::endl;
    
    try {
        // Test ClusterManager instantiation
        ClusterManager clusterManager;
        std::cout << "✓ ClusterManager instantiated successfully" << std::endl;
        
        // Test NodeDiscovery instantiation
        NodeDiscovery nodeDiscovery;
        std::cout << "✓ NodeDiscovery instantiated successfully" << std::endl;
        
        // Test MembershipService instantiation
        MembershipService membershipService;
        std::cout << "✓ MembershipService instantiated successfully" << std::endl;
        
        // Test ShardingStrategy instantiation
        ShardingStrategy shardingStrategy;
        std::cout << "✓ ShardingStrategy instantiated successfully" << std::endl;
        
        // Test LoadBalancer instantiation
        LoadBalancer loadBalancer;
        std::cout << "✓ LoadBalancer instantiated successfully" << std::endl;
        
        std::cout << "All distributed components verified successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error verifying distributed components: " << e.what() << std::endl;
        return 1;
    }
}