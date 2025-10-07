#ifndef PHANTOMDB_NODE_DISCOVERY_H
#define PHANTOMDB_NODE_DISCOVERY_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>

namespace phantomdb {
namespace distributed {

// Callback function type for node discovery events
using NodeDiscoveryCallback = std::function<void(const std::string&, const std::string&, int)>;

class NodeDiscovery {
public:
    NodeDiscovery();
    ~NodeDiscovery();
    
    // Initialize node discovery
    bool initialize();
    
    // Shutdown node discovery
    void shutdown();
    
    // Start discovery process
    bool startDiscovery();
    
    // Stop discovery process
    void stopDiscovery();
    
    // Register callback for node discovery events
    void registerNodeCallback(const NodeDiscoveryCallback& callback);
    
    // Register callback for node failure events
    void registerFailureCallback(const NodeDiscoveryCallback& callback);
    
    // Manually add a node
    bool addNode(const std::string& nodeId, const std::string& address, int port);
    
    // Manually remove a node
    bool removeNode(const std::string& nodeId);
    
    // Set discovery interval (in milliseconds)
    void setDiscoveryInterval(int intervalMs);
    
    // Get discovery interval
    int getDiscoveryInterval() const;

private:
    // Discovery thread
    std::unique_ptr<std::thread> discoveryThread_;
    
    // Flag to control discovery loop
    std::atomic<bool> discoveryRunning_;
    
    // Flag to indicate if discovery is initialized
    std::atomic<bool> initialized_;
    
    // Discovery interval in milliseconds
    std::atomic<int> discoveryIntervalMs_;
    
    // Callback for node discovery events
    NodeDiscoveryCallback nodeCallback_;
    
    // Callback for node failure events
    NodeDiscoveryCallback failureCallback_;
    
    // Internal discovery implementation
    void discoveryLoop();
    
    // Simulate node discovery (in a real implementation, this would use network protocols)
    void simulateNodeDiscovery();
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_NODE_DISCOVERY_H