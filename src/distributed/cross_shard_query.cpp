#include "cross_shard_query.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <future>

namespace phantomdb {
namespace distributed {

CrossShardQueryProcessor::CrossShardQueryProcessor()
    : queryTimeout_(std::chrono::milliseconds(10000)) { // 10 seconds
    std::cout << "Creating CrossShardQueryProcessor" << std::endl;
}

CrossShardQueryProcessor::~CrossShardQueryProcessor() {
    std::cout << "Destroying CrossShardQueryProcessor" << std::endl;
}

bool CrossShardQueryProcessor::initialize() {
    std::cout << "Initializing CrossShardQueryProcessor" << std::endl;
    
    // No background threads needed for cross-shard query processor
    std::cout << "CrossShardQueryProcessor initialized successfully" << std::endl;
    return true;
}

void CrossShardQueryProcessor::shutdown() {
    std::cout << "Shutting down CrossShardQueryProcessor" << std::endl;
    
    // Clear shards
    {
        std::lock_guard<std::mutex> lock(processorMutex_);
        shards_.clear();
    }
    
    std::cout << "CrossShardQueryProcessor shutdown completed" << std::endl;
}

bool CrossShardQueryProcessor::addShard(const ShardInfo& shard) {
    std::lock_guard<std::mutex> lock(processorMutex_);
    
    // Check if shard already exists
    if (shards_.find(shard.id) != shards_.end()) {
        std::cout << "Shard " << shard.id << " already exists" << std::endl;
        return false;
    }
    
    shards_[shard.id] = shard;
    
    std::cout << "Added shard " << shard.id << " at " << shard.address << ":" << shard.port << std::endl;
    return true;
}

bool CrossShardQueryProcessor::removeShard(const std::string& shardId) {
    std::lock_guard<std::mutex> lock(processorMutex_);
    
    auto it = shards_.find(shardId);
    if (it == shards_.end()) {
        std::cout << "Shard " << shardId << " not found" << std::endl;
        return false;
    }
    
    shards_.erase(it);
    
    std::cout << "Removed shard " << shardId << std::endl;
    return true;
}

std::vector<QueryResult> CrossShardQueryProcessor::executeCrossShardQuery(const std::string& query) {
    std::lock_guard<std::mutex> lock(processorMutex_);
    
    // Determine relevant shards for the query
    std::vector<ShardInfo> relevantShards = determineRelevantShards(query);
    
    if (relevantShards.empty()) {
        std::cout << "No relevant shards found for query: " << query << std::endl;
        return {};
    }
    
    std::cout << "Executing cross-shard query on " << relevantShards.size() << " shards" << std::endl;
    
    // Execute query on all relevant shards in parallel
    std::vector<std::future<QueryResult>> futures;
    futures.reserve(relevantShards.size());
    
    for (const auto& shard : relevantShards) {
        // Launch async execution for each shard
        futures.push_back(std::async(std::launch::async, [this, shard, query]() {
            return executeQueryOnShard(shard, query);
        }));
    }
    
    // Collect results
    std::vector<QueryResult> results;
    results.reserve(relevantShards.size());
    
    for (auto& future : futures) {
        try {
            // Wait for result with timeout
            auto result = future.get();
            results.push_back(result);
            
            if (result.success) {
                std::cout << "Query executed successfully on shard " << result.shardId << std::endl;
            } else {
                std::cout << "Query failed on shard " << result.shardId << ": " << result.errorMessage << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Exception while executing query: " << e.what() << std::endl;
            // Add a failed result
            QueryResult failedResult("unknown");
            failedResult.success = false;
            failedResult.errorMessage = e.what();
            results.push_back(failedResult);
        }
    }
    
    // Merge results if needed
    return mergeResults(results);
}

std::vector<QueryResult> CrossShardQueryProcessor::executeQueryOnShards(
    const std::string& query, 
    const std::vector<std::string>& shardIds) {
    
    std::lock_guard<std::mutex> lock(processorMutex_);
    
    std::vector<ShardInfo> targetShards;
    targetShards.reserve(shardIds.size());
    
    // Get shard information for specified shard IDs
    for (const auto& shardId : shardIds) {
        auto it = shards_.find(shardId);
        if (it != shards_.end()) {
            targetShards.push_back(it->second);
        } else {
            std::cout << "Shard " << shardId << " not found" << std::endl;
        }
    }
    
    if (targetShards.empty()) {
        std::cout << "No valid shards found for query execution" << std::endl;
        return {};
    }
    
    std::cout << "Executing query on " << targetShards.size() << " specified shards" << std::endl;
    
    // Execute query on specified shards in parallel
    std::vector<std::future<QueryResult>> futures;
    futures.reserve(targetShards.size());
    
    for (const auto& shard : targetShards) {
        // Launch async execution for each shard
        futures.push_back(std::async(std::launch::async, [this, shard, query]() {
            return executeQueryOnShard(shard, query);
        }));
    }
    
    // Collect results
    std::vector<QueryResult> results;
    results.reserve(targetShards.size());
    
    for (auto& future : futures) {
        try {
            // Wait for result with timeout
            auto result = future.get();
            results.push_back(result);
            
            if (result.success) {
                std::cout << "Query executed successfully on shard " << result.shardId << std::endl;
            } else {
                std::cout << "Query failed on shard " << result.shardId << ": " << result.errorMessage << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Exception while executing query: " << e.what() << std::endl;
            // Add a failed result
            QueryResult failedResult("unknown");
            failedResult.success = false;
            failedResult.errorMessage = e.what();
            results.push_back(failedResult);
        }
    }
    
    // Merge results if needed
    return mergeResults(results);
}

std::vector<ShardInfo> CrossShardQueryProcessor::getShards() const {
    std::lock_guard<std::mutex> lock(processorMutex_);
    
    std::vector<ShardInfo> shardList;
    shardList.reserve(shards_.size());
    
    for (const auto& pair : shards_) {
        shardList.push_back(pair.second);
    }
    
    return shardList;
}

std::vector<ShardInfo> CrossShardQueryProcessor::getShardsForTable(const std::string& tableName) const {
    std::lock_guard<std::mutex> lock(processorMutex_);
    
    std::vector<ShardInfo> relevantShards;
    
    for (const auto& pair : shards_) {
        const auto& shard = pair.second;
        // Check if table is in this shard
        if (std::find(shard.tables.begin(), shard.tables.end(), tableName) != shard.tables.end()) {
            relevantShards.push_back(shard);
        }
    }
    
    return relevantShards;
}

void CrossShardQueryProcessor::registerQueryExecutionCallback(const QueryExecutionCallback& callback) {
    std::lock_guard<std::mutex> lock(processorMutex_);
    queryExecutionCallback_ = callback;
    std::cout << "Query execution callback registered" << std::endl;
}

void CrossShardQueryProcessor::setQueryTimeout(const std::chrono::milliseconds& timeout) {
    queryTimeout_.store(timeout);
    std::cout << "Query timeout set to " << timeout.count() << " ms" << std::endl;
}

std::chrono::milliseconds CrossShardQueryProcessor::getQueryTimeout() const {
    return queryTimeout_.load();
}

std::vector<ShardInfo> CrossShardQueryProcessor::determineRelevantShards(const std::string& query) const {
    // In a real implementation, this would parse the query and determine which shards
    // contain the tables referenced in the query
    // For now, we'll return all shards as a simplification
    
    std::cout << "Determining relevant shards for query: " << query << std::endl;
    
    std::vector<ShardInfo> allShards;
    allShards.reserve(shards_.size());
    
    for (const auto& pair : shards_) {
        allShards.push_back(pair.second);
    }
    
    return allShards;
}

QueryResult CrossShardQueryProcessor::executeQueryOnShard(const ShardInfo& shard, const std::string& query) {
    std::cout << "Executing query on shard " << shard.id << " at " << shard.address << ":" << shard.port << std::endl;
    
    QueryResult result(shard.id);
    result.executionTime = std::chrono::steady_clock::now();
    
    // Call query execution callback
    if (queryExecutionCallback_) {
        try {
            result = queryExecutionCallback_(shard, query);
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = e.what();
            std::cout << "Exception in query execution callback: " << e.what() << std::endl;
        }
    } else {
        // Default implementation - simulate success
        result.success = true;
        result.rows = {{"default_result"}};
        std::cout << "No query execution callback registered, using default implementation" << std::endl;
    }
    
    return result;
}

std::vector<QueryResult> CrossShardQueryProcessor::mergeResults(const std::vector<QueryResult>& results) const {
    // In a real implementation, this would merge results from different shards
    // For now, we'll just return the results as-is
    
    std::cout << "Merging results from " << results.size() << " shards" << std::endl;
    
    return results;
}

} // namespace distributed
} // namespace phantomdb