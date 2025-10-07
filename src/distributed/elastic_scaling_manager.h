#ifndef PHANTOMDB_ELASTIC_SCALING_MANAGER_H
#define PHANTOMDB_ELASTIC_SCALING_MANAGER_H

#include "node_manager.h"
#include "data_rebalancer.h"
#include "resource_scaler.h"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>

namespace phantomdb {
namespace distributed {

// Elastic scaling configuration
struct ElasticScalingConfig {
    RebalancingStrategy rebalancingStrategy;
    ScalingPolicy scalingPolicy;
    ScalingTrigger scalingTrigger;
    std::chrono::milliseconds evaluationInterval;
    double rebalancingThreshold;
    
    ElasticScalingConfig() 
        : rebalancingStrategy(RebalancingStrategy::CONSISTENT_HASHING),
          scalingPolicy(ScalingPolicy::AUTOMATIC),
          evaluationInterval(std::chrono::milliseconds(5000)),
          rebalancingThreshold(0.2) {}
};

// Elastic scaling status
enum class ElasticScalingStatus {
    IDLE,
    SCALING_UP,
    SCALING_DOWN,
    REBALANCING,
    ERROR
};

class ElasticScalingManager {
public:
    ElasticScalingManager(const ElasticScalingConfig& config = ElasticScalingConfig());
    ~ElasticScalingManager();
    
    // Initialize the elastic scaling manager
    bool initialize();
    
    // Shutdown the elastic scaling manager
    void shutdown();
    
    // Add a node to the cluster
    bool addNode(const std::string& nodeId, const std::string& address, int port);
    
    // Remove a node from the cluster
    bool removeNode(const std::string& nodeId);
    
    // Trigger manual rebalancing
    bool triggerRebalancing();
    
    // Update resource metrics for a node
    bool updateResourceMetrics(const ResourceMetrics& metrics);
    
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
    
    // Get elastic scaling status
    ElasticScalingStatus getElasticScalingStatus() const;
    
    // Get cluster size
    size_t getClusterSize() const;
    
    // Get active cluster size
    size_t getActiveClusterSize() const;
    
    // Register callbacks
    void registerNodeAddedCallback(const NodeAddedCallback& callback);
    void registerNodeRemovedCallback(const NodeRemovedCallback& callback);
    void registerNodeStatusCallback(const NodeStatusCallback& callback);
    void registerScalingDecisionCallback(const ScalingDecisionCallback& callback);
    void registerDataMoveCallback(const DataMoveCallback& callback);

private:
    // Configuration
    ElasticScalingConfig config_;
    
    // Node manager
    std::unique_ptr<NodeManager> nodeManager_;
    
    // Data rebalancer
    std::unique_ptr<DataRebalancer> dataRebalancer_;
    
    // Resource scaler
    std::unique_ptr<ResourceScaler> resourceScaler_;
    
    // Elastic scaling status
    std::atomic<ElasticScalingStatus> elasticScalingStatus_;
    
    // Mutex for thread safety
    mutable std::mutex managerMutex_;
    
    // Internal callback handlers
    void onNodeAdded(const NodeInfo& node);
    void onNodeRemoved(const std::string& nodeId);
    void onNodeStatus(const NodeStatus& status);
    void onScalingDecision(const ScalingDecision& decision);
    void onRebalanceStart();
    void onRebalanceComplete(bool success);
    bool onDataMove(const std::string& shardId, const std::string& fromNode, const std::string& toNode);
    bool onNodeScaleUp(const std::string& nodeId);
    bool onNodeScaleDown(const std::string& nodeId);
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_ELASTIC_SCALING_MANAGER_H