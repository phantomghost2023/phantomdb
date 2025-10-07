#include "node_manager.h"
#include <iostream>
#include <algorithm>
#include <thread>

namespace phantomdb {
namespace distributed {

NodeManager::NodeManager()
    : heartbeatTimeout_(std::chrono::milliseconds(30000)), // 30 seconds
      running_(false) {
    std::cout << "Creating NodeManager" << std::endl;
}

NodeManager::~NodeManager() {
    if (running_) {
        shutdown();
    }
    std::cout << "Destroying NodeManager" << std::endl;
}

bool NodeManager::initialize() {
    std::cout << "Initializing NodeManager" << std::endl;
    
    running_ = true;
    monitorThread_ = std::make_unique<std::thread>(&NodeManager::monitorNodes, this);
    
    std::cout << "NodeManager initialized successfully" << std::endl;
    return true;
}

void NodeManager::shutdown() {
    std::cout << "Shutting down NodeManager" << std::endl;
    
    running_ = false;
    
    if (monitorThread_ && monitorThread_->joinable()) {
        monitorThread_->join();
        monitorThread_.reset();
    }
    
    // Clear nodes and statuses
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        nodes_.clear();
        nodeStatuses_.clear();
    }
    
    std::cout << "NodeManager shutdown completed" << std::endl;
}

bool NodeManager::addNode(const std::string& nodeId, const std::string& address, int port) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    // Check if node already exists
    if (nodes_.find(nodeId) != nodes_.end()) {
        std::cout << "Node " << nodeId << " already exists" << std::endl;
        return false;
    }
    
    // Create new node
    auto node = std::make_shared<NodeInfo>(nodeId, address, port);
    nodes_[nodeId] = node;
    
    // Create node status
    nodeStatuses_[nodeId] = NodeStatus(nodeId);
    
    std::cout << "Added node " << nodeId << " at " << address << ":" << port << std::endl;
    
    // Notify callback
    if (nodeAddedCallback_) {
        nodeAddedCallback_(*node);
    }
    
    return true;
}

bool NodeManager::removeNode(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) {
        std::cout << "Node " << nodeId << " not found" << std::endl;
        return false;
    }
    
    // Remove node
    nodes_.erase(it);
    nodeStatuses_.erase(nodeId);
    
    std::cout << "Removed node " << nodeId << std::endl;
    
    // Notify callback
    if (nodeRemovedCallback_) {
        nodeRemovedCallback_(nodeId);
    }
    
    return true;
}

bool NodeManager::activateNode(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) {
        std::cout << "Node " << nodeId << " not found" << std::endl;
        return false;
    }
    
    it->second->isActive.store(true);
    std::cout << "Activated node " << nodeId << std::endl;
    
    // Update status
    auto statusIt = nodeStatuses_.find(nodeId);
    if (statusIt != nodeStatuses_.end()) {
        statusIt->second.isActive = true;
        notifyNodeStatus(nodeId);
    }
    
    return true;
}

bool NodeManager::deactivateNode(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) {
        std::cout << "Node " << nodeId << " not found" << std::endl;
        return false;
    }
    
    it->second->isActive.store(false);
    std::cout << "Deactivated node " << nodeId << std::endl;
    
    // Update status
    auto statusIt = nodeStatuses_.find(nodeId);
    if (statusIt != nodeStatuses_.end()) {
        statusIt->second.isActive = false;
        notifyNodeStatus(nodeId);
    }
    
    return true;
}

bool NodeManager::updateNodeHeartbeat(const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = nodes_.find(nodeId);
    if (it == nodes_.end()) {
        std::cout << "Node " << nodeId << " not found" << std::endl;
        return false;
    }
    
    it->second->lastHeartbeat = std::chrono::steady_clock::now();
    
    // Update status
    auto statusIt = nodeStatuses_.find(nodeId);
    if (statusIt != nodeStatuses_.end()) {
        statusIt->second.lastHeartbeat = it->second->lastHeartbeat;
        notifyNodeStatus(nodeId);
    }
    
    return true;
}

std::shared_ptr<NodeInfo> NodeManager::getNode(const std::string& nodeId) const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<NodeInfo>> NodeManager::getAllNodes() const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    std::vector<std::shared_ptr<NodeInfo>> result;
    result.reserve(nodes_.size());
    
    for (const auto& pair : nodes_) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::vector<std::shared_ptr<NodeInfo>> NodeManager::getActiveNodes() const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    std::vector<std::shared_ptr<NodeInfo>> result;
    
    for (const auto& pair : nodes_) {
        if (pair.second->isActive.load()) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

NodeStatus NodeManager::getNodeStatus(const std::string& nodeId) const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = nodeStatuses_.find(nodeId);
    if (it != nodeStatuses_.end()) {
        return it->second;
    }
    
    return NodeStatus(nodeId); // Return default status
}

std::vector<NodeStatus> NodeManager::getAllNodeStatus() const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    std::vector<NodeStatus> result;
    result.reserve(nodeStatuses_.size());
    
    for (const auto& pair : nodeStatuses_) {
        result.push_back(pair.second);
    }
    
    return result;
}

void NodeManager::registerNodeAddedCallback(const NodeAddedCallback& callback) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    nodeAddedCallback_ = callback;
    std::cout << "Node added callback registered" << std::endl;
}

void NodeManager::registerNodeRemovedCallback(const NodeRemovedCallback& callback) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    nodeRemovedCallback_ = callback;
    std::cout << "Node removed callback registered" << std::endl;
}

void NodeManager::registerNodeStatusCallback(const NodeStatusCallback& callback) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    nodeStatusCallback_ = callback;
    std::cout << "Node status callback registered" << std::endl;
}

void NodeManager::setHeartbeatTimeout(const std::chrono::milliseconds& timeout) {
    heartbeatTimeout_.store(timeout);
    std::cout << "Heartbeat timeout set to " << timeout.count() << " ms" << std::endl;
}

std::chrono::milliseconds NodeManager::getHeartbeatTimeout() const {
    return heartbeatTimeout_.load();
}

bool NodeManager::isClusterHealthy() const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    if (nodes_.empty()) {
        return false;
    }
    
    size_t activeCount = 0;
    for (const auto& pair : nodes_) {
        if (isNodeHealthy(*pair.second)) {
            activeCount++;
        }
    }
    
    // Cluster is healthy if more than half the nodes are healthy
    return activeCount > (nodes_.size() / 2);
}

size_t NodeManager::getClusterSize() const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    return nodes_.size();
}

size_t NodeManager::getActiveClusterSize() const {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    size_t activeCount = 0;
    for (const auto& pair : nodes_) {
        if (pair.second->isActive.load()) {
            activeCount++;
        }
    }
    
    return activeCount;
}

void NodeManager::monitorNodes() {
    std::cout << "Starting node monitoring thread" << std::endl;
    
    while (running_) {
        try {
            // Check node health
            {
                std::lock_guard<std::mutex> lock(managerMutex_);
                
                for (const auto& pair : nodes_) {
                    const auto& node = pair.second;
                    bool wasActive = node->isActive.load();
                    bool isHealthy = isNodeHealthy(*node);
                    
                    // Update node active status based on health
                    node->isActive.store(isHealthy);
                    
                    // If status changed, update and notify
                    if (wasActive != isHealthy) {
                        auto statusIt = nodeStatuses_.find(node->id);
                        if (statusIt != nodeStatuses_.end()) {
                            statusIt->second.isActive = isHealthy;
                            notifyNodeStatus(node->id);
                        }
                        
                        std::cout << "Node " << node->id << " status changed to " << 
                                    (isHealthy ? "healthy" : "unhealthy") << std::endl;
                    }
                }
            }
            
            // Sleep for a short interval
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } catch (const std::exception& e) {
            std::cout << "Exception in node monitoring thread: " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Node monitoring thread ended" << std::endl;
}

bool NodeManager::isNodeHealthy(const NodeInfo& node) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - node.lastHeartbeat);
    return elapsed <= heartbeatTimeout_.load();
}

void NodeManager::notifyNodeStatus(const std::string& nodeId) {
    auto statusIt = nodeStatuses_.find(nodeId);
    if (statusIt != nodeStatuses_.end() && nodeStatusCallback_) {
        nodeStatusCallback_(statusIt->second);
    }
}

} // namespace distributed
} // namespace phantomdb