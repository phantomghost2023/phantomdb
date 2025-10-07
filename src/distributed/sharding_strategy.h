#ifndef PHANTOMDB_SHARDING_STRATEGY_H
#define PHANTOMDB_SHARDING_STRATEGY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace phantomdb {
namespace distributed {

// Sharding strategy types
enum class ShardingStrategyType {
    HASH_BASED,
    RANGE_BASED,
    CONSISTENT_HASHING,
    DIRECTORY_BASED
};

// Shard information
struct ShardInfo {
    std::string id;
    std::vector<std::string> nodes;  // Nodes responsible for this shard
    std::string keyRangeStart;
    std::string keyRangeEnd;
    
    ShardInfo() : id("") {}
    
    ShardInfo(const std::string& shardId) : id(shardId) {}
};

class ShardingStrategy {
public:
    ShardingStrategy(ShardingStrategyType strategyType = ShardingStrategyType::HASH_BASED);
    virtual ~ShardingStrategy();
    
    // Initialize sharding strategy
    bool initialize();
    
    // Shutdown sharding strategy
    void shutdown();
    
    // Create shards based on nodes
    bool createShards(const std::vector<std::string>& nodeIds, int shardCount);
    
    // Get shard for a given key
    std::string getShardForKey(const std::string& key) const;
    
    // Get nodes responsible for a shard
    std::vector<std::string> getNodesForShard(const std::string& shardId) const;
    
    // Get all shards
    std::vector<std::shared_ptr<ShardInfo>> getAllShards() const;
    
    // Add a node to a shard
    bool addNodeToShard(const std::string& shardId, const std::string& nodeId);
    
    // Remove a node from a shard
    bool removeNodeFromShard(const std::string& shardId, const std::string& nodeId);
    
    // Rebalance shards when nodes are added/removed
    bool rebalanceShards(const std::vector<std::string>& currentNodes);
    
    // Get sharding strategy type
    ShardingStrategyType getStrategyType() const;

protected:
    // Hash-based sharding implementation
    std::string hashBasedSharding(const std::string& key) const;
    
    // Range-based sharding implementation
    std::string rangeBasedSharding(const std::string& key) const;

private:
    // Sharding strategy type
    ShardingStrategyType strategyType_;
    
    // Shard information
    std::unordered_map<std::string, std::shared_ptr<ShardInfo>> shards_;
    
    // Mutex for thread safety
    mutable std::mutex shardsMutex_;
    
    // Flag to indicate if strategy is initialized
    bool initialized_;
    
    // Number of shards
    int shardCount_;
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_SHARDING_STRATEGY_H