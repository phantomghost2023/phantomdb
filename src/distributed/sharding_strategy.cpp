#include "sharding_strategy.h"
#include <iostream>
#include <functional>
#include <algorithm>
#include <numeric>
#include <cctype>

namespace phantomdb {
namespace distributed {

ShardingStrategy::ShardingStrategy(ShardingStrategyType strategyType)
    : strategyType_(strategyType), initialized_(false), shardCount_(0) {
    std::cout << "Creating ShardingStrategy with type " << static_cast<int>(strategyType) << std::endl;
}

ShardingStrategy::~ShardingStrategy() {
    if (initialized_) {
        shutdown();
    }
    std::cout << "Destroying ShardingStrategy" << std::endl;
}

bool ShardingStrategy::initialize() {
    if (initialized_) {
        std::cout << "ShardingStrategy already initialized" << std::endl;
        return true;
    }
    
    initialized_ = true;
    std::cout << "ShardingStrategy initialized successfully" << std::endl;
    return true;
}

void ShardingStrategy::shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "Shutting down ShardingStrategy" << std::endl;
    
    // Clear shards
    {
        std::lock_guard<std::mutex> lock(shardsMutex_);
        shards_.clear();
    }
    
    initialized_ = false;
    std::cout << "ShardingStrategy shutdown completed" << std::endl;
}

bool ShardingStrategy::createShards(const std::vector<std::string>& nodeIds, int shardCount) {
    if (!initialized_) {
        std::cout << "ShardingStrategy not initialized" << std::endl;
        return false;
    }
    
    if (nodeIds.empty() || shardCount <= 0) {
        std::cout << "Invalid parameters for shard creation" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(shardsMutex_);
    
    // Clear existing shards
    shards_.clear();
    
    shardCount_ = shardCount;
    
    // Create shards and distribute nodes
    for (int i = 0; i < shardCount; i++) {
        std::string shardId = "shard_" + std::to_string(i);
        auto shardInfo = std::make_shared<ShardInfo>(shardId);
        
        // Distribute nodes across shards
        for (size_t j = 0; j < nodeIds.size(); j++) {
            if (j % shardCount == static_cast<size_t>(i)) {
                shardInfo->nodes.push_back(nodeIds[j]);
            }
        }
        
        shards_[shardId] = shardInfo;
    }
    
    std::cout << "Created " << shardCount << " shards distributed across " << nodeIds.size() << " nodes" << std::endl;
    return true;
}

std::string ShardingStrategy::getShardForKey(const std::string& key) const {
    if (!initialized_) {
        return "";
    }
    
    std::lock_guard<std::mutex> lock(shardsMutex_);
    
    if (shards_.empty()) {
        return "";
    }
    
    switch (strategyType_) {
        case ShardingStrategyType::HASH_BASED:
            return hashBasedSharding(key);
        case ShardingStrategyType::RANGE_BASED:
            return rangeBasedSharding(key);
        default:
            // Default to hash-based for other strategies
            return hashBasedSharding(key);
    }
}

std::vector<std::string> ShardingStrategy::getNodesForShard(const std::string& shardId) const {
    if (!initialized_) {
        return {};
    }
    
    std::lock_guard<std::mutex> lock(shardsMutex_);
    
    auto it = shards_.find(shardId);
    if (it != shards_.end()) {
        return it->second->nodes;
    }
    
    return {};
}

std::vector<std::shared_ptr<ShardInfo>> ShardingStrategy::getAllShards() const {
    if (!initialized_) {
        return {};
    }
    
    std::lock_guard<std::mutex> lock(shardsMutex_);
    
    std::vector<std::shared_ptr<ShardInfo>> result;
    result.reserve(shards_.size());
    
    for (const auto& pair : shards_) {
        result.push_back(pair.second);
    }
    
    return result;
}

bool ShardingStrategy::addNodeToShard(const std::string& shardId, const std::string& nodeId) {
    if (!initialized_) {
        std::cout << "ShardingStrategy not initialized" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(shardsMutex_);
    
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

bool ShardingStrategy::removeNodeFromShard(const std::string& shardId, const std::string& nodeId) {
    if (!initialized_) {
        std::cout << "ShardingStrategy not initialized" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(shardsMutex_);
    
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

bool ShardingStrategy::rebalanceShards(const std::vector<std::string>& currentNodes) {
    if (!initialized_) {
        std::cout << "ShardingStrategy not initialized" << std::endl;
        return false;
    }
    
    if (currentNodes.empty()) {
        std::cout << "No nodes provided for rebalancing" << std::endl;
        return false;
    }
    
    std::lock_guard<std::mutex> lock(shardsMutex_);
    
    // Simple rebalancing: redistribute nodes evenly across existing shards
    int shardIndex = 0;
    for (const auto& node : currentNodes) {
        // Assign node to shard in round-robin fashion
        std::string shardId = "shard_" + std::to_string(shardIndex % shardCount_);
        
        // Find the shard
        auto it = shards_.find(shardId);
        if (it != shards_.end()) {
            // Add node to shard if not already present
            auto& nodes = it->second->nodes;
            if (std::find(nodes.begin(), nodes.end(), node) == nodes.end()) {
                nodes.push_back(node);
            }
        }
        
        shardIndex++;
    }
    
    std::cout << "Rebalanced shards across " << currentNodes.size() << " nodes" << std::endl;
    return true;
}

ShardingStrategyType ShardingStrategy::getStrategyType() const {
    return strategyType_;
}

std::string ShardingStrategy::hashBasedSharding(const std::string& key) const {
    // Simple hash-based sharding
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

std::string ShardingStrategy::rangeBasedSharding(const std::string& key) const {
    // Simple range-based sharding (for demonstration)
    if (shards_.empty()) {
        return "";
    }
    
    // For range-based, we could use the first character of the key
    // This is a simplified implementation
    if (key.empty()) {
        return shards_.begin()->first;
    }
    
    char firstChar = std::tolower(key[0]);
    
    // Distribute based on character range
    size_t shardIndex = 0;
    if (firstChar >= 'a' && firstChar <= 'z') {
        shardIndex = (firstChar - 'a') % shards_.size();
    }
    
    // Find the shard at this index
    auto it = shards_.begin();
    std::advance(it, shardIndex);
    
    return it->first;
}

} // namespace distributed
} // namespace phantomdb