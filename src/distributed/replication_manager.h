#ifndef PHANTOMDB_REPLICATION_MANAGER_H
#define PHANTOMDB_REPLICATION_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

namespace phantomdb {
namespace distributed {

// Replication strategy types
enum class ReplicationStrategy {
    SYNCHRONOUS,
    ASYNCHRONOUS,
    SEMI_SYNCHRONOUS
};

// Region information
struct RegionInfo {
    std::string id;
    std::string address;
    int port;
    bool isPrimary;
    
    RegionInfo(const std::string& regionId, const std::string& addr, int p, bool primary = false)
        : id(regionId), address(addr), port(p), isPrimary(primary) {}
};

// Replication status
struct ReplicationStatus {
    std::string regionId;
    bool isConnected;
    uint64_t lastReplicatedIndex;
    std::chrono::steady_clock::time_point lastHeartbeat;
    std::string errorMessage;
    
    ReplicationStatus(const std::string& region) 
        : regionId(region), isConnected(false), lastReplicatedIndex(0) {}
};

class ReplicationManager {
public:
    ReplicationManager(ReplicationStrategy strategy = ReplicationStrategy::ASYNCHRONOUS);
    ~ReplicationManager();
    
    // Initialize the replication manager
    bool initialize();
    
    // Shutdown the replication manager
    void shutdown();
    
    // Add a region to replicate to
    bool addRegion(const RegionInfo& region);
    
    // Remove a region from replication
    bool removeRegion(const std::string& regionId);
    
    // Set replication strategy
    void setReplicationStrategy(ReplicationStrategy strategy);
    
    // Get replication strategy
    ReplicationStrategy getReplicationStrategy() const;
    
    // Replicate data to all regions
    bool replicateData(const std::string& key, const std::string& value);
    
    // Get replication status for all regions
    std::vector<ReplicationStatus> getReplicationStatus() const;
    
    // Get region information
    std::vector<RegionInfo> getRegions() const;
    
    // Check if all regions are connected
    bool areAllRegionsConnected() const;
    
    // Get primary region
    std::string getPrimaryRegion() const;
    
    // Set primary region
    bool setPrimaryRegion(const std::string& regionId);

private:
    // Replication strategy
    std::atomic<ReplicationStrategy> strategy_;
    
    // Regions to replicate to
    std::unordered_map<std::string, RegionInfo> regions_;
    
    // Replication status for each region
    std::unordered_map<std::string, ReplicationStatus> regionStatus_;
    
    // Primary region
    std::string primaryRegion_;
    
    // Background thread for replication
    std::unique_ptr<std::thread> replicationThread_;
    std::atomic<bool> running_;
    
    // Mutex for thread safety
    mutable std::mutex replicationMutex_;
    
    // Heartbeat interval
    std::chrono::milliseconds heartbeatInterval_;
    
    // Background replication loop
    void replicationLoop();
    
    // Send heartbeat to all regions
    void sendHeartbeats();
    
    // Replicate data synchronously
    bool replicateSynchronously(const std::string& key, const std::string& value);
    
    // Replicate data asynchronously
    bool replicateAsynchronously(const std::string& key, const std::string& value);
    
    // Replicate data semi-synchronously
    bool replicateSemiSynchronously(const std::string& key, const std::string& value);
    
    // Connect to a region
    bool connectToRegion(const RegionInfo& region);
    
    // Disconnect from a region
    void disconnectFromRegion(const std::string& regionId);
    
    // Update region status
    void updateRegionStatus(const std::string& regionId, bool connected, 
                           uint64_t lastReplicatedIndex = 0, 
                           const std::string& errorMessage = "");
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_REPLICATION_MANAGER_H