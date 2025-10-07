#include "cluster_manager.h"
#include "node_discovery.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace phantomdb::distributed;

// Callback for node discovery events
void onNodeDiscovered(const std::string& nodeId, const std::string& address, int port) {
    std::cout << "Node discovered: " << nodeId << " at " << address << ":" << port << std::endl;
}

// Callback for node failure events
void onNodeFailed(const std::string& nodeId, const std::string& address, int port) {
    std::cout << "Node failed: " << nodeId << std::endl;
}

int main() {
    // No equivalent to spdlog::set_level in iostream, so we'll just output all messages
    
    try {
        std::cout << "Starting PhantomDB Distributed Cluster Test" << std::endl;
        
        // Create cluster manager
        ClusterManager clusterManager;
        if (!clusterManager.initialize()) {
            std::cout << "Failed to initialize ClusterManager" << std::endl;
            return 1;
        }
        
        // Add some nodes manually for testing
        clusterManager.addNode("node1", "192.168.1.101", 8001);
        clusterManager.addNode("node2", "192.168.1.102", 8002);
        clusterManager.addNode("node3", "192.168.1.103", 8003);
        
        // Test node retrieval
        auto node1 = clusterManager.getNode("node1");
        if (node1) {
            std::cout << "Retrieved node: " << node1->id << " at " << node1->address << ":" << node1->port << std::endl;
        }
        
        // Test getting all nodes
        auto allNodes = clusterManager.getAllNodes();
        std::cout << "Total nodes in cluster: " << allNodes.size() << std::endl;
        
        // Test getting active nodes
        auto activeNodes = clusterManager.getActiveNodes();
        std::cout << "Active nodes in cluster: " << activeNodes.size() << std::endl;
        
        // Test cluster health
        bool isHealthy = clusterManager.isClusterHealthy();
        std::cout << "Cluster health status: " << (isHealthy ? "HEALTHY" : "UNHEALTHY") << std::endl;
        
        // Test updating node heartbeat
        clusterManager.updateNodeHeartbeat("node1");
        clusterManager.updateNodeHeartbeat("node2");
        clusterManager.updateNodeHeartbeat("node3");
        
        // Test cluster size
        std::cout << "Cluster size: " << clusterManager.getClusterSize() << std::endl;
        std::cout << "Active cluster size: " << clusterManager.getActiveClusterSize() << std::endl;
        
        // Test removing a node
        clusterManager.removeNode("node3");
        std::cout << "After removal - Cluster size: " << clusterManager.getClusterSize() << std::endl;
        
        std::cout << "Distributed Cluster Test completed successfully" << std::endl;
        
        // Shutdown cluster manager
        clusterManager.shutdown();
        
    } catch (const std::exception& e) {
        std::cout << "Exception in cluster test: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}