#include "elastic_scaling_manager.h"
#include <iostream>

namespace phantomdb {
namespace distributed {

ElasticScalingManager::ElasticScalingManager(const ElasticScalingConfig& config)
    : config_(config), elasticScalingStatus_(ElasticScalingStatus::IDLE) {
    std::cout << "Creating ElasticScalingManager" << std::endl;
}

ElasticScalingManager::~ElasticScalingManager() {
    if (nodeManager_ || dataRebalancer_ || resourceScaler_) {
        shutdown();
    }
    std::cout << "Destroying ElasticScalingManager" << std::endl;
}

bool ElasticScalingManager::initialize() {
    std::cout << "Initializing ElasticScalingManager" << std::endl;
    
    try {
        // Initialize node manager
        nodeManager_ = std::make_unique<NodeManager>();
        if (!nodeManager_->initialize()) {
            std::cout << "Failed to initialize NodeManager" << std::endl;
            return false;
        }
        
        // Initialize data rebalancer
        dataRebalancer_ = std::make_unique<DataRebalancer>(config_.rebalancingStrategy);
        if (!dataRebalancer_->initialize()) {
            std::cout << "Failed to initialize DataRebalancer" << std::endl;
            return false;
        }
        
        // Initialize resource scaler
        resourceScaler_ = std::make_unique<ResourceScaler>(config_.scalingPolicy);
        if (!resourceScaler_->initialize()) {
            std::cout << "Failed to initialize ResourceScaler" << std::endl;
            return false;
        }
        
        // Register callbacks
        nodeManager_->registerNodeAddedCallback(
            [this](const NodeInfo& node) { this->onNodeAdded(node); });
        nodeManager_->registerNodeRemovedCallback(
            [this](const std::string& nodeId) { this->onNodeRemoved(nodeId); });
        nodeManager_->registerNodeStatusCallback(
            [this](const NodeStatus& status) { this->onNodeStatus(status); });
        
        resourceScaler_->registerScalingDecisionCallback(
            [this](const ScalingDecision& decision) { this->onScalingDecision(decision); });
        resourceScaler_->registerNodeScaleUpCallback(
            [this](const std::string& nodeId) { return this->onNodeScaleUp(nodeId); });
        resourceScaler_->registerNodeScaleDownCallback(
            [this](const std::string& nodeId) { return this->onNodeScaleDown(nodeId); });
        
        dataRebalancer_->registerRebalanceStartCallback(
            [this]() { this->onRebalanceStart(); });
        dataRebalancer_->registerRebalanceCompleteCallback(
            [this](bool success) { this->onRebalanceComplete(success); });
        dataRebalancer_->registerDataMoveCallback(
            [this](const std::string& shardId, const std::string& fromNode, const std::string& toNode) {
                return this->onDataMove(shardId, fromNode, toNode);
            });
        
        // Configure components
        resourceScaler_->setScalingTrigger(config_.scalingTrigger);
        resourceScaler_->setEvaluationInterval(config_.evaluationInterval);
        dataRebalancer_->setRebalancingThreshold(config_.rebalancingThreshold);
        
        std::cout << "ElasticScalingManager initialized successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "Exception during ElasticScalingManager initialization: " << e.what() << std::endl;
        return false;
    }
}

void ElasticScalingManager::shutdown() {
    std::cout << "Shutting down ElasticScalingManager" << std::endl;
    
    // Shutdown components in reverse order
    if (resourceScaler_) {
        resourceScaler_->shutdown();
        resourceScaler_.reset();
    }
    
    if (dataRebalancer_) {
        dataRebalancer_->shutdown();
        dataRebalancer_.reset();
    }
    
    if (nodeManager_) {
        nodeManager_->shutdown();
        nodeManager_.reset();
    }
    
    std::cout << "ElasticScalingManager shutdown completed" << std::endl;
}

bool ElasticScalingManager::addNode(const std::string& nodeId, const std::string& address, int port) {
    if (!nodeManager_) {
        std::cout << "NodeManager not initialized" << std::endl;
        return false;
    }
    
    bool success = nodeManager_->addNode(nodeId, address, port);
    
    if (success) {
        std::cout << "Added node " << nodeId << " to cluster" << std::endl;
        
        // Update data rebalancer with new node
        if (dataRebalancer_) {
            auto allNodes = nodeManager_->getAllNodes();
            std::vector<std::string> nodeIds;
            nodeIds.reserve(allNodes.size());
            
            for (const auto& node : allNodes) {
                nodeIds.push_back(node->id);
            }
            
            // Recreate shards with new node distribution
            dataRebalancer_->createShards(nodeIds, nodeIds.size() * 2); // 2 shards per node
        }
    }
    
    return success;
}

bool ElasticScalingManager::removeNode(const std::string& nodeId) {
    if (!nodeManager_) {
        std::cout << "NodeManager not initialized" << std::endl;
        return false;
    }
    
    bool success = nodeManager_->removeNode(nodeId);
    
    if (success) {
        std::cout << "Removed node " << nodeId << " from cluster" << std::endl;
        
        // Update data rebalancer after node removal
        if (dataRebalancer_) {
            auto allNodes = nodeManager_->getAllNodes();
            std::vector<std::string> nodeIds;
            nodeIds.reserve(allNodes.size());
            
            for (const auto& node : allNodes) {
                nodeIds.push_back(node->id);
            }
            
            if (!nodeIds.empty()) {
                // Recreate shards with updated node distribution
                dataRebalancer_->createShards(nodeIds, nodeIds.size() * 2); // 2 shards per node
            }
        }
    }
    
    return success;
}

bool ElasticScalingManager::triggerRebalancing() {
    if (!dataRebalancer_) {
        std::cout << "DataRebalancer not initialized" << std::endl;
        return false;
    }
    
    if (!nodeManager_) {
        std::cout << "NodeManager not initialized" << std::endl;
        return false;
    }
    
    auto allNodes = nodeManager_->getAllNodes();
    std::vector<std::string> nodeIds;
    nodeIds.reserve(allNodes.size());
    
    for (const auto& node : allNodes) {
        nodeIds.push_back(node->id);
    }
    
    if (nodeIds.empty()) {
        std::cout << "No nodes available for rebalancing" << std::endl;
        return false;
    }
    
    elasticScalingStatus_.store(ElasticScalingStatus::REBALANCING);
    bool success = dataRebalancer_->rebalanceShards(nodeIds);
    elasticScalingStatus_.store(success ? ElasticScalingStatus::IDLE : ElasticScalingStatus::ERROR);
    
    return success;
}

bool ElasticScalingManager::updateResourceMetrics(const ResourceMetrics& metrics) {
    if (!resourceScaler_) {
        std::cout << "ResourceScaler not initialized" << std::endl;
        return false;
    }
    
    return resourceScaler_->updateResourceMetrics(metrics);
}

std::shared_ptr<NodeInfo> ElasticScalingManager::getNode(const std::string& nodeId) const {
    if (!nodeManager_) {
        return nullptr;
    }
    
    return nodeManager_->getNode(nodeId);
}

std::vector<std::shared_ptr<NodeInfo>> ElasticScalingManager::getAllNodes() const {
    if (!nodeManager_) {
        return {};
    }
    
    return nodeManager_->getAllNodes();
}

std::vector<std::shared_ptr<NodeInfo>> ElasticScalingManager::getActiveNodes() const {
    if (!nodeManager_) {
        return {};
    }
    
    return nodeManager_->getActiveNodes();
}

NodeStatus ElasticScalingManager::getNodeStatus(const std::string& nodeId) const {
    if (!nodeManager_) {
        return NodeStatus(nodeId);
    }
    
    return nodeManager_->getNodeStatus(nodeId);
}

std::vector<NodeStatus> ElasticScalingManager::getAllNodeStatus() const {
    if (!nodeManager_) {
        return {};
    }
    
    return nodeManager_->getAllNodeStatus();
}

ElasticScalingStatus ElasticScalingManager::getElasticScalingStatus() const {
    return elasticScalingStatus_.load();
}

size_t ElasticScalingManager::getClusterSize() const {
    if (!nodeManager_) {
        return 0;
    }
    
    return nodeManager_->getClusterSize();
}

size_t ElasticScalingManager::getActiveClusterSize() const {
    if (!nodeManager_) {
        return 0;
    }
    
    return nodeManager_->getActiveClusterSize();
}

void ElasticScalingManager::registerNodeAddedCallback(const NodeAddedCallback& callback) {
    if (nodeManager_) {
        nodeManager_->registerNodeAddedCallback(callback);
    }
}

void ElasticScalingManager::registerNodeRemovedCallback(const NodeRemovedCallback& callback) {
    if (nodeManager_) {
        nodeManager_->registerNodeRemovedCallback(callback);
    }
}

void ElasticScalingManager::registerNodeStatusCallback(const NodeStatusCallback& callback) {
    if (nodeManager_) {
        nodeManager_->registerNodeStatusCallback(callback);
    }
}

void ElasticScalingManager::registerScalingDecisionCallback(const ScalingDecisionCallback& callback) {
    if (resourceScaler_) {
        resourceScaler_->registerScalingDecisionCallback(callback);
    }
}

void ElasticScalingManager::registerDataMoveCallback(const DataMoveCallback& callback) {
    if (dataRebalancer_) {
        dataRebalancer_->registerDataMoveCallback(callback);
    }
}

void ElasticScalingManager::onNodeAdded(const NodeInfo& node) {
    std::cout << "Node added callback: " << node.id << std::endl;
    // Additional handling can be added here if needed
}

void ElasticScalingManager::onNodeRemoved(const std::string& nodeId) {
    std::cout << "Node removed callback: " << nodeId << std::endl;
    // Additional handling can be added here if needed
}

void ElasticScalingManager::onNodeStatus(const NodeStatus& status) {
    std::cout << "Node status callback: " << status.id << " - active: " << status.isActive << std::endl;
    // Additional handling can be added here if needed
}

void ElasticScalingManager::onScalingDecision(const ScalingDecision& decision) {
    std::cout << "Scaling decision callback: " << 
                ((decision.action == ScalingAction::SCALE_UP) ? "SCALE_UP" :
                 (decision.action == ScalingAction::SCALE_DOWN) ? "SCALE_DOWN" : "NO_ACTION") << 
                " - " << decision.reason << std::endl;
    
    // Execute the scaling decision
    if (resourceScaler_) {
        resourceScaler_->executeScalingDecision(decision);
    }
}

void ElasticScalingManager::onRebalanceStart() {
    std::cout << "Rebalancing started" << std::endl;
    elasticScalingStatus_.store(ElasticScalingStatus::REBALANCING);
}

void ElasticScalingManager::onRebalanceComplete(bool success) {
    std::cout << "Rebalancing completed: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    elasticScalingStatus_.store(success ? ElasticScalingStatus::IDLE : ElasticScalingStatus::ERROR);
}

bool ElasticScalingManager::onDataMove(const std::string& shardId, const std::string& fromNode, const std::string& toNode) {
    std::cout << "Data move callback: shard " << shardId << " from " << fromNode << " to " << toNode << std::endl;
    
    // In a real implementation, this would execute the actual data movement
    // For now, we'll just simulate success
    return true;
}

bool ElasticScalingManager::onNodeScaleUp(const std::string& nodeId) {
    std::cout << "Node scale up callback: " << nodeId << std::endl;
    
    // In a real implementation, this would actually scale up the node
    // For now, we'll just simulate success
    return true;
}

bool ElasticScalingManager::onNodeScaleDown(const std::string& nodeId) {
    std::cout << "Node scale down callback: " << nodeId << std::endl;
    
    // In a real implementation, this would actually scale down the node
    // For now, we'll just simulate success
    return true;
}

} // namespace distributed
} // namespace phantomdb