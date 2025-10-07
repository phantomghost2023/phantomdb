#ifndef PHANTOMDB_NODE_MANAGER_H
#define PHANTOMDB_NODE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>

namespace phantomdb {
namespace distributed {

// Node information
struct NodeInfo {
    std::string id;
    std::string address;
    int port;
    std::atomic<bool> isActive;
    std::chrono::steady_clock::time_point lastHeartbeat;
    
    NodeInfo(const std::string& nodeId, const std::string& addr, int p)
        : id(nodeId), address(addr), port(p), isActive(true) {
        lastHeartbeat = std::chrono::steady_clock::now();
    }
};

// Node status information
struct NodeStatus {
    std::string id;
    bool isActive;
    std::chrono::steady_clock::time_point lastHeartbeat;
    size_t dataShards;
    size_t cpuUsage;  // Percentage
    size_t memoryUsage;  // Percentage
    size_t diskUsage;  // Percentage
    
    NodeStatus() : id(""), isActive(false), dataShards(0), 
          cpuUsage(0), memoryUsage(0), diskUsage(0) {}
    
    NodeStatus(const std::string& nodeId) 
        : id(nodeId), isActive(false), dataShards(0), 
          cpuUsage(0), memoryUsage(0), diskUsage(0) {}
};

// Callback function types
using NodeAddedCallback = std::function<void(const NodeInfo&)>;
using NodeRemovedCallback = std::function<void(const std::string&)>;
using NodeStatusCallback = std::function<void(const NodeStatus&)>;

class NodeManager {
public:
    NodeManager();
    ~NodeManager();
    
    // Initialize the node manager
    bool initialize();
    
    // Shutdown the node manager
    void shutdown();
    
    // Add a new node to the cluster
    bool addNode(const std::string& nodeId, const std::string& address, int port);
    
    // Remove a node from the cluster
    bool removeNode(const std::string& nodeId);
    
    // Mark a node as active
    bool activateNode(const std::string& nodeId);
    
    // Mark a node as inactive
    bool deactivateNode(const std::string& nodeId);
    
    // Update node heartbeat
    bool updateNodeHeartbeat(const std::string& nodeId);
    
    // Get node information
    std::shared_ptr<NodeInfo> getNode(const std::string& nodeId) const;
    
    // Get all nodes
    std::vector<std::shared_ptr<NodeInfo>> getAllNodes() const;
    
    // Get active nodes
    std::vector<std::shared_ptr<NodeInfo>> getActiveNodes() const;
    
    // Get node status
    NodeStatus getNodeStatus(const std::string& nodeId) const;
    
    // Get status of all nodes
    std::vector<NodeStatus> getAllNodeStatus() const;
    
    // Register callbacks
    void registerNodeAddedCallback(const NodeAddedCallback& callback);
    void registerNodeRemovedCallback(const NodeRemovedCallback& callback);
    void registerNodeStatusCallback(const NodeStatusCallback& callback);
    
    // Set heartbeat timeout
    void setHeartbeatTimeout(const std::chrono::milliseconds& timeout);
    
    // Get heartbeat timeout
    std::chrono::milliseconds getHeartbeatTimeout() const;
    
    // Check if cluster is healthy
    bool isClusterHealthy() const;
    
    // Get cluster size
    size_t getClusterSize() const;
    
    // Get active cluster size
    size_t getActiveClusterSize() const;

private:
    // Nodes in the cluster
    std::unordered_map<std::string, std::shared_ptr<NodeInfo>> nodes_;
    
    // Node statuses
    std::unordered_map<std::string, NodeStatus> nodeStatuses_;
    
    // Callbacks
    NodeAddedCallback nodeAddedCallback_;
    NodeRemovedCallback nodeRemovedCallback_;
    NodeStatusCallback nodeStatusCallback_;
    
    // Heartbeat timeout
    std::atomic<std::chrono::milliseconds> heartbeatTimeout_;
    
    // Mutex for thread safety
    mutable std::mutex managerMutex_;
    
    // Background thread for node monitoring
    std::unique_ptr<std::thread> monitorThread_;
    std::atomic<bool> running_;
    
    // Internal methods
    void monitorNodes();
    bool isNodeHealthy(const NodeInfo& node) const;
    void notifyNodeStatus(const std::string& nodeId);
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_NODE_MANAGER_H