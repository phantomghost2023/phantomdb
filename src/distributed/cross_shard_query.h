#ifndef PHANTOMDB_CROSS_SHARD_QUERY_H
#define PHANTOMDB_CROSS_SHARD_QUERY_H

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
    std::string address;
    int port;
    std::vector<std::string> tables;
    
    ShardInfo() : id(""), address(""), port(0) {}
    
    ShardInfo(const std::string& shardId, const std::string& addr, int p)
        : id(shardId), address(addr), port(p) {}
};

// Query execution result
struct QueryResult {
    std::string shardId;
    std::vector<std::vector<std::string>> rows;
    bool success;
    std::string errorMessage;
    std::chrono::steady_clock::time_point executionTime;
    
    QueryResult() : shardId(""), success(false), executionTime(std::chrono::steady_clock::now()) {}
    
    QueryResult(const std::string& id) 
        : shardId(id), success(false), executionTime(std::chrono::steady_clock::now()) {}
};

// Callback function types
using QueryExecutionCallback = std::function<QueryResult(const ShardInfo&, const std::string& query)>;

class CrossShardQueryProcessor {
public:
    CrossShardQueryProcessor();
    ~CrossShardQueryProcessor();
    
    // Initialize the cross-shard query processor
    bool initialize();
    
    // Shutdown the cross-shard query processor
    void shutdown();
    
    // Add a shard to the processor
    bool addShard(const ShardInfo& shard);
    
    // Remove a shard from the processor
    bool removeShard(const std::string& shardId);
    
    // Execute a query across multiple shards
    std::vector<QueryResult> executeCrossShardQuery(const std::string& query);
    
    // Execute a query with specific shards
    std::vector<QueryResult> executeQueryOnShards(const std::string& query, 
                                                const std::vector<std::string>& shardIds);
    
    // Get all shards
    std::vector<ShardInfo> getShards() const;
    
    // Get shard for a specific table
    std::vector<ShardInfo> getShardsForTable(const std::string& tableName) const;
    
    // Register callback for query execution
    void registerQueryExecutionCallback(const QueryExecutionCallback& callback);
    
    // Set timeout for query execution
    void setQueryTimeout(const std::chrono::milliseconds& timeout);
    
    // Get timeout value
    std::chrono::milliseconds getQueryTimeout() const;

private:
    // Shards
    std::unordered_map<std::string, ShardInfo> shards_;
    
    // Callback for query execution
    QueryExecutionCallback queryExecutionCallback_;
    
    // Timeout value
    std::atomic<std::chrono::milliseconds> queryTimeout_;
    
    // Mutex for thread safety
    mutable std::mutex processorMutex_;
    
    // Internal methods
    std::vector<ShardInfo> determineRelevantShards(const std::string& query) const;
    QueryResult executeQueryOnShard(const ShardInfo& shard, const std::string& query);
    std::vector<QueryResult> mergeResults(const std::vector<QueryResult>& results) const;
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_CROSS_SHARD_QUERY_H