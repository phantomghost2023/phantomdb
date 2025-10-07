#include "src/distributed/cluster_manager.h"
#include "src/distributed/node_discovery.h"
#include "src/distributed/membership_service.h"
#include "src/distributed/sharding_strategy.h"
#include "src/distributed/load_balancer.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::distributed;

int main() {
    std::cout << "Testing PhantomDB Distributed Components..." << std::endl;
    
    try {
        // Test ClusterManager
        std::cout << "Testing ClusterManager..." << std::endl;
        ClusterManager clusterManager;
        assert(clusterManager.initialize());
        assert(clusterManager.addNode("node1", "192.168.1.101", 8001));
        assert(clusterManager.addNode("node2", "192.168.1.102", 8002));
        assert(clusterManager.getClusterSize() == 2);
        assert(clusterManager.getActiveClusterSize() == 2);
        assert(clusterManager.isClusterHealthy());
        assert(clusterManager.removeNode("node2"));
        assert(clusterManager.getClusterSize() == 1);
        clusterManager.shutdown();
        std::cout << "ClusterManager tests passed!" << std::endl;
        
        // Test NodeDiscovery
        std::cout << "Testing NodeDiscovery..." << std::endl;
        NodeDiscovery nodeDiscovery;
        assert(nodeDiscovery.initialize());
        assert(nodeDiscovery.addNode("test_node", "192.168.1.100", 8000));
        assert(nodeDiscovery.removeNode("test_node"));
        nodeDiscovery.shutdown();
        std::cout << "NodeDiscovery tests passed!" << std::endl;
        
        // Test MembershipService
        std::cout << "Testing MembershipService..." << std::endl;
        MembershipService membershipService;
        assert(membershipService.initialize());
        assert(membershipService.addNode("member1"));
        assert(membershipService.addNode("member2"));
        assert(membershipService.getMemberCount() == 2);
        assert(membershipService.isMember("member1"));
        assert(membershipService.updateHeartbeat("member1"));
        assert(membershipService.removeNode("member2"));
        assert(membershipService.getMemberCount() == 1);
        membershipService.shutdown();
        std::cout << "MembershipService tests passed!" << std::endl;
        
        // Test ShardingStrategy
        std::cout << "Testing ShardingStrategy..." << std::endl;
        ShardingStrategy shardingStrategy;
        assert(shardingStrategy.initialize());
        std::vector<std::string> nodes = {"node1", "node2", "node3"};
        assert(shardingStrategy.createShards(nodes, 3));
        auto shards = shardingStrategy.getAllShards();
        assert(shards.size() == 3);
        shardingStrategy.shutdown();
        std::cout << "ShardingStrategy tests passed!" << std::endl;
        
        // Test LoadBalancer
        std::cout << "Testing LoadBalancer..." << std::endl;
        LoadBalancer loadBalancer;
        assert(loadBalancer.initialize());
        assert(loadBalancer.addNode("node1"));
        assert(loadBalancer.addNode("node2"));
        std::vector<std::string> availableNodes = {"node1", "node2"};
        std::string selectedNode = loadBalancer.getNextNode(availableNodes);
        assert(!selectedNode.empty());
        assert(loadBalancer.isNodeHealthy(selectedNode));
        loadBalancer.shutdown();
        std::cout << "LoadBalancer tests passed!" << std::endl;
        
        std::cout << "All distributed component tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception in test: " << e.what() << std::endl;
        return 1;
    }
}