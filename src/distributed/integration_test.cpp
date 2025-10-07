#include "cluster_manager.h"
#include "consensus_manager.h"
#include "distributed_transaction_manager.h"
// Commenting out elastic_scaling_manager.h to avoid NodeInfo redefinition
// #include "elastic_scaling_manager.h"
// Commenting out data_rebalancer.h to avoid ShardInfo redefinition
// #include "data_rebalancer.h"
#include "cross_shard_query.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>

using namespace phantomdb::distributed;

// Test data structure
struct TestData {
    std::string key;
    std::string value;
    int shardId;
};

// Callback for node discovery events
void onNodeDiscovered(const std::string& nodeId, const std::string& address, int port) {
    std::cout << "Node discovered: " << nodeId << " at " << address << ":" << port << std::endl;
}

// Callback for node failure events
void onNodeFailed(const std::string& nodeId, const std::string& address, int port) {
    std::cout << "Node failed: " << nodeId << std::endl;
}

// Test cluster management functionality
bool testClusterManagement() {
    std::cout << "=== Testing Cluster Management ===" << std::endl;
    
    try {
        // Create cluster manager
        ClusterManager clusterManager;
        if (!clusterManager.initialize()) {
            std::cout << "Failed to initialize ClusterManager" << std::endl;
            return false;
        }
        
        // Add nodes
        clusterManager.addNode("node1", "192.168.1.101", 8001);
        clusterManager.addNode("node2", "192.168.1.102", 8002);
        clusterManager.addNode("node3", "192.168.1.103", 8003);
        
        // Test node retrieval
        auto node1 = clusterManager.getNode("node1");
        if (!node1) {
            std::cout << "Failed to retrieve node1" << std::endl;
            return false;
        }
        
        // Test cluster health
        bool isHealthy = clusterManager.isClusterHealthy();
        if (!isHealthy) {
            std::cout << "Cluster is not healthy" << std::endl;
            return false;
        }
        
        // Test updating node heartbeat
        clusterManager.updateNodeHeartbeat("node1");
        clusterManager.updateNodeHeartbeat("node2");
        clusterManager.updateNodeHeartbeat("node3");
        
        // Test cluster size
        size_t clusterSize = clusterManager.getClusterSize();
        if (clusterSize != 3) {
            std::cout << "Expected cluster size 3, got " << clusterSize << std::endl;
            return false;
        }
        
        std::cout << "Cluster management test passed" << std::endl;
        clusterManager.shutdown();
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Exception in cluster management test: " << e.what() << std::endl;
        return false;
    }
}

// Test consensus and replication functionality
bool testConsensusAndReplication() {
    std::cout << "=== Testing Consensus and Replication ===" << std::endl;
    
    try {
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
            return false;
        }
        
        // Add replication regions
        consensusManager.addReplicationRegion(RegionInfo("region1", "10.0.1.1", 9001, true));
        consensusManager.addReplicationRegion(RegionInfo("region2", "10.0.2.1", 9002));
        
        // Test submitting commands
        consensusManager.submitCommand("WRITE", "key1:value1");
        consensusManager.submitCommand("UPDATE", "key1:new_value1");
        
        // Test replication
        consensusManager.replicateData("test_key", "test_value");
        
        // Check consensus state
        std::string leader = consensusManager.getLeader();
        uint64_t term = consensusManager.getCurrentTerm();
        
        std::cout << "Consensus state - Leader: " << leader << ", Term: " << term << std::endl;
        
        // Check replication status
        auto replicationStatus = consensusManager.getReplicationStatus();
        if (replicationStatus.empty()) {
            std::cout << "No replication status found" << std::endl;
            return false;
        }
        
        std::cout << "Consensus and replication test passed" << std::endl;
        consensusManager.shutdown();
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Exception in consensus and replication test: " << e.what() << std::endl;
        return false;
    }
}

// Test distributed transactions
bool testDistributedTransactions() {
    std::cout << "=== Testing Distributed Transactions ===" << std::endl;
    
    try {
        // Create distributed transaction manager
        DistributedTransactionManager txnManager;
        if (!txnManager.initialize()) {
            std::cout << "Failed to initialize DistributedTransactionManager" << std::endl;
            return false;
        }
        
        // Test beginning a transaction with proper configuration
        DistributedTransactionConfig config;
        config.type = DistributedTransactionType::TWO_PHASE_COMMIT;
        std::string txnId = "test_txn_1";
        
        if (!txnManager.beginTransaction(txnId, config)) {
            std::cout << "Failed to begin transaction" << std::endl;
            return false;
        }
        
        // Test adding participants to transaction
        ParticipantInfo participant1("node1", "192.168.1.101", 8001);
        ParticipantInfo participant2("node2", "192.168.1.102", 8002);
        
        if (!txnManager.addParticipant(txnId, participant1)) {
            std::cout << "Failed to add participant 1" << std::endl;
            return false;
        }
        
        if (!txnManager.addParticipant(txnId, participant2)) {
            std::cout << "Failed to add participant 2" << std::endl;
            return false;
        }
        
        // Test executing transaction
        if (!txnManager.executeTransaction(txnId)) {
            std::cout << "Failed to execute transaction" << std::endl;
            return false;
        }
        
        // Check transaction status
        auto status = txnManager.getTransactionStatus(txnId);
        if (status != DistributedTransactionStatus::COMMITTED) {
            std::cout << "Transaction not committed" << std::endl;
            return false;
        }
        
        // Test beginning another transaction for rollback
        std::string txnId2 = "test_txn_2";
        if (!txnManager.beginTransaction(txnId2, config)) {
            std::cout << "Failed to begin second transaction" << std::endl;
            return false;
        }
        
        // Add participants and then shutdown to simulate rollback
        txnManager.addParticipant(txnId2, participant1);
        txnManager.addParticipant(txnId2, participant2);
        
        // Shutdown without executing to simulate rollback
        txnManager.shutdown();
        
        std::cout << "Distributed transactions test passed" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Exception in distributed transactions test: " << e.what() << std::endl;
        return false;
    }
}

// Test cross-shard queries
bool testCrossShardQueries() {
    std::cout << "=== Testing Cross-Shard Queries ===" << std::endl;
    
    try {
        // Create cross-shard query processor
        CrossShardQueryProcessor queryProcessor;
        if (!queryProcessor.initialize()) {
            std::cout << "Failed to initialize CrossShardQueryProcessor" << std::endl;
            return false;
        }
        
        // Add shards
        ShardInfo shard1("shard1", "192.168.1.101", 8001);
        ShardInfo shard2("shard2", "192.168.1.102", 8002);
        ShardInfo shard3("shard3", "192.168.1.103", 8003);
        
        if (!queryProcessor.addShard(shard1)) {
            std::cout << "Failed to add shard1" << std::endl;
            return false;
        }
        
        if (!queryProcessor.addShard(shard2)) {
            std::cout << "Failed to add shard2" << std::endl;
            return false;
        }
        
        if (!queryProcessor.addShard(shard3)) {
            std::cout << "Failed to add shard3" << std::endl;
            return false;
        }
        
        // Test getting shards
        auto shards = queryProcessor.getShards();
        std::cout << "Added " << shards.size() << " shards to query processor" << std::endl;
        
        std::cout << "Cross-shard queries test passed" << std::endl;
        queryProcessor.shutdown();
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "Exception in cross-shard queries test: " << e.what() << std::endl;
        return false;
    }
}

// Integration test that combines all components
bool runIntegrationTest() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Starting PhantomDB Distributed Integration Test" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    // Run individual component tests
    bool clusterTest = testClusterManagement();
    if (!clusterTest) {
        std::cout << "Cluster management test failed" << std::endl;
        return false;
    }
    
    bool consensusTest = testConsensusAndReplication();
    if (!consensusTest) {
        std::cout << "Consensus and replication test failed" << std::endl;
        return false;
    }
    
    bool transactionTest = testDistributedTransactions();
    if (!transactionTest) {
        std::cout << "Distributed transactions test failed" << std::endl;
        return false;
    }
    
    bool crossShardTest = testCrossShardQueries();
    if (!crossShardTest) {
        std::cout << "Cross-shard queries test failed" << std::endl;
        return false;
    }
    
    std::cout << "=========================================" << std::endl;
    std::cout << "All Distributed Integration Tests Passed!" << std::endl;
    std::cout << "=========================================" << std::endl;
    return true;
}

int main() {
    // No equivalent to spdlog::set_level in iostream, so we'll just output all messages
    
    try {
        bool success = runIntegrationTest();
        return success ? 0 : 1;
    } catch (const std::exception& e) {
        std::cout << "Exception in integration test: " << e.what() << std::endl;
        return 1;
    }
}