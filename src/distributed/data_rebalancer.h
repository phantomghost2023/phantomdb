#ifndef PHANTOMDB_DATA_REBALANCER_H
#define PHANTOMDB_DATA_REBALANCER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <functional>
#include <chrono>

namespace phantomdb {
namespace distributed {

// Shard information
struct ShardInfo {
    std::string id;
    std::vector<std::string> nodes;  // Nodes responsible for this shard
    size_t dataSize;  // Approximate size of data in this shard
    std::chrono::steady_clock::time_point lastRebalanced;
    
    ShardInfo(const std::string& shardId) 
        : id(shardId), dataSize(0) {
        lastRebalanced = std::chrono::steady_clock::now();
    }
};

// Rebalancing strategy
enum class RebalancingStrategy {
    CONSISTENT_HASHING,
    ROUND_ROBIN,
    LOAD_BASED
};

// Rebalancing status
enum class RebalancingStatus {
    IDLE,
    IN_PROGRESS,
    COMPLETED,
    FAILED
};

// Node information for rebalancing
struct RebalancingNodeInfo {
    std::string id;
    size_t capacity;  // Total capacity
    size_t usedSpace;  // Used space
    size_t availableSpace;  // Available space
    
    RebalancingNodeInfo() : id(""), capacity(0), usedSpace(0), availableSpace(0) {}
    
    RebalancingNodeInfo(const std::string& nodeId) 
        : id(nodeId), capacity(0), usedSpace(0), availableSpace(0) {}
};

// Callback function types
using RebalanceStartCallback = std::function<void()>;
using RebalanceCompleteCallback = std::function<void(bool success)>;
using DataMoveCallback = std::function<bool(const std::string& shardId, 
                                          const std::string& fromNode, 
                                          const std::string& toNode)>;

class DataRebalancer {
public:
    DataRebalancer(RebalancingStrategy strategy = RebalancingStrategy::CONSISTENT_HASHING);
    ~DataRebalancer();
    
    // Initialize the data rebalancer
    bool initialize();
    
    // Shutdown the data rebalancer
    void shutdown();
    
    // Create shards based on nodes
    bool createShards(const std::vector<std::string>& nodeIds, size_t shardCount);
    
    // Rebalance shards when nodes are added/removed
    bool rebalanceShards(const std::vector<std::string>& currentNodes);
    
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
    
    // Update shard data size
    bool updateShardDataSize(const std::string& shardId, size_t dataSize);
    
    // Get rebalancing status
    RebalancingStatus getRebalancingStatus() const;
    
    // Get rebalancing strategy
    RebalancingStrategy getStrategy() const;
    
    // Register callbacks
    void registerRebalanceStartCallback(const RebalanceStartCallback& callback);
    void registerRebalanceCompleteCallback(const RebalanceCompleteCallback& callback);
    void registerDataMoveCallback(const DataMoveCallback& callback);
    
    // Set rebalancing threshold (percentage of imbalance that triggers rebalancing)
    void setRebalancingThreshold(double threshold);
    
    // Get rebalancing threshold
    double getRebalancingThreshold() const;

private:
    // Rebalancing strategy
    RebalancingStrategy strategy_;
    
    // Shard information
    std::unordered_map<std::string, std::shared_ptr<ShardInfo>> shards_;
    
    // Node information
    std::unordered_map<std::string, RebalancingNodeInfo> nodeInfos_;
    
    // Rebalancing status
    std::atomic<RebalancingStatus> rebalancingStatus_;
    
    // Rebalancing threshold
    std::atomic<double> rebalancingThreshold_;
    
    // Callbacks
    RebalanceStartCallback rebalanceStartCallback_;
    RebalanceCompleteCallback rebalanceCompleteCallback_;
    DataMoveCallback dataMoveCallback_;
    
    // Mutex for thread safety
    mutable std::mutex rebalancerMutex_;
    
    // Internal methods
    std::string consistentHashBasedSharding(const std::string& key) const;
    std::string roundRobinSharding(const std::string& key) const;
    std::string loadBasedSharding(const std::string& key) const;
    
    bool needsRebalancing() const;
    std::vector<std::pair<std::string, std::string>> calculateRebalancingMoves() const;
    bool executeRebalancingMoves(const std::vector<std::pair<std::string, std::string>>& moves);
    
    // Helper methods
    size_t getTotalDataSize() const;
    double getAverageDataSizePerShard() const;
    double getMaxDataSizeDeviation() const;
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_DATA_REBALANCER_H