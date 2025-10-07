#ifndef PHANTOMDB_CLUSTER_MANAGER_H
#define PHANTOMDB_CLUSTER_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace phantomdb {
namespace distributed {

// Forward declarations
class NodeDiscovery;
class MembershipService;
class ShardingStrategy;
class LoadBalancer;

// Node information structure
struct NodeInfo {
    std::string id;
    std::string address;
    int port;
    std::chrono::steady_clock::time_point last_heartbeat;
    bool is_active;
    
    NodeInfo(const std::string& nodeId, const std::string& addr, int p)
        : id(nodeId), address(addr), port(p), 
          last_heartbeat(std::chrono::steady_clock::now()), is_active(true) {}
};

class ClusterManager {
public:
    ClusterManager();
    ~ClusterManager();
    
    // Initialize the cluster manager
    bool initialize();
    
    // Shutdown the cluster manager
    void shutdown();
    
    // Add a node to the cluster
    bool addNode(const std::string& nodeId, const std::string& address, int port);
    
    // Remove a node from the cluster
    bool removeNode(const std::string& nodeId);
    
    // Get information about a specific node
    std::shared_ptr<NodeInfo> getNode(const std::string& nodeId) const;
    
    // Get all nodes in the cluster
    std::vector<std::shared_ptr<NodeInfo>> getAllNodes() const;
    
    // Get active nodes only
    std::vector<std::shared_ptr<NodeInfo>> getActiveNodes() const;
    
    // Update node heartbeat
    bool updateNodeHeartbeat(const std::string& nodeId);
    
    // Check if cluster is healthy
    bool isClusterHealthy() const;
    
    // Get cluster size
    size_t getClusterSize() const;
    
    // Get active cluster size
    size_t getActiveClusterSize() const;

private:
    // Node discovery service
    std::unique_ptr<NodeDiscovery> nodeDiscovery_;
    
    // Membership service
    std::unique_ptr<MembershipService> membershipService_;
    
    // Sharding strategy
    std::unique_ptr<ShardingStrategy> shardingStrategy_;
    
    // Load balancer
    std::unique_ptr<LoadBalancer> loadBalancer_;
    
    // Nodes in the cluster
    std::unordered_map<std::string, std::shared_ptr<NodeInfo>> nodes_;
    
    // Mutex for thread safety
    mutable std::mutex nodesMutex_;
    
    // Cluster health threshold (in seconds)
    static constexpr int HEALTH_THRESHOLD_SECONDS = 30;
    
    // Flag to indicate if cluster manager is initialized
    bool initialized_;
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_CLUSTER_MANAGER_H