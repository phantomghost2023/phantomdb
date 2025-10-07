#include "consensus_manager.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace phantomdb {
namespace distributed {

ConsensusManager::ConsensusManager(const ConsensusConfig& config)
    : config_(config) {
    std::cout << "Creating ConsensusManager for node " << config_.nodeId << std::endl;
}

ConsensusManager::~ConsensusManager() {
    if (raft_ && replicationManager_ && conflictResolver_) {
        shutdown();
    }
    std::cout << "Destroying ConsensusManager for node " << config_.nodeId << std::endl;
}

bool ConsensusManager::initialize() {
    std::cout << "Initializing ConsensusManager for node " << config_.nodeId << std::endl;
    
    try {
        // Initialize Raft consensus
        raft_ = std::make_unique<RaftConsensus>(config_.nodeId, config_.raftNodes);
        
        // Register callbacks
        raft_->registerApplyCommandCallback(
            [this](const std::string& command, const std::string& data) {
                this->onApplyCommand(command, data);
            }
        );
        
        raft_->registerSendRequestVoteCallback(
            [this](const RaftNodeInfo& node, const RequestVoteRequest& request, RequestVoteResponse& response) {
                return this->onSendRequestVote(node, request, response);
            }
        );
        
        raft_->registerSendAppendEntriesCallback(
            [this](const RaftNodeInfo& node, const AppendEntriesRequest& request, AppendEntriesResponse& response) {
                return this->onSendAppendEntries(node, request, response);
            }
        );
        
        // Initialize replication manager
        replicationManager_ = std::make_unique<ReplicationManager>(config_.replicationStrategy);
        
        // Initialize conflict resolver
        conflictResolver_ = std::make_unique<ConflictResolver>(config_.conflictResolutionStrategy);
        
        // Initialize all components
        if (!raft_->initialize()) {
            std::cout << "Failed to initialize Raft consensus" << std::endl;
            return false;
        }
        
        if (!replicationManager_->initialize()) {
            std::cout << "Failed to initialize replication manager" << std::endl;
            return false;
        }
        
        if (!conflictResolver_->initialize()) {
            std::cout << "Failed to initialize conflict resolver" << std::endl;
            return false;
        }
        
        std::cout << "ConsensusManager initialized successfully for node " << config_.nodeId << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "Exception during ConsensusManager initialization: " << e.what() << std::endl;
        return false;
    }
}

void ConsensusManager::shutdown() {
    std::cout << "Shutting down ConsensusManager for node " << config_.nodeId << std::endl;
    
    // Shutdown components in reverse order
    if (conflictResolver_) {
        conflictResolver_->shutdown();
        conflictResolver_.reset();
    }
    
    if (replicationManager_) {
        replicationManager_->shutdown();
        replicationManager_.reset();
    }
    
    if (raft_) {
        raft_->shutdown();
        raft_.reset();
    }
    
    std::cout << "ConsensusManager shutdown completed for node " << config_.nodeId << std::endl;
}

bool ConsensusManager::submitCommand(const std::string& command, const std::string& data) {
    if (!raft_) {
        std::cout << "Raft consensus not initialized" << std::endl;
        return false;
    }
    
    std::cout << "Submitting command " << command << " for node " << config_.nodeId << std::endl;
    return raft_->submitCommand(command, data);
}

void ConsensusManager::registerApplyCommandCallback(const ApplyCommandCallback& callback) {
    // This would typically be used to register application-specific command handling
    // For now, we'll use the internal onApplyCommand method
    std::cout << "Apply command callback registered" << std::endl;
}

void ConsensusManager::registerCustomConflictResolver(const CustomConflictResolver& resolver) {
    if (!conflictResolver_) {
        std::cout << "Conflict resolver not initialized" << std::endl;
        return;
    }
    
    conflictResolver_->registerCustomResolver(resolver);
    std::cout << "Custom conflict resolver registered" << std::endl;
}

bool ConsensusManager::addReplicationRegion(const RegionInfo& region) {
    if (!replicationManager_) {
        std::cout << "Replication manager not initialized" << std::endl;
        return false;
    }
    
    return replicationManager_->addRegion(region);
}

bool ConsensusManager::removeReplicationRegion(const std::string& regionId) {
    if (!replicationManager_) {
        std::cout << "Replication manager not initialized" << std::endl;
        return false;
    }
    
    return replicationManager_->removeRegion(regionId);
}

bool ConsensusManager::replicateData(const std::string& key, const std::string& value) {
    if (!replicationManager_) {
        std::cout << "Replication manager not initialized" << std::endl;
        return false;
    }
    
    return replicationManager_->replicateData(key, value);
}

std::string ConsensusManager::getLeader() const {
    if (!raft_) {
        return "";
    }
    
    return raft_->getLeader();
}

uint64_t ConsensusManager::getCurrentTerm() const {
    if (!raft_) {
        return 0;
    }
    
    return raft_->getCurrentTerm();
}

RaftState ConsensusManager::getNodeState() const {
    if (!raft_) {
        return RaftState::FOLLOWER; // Default state
    }
    
    return raft_->getState();
}

uint64_t ConsensusManager::getCommitIndex() const {
    if (!raft_) {
        return 0;
    }
    
    return raft_->getCommitIndex();
}

uint64_t ConsensusManager::getLastApplied() const {
    if (!raft_) {
        return 0;
    }
    
    return raft_->getLastApplied();
}

std::vector<ReplicationStatus> ConsensusManager::getReplicationStatus() const {
    if (!replicationManager_) {
        return {};
    }
    
    return replicationManager_->getReplicationStatus();
}

ConflictResolver::ResolutionStats ConsensusManager::getConflictResolutionStats() const {
    if (!conflictResolver_) {
        return ConflictResolver::ResolutionStats();
    }
    
    return conflictResolver_->getResolutionStats();
}

void ConsensusManager::onApplyCommand(const std::string& command, const std::string& data) {
    std::cout << "Applying command " << command << " with data " << data << " for node " << config_.nodeId << std::endl;
    
    // In a real implementation, this would apply the command to the state machine
    // For now, we'll just log it and potentially trigger replication
    
    // If this is a write command, replicate it to other regions
    if (command == "WRITE" || command == "UPDATE" || command == "DELETE") {
        // Extract key from data (simplified)
        std::string key = data.substr(0, data.find(':'));
        replicateData(key, data);
    }
}

bool ConsensusManager::onSendRequestVote(const RaftNodeInfo& node, const RequestVoteRequest& request, RequestVoteResponse& response) {
    std::cout << "Sending RequestVote from node " << config_.nodeId << " to node " << node.id << std::endl;
    
    // In a real implementation, this would send an RPC to the target node
    // For now, we'll simulate the network communication
    return sendRequestVote(node, request, response);
}

bool ConsensusManager::onSendAppendEntries(const RaftNodeInfo& node, const AppendEntriesRequest& request, AppendEntriesResponse& response) {
    std::cout << "Sending AppendEntries from node " << config_.nodeId << " to node " << node.id << std::endl;
    
    // In a real implementation, this would send an RPC to the target node
    // For now, we'll simulate the network communication
    return sendAppendEntries(node, request, response);
}

bool ConsensusManager::sendRequestVote(const RaftNodeInfo& node, const RequestVoteRequest& request, RequestVoteResponse& response) {
    // Simulate network communication delay
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Simulate successful RPC response
    response.term = request.term;
    response.voteGranted = (node.id != config_.nodeId); // Don't vote for self in simulation
    
    std::cout << "RequestVote response from node " << node.id << ": voteGranted=" << response.voteGranted << std::endl;
    return true;
}

bool ConsensusManager::sendAppendEntries(const RaftNodeInfo& node, const AppendEntriesRequest& request, AppendEntriesResponse& response) {
    // Simulate network communication delay
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    
    // Simulate successful RPC response
    response.term = request.term;
    response.success = true;
    response.conflictIndex = 0;
    response.conflictTerm = 0;
    
    std::cout << "AppendEntries response from node " << node.id << ": success=" << response.success << std::endl;
    return true;
}

} // namespace distributed
} // namespace phantomdb