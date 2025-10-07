#include "replication_manager.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

namespace phantomdb {
namespace distributed {

ReplicationManager::ReplicationManager(ReplicationStrategy strategy)
    : strategy_(strategy), running_(false), 
      heartbeatInterval_(std::chrono::milliseconds(1000)) { // 1 second
    std::cout << "Creating ReplicationManager with strategy " << static_cast<int>(strategy) << std::endl;
}

ReplicationManager::~ReplicationManager() {
    if (running_) {
        shutdown();
    }
    std::cout << "Destroying ReplicationManager" << std::endl;
}

bool ReplicationManager::initialize() {
    std::cout << "Initializing ReplicationManager" << std::endl;
    
    running_ = true;
    replicationThread_ = std::make_unique<std::thread>(&ReplicationManager::replicationLoop, this);
    
    std::cout << "ReplicationManager initialized successfully" << std::endl;
    return true;
}

void ReplicationManager::shutdown() {
    std::cout << "Shutting down ReplicationManager" << std::endl;
    
    running_ = false;
    
    if (replicationThread_ && replicationThread_->joinable()) {
        replicationThread_->join();
        replicationThread_.reset();
    }
    
    // Disconnect from all regions
    {
        std::lock_guard<std::mutex> lock(replicationMutex_);
        for (const auto& pair : regions_) {
            disconnectFromRegion(pair.first);
        }
        regions_.clear();
        regionStatus_.clear();
    }
    
    std::cout << "ReplicationManager shutdown completed" << std::endl;
}

bool ReplicationManager::addRegion(const RegionInfo& region) {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    // Check if region already exists
    if (regions_.find(region.id) != regions_.end()) {
        std::cout << "Region " << region.id << " already exists" << std::endl;
        return false;
    }
    
    // Add region
    regions_[region.id] = region;
    regionStatus_[region.id] = ReplicationStatus(region.id);
    
    // If this is the first region and it's primary, set it as primary
    if (regions_.size() == 1 && region.isPrimary) {
        primaryRegion_ = region.id;
    }
    
    std::cout << "Added region " << region.id << " at " << region.address << ":" << region.port << std::endl;
    return true;
}

bool ReplicationManager::removeRegion(const std::string& regionId) {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    // Check if region exists
    auto it = regions_.find(regionId);
    if (it == regions_.end()) {
        std::cout << "Region " << regionId << " not found" << std::endl;
        return false;
    }
    
    // Remove region
    regions_.erase(it);
    regionStatus_.erase(regionId);
    
    // If this was the primary region, clear primary region
    if (primaryRegion_ == regionId) {
        primaryRegion_.clear();
    }
    
    std::cout << "Removed region " << regionId << std::endl;
    return true;
}

void ReplicationManager::setReplicationStrategy(ReplicationStrategy strategy) {
    strategy_.store(strategy);
    std::cout << "Replication strategy set to " << static_cast<int>(strategy) << std::endl;
}

ReplicationStrategy ReplicationManager::getReplicationStrategy() const {
    return strategy_.load();
}

bool ReplicationManager::replicateData(const std::string& key, const std::string& value) {
    switch (strategy_.load()) {
        case ReplicationStrategy::SYNCHRONOUS:
            return replicateSynchronously(key, value);
        case ReplicationStrategy::ASYNCHRONOUS:
            return replicateAsynchronously(key, value);
        case ReplicationStrategy::SEMI_SYNCHRONOUS:
            return replicateSemiSynchronously(key, value);
        default:
            return replicateAsynchronously(key, value);
    }
}

std::vector<ReplicationStatus> ReplicationManager::getReplicationStatus() const {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    std::vector<ReplicationStatus> status;
    status.reserve(regionStatus_.size());
    
    for (const auto& pair : regionStatus_) {
        status.push_back(pair.second);
    }
    
    return status;
}

std::vector<RegionInfo> ReplicationManager::getRegions() const {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    std::vector<RegionInfo> regions;
    regions.reserve(regions_.size());
    
    for (const auto& pair : regions_) {
        regions.push_back(pair.second);
    }
    
    return regions;
}

bool ReplicationManager::areAllRegionsConnected() const {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    for (const auto& pair : regionStatus_) {
        if (!pair.second.isConnected) {
            return false;
        }
    }
    
    return true;
}

std::string ReplicationManager::getPrimaryRegion() const {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    return primaryRegion_;
}

bool ReplicationManager::setPrimaryRegion(const std::string& regionId) {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    // Check if region exists
    if (regions_.find(regionId) == regions_.end()) {
        std::cout << "Region " << regionId << " not found" << std::endl;
        return false;
    }
    
    primaryRegion_ = regionId;
    std::cout << "Set primary region to " << regionId << std::endl;
    return true;
}

void ReplicationManager::replicationLoop() {
    std::cout << "Starting replication loop" << std::endl;
    
    while (running_) {
        try {
            // Send heartbeats to all regions
            sendHeartbeats();
            
            // Small delay to prevent busy waiting
            std::this_thread::sleep_for(heartbeatInterval_);
        } catch (const std::exception& e) {
            std::cout << "Exception in replication loop: " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Replication loop ended" << std::endl;
}

void ReplicationManager::sendHeartbeats() {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    for (auto& pair : regions_) {
        const std::string& regionId = pair.first;
        const RegionInfo& region = pair.second;
        
        // Simulate heartbeat
        bool connected = connectToRegion(region);
        
        // Update region status
        auto statusIt = regionStatus_.find(regionId);
        if (statusIt != regionStatus_.end()) {
            statusIt->second.isConnected = connected;
            statusIt->second.lastHeartbeat = std::chrono::steady_clock::now();
            if (!connected) {
                statusIt->second.errorMessage = "Connection failed";
            } else {
                statusIt->second.errorMessage.clear();
            }
        }
        
        if (connected) {
            std::cout << "Heartbeat sent to region " << regionId << std::endl;
        } else {
            std::cout << "Failed to send heartbeat to region " << regionId << std::endl;
        }
    }
}

bool ReplicationManager::replicateSynchronously(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    bool success = true;
    
    // Replicate to all regions and wait for acknowledgment
    for (const auto& pair : regions_) {
        const std::string& regionId = pair.first;
        const RegionInfo& region = pair.second;
        
        // Simulate synchronous replication
        bool replicated = connectToRegion(region);
        
        if (replicated) {
            // Update region status
            auto statusIt = regionStatus_.find(regionId);
            if (statusIt != regionStatus_.end()) {
                statusIt->second.lastReplicatedIndex++;
            }
            
            std::cout << "Synchronously replicated data to region " << regionId << std::endl;
        } else {
            std::cout << "Failed to synchronously replicate data to region " << regionId << std::endl;
            success = false;
        }
    }
    
    return success;
}

bool ReplicationManager::replicateAsynchronously(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    // Replicate to all regions without waiting for acknowledgment
    for (const auto& pair : regions_) {
        const std::string& regionId = pair.first;
        const RegionInfo& region = pair.second;
        
        // Simulate asynchronous replication (in a real implementation, this would be done in background threads)
        bool replicated = connectToRegion(region);
        
        if (replicated) {
            // Update region status
            auto statusIt = regionStatus_.find(regionId);
            if (statusIt != regionStatus_.end()) {
                statusIt->second.lastReplicatedIndex++;
            }
            
            std::cout << "Asynchronously replicated data to region " << regionId << std::endl;
        } else {
            std::cout << "Failed to asynchronously replicate data to region " << regionId << std::endl;
        }
    }
    
    return true; // Always return true for asynchronous replication
}

bool ReplicationManager::replicateSemiSynchronously(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    bool success = true;
    
    // Replicate to primary region synchronously
    if (!primaryRegion_.empty()) {
        auto primaryIt = regions_.find(primaryRegion_);
        if (primaryIt != regions_.end()) {
            const RegionInfo& primaryRegion = primaryIt->second;
            
            // Simulate synchronous replication to primary
            bool replicated = connectToRegion(primaryRegion);
            
            if (replicated) {
                // Update region status
                auto statusIt = regionStatus_.find(primaryRegion_);
                if (statusIt != regionStatus_.end()) {
                    statusIt->second.lastReplicatedIndex++;
                }
                
                std::cout << "Semi-synchronously replicated data to primary region " << primaryRegion_ << std::endl;
            } else {
                std::cout << "Failed to semi-synchronously replicate data to primary region " << primaryRegion_ << std::endl;
                success = false;
            }
        }
    }
    
    // Replicate to other regions asynchronously
    for (const auto& pair : regions_) {
        const std::string& regionId = pair.first;
        const RegionInfo& region = pair.second;
        
        // Skip primary region as it was already handled
        if (regionId == primaryRegion_) {
            continue;
        }
        
        // Simulate asynchronous replication
        bool replicated = connectToRegion(region);
        
        if (replicated) {
            // Update region status
            auto statusIt = regionStatus_.find(regionId);
            if (statusIt != regionStatus_.end()) {
                statusIt->second.lastReplicatedIndex++;
            }
            
            std::cout << "Asynchronously replicated data to region " << regionId << std::endl;
        } else {
            std::cout << "Failed to asynchronously replicate data to region " << regionId << std::endl;
        }
    }
    
    return success;
}

bool ReplicationManager::connectToRegion(const RegionInfo& region) {
    // Simulate connection to region
    // In a real implementation, this would establish a network connection
    
    // Simulate occasional connection failures
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1, 100);
    
    // 95% success rate
    return dis(gen) <= 95;
}

void ReplicationManager::disconnectFromRegion(const std::string& regionId) {
    // Simulate disconnection from region
    // In a real implementation, this would close the network connection
    
    std::cout << "Disconnected from region " << regionId << std::endl;
}

void ReplicationManager::updateRegionStatus(const std::string& regionId, bool connected, 
                                           uint64_t lastReplicatedIndex, 
                                           const std::string& errorMessage) {
    std::lock_guard<std::mutex> lock(replicationMutex_);
    
    auto statusIt = regionStatus_.find(regionId);
    if (statusIt != regionStatus_.end()) {
        statusIt->second.isConnected = connected;
        statusIt->second.lastReplicatedIndex = lastReplicatedIndex;
        statusIt->second.errorMessage = errorMessage;
    }
}

} // namespace distributed
} // namespace phantomdb