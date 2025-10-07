#ifndef PHANTOMDB_RESOURCE_SCALER_H
#define PHANTOMDB_RESOURCE_SCALER_H

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

// Resource metrics
struct ResourceMetrics {
    std::string nodeId;
    double cpuUsage;      // Percentage (0-100)
    double memoryUsage;   // Percentage (0-100)
    double diskUsage;     // Percentage (0-100)
    double networkUsage;  // Percentage (0-100)
    size_t queryRate;     // Queries per second
    size_t transactionRate; // Transactions per second
    
    ResourceMetrics() : nodeId(""), cpuUsage(0.0), memoryUsage(0.0), 
          diskUsage(0.0), networkUsage(0.0), 
          queryRate(0), transactionRate(0) {}
    
    ResourceMetrics(const std::string& id) 
        : nodeId(id), cpuUsage(0.0), memoryUsage(0.0), 
          diskUsage(0.0), networkUsage(0.0), 
          queryRate(0), transactionRate(0) {}
};

// Scaling policy
enum class ScalingPolicy {
    AUTOMATIC,
    MANUAL,
    SCHEDULED
};

// Scaling action
enum class ScalingAction {
    SCALE_UP,
    SCALE_DOWN,
    NO_ACTION
};

// Scaling trigger
struct ScalingTrigger {
    double cpuThreshold;      // CPU usage threshold (percentage)
    double memoryThreshold;   // Memory usage threshold (percentage)
    double diskThreshold;     // Disk usage threshold (percentage)
    size_t queryRateThreshold; // Query rate threshold
    size_t transactionRateThreshold; // Transaction rate threshold
    
    ScalingTrigger() 
        : cpuThreshold(80.0), memoryThreshold(80.0), diskThreshold(80.0),
          queryRateThreshold(1000), transactionRateThreshold(100) {}
};

// Scaling decision
struct ScalingDecision {
    ScalingAction action;
    std::string reason;
    std::chrono::steady_clock::time_point decisionTime;
    
    ScalingDecision(ScalingAction a, const std::string& r) 
        : action(a), reason(r) {
        decisionTime = std::chrono::steady_clock::now();
    }
};

// Callback function types
using ScalingDecisionCallback = std::function<void(const ScalingDecision&)>;
using NodeScaleUpCallback = std::function<bool(const std::string&)>;
using NodeScaleDownCallback = std::function<bool(const std::string&)>;

class ResourceScaler {
public:
    ResourceScaler(ScalingPolicy policy = ScalingPolicy::AUTOMATIC);
    ~ResourceScaler();
    
    // Initialize the resource scaler
    bool initialize();
    
    // Shutdown the resource scaler
    void shutdown();
    
    // Update resource metrics for a node
    bool updateResourceMetrics(const ResourceMetrics& metrics);
    
    // Get resource metrics for a node
    ResourceMetrics getResourceMetrics(const std::string& nodeId) const;
    
    // Get metrics for all nodes
    std::vector<ResourceMetrics> getAllResourceMetrics() const;
    
    // Evaluate scaling needs
    ScalingDecision evaluateScalingNeeds() const;
    
    // Execute scaling decision
    bool executeScalingDecision(const ScalingDecision& decision);
    
    // Set scaling trigger thresholds
    void setScalingTrigger(const ScalingTrigger& trigger);
    
    // Get scaling trigger
    ScalingTrigger getScalingTrigger() const;
    
    // Get scaling policy
    ScalingPolicy getScalingPolicy() const;
    
    // Set scaling policy
    void setScalingPolicy(ScalingPolicy policy);
    
    // Register callbacks
    void registerScalingDecisionCallback(const ScalingDecisionCallback& callback);
    void registerNodeScaleUpCallback(const NodeScaleUpCallback& callback);
    void registerNodeScaleDownCallback(const NodeScaleDownCallback& callback);
    
    // Set evaluation interval
    void setEvaluationInterval(const std::chrono::milliseconds& interval);
    
    // Get evaluation interval
    std::chrono::milliseconds getEvaluationInterval() const;

private:
    // Scaling policy
    std::atomic<ScalingPolicy> policy_;
    
    // Scaling trigger
    ScalingTrigger scalingTrigger_;
    
    // Resource metrics for nodes
    std::unordered_map<std::string, ResourceMetrics> nodeMetrics_;
    
    // Callbacks
    ScalingDecisionCallback scalingDecisionCallback_;
    NodeScaleUpCallback nodeScaleUpCallback_;
    NodeScaleDownCallback nodeScaleDownCallback_;
    
    // Evaluation interval
    std::atomic<std::chrono::milliseconds> evaluationInterval_;
    
    // Mutex for thread safety
    mutable std::mutex scalerMutex_;
    
    // Background thread for automatic scaling
    std::unique_ptr<std::thread> scalerThread_;
    std::atomic<bool> running_;
    
    // Internal methods
    void evaluateAndScale();
    bool shouldScaleUp(const ResourceMetrics& metrics) const;
    bool shouldScaleDown(const ResourceMetrics& metrics) const;
    std::string getNodeWithHighestLoad() const;
    std::string getNodeWithLowestLoad() const;
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_RESOURCE_SCALER_H