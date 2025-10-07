#include "consensus_manager.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace phantomdb::distributed;

int main() {
    // No equivalent to spdlog::set_level in iostream, so we'll just output all messages
    
    try {
        std::cout << "Starting PhantomDB Consensus and Replication Test" << std::endl;
        
        // Create Raft node information
        std::vector<RaftNodeInfo> raftNodes = {
            RaftNodeInfo("node1", "192.168.1.101", 8001),
            RaftNodeInfo("node2", "192.168.1.102", 8002),
            RaftNodeInfo("node3", "192.168.1.103", 8003)
        };
        
        // Create consensus configuration
        ConsensusConfig config;
        config.nodeId = "node1";
        config.raftNodes = raftNodes;
        config.replicationStrategy = ReplicationStrategy::ASYNCHRONOUS;
        config.conflictResolutionStrategy = ConflictResolutionStrategy::LATEST_TIMESTAMP;
        
        // Create consensus manager
        ConsensusManager consensusManager(config);
        if (!consensusManager.initialize()) {
            std::cout << "Failed to initialize ConsensusManager" << std::endl;
            return 1;
        }
        
        // Add replication regions
        consensusManager.addReplicationRegion(RegionInfo("region1", "10.0.1.1", 9001, true));
        consensusManager.addReplicationRegion(RegionInfo("region2", "10.0.2.1", 9002));
        consensusManager.addReplicationRegion(RegionInfo("region3", "10.0.3.1", 9003));
        
        // Test submitting commands
        std::cout << "Testing command submission..." << std::endl;
        consensusManager.submitCommand("WRITE", "key1:value1");
        consensusManager.submitCommand("UPDATE", "key1:new_value1");
        consensusManager.submitCommand("DELETE", "key1");
        
        // Test replication
        std::cout << "Testing data replication..." << std::endl;
        consensusManager.replicateData("test_key", "test_value");
        
        // Check consensus state
        std::cout << "Checking consensus state..." << std::endl;
        std::cout << "Current leader: " << consensusManager.getLeader() << std::endl;
        std::cout << "Current term: " << consensusManager.getCurrentTerm() << std::endl;
        std::cout << "Node state: " << static_cast<int>(consensusManager.getNodeState()) << std::endl;
        std::cout << "Commit index: " << consensusManager.getCommitIndex() << std::endl;
        std::cout << "Last applied: " << consensusManager.getLastApplied() << std::endl;
        
        // Check replication status
        auto replicationStatus = consensusManager.getReplicationStatus();
        std::cout << "Replication status for " << replicationStatus.size() << " regions:" << std::endl;
        for (const auto& status : replicationStatus) {
            std::cout << "  Region " << status.regionId << ": connected=" << status.isConnected 
                      << ", last_replicated=" << status.lastReplicatedIndex << std::endl;
        }
        
        // Check conflict resolution stats
        auto conflictStats = consensusManager.getConflictResolutionStats();
        std::cout << "Conflict resolution stats:" << std::endl;
        std::cout << "  Total conflicts: " << conflictStats.totalConflicts << std::endl;
        std::cout << "  Resolved by timestamp: " << conflictStats.resolvedByTimestamp << std::endl;
        std::cout << "  Resolved by LWW: " << conflictStats.resolvedByLWW << std::endl;
        std::cout << "  Resolved by multi-value: " << conflictStats.resolvedByMultiValue << std::endl;
        std::cout << "  Resolved by custom: " << conflictStats.resolvedByCustom << std::endl;
        std::cout << "  Unresolved: " << conflictStats.unresolvedConflicts << std::endl;
        
        // Wait a bit to let the consensus algorithm run
        std::cout << "Waiting for consensus algorithm to run..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        std::cout << "Consensus and Replication Test completed successfully" << std::endl;
        
        // Shutdown consensus manager
        consensusManager.shutdown();
        
    } catch (const std::exception& e) {
        std::cout << "Exception in consensus test: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}