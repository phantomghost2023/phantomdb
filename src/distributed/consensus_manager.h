#ifndef PHANTOMDB_CONSENSUS_MANAGER_H
#define PHANTOMDB_CONSENSUS_MANAGER_H

#include "raft.h"
#include "replication_manager.h"
#include "conflict_resolver.h"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <functional>

namespace phantomdb {
namespace distributed {

// Consensus manager configuration
struct ConsensusConfig {
    std::string nodeId;
    std::vector<RaftNodeInfo> raftNodes;
    ReplicationStrategy replicationStrategy;
    ConflictResolutionStrategy conflictResolutionStrategy;
    
    ConsensusConfig() : replicationStrategy(ReplicationStrategy::ASYNCHRONOUS),
                       conflictResolutionStrategy(ConflictResolutionStrategy::LATEST_TIMESTAMP) {}
};

class ConsensusManager {
public:
    ConsensusManager(const ConsensusConfig& config);
    ~ConsensusManager();
    
    // Initialize the consensus manager
    bool initialize();
    
    // Shutdown the consensus manager
    void shutdown();
    
    // Submit a command to be replicated and committed
    bool submitCommand(const std::string& command, const std::string& data);
    
    // Register callback for applying commands
    void registerApplyCommandCallback(const ApplyCommandCallback& callback);
    
    // Register custom conflict resolver
    void registerCustomConflictResolver(const CustomConflictResolver& resolver);
    
    // Add a region for replication
    bool addReplicationRegion(const RegionInfo& region);
    
    // Remove a replication region
    bool removeReplicationRegion(const std::string& regionId);
    
    // Replicate data to all regions
    bool replicateData(const std::string& key, const std::string& value);
    
    // Get current leader
    std::string getLeader() const;
    
    // Get current term
    uint64_t getCurrentTerm() const;
    
    // Get node state
    RaftState getNodeState() const;
    
    // Get commit index
    uint64_t getCommitIndex() const;
    
    // Get last applied index
    uint64_t getLastApplied() const;
    
    // Get replication status
    std::vector<ReplicationStatus> getReplicationStatus() const;
    
    // Get conflict resolution statistics
    ConflictResolver::ResolutionStats getConflictResolutionStats() const;

private:
    // Configuration
    ConsensusConfig config_;
    
    // Raft consensus module
    std::unique_ptr<RaftConsensus> raft_;
    
    // Replication manager
    std::unique_ptr<ReplicationManager> replicationManager_;
    
    // Conflict resolver
    std::unique_ptr<ConflictResolver> conflictResolver_;
    
    // Mutex for thread safety
    mutable std::mutex managerMutex_;
    
    // Internal callbacks
    void onApplyCommand(const std::string& command, const std::string& data);
    bool onSendRequestVote(const RaftNodeInfo& node, const RequestVoteRequest& request, RequestVoteResponse& response);
    bool onSendAppendEntries(const RaftNodeInfo& node, const AppendEntriesRequest& request, AppendEntriesResponse& response);
    
    // Network simulation methods (in a real implementation, these would use actual network communication)
    bool sendRequestVote(const RaftNodeInfo& node, const RequestVoteRequest& request, RequestVoteResponse& response);
    bool sendAppendEntries(const RaftNodeInfo& node, const AppendEntriesRequest& request, AppendEntriesResponse& response);
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_CONSENSUS_MANAGER_H