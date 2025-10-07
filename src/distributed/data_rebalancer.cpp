#include "data_rebalancer.h"
#include <iostream>
#include <functional>
#include <algorithm>
#include <numeric>
#include <random>

namespace phantomdb {
namespace distributed {

DataRebalancer::DataRebalancer(RebalancingStrategy strategy)
    : strategy_(strategy), rebalancingStatus_(RebalancingStatus::IDLE),
      rebalancingThreshold_(0.2) { // 20% imbalance threshold
    std::cout << "Creating DataRebalancer with strategy " << static_cast<int>(strategy) << std::endl;
}

DataRebalancer::~DataRebalancer() {
    std::cout << "Destroying DataRebalancer" << std::endl;
}

bool DataRebalancer::initialize() {
    std::cout << "Initializing DataRebalancer" << std::endl;
    
    // No background threads needed for data rebalancer
    std::cout << "DataRebalancer initialized successfully" << std::endl;
    return true;
}

void DataRebalancer::shutdown() {
    std::cout << "Shutting down DataRebalancer" << std::endl;
    
    // Clear shards and node infos
    {
        std::lock_guard<std::mutex> lock(rebalancerMutex_);
        shards_.clear();
        nodeInfos_.clear();
    }
    
    std::cout << "DataRebalancer shutdown completed" << std::endl;
}

bool DataRebalancer::createShards(const std::vector<std::string>& nodeIds, size_t shardCount) {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    
    if (nodeIds.empty() || shardCount == 0) {
        std::cout << "Invalid parameters for shard creation" << std::endl;
        return false;
    }
    
    // Clear existing shards
    shards_.clear();
    
    // Create shards and distribute nodes
    for (size_t i = 0; i < shardCount; i++) {
        std::string shardId = "shard_" + std::to_string(i);
        auto shardInfo = std::make_shared<ShardInfo>(shardId);
        
        // Distribute nodes across shards
        for (size_t j = 0; j < nodeIds.size(); j++) {
            if (j % shardCount == i) {
                shardInfo->nodes.push_back(nodeIds[j]);
            }
        }
        
        shards_[shardId] = shardInfo;
    }
    
    // Initialize node infos
    nodeInfos_.clear();
    for (const auto& nodeId : nodeIds) {
        nodeInfos_[nodeId] = RebalancingNodeInfo(nodeId);
    }
    
    std::cout << "Created " << shardCount << " shards distributed across " << nodeIds.size() << " nodes" << std::endl;
    return true;
}

bool DataRebalancer::rebalanceShards(const std::vector<std::string>& currentNodes) {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    
    if (currentNodes.empty()) {
        std::cout << "No nodes provided for rebalancing" << std::endl;
        return false;
    }
    
    std::cout << "Starting rebalancing for " << currentNodes.size() << " nodes" << std::endl;
    
    // Notify that rebalancing is starting
    rebalancingStatus_.store(RebalancingStatus::IN_PROGRESS);
    if (rebalanceStartCallback_) {
        rebalanceStartCallback_();
    }
    
    bool success = false;
    
    try {
        // Check if rebalancing is actually needed
        if (!needsRebalancing()) {
            std::cout << "No rebalancing needed, data distribution is balanced" << std::endl;
            success = true;
        } else {
            // Calculate moves needed for rebalancing
            auto moves = calculateRebalancingMoves();
            
            // Execute the moves
            success = executeRebalancingMoves(moves);
        }
        
        rebalancingStatus_.store(success ? RebalancingStatus::COMPLETED : RebalancingStatus::FAILED);
    } catch (const std::exception& e) {
        std::cout << "Exception during rebalancing: " << e.what() << std::endl;
        rebalancingStatus_.store(RebalancingStatus::FAILED);
        success = false;
    }
    
    // Notify that rebalancing is complete
    if (rebalanceCompleteCallback_) {
        rebalanceCompleteCallback_(success);
    }
    
    if (success) {
        std::cout << "Rebalancing completed successfully" << std::endl;
    } else {
        std::cout << "Rebalancing failed" << std::endl;
    }
    
    return success;
}

std::string DataRebalancer::getShardForKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    
    if (shards_.empty()) {
        return "";
    }
    
    switch (strategy_) {
        case RebalancingStrategy::CONSISTENT_HASHING:
            return consistentHashBasedSharding(key);
        case RebalancingStrategy::ROUND_ROBIN:
            return roundRobinSharding(key);
        case RebalancingStrategy::LOAD_BASED:
            return loadBasedSharding(key);
        default:
            return consistentHashBasedSharding(key);
    }
}

std::vector<std::string> DataRebalancer::getNodesForShard(const std::string& shardId) const {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    
    auto it = shards_.find(shardId);
    if (it != shards_.end()) {
        return it->second->nodes;
    }
    
    return {};
}

std::vector<std::shared_ptr<ShardInfo>> DataRebalancer::getAllShards() const {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    
    std::vector<std::shared_ptr<ShardInfo>> result;
    result.reserve(shards_.size());
    
    for (const auto& pair : shards_) {
        result.push_back(pair.second);
    }
    
    return result;
}

bool DataRebalancer::addNodeToShard(const std::string& shardId, const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    
    auto it = shards_.find(shardId);
    if (it == shards_.end()) {
        std::cout << "Shard " << shardId << " not found" << std::endl;
        return false;
    }
    
    // Check if node already exists in shard
    auto& nodes = it->second->nodes;
    if (std::find(nodes.begin(), nodes.end(), nodeId) != nodes.end()) {
        std::cout << "Node " << nodeId << " already exists in shard " << shardId << std::endl;
        return false;
    }
    
    // Add node to shard
    nodes.push_back(nodeId);
    std::cout << "Added node " << nodeId << " to shard " << shardId << std::endl;
    return true;
}

bool DataRebalancer::removeNodeFromShard(const std::string& shardId, const std::string& nodeId) {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    
    auto it = shards_.find(shardId);
    if (it == shards_.end()) {
        std::cout << "Shard " << shardId << " not found" << std::endl;
        return false;
    }
    
    // Find and remove node from shard
    auto& nodes = it->second->nodes;
    auto nodeIt = std::find(nodes.begin(), nodes.end(), nodeId);
    if (nodeIt == nodes.end()) {
        std::cout << "Node " << nodeId << " not found in shard " << shardId << std::endl;
        return false;
    }
    
    nodes.erase(nodeIt);
    std::cout << "Removed node " << nodeId << " from shard " << shardId << std::endl;
    return true;
}

bool DataRebalancer::updateShardDataSize(const std::string& shardId, size_t dataSize) {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    
    auto it = shards_.find(shardId);
    if (it == shards_.end()) {
        std::cout << "Shard " << shardId << " not found" << std::endl;
        return false;
    }
    
    it->second->dataSize = dataSize;
    it->second->lastRebalanced = std::chrono::steady_clock::now();
    
    std::cout << "Updated shard " << shardId << " data size to " << dataSize << std::endl;
    return true;
}

RebalancingStatus DataRebalancer::getRebalancingStatus() const {
    return rebalancingStatus_.load();
}

RebalancingStrategy DataRebalancer::getStrategy() const {
    return strategy_;
}

void DataRebalancer::registerRebalanceStartCallback(const RebalanceStartCallback& callback) {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    rebalanceStartCallback_ = callback;
    std::cout << "Rebalance start callback registered" << std::endl;
}

void DataRebalancer::registerRebalanceCompleteCallback(const RebalanceCompleteCallback& callback) {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    rebalanceCompleteCallback_ = callback;
    std::cout << "Rebalance complete callback registered" << std::endl;
}

void DataRebalancer::registerDataMoveCallback(const DataMoveCallback& callback) {
    std::lock_guard<std::mutex> lock(rebalancerMutex_);
    dataMoveCallback_ = callback;
    std::cout << "Data move callback registered" << std::endl;
}

void DataRebalancer::setRebalancingThreshold(double threshold) {
    if (threshold > 0 && threshold <= 1.0) {
        rebalancingThreshold_.store(threshold);
        std::cout << "Rebalancing threshold set to " << threshold << std::endl;
    }
}

double DataRebalancer::getRebalancingThreshold() const {
    return rebalancingThreshold_.load();
}

std::string DataRebalancer::consistentHashBasedSharding(const std::string& key) const {
    // Simple consistent hash-based sharding
    std::hash<std::string> hasher;
    size_t hashValue = hasher(key);
    
    if (shards_.empty()) {
        return "";
    }
    
    // Distribute based on hash modulo shard count
    size_t shardIndex = hashValue % shards_.size();
    
    // Find the shard at this index
    auto it = shards_.begin();
    std::advance(it, shardIndex);
    
    return it->first;
}

std::string DataRebalancer::roundRobinSharding(const std::string& key) const {
    // Simple round-robin sharding
    if (shards_.empty()) {
        return "";
    }
    
    // Use hash of key to determine starting point
    std::hash<std::string> hasher;
    size_t hashValue = hasher(key);
    
    // Distribute based on hash modulo shard count
    size_t shardIndex = hashValue % shards_.size();
    
    // Find the shard at this index
    auto it = shards_.begin();
    std::advance(it, shardIndex);
    
    return it->first;
}

std::string DataRebalancer::loadBasedSharding(const std::string& key) const {
    // Load-based sharding - distribute to shard with least data
    if (shards_.empty()) {
        return "";
    }
    
    // Find shard with minimum data size
    auto minIt = std::min_element(shards_.begin(), shards_.end(),
        [](const auto& a, const auto& b) {
            return a.second->dataSize < b.second->dataSize;
        });
    
    return minIt->first;
}

bool DataRebalancer::needsRebalancing() const {
    if (shards_.empty()) {
        return false;
    }
    
    double avgSize = getAverageDataSizePerShard();
    double maxDeviation = getMaxDataSizeDeviation();
    
    // Need rebalancing if max deviation exceeds threshold
    return (maxDeviation / avgSize) > rebalancingThreshold_.load();
}

std::vector<std::pair<std::string, std::string>> DataRebalancer::calculateRebalancingMoves() const {
    std::vector<std::pair<std::string, std::string>> moves;
    
    // This is a simplified implementation
    // In a real system, this would be much more complex
    
    double avgSize = getAverageDataSizePerShard();
    
    // Find shards that are significantly over the average
    for (const auto& pair : shards_) {
        const auto& shard = pair.second;
        if (shard->dataSize > avgSize * (1 + rebalancingThreshold_.load())) {
            // This shard needs to move some data
            // For simplicity, we'll just note that this shard needs rebalancing
            // In a real implementation, we would calculate specific moves
            moves.emplace_back(shard->id, "needs_rebalancing");
        }
    }
    
    return moves;
}

bool DataRebalancer::executeRebalancingMoves(const std::vector<std::pair<std::string, std::string>>& moves) {
    bool success = true;
    
    for (const auto& move : moves) {
        const std::string& shardId = move.first;
        const std::string& action = move.second;
        
        // In a real implementation, this would execute actual data moves
        // For now, we'll just simulate success
        
        std::cout << "Executing rebalancing move for shard " << shardId << ": " << action << std::endl;
        
        if (dataMoveCallback_) {
            // In a real implementation, we would have specific fromNode and toNode
            // For now, we'll use dummy values
            if (!dataMoveCallback_(shardId, "source_node", "target_node")) {
                std::cout << "Failed to move data for shard " << shardId << std::endl;
                success = false;
            }
        }
    }
    
    return success;
}

size_t DataRebalancer::getTotalDataSize() const {
    size_t totalSize = 0;
    for (const auto& pair : shards_) {
        totalSize += pair.second->dataSize;
    }
    return totalSize;
}

double DataRebalancer::getAverageDataSizePerShard() const {
    if (shards_.empty()) {
        return 0.0;
    }
    
    size_t totalSize = getTotalDataSize();
    return static_cast<double>(totalSize) / shards_.size();
}

double DataRebalancer::getMaxDataSizeDeviation() const {
    if (shards_.empty()) {
        return 0.0;
    }
    
    double avgSize = getAverageDataSizePerShard();
    double maxDeviation = 0.0;
    
    for (const auto& pair : shards_) {
        double deviation = std::abs(static_cast<double>(pair.second->dataSize) - avgSize);
        if (deviation > maxDeviation) {
            maxDeviation = deviation;
        }
    }
    
    return maxDeviation;
}

} // namespace distributed
} // namespace phantomdb