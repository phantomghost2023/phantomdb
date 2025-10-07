#include "load_balancer.h"
#include <iostream>
#include <algorithm>
#include <limits>

namespace phantomdb {
namespace distributed {

LoadBalancer::LoadBalancer(LoadBalancingAlgorithm algorithm)
    : algorithm_(algorithm), roundRobinCounter_(0), 
      randomGenerator_(std::random_device{}()), initialized_(false) {
    std::cout << "Creating LoadBalancer with algorithm " << static_cast<int>(algorithm) << std::endl;
}

LoadBalancer::~LoadBalancer() {
    if (initialized_) {
        shutdown();
    }
    std::cout << "Destroying LoadBalancer" << std::endl;
}

bool LoadBalancer::initialize() {
    if (initialized_) {
        std::cout << "LoadBalancer already initialized" << std::endl;
        return true;
    }
    
    initialized_ = true;
    std::cout << "LoadBalancer initialized successfully" << std::endl;
    return true;
}

void LoadBalancer::shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "Shutting down LoadBalancer" << std::endl;
    
    // Clear statistics and health status
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        nodeStats_.clear();
        nodeHealth_.clear();
    }
    
    initialized_ = false;
    std::cout << "LoadBalancer shutdown completed" << std::endl;
}

bool LoadBalancer::addNode(const std::string& nodeId) {
    if (!initialized_) {
        std::cout << "LoadBalancer not initialized" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    // Check if node already exists
    if (nodeStats_.find(nodeId) != nodeStats_.end()) {
        std::cout << "Node " << nodeId << " already exists in load balancer" << std::endl;
        return false;
    }
    
    // Create node statistics
    auto stats = std::make_shared<NodeStatistics>(nodeId);
    nodeStats_[nodeId] = stats;
    
    // Mark node as healthy by default
    nodeHealth_[nodeId] = true;
    
    std::cout << "Node " << nodeId << " added to load balancer" << std::endl;
    return true;
}

bool LoadBalancer::removeNode(const std::string& nodeId) {
    if (!initialized_) {
        std::cout << "LoadBalancer not initialized" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    // Check if node exists
    if (nodeStats_.find(nodeId) == nodeStats_.end()) {
        std::cout << "Node " << nodeId << " not found in load balancer" << std::endl;
        return false;
    }
    
    // Remove node statistics
    nodeStats_.erase(nodeId);
    
    // Remove node health status
    nodeHealth_.erase(nodeId);
    
    std::cout << "Node " << nodeId << " removed from load balancer" << std::endl;
    return true;
}

std::string LoadBalancer::getNextNode(const std::vector<std::string>& availableNodes) {
    if (!initialized_) {
        return "";
    }
    
    if (availableNodes.empty()) {
        return "";
    }
    
    // Filter only healthy nodes
    std::vector<std::string> healthyNodes;
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        for (const auto& node : availableNodes) {
            auto it = nodeHealth_.find(node);
            if (it != nodeHealth_.end() && it->second) {
                healthyNodes.push_back(node);
            }
        }
    }
    
    if (healthyNodes.empty()) {
        return "";
    }
    
    // Apply load balancing algorithm
    switch (algorithm_) {
        case LoadBalancingAlgorithm::ROUND_ROBIN:
            return roundRobinSelection(healthyNodes);
        case LoadBalancingAlgorithm::RANDOM:
            return randomSelection(healthyNodes);
        case LoadBalancingAlgorithm::LEAST_CONNECTIONS:
            return leastConnectionsSelection(healthyNodes);
        case LoadBalancingAlgorithm::WEIGHTED_ROUND_ROBIN:
            return weightedRoundRobinSelection(healthyNodes);
        default:
            // Default to round-robin
            return roundRobinSelection(healthyNodes);
    }
}

bool LoadBalancer::updateNodeStatistics(const std::string& nodeId, const NodeStatistics& stats) {
    if (!initialized_) {
        std::cout << "LoadBalancer not initialized" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    auto it = nodeStats_.find(nodeId);
    if (it == nodeStats_.end()) {
        std::cout << "Node " << nodeId << " not found in load balancer" << std::endl;
        return false;
    }
    
    // Update statistics
    it->second->connectionCount = stats.connectionCount;
    it->second->cpuUsage = stats.cpuUsage;
    it->second->memoryUsage = stats.memoryUsage;
    it->second->responseTimeAvg = stats.responseTimeAvg;
    
    return true;
}

std::shared_ptr<NodeStatistics> LoadBalancer::getNodeStatistics(const std::string& nodeId) const {
    if (!initialized_) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    auto it = nodeStats_.find(nodeId);
    if (it != nodeStats_.end()) {
        return it->second;
    }
    
    return nullptr;
}

void LoadBalancer::setNodeHealth(const std::string& nodeId, bool isHealthy) {
    if (!initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    nodeHealth_[nodeId] = isHealthy;
    std::cout << "Node " << nodeId << " marked as " << (isHealthy ? "healthy" : "unhealthy") << std::endl;
}

bool LoadBalancer::isNodeHealthy(const std::string& nodeId) const {
    if (!initialized_) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    auto it = nodeHealth_.find(nodeId);
    if (it != nodeHealth_.end()) {
        return it->second;
    }
    
    return false;
}

void LoadBalancer::setAlgorithm(LoadBalancingAlgorithm algorithm) {
    algorithm_ = algorithm;
    std::cout << "Load balancing algorithm set to " << static_cast<int>(algorithm) << std::endl;
}

LoadBalancingAlgorithm LoadBalancer::getAlgorithm() const {
    return algorithm_;
}

std::string LoadBalancer::roundRobinSelection(const std::vector<std::string>& availableNodes) {
    size_t index = roundRobinCounter_.fetch_add(1) % availableNodes.size();
    return availableNodes[index];
}

std::string LoadBalancer::randomSelection(const std::vector<std::string>& availableNodes) {
    std::uniform_int_distribution<size_t> dis(0, availableNodes.size() - 1);
    size_t index = dis(randomGenerator_);
    return availableNodes[index];
}

std::string LoadBalancer::leastConnectionsSelection(const std::vector<std::string>& availableNodes) {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    std::string selectedNode = availableNodes[0];
    int minConnections = std::numeric_limits<int>::max();
    
    for (const auto& nodeId : availableNodes) {
        auto it = nodeStats_.find(nodeId);
        if (it != nodeStats_.end()) {
            int connections = it->second->connectionCount;
            if (connections < minConnections) {
                minConnections = connections;
                selectedNode = nodeId;
            }
        }
    }
    
    return selectedNode;
}

std::string LoadBalancer::weightedRoundRobinSelection(const std::vector<std::string>& availableNodes) {
    // Simplified weighted round-robin based on node performance
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    std::string selectedNode = availableNodes[0];
    long long bestScore = std::numeric_limits<long long>::max();
    
    for (const auto& nodeId : availableNodes) {
        auto it = nodeStats_.find(nodeId);
        if (it != nodeStats_.end()) {
            // Calculate score based on CPU usage, memory usage, and response time
            // Lower score is better
            long long score = it->second->cpuUsage + 
                             it->second->memoryUsage + 
                             (it->second->responseTimeAvg / 1000); // Convert to milliseconds
            
            if (score < bestScore) {
                bestScore = score;
                selectedNode = nodeId;
            }
        }
    }
    
    return selectedNode;
}

} // namespace distributed
} // namespace phantomdb