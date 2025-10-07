#ifndef PHANTOMDB_LOAD_BALANCER_H
#define PHANTOMDB_LOAD_BALANCER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <random>
#include <atomic>

namespace phantomdb {
namespace distributed {

// Load balancing algorithms
enum class LoadBalancingAlgorithm {
    ROUND_ROBIN,
    RANDOM,
    LEAST_CONNECTIONS,
    WEIGHTED_ROUND_ROBIN
};

// Node statistics
struct NodeStatistics {
    std::string nodeId;
    int connectionCount;
    int cpuUsage;  // Percentage
    int memoryUsage;  // Percentage
    long long responseTimeAvg;  // Average response time in microseconds
    
    NodeStatistics(const std::string& id) 
        : nodeId(id), connectionCount(0), cpuUsage(0), memoryUsage(0), responseTimeAvg(0) {}
};

class LoadBalancer {
public:
    LoadBalancer(LoadBalancingAlgorithm algorithm = LoadBalancingAlgorithm::ROUND_ROBIN);
    ~LoadBalancer();
    
    // Initialize load balancer
    bool initialize();
    
    // Shutdown load balancer
    void shutdown();
    
    // Add a node to the load balancer
    bool addNode(const std::string& nodeId);
    
    // Remove a node from the load balancer
    bool removeNode(const std::string& nodeId);
    
    // Get next node based on load balancing algorithm
    std::string getNextNode(const std::vector<std::string>& availableNodes);
    
    // Update node statistics
    bool updateNodeStatistics(const std::string& nodeId, const NodeStatistics& stats);
    
    // Get node statistics
    std::shared_ptr<NodeStatistics> getNodeStatistics(const std::string& nodeId) const;
    
    // Mark node as healthy/unhealthy
    void setNodeHealth(const std::string& nodeId, bool isHealthy);
    
    // Check if node is healthy
    bool isNodeHealthy(const std::string& nodeId) const;
    
    // Set load balancing algorithm
    void setAlgorithm(LoadBalancingAlgorithm algorithm);
    
    // Get current load balancing algorithm
    LoadBalancingAlgorithm getAlgorithm() const;

private:
    // Load balancing algorithm
    LoadBalancingAlgorithm algorithm_;
    
    // Node statistics
    std::unordered_map<std::string, std::shared_ptr<NodeStatistics>> nodeStats_;
    
    // Node health status
    std::unordered_map<std::string, bool> nodeHealth_;
    
    // Mutex for thread safety
    mutable std::mutex statsMutex_;
    
    // Round-robin counter
    std::atomic<size_t> roundRobinCounter_;
    
    // Random number generator
    std::mt19937 randomGenerator_;
    
    // Flag to indicate if load balancer is initialized
    std::atomic<bool> initialized_;
    
    // Helper methods for different algorithms
    std::string roundRobinSelection(const std::vector<std::string>& availableNodes);
    std::string randomSelection(const std::vector<std::string>& availableNodes);
    std::string leastConnectionsSelection(const std::vector<std::string>& availableNodes);
    std::string weightedRoundRobinSelection(const std::vector<std::string>& availableNodes);
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_LOAD_BALANCER_H