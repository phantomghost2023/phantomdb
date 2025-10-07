#include "elastic_scaling_manager.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace phantomdb::distributed;

// Mock callback implementations for testing
void mockNodeAddedCallback(const NodeInfo& node) {
    std::cout << "Mock node added callback: " << node.id << " at " << node.address << ":" << node.port << std::endl;
}

void mockNodeRemovedCallback(const std::string& nodeId) {
    std::cout << "Mock node removed callback: " << nodeId << std::endl;
}

void mockNodeStatusCallback(const NodeStatus& status) {
    std::cout << "Mock node status callback: " << status.id << " - active: " << status.isActive << std::endl;
}

void mockScalingDecisionCallback(const ScalingDecision& decision) {
    std::cout << "Mock scaling decision callback: " << 
                ((decision.action == ScalingAction::SCALE_UP) ? "SCALE_UP" :
                 (decision.action == ScalingAction::SCALE_DOWN) ? "SCALE_DOWN" : "NO_ACTION") << 
                " - " << decision.reason << std::endl;
}

int main() {
    // No equivalent to spdlog::set_level in iostream, so we'll just output all messages
    
    try {
        std::cout << "Starting PhantomDB Elastic Scaling Test" << std::endl;
        
        // Create elastic scaling configuration
        ElasticScalingConfig config;
        config.rebalancingStrategy = RebalancingStrategy::CONSISTENT_HASHING;
        config.scalingPolicy = ScalingPolicy::AUTOMATIC;
        config.scalingTrigger.cpuThreshold = 70.0;
        config.scalingTrigger.memoryThreshold = 70.0;
        config.evaluationInterval = std::chrono::milliseconds(2000); // 2 seconds
        config.rebalancingThreshold = 0.15; // 15% imbalance threshold
        
        // Create elastic scaling manager
        ElasticScalingManager elasticScalingManager(config);
        if (!elasticScalingManager.initialize()) {
            std::cout << "Failed to initialize ElasticScalingManager" << std::endl;
            return 1;
        }
        
        // Register callbacks
        elasticScalingManager.registerNodeAddedCallback(mockNodeAddedCallback);
        elasticScalingManager.registerNodeRemovedCallback(mockNodeRemovedCallback);
        elasticScalingManager.registerNodeStatusCallback(mockNodeStatusCallback);
        elasticScalingManager.registerScalingDecisionCallback(mockScalingDecisionCallback);
        
        // Test 1: Add nodes to the cluster
        std::cout << "Testing node addition" << std::endl;
        
        elasticScalingManager.addNode("node1", "192.168.1.101", 8001);
        elasticScalingManager.addNode("node2", "192.168.1.102", 8002);
        elasticScalingManager.addNode("node3", "192.168.1.103", 8003);
        
        // Check cluster status
        std::cout << "Cluster size: " << elasticScalingManager.getClusterSize() << std::endl;
        std::cout << "Active cluster size: " << elasticScalingManager.getActiveClusterSize() << std::endl;
        
        // Test 2: Update resource metrics
        std::cout << "Testing resource metrics update" << std::endl;
        
        ResourceMetrics metrics1("node1");
        metrics1.cpuUsage = 45.0;
        metrics1.memoryUsage = 30.0;
        metrics1.diskUsage = 25.0;
        metrics1.queryRate = 500;
        metrics1.transactionRate = 50;
        elasticScalingManager.updateResourceMetrics(metrics1);
        
        ResourceMetrics metrics2("node2");
        metrics2.cpuUsage = 85.0; // High CPU usage
        metrics2.memoryUsage = 75.0; // High memory usage
        metrics2.diskUsage = 60.0;
        metrics2.queryRate = 1200; // High query rate
        metrics2.transactionRate = 120; // High transaction rate
        elasticScalingManager.updateResourceMetrics(metrics2);
        
        ResourceMetrics metrics3("node3");
        metrics3.cpuUsage = 30.0;
        metrics3.memoryUsage = 20.0;
        metrics3.diskUsage = 15.0;
        metrics3.queryRate = 200;
        metrics3.transactionRate = 20;
        elasticScalingManager.updateResourceMetrics(metrics3);
        
        // Test 3: Check node information
        std::cout << "Testing node information retrieval" << std::endl;
        
        auto node1 = elasticScalingManager.getNode("node1");
        if (node1) {
            std::cout << "Retrieved node: " << node1->id << " at " << node1->address << ":" << node1->port << std::endl;
        }
        
        auto allNodes = elasticScalingManager.getAllNodes();
        std::cout << "Total nodes: " << allNodes.size() << std::endl;
        
        auto activeNodes = elasticScalingManager.getActiveNodes();
        std::cout << "Active nodes: " << activeNodes.size() << std::endl;
        
        auto nodeStatus = elasticScalingManager.getNodeStatus("node1");
        std::cout << "Node1 status - active: " << nodeStatus.isActive << std::endl;
        
        auto allStatuses = elasticScalingManager.getAllNodeStatus();
        std::cout << "All node statuses: " << allStatuses.size() << std::endl;
        
        // Test 4: Trigger manual rebalancing
        std::cout << "Testing manual rebalancing" << std::endl;
        
        bool rebalanceSuccess = elasticScalingManager.triggerRebalancing();
        std::cout << "Manual rebalancing result: " << (rebalanceSuccess ? "SUCCESS" : "FAILED") << std::endl;
        
        // Check elastic scaling status
        auto scalingStatus = elasticScalingManager.getElasticScalingStatus();
        std::cout << "Elastic scaling status: " << static_cast<int>(scalingStatus) << std::endl;
        
        // Test 5: Remove a node
        std::cout << "Testing node removal" << std::endl;
        
        bool removeSuccess = elasticScalingManager.removeNode("node3");
        std::cout << "Node removal result: " << (removeSuccess ? "SUCCESS" : "FAILED") << std::endl;
        
        // Check updated cluster status
        std::cout << "Updated cluster size: " << elasticScalingManager.getClusterSize() << std::endl;
        std::cout << "Updated active cluster size: " << elasticScalingManager.getActiveClusterSize() << std::endl;
        
        // Wait a bit to let automatic scaling evaluation run
        std::cout << "Waiting for automatic scaling evaluation..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        std::cout << "Elastic Scaling Test completed successfully" << std::endl;
        
        // Shutdown elastic scaling manager
        elasticScalingManager.shutdown();
        
    } catch (const std::exception& e) {
        std::cout << "Exception in elastic scaling test: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}