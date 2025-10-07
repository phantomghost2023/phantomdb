#include "resource_scaler.h"
#include <iostream>
#include <algorithm>
#include <thread>

namespace phantomdb {
namespace distributed {

ResourceScaler::ResourceScaler(ScalingPolicy policy)
    : policy_(policy), evaluationInterval_(std::chrono::milliseconds(5000)), // 5 seconds
      running_(false) {
    std::cout << "Creating ResourceScaler with policy " << static_cast<int>(policy) << std::endl;
}

ResourceScaler::~ResourceScaler() {
    if (running_) {
        shutdown();
    }
    std::cout << "Destroying ResourceScaler" << std::endl;
}

bool ResourceScaler::initialize() {
    std::cout << "Initializing ResourceScaler" << std::endl;
    
    if (policy_.load() == ScalingPolicy::AUTOMATIC) {
        running_ = true;
        scalerThread_ = std::make_unique<std::thread>(&ResourceScaler::evaluateAndScale, this);
        std::cout << "Started automatic scaling thread" << std::endl;
    }
    
    std::cout << "ResourceScaler initialized successfully" << std::endl;
    return true;
}

void ResourceScaler::shutdown() {
    std::cout << "Shutting down ResourceScaler" << std::endl;
    
    running_ = false;
    
    if (scalerThread_ && scalerThread_->joinable()) {
        scalerThread_->join();
        scalerThread_.reset();
    }
    
    // Clear node metrics
    {
        std::lock_guard<std::mutex> lock(scalerMutex_);
        nodeMetrics_.clear();
    }
    
    std::cout << "ResourceScaler shutdown completed" << std::endl;
}

bool ResourceScaler::updateResourceMetrics(const ResourceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(scalerMutex_);
    
    nodeMetrics_[metrics.nodeId] = metrics;
    std::cout << "Updated resource metrics for node " << metrics.nodeId << std::endl;
    
    return true;
}

ResourceMetrics ResourceScaler::getResourceMetrics(const std::string& nodeId) const {
    std::lock_guard<std::mutex> lock(scalerMutex_);
    
    auto it = nodeMetrics_.find(nodeId);
    if (it != nodeMetrics_.end()) {
        return it->second;
    }
    
    return ResourceMetrics(nodeId); // Return default metrics
}

std::vector<ResourceMetrics> ResourceScaler::getAllResourceMetrics() const {
    std::lock_guard<std::mutex> lock(scalerMutex_);
    
    std::vector<ResourceMetrics> result;
    result.reserve(nodeMetrics_.size());
    
    for (const auto& pair : nodeMetrics_) {
        result.push_back(pair.second);
    }
    
    return result;
}

ScalingDecision ResourceScaler::evaluateScalingNeeds() const {
    std::lock_guard<std::mutex> lock(scalerMutex_);
    
    // Check if any node needs scaling up
    for (const auto& pair : nodeMetrics_) {
        const auto& metrics = pair.second;
        if (shouldScaleUp(metrics)) {
            std::string reason = "Node " + metrics.nodeId + " exceeded resource thresholds";
            return ScalingDecision(ScalingAction::SCALE_UP, reason);
        }
    }
    
    // Check if we can scale down (if all nodes are under low load)
    bool canScaleDown = true;
    std::string lowLoadNode;
    
    for (const auto& pair : nodeMetrics_) {
        const auto& metrics = pair.second;
        if (!shouldScaleDown(metrics)) {
            canScaleDown = false;
            break;
        }
        lowLoadNode = metrics.nodeId;
    }
    
    if (canScaleDown && !nodeMetrics_.empty()) {
        std::string reason = "Node " + lowLoadNode + " under low load, can scale down";
        return ScalingDecision(ScalingAction::SCALE_DOWN, reason);
    }
    
    // No scaling needed
    return ScalingDecision(ScalingAction::NO_ACTION, "No scaling needed");
}

bool ResourceScaler::executeScalingDecision(const ScalingDecision& decision) {
    std::cout << "Executing scaling decision: " << 
                ((decision.action == ScalingAction::SCALE_UP) ? "SCALE_UP" :
                (decision.action == ScalingAction::SCALE_DOWN) ? "SCALE_DOWN" : "NO_ACTION") <<
                " - " << decision.reason << std::endl;
    
    bool success = true;
    
    switch (decision.action) {
        case ScalingAction::SCALE_UP:
            {
                std::string nodeToScale = getNodeWithHighestLoad();
                if (!nodeToScale.empty() && nodeScaleUpCallback_) {
                    success = nodeScaleUpCallback_(nodeToScale);
                    if (success) {
                        std::cout << "Successfully scaled up node " << nodeToScale << std::endl;
                    } else {
                        std::cout << "Failed to scale up node " << nodeToScale << std::endl;
                    }
                }
            }
            break;
            
        case ScalingAction::SCALE_DOWN:
            {
                std::string nodeToScale = getNodeWithLowestLoad();
                if (!nodeToScale.empty() && nodeScaleDownCallback_) {
                    success = nodeScaleDownCallback_(nodeToScale);
                    if (success) {
                        std::cout << "Successfully scaled down node " << nodeToScale << std::endl;
                    } else {
                        std::cout << "Failed to scale down node " << nodeToScale << std::endl;
                    }
                }
            }
            break;
            
        case ScalingAction::NO_ACTION:
            std::cout << "No scaling action needed" << std::endl;
            break;
    }
    
    // Notify callback
    if (scalingDecisionCallback_) {
        scalingDecisionCallback_(decision);
    }
    
    return success;
}

void ResourceScaler::setScalingTrigger(const ScalingTrigger& trigger) {
    std::lock_guard<std::mutex> lock(scalerMutex_);
    scalingTrigger_ = trigger;
    std::cout << "Scaling trigger updated" << std::endl;
}

ScalingTrigger ResourceScaler::getScalingTrigger() const {
    std::lock_guard<std::mutex> lock(scalerMutex_);
    return scalingTrigger_;
}

ScalingPolicy ResourceScaler::getScalingPolicy() const {
    return policy_.load();
}

void ResourceScaler::setScalingPolicy(ScalingPolicy policy) {
    ScalingPolicy oldPolicy = policy_.load();
    policy_.store(policy);
    
    if (oldPolicy != ScalingPolicy::AUTOMATIC && policy == ScalingPolicy::AUTOMATIC) {
        // Start automatic scaling thread
        if (!running_) {
            running_ = true;
            scalerThread_ = std::make_unique<std::thread>(&ResourceScaler::evaluateAndScale, this);
            std::cout << "Started automatic scaling thread" << std::endl;
        }
    } else if (oldPolicy == ScalingPolicy::AUTOMATIC && policy != ScalingPolicy::AUTOMATIC) {
        // Stop automatic scaling thread
        running_ = false;
        if (scalerThread_ && scalerThread_->joinable()) {
            scalerThread_->join();
            scalerThread_.reset();
            std::cout << "Stopped automatic scaling thread" << std::endl;
        }
    }
    
    std::cout << "Scaling policy changed from " << 
                static_cast<int>(oldPolicy) << " to " << static_cast<int>(policy) << std::endl;
}

void ResourceScaler::registerScalingDecisionCallback(const ScalingDecisionCallback& callback) {
    std::lock_guard<std::mutex> lock(scalerMutex_);
    scalingDecisionCallback_ = callback;
    std::cout << "Scaling decision callback registered" << std::endl;
}

void ResourceScaler::registerNodeScaleUpCallback(const NodeScaleUpCallback& callback) {
    std::lock_guard<std::mutex> lock(scalerMutex_);
    nodeScaleUpCallback_ = callback;
    std::cout << "Node scale up callback registered" << std::endl;
}

void ResourceScaler::registerNodeScaleDownCallback(const NodeScaleDownCallback& callback) {
    std::lock_guard<std::mutex> lock(scalerMutex_);
    nodeScaleDownCallback_ = callback;
    std::cout << "Node scale down callback registered" << std::endl;
}

void ResourceScaler::setEvaluationInterval(const std::chrono::milliseconds& interval) {
    evaluationInterval_.store(interval);
    std::cout << "Evaluation interval set to " << interval.count() << " ms" << std::endl;
}

std::chrono::milliseconds ResourceScaler::getEvaluationInterval() const {
    return evaluationInterval_.load();
}

void ResourceScaler::evaluateAndScale() {
    std::cout << "Starting automatic scaling evaluation thread" << std::endl;
    
    while (running_) {
        try {
            // Evaluate scaling needs
            ScalingDecision decision = evaluateScalingNeeds();
            
            // Execute scaling decision if needed
            if (decision.action != ScalingAction::NO_ACTION) {
                executeScalingDecision(decision);
            }
            
            // Sleep for evaluation interval
            std::this_thread::sleep_for(evaluationInterval_.load());
        } catch (const std::exception& e) {
            std::cout << "Exception in scaling evaluation thread: " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Automatic scaling evaluation thread ended" << std::endl;
}

bool ResourceScaler::shouldScaleUp(const ResourceMetrics& metrics) const {
    const ScalingTrigger& trigger = scalingTrigger_;
    
    return (metrics.cpuUsage > trigger.cpuThreshold) ||
           (metrics.memoryUsage > trigger.memoryThreshold) ||
           (metrics.diskUsage > trigger.diskThreshold) ||
           (metrics.queryRate > trigger.queryRateThreshold) ||
           (metrics.transactionRate > trigger.transactionRateThreshold);
}

bool ResourceScaler::shouldScaleDown(const ResourceMetrics& metrics) const {
    const ScalingTrigger& trigger = scalingTrigger_;
    
    // Scale down if all metrics are below a certain percentage of thresholds
    double scaleDownThreshold = trigger.cpuThreshold * 0.3; // 30% of threshold
    
    return (metrics.cpuUsage < scaleDownThreshold) &&
           (metrics.memoryUsage < scaleDownThreshold) &&
           (metrics.diskUsage < scaleDownThreshold) &&
           (metrics.queryRate < trigger.queryRateThreshold * 0.3) &&
           (metrics.transactionRate < trigger.transactionRateThreshold * 0.3);
}

std::string ResourceScaler::getNodeWithHighestLoad() const {
    if (nodeMetrics_.empty()) {
        return "";
    }
    
    auto maxIt = std::max_element(nodeMetrics_.begin(), nodeMetrics_.end(),
        [](const auto& a, const auto& b) {
            const ResourceMetrics& metricsA = a.second;
            const ResourceMetrics& metricsB = b.second;
            double loadA = metricsA.cpuUsage + metricsA.memoryUsage + metricsA.diskUsage;
            double loadB = metricsB.cpuUsage + metricsB.memoryUsage + metricsB.diskUsage;
            return loadA < loadB;
        });
    
    return maxIt->first;
}

std::string ResourceScaler::getNodeWithLowestLoad() const {
    if (nodeMetrics_.empty()) {
        return "";
    }
    
    auto minIt = std::min_element(nodeMetrics_.begin(), nodeMetrics_.end(),
        [](const auto& a, const auto& b) {
            const ResourceMetrics& metricsA = a.second;
            const ResourceMetrics& metricsB = b.second;
            double loadA = metricsA.cpuUsage + metricsA.memoryUsage + metricsA.diskUsage;
            double loadB = metricsB.cpuUsage + metricsB.memoryUsage + metricsB.diskUsage;
            return loadA < loadB;
        });
    
    return minIt->first;
}

} // namespace distributed
} // namespace phantomdb