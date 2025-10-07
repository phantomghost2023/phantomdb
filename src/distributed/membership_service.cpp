#include "membership_service.h"
#include <iostream>
#include <algorithm>

namespace phantomdb {
namespace distributed {

MembershipService::MembershipService() 
    : heartbeatTimeoutSeconds_(30), initialized_(false) {
    std::cout << "Creating MembershipService instance" << std::endl;
}

MembershipService::~MembershipService() {
    if (initialized_) {
        shutdown();
    }
    std::cout << "Destroying MembershipService instance" << std::endl;
}

bool MembershipService::initialize() {
    if (initialized_) {
        std::cout << "MembershipService already initialized" << std::endl;
        return true;
    }
    
    // In a real implementation, we would initialize network components here
    initialized_ = true;
    std::cout << "MembershipService initialized successfully" << std::endl;
    return true;
}

void MembershipService::shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "Shutting down MembershipService" << std::endl;
    
    // Clear members and heartbeats
    {
        std::lock_guard<std::mutex> lock(membersMutex_);
        members_.clear();
        heartbeats_.clear();
    }
    
    initialized_ = false;
    std::cout << "MembershipService shutdown completed" << std::endl;
}

bool MembershipService::addNode(const std::string& nodeId) {
    if (!initialized_) {
        std::cout << "MembershipService not initialized" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    // Check if node already exists
    if (members_.find(nodeId) != members_.end()) {
        std::cout << "Node " << nodeId << " already exists in membership" << std::endl;
        return false;
    }
    
    // Add node to membership
    members_.insert(nodeId);
    
    // Initialize heartbeat
    heartbeats_[nodeId] = std::chrono::steady_clock::now();
    
    std::cout << "Node " << nodeId << " added to membership" << std::endl;
    return true;
}

bool MembershipService::removeNode(const std::string& nodeId) {
    if (!initialized_) {
        std::cout << "MembershipService not initialized" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    // Check if node exists
    if (members_.find(nodeId) == members_.end()) {
        std::cout << "Node " << nodeId << " not found in membership" << std::endl;
        return false;
    }
    
    // Remove node from membership
    members_.erase(nodeId);
    
    // Remove heartbeat record
    heartbeats_.erase(nodeId);
    
    std::cout << "Node " << nodeId << " removed from membership" << std::endl;
    return true;
}

bool MembershipService::isMember(const std::string& nodeId) const {
    if (!initialized_) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(membersMutex_);
    return members_.find(nodeId) != members_.end();
}

std::vector<std::string> MembershipService::getMembers() const {
    if (!initialized_) {
        return {};
    }
    
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    std::vector<std::string> result;
    result.reserve(members_.size());
    
    for (const auto& member : members_) {
        result.push_back(member);
    }
    
    return result;
}

size_t MembershipService::getMemberCount() const {
    if (!initialized_) {
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(membersMutex_);
    return members_.size();
}

bool MembershipService::updateHeartbeat(const std::string& nodeId) {
    if (!initialized_) {
        std::cout << "MembershipService not initialized" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    // Check if node exists
    if (members_.find(nodeId) == members_.end()) {
        std::cout << "Node " << nodeId << " not found in membership" << std::endl;
        return false;
    }
    
    // Update heartbeat
    heartbeats_[nodeId] = std::chrono::steady_clock::now();
    
    return true;
}

std::vector<std::string> MembershipService::detectFailedNodes(int timeoutSeconds) {
    if (!initialized_) {
        return {};
    }
    
    std::lock_guard<std::mutex> lock(membersMutex_);
    
    std::vector<std::string> failedNodes;
    auto now = std::chrono::steady_clock::now();
    auto threshold = std::chrono::seconds(timeoutSeconds);
    
    for (const auto& member : members_) {
        auto it = heartbeats_.find(member);
        if (it != heartbeats_.end()) {
            if ((now - it->second) > threshold) {
                failedNodes.push_back(member);
            }
        }
    }
    
    // Remove failed nodes from membership
    for (const auto& failedNode : failedNodes) {
        members_.erase(failedNode);
        heartbeats_.erase(failedNode);
        std::cout << "Node " << failedNode << " detected as failed and removed from membership" << std::endl;
    }
    
    return failedNodes;
}

void MembershipService::setHeartbeatTimeout(int timeoutSeconds) {
    if (timeoutSeconds > 0) {
        heartbeatTimeoutSeconds_ = timeoutSeconds;
        std::cout << "Heartbeat timeout set to " << timeoutSeconds << " seconds" << std::endl;
    }
}

int MembershipService::getHeartbeatTimeout() const {
    return heartbeatTimeoutSeconds_.load();
}

} // namespace distributed
} // namespace phantomdb