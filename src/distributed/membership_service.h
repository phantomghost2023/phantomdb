#ifndef PHANTOMDB_MEMBERSHIP_SERVICE_H
#define PHANTOMDB_MEMBERSHIP_SERVICE_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <atomic>

namespace phantomdb {
namespace distributed {

class MembershipService {
public:
    MembershipService();
    ~MembershipService();
    
    // Initialize membership service
    bool initialize();
    
    // Shutdown membership service
    void shutdown();
    
    // Add a node to the membership list
    bool addNode(const std::string& nodeId);
    
    // Remove a node from the membership list
    bool removeNode(const std::string& nodeId);
    
    // Check if a node is a member
    bool isMember(const std::string& nodeId) const;
    
    // Get all members
    std::vector<std::string> getMembers() const;
    
    // Get member count
    size_t getMemberCount() const;
    
    // Update node heartbeat
    bool updateHeartbeat(const std::string& nodeId);
    
    // Check for failed nodes
    std::vector<std::string> detectFailedNodes(int timeoutSeconds = 30);
    
    // Set heartbeat timeout
    void setHeartbeatTimeout(int timeoutSeconds);
    
    // Get heartbeat timeout
    int getHeartbeatTimeout() const;

private:
    // Set of member nodes
    std::unordered_set<std::string> members_;
    
    // Last heartbeat time for each node
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> heartbeats_;
    
    // Mutex for thread safety
    mutable std::mutex membersMutex_;
    
    // Heartbeat timeout in seconds
    std::atomic<int> heartbeatTimeoutSeconds_;
    
    // Flag to indicate if service is initialized
    std::atomic<bool> initialized_;
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_MEMBERSHIP_SERVICE_H