#include "cluster_manager.h"
#include "node_discovery.h"
#include "membership_service.h"
#include "sharding_strategy.h"
#include "load_balancer.h"
#include <iostream>
#include <algorithm>
#include <chrono>

namespace phantomdb {
namespace distributed {

ClusterManager::ClusterManager() : initialized_(false) {
}

ClusterManager::~ClusterManager() {
    shutdown();
}

bool ClusterManager::initialize() {
    std::cout << "Initializing ClusterManager" << std::endl;
    
    // Initialize node discovery
    nodeDiscovery_ = std::make_unique<NodeDiscovery>();
    if (!nodeDiscovery_->initialize()) {
        std::cout << "Failed to initialize NodeDiscovery" << std::endl;
        return false;
    }
    
    // Initialize membership service
    membershipService_ = std::make_unique<MembershipService>();
    if (!membershipService_->initialize()) {
        std::cout << "Failed to initialize MembershipService" << std::endl;
        return false;
    }
    
    // Initialize sharding strategy
    shardingStrategy_ = std::make_unique<ShardingStrategy>();
    if (!shardingStrategy_->initialize()) {
        std::cout << "Failed to initialize ShardingStrategy" << std::endl;
        return false;
    }
    
    // Initialize load balancer
    loadBalancer_ = std::make_unique<LoadBalancer>();
    if (!loadBalancer_->initialize()) {
        std::cout << "Failed to initialize LoadBalancer" << std::endl;
        return false;
    }
    
    initialized_ = true;
    std::cout << "ClusterManager initialized successfully" << std::endl;
    return true;
}

void ClusterManager::shutdown() {
    std::cout << "Shutting down ClusterManager" << std::endl;
    
    if (loadBalancer_) {
        loadBalancer_->shutdown();
    }
    
    if (shardingStrategy_) {
        shardingStrategy_->shutdown();
    }
    
    if (membershipService_) {
        membershipService_->shutdown();
    }
    
    if (nodeDiscovery_) {
        nodeDiscovery_->shutdown();
    }
    
    std::lock_guard<std::mutex> lock(nodesMutex_);
    nodes_.clear();
    initialized_ = false;
    std::cout << "ClusterManager shutdown complete" << std::endl;
}

bool ClusterManager::addNode(const std::string& nodeId, const std::string& address, int port) {
    if (!initialized_) {
        std::cout << "ClusterManager not initialized" << std::endl;
        return false;
    }
    
    std::cout << "Adding node " << nodeId << " at " << address << ":" << port << std::endl;
    
    // Add to node discovery
    if (!nodeDiscovery_->addNode(nodeId, address, port)) {
        std::cout << "Failed to add node to NodeDiscovery" << std::endl;
        return false;
    }
    
    // Add to membership service
    if (!membershipService_->addNode(nodeId)) {
        std::cout << "Failed to add node to MembershipService" << std::endl;
        return false;
    }
    
    // Add to load balancer
    if (!loadBalancer_->addNode(nodeId)) {
        std::cout << "Failed to add node to LoadBalancer" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(nodesMutex_);
    auto node = std::make_shared<NodeInfo>(nodeId, address, port);
    nodes_[nodeId] = node;
    
    return true;
}

bool ClusterManager::removeNode(const std::string& nodeId) {
    if (!initialized_) {
        std::cout << "ClusterManager not initialized" << std::endl;
        return false;
    }
    
    std::cout << "Removing node " << nodeId << std::endl;
    
    // Remove from node discovery
    nodeDiscovery_->removeNode(nodeId);
    
    // Remove from membership service
    membershipService_->removeNode(nodeId);
    
    // Remove from load balancer
    loadBalancer_->removeNode(nodeId);
    
    std::lock_guard<std::mutex> lock(nodesMutex_);
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end()) {
        nodes_.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<NodeInfo> ClusterManager::getNode(const std::string& nodeId) const {
    if (!initialized_) {
        std::cout << "ClusterManager not initialized" << std::endl;
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(nodesMutex_);
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<NodeInfo>> ClusterManager::getAllNodes() const {
    if (!initialized_) {
        std::cout << "ClusterManager not initialized" << std::endl;
        return {};
    }
    
    std::lock_guard<std::mutex> lock(nodesMutex_);
    std::vector<std::shared_ptr<NodeInfo>> nodes;
    for (const auto& pair : nodes_) {
        nodes.push_back(pair.second);
    }
    return nodes;
}

std::vector<std::shared_ptr<NodeInfo>> ClusterManager::getActiveNodes() const {
    if (!initialized_) {
        std::cout << "ClusterManager not initialized" << std::endl;
        return {};
    }
    
    std::lock_guard<std::mutex> lock(nodesMutex_);
    std::vector<std::shared_ptr<NodeInfo>> activeNodes;
    auto now = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(HEALTH_THRESHOLD_SECONDS);
    
    for (const auto& pair : nodes_) {
        auto node = pair.second;
        // Check if node is active based on last heartbeat
        if (node->is_active && (now - node->last_heartbeat) < timeout) {
            activeNodes.push_back(node);
        }
    }
    return activeNodes;
}

bool ClusterManager::isClusterHealthy() const {
    if (!initialized_) {
        return false;
    }
    
    auto activeNodes = getActiveNodes();
    return !activeNodes.empty();
}

bool ClusterManager::updateNodeHeartbeat(const std::string& nodeId) {
    if (!initialized_) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(nodesMutex_);
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end()) {
        it->second->last_heartbeat = std::chrono::steady_clock::now();
        it->second->is_active = true;
        return true;
    }
    return false;
}

size_t ClusterManager::getClusterSize() const {
    if (!initialized_) {
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(nodesMutex_);
    return nodes_.size();
}

size_t ClusterManager::getActiveClusterSize() const {
    if (!initialized_) {
        return 0;
    }
    
    return getActiveNodes().size();
}

} // namespace distributed
} // namespace phantomdb