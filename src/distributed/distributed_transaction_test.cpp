#include "distributed_transaction_manager.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace phantomdb::distributed;

// Mock callback implementations for testing
bool mockPrepareCallback(const ParticipantInfo& participant, const std::string& transactionId) {
    std::cout << "Mock prepare callback for participant " << participant.id << " in transaction " << transactionId << std::endl;
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return true; // Always succeed in mock
}

bool mockCommitCallback(const ParticipantInfo& participant, const std::string& transactionId) {
    std::cout << "Mock commit callback for participant " << participant.id << " in transaction " << transactionId << std::endl;
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return true; // Always succeed in mock
}

bool mockAbortCallback(const ParticipantInfo& participant, const std::string& transactionId) {
    std::cout << "Mock abort callback for participant " << participant.id << " in transaction " << transactionId << std::endl;
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return true; // Always succeed in mock
}

bool mockActionCallback(const std::string& participantId, 
                       const std::string& action, 
                       const std::string& data) {
    std::cout << "Mock action callback for participant " << participantId << " with action " << action << " and data " << data << std::endl;
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return true; // Always succeed in mock
}

bool mockCompensationCallback(const std::string& participantId, 
                             const std::string& compensation, 
                             const std::string& data) {
    std::cout << "Mock compensation callback for participant " << participantId << " with compensation " << compensation << " and data " << data << std::endl;
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return true; // Always succeed in mock
}

QueryResult mockQueryExecutionCallback(const ShardInfo& shard, const std::string& query) {
    std::cout << "Mock query execution callback for shard " << shard.id << " with query " << query << std::endl;
    
    QueryResult result(shard.id);
    result.success = true;
    result.rows = {{"result1", "value1"}, {"result2", "value2"}};
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    return result;
}

int main() {
    // No equivalent to spdlog::set_level in iostream, so we'll just output all messages
    
    try {
        std::cout << "Starting PhantomDB Distributed Transaction Test" << std::endl;
        
        // Create distributed transaction manager
        DistributedTransactionManager dtm;
        if (!dtm.initialize()) {
            std::cout << "Failed to initialize DistributedTransactionManager" << std::endl;
            return 1;
        }
        
        // Register callbacks
        dtm.registerPrepareCallback(mockPrepareCallback);
        dtm.registerCommitCallback(mockCommitCallback);
        dtm.registerAbortCallback(mockAbortCallback);
        dtm.registerActionCallback(mockActionCallback);
        dtm.registerCompensationCallback(mockCompensationCallback);
        dtm.registerQueryExecutionCallback(mockQueryExecutionCallback);
        
        // Add shards for cross-shard queries
        dtm.addShard(ShardInfo("shard1", "192.168.1.101", 8001));
        dtm.addShard(ShardInfo("shard2", "192.168.1.102", 8002));
        dtm.addShard(ShardInfo("shard3", "192.168.1.103", 8003));
        
        // Test 1: Two-Phase Commit Transaction
        std::cout << "Testing Two-Phase Commit Transaction" << std::endl;
        
        DistributedTransactionConfig config2PC;
        config2PC.type = DistributedTransactionType::TWO_PHASE_COMMIT;
        config2PC.participants = {
            ParticipantInfo("participant1", "192.168.1.201", 9001),
            ParticipantInfo("participant2", "192.168.1.202", 9002),
            ParticipantInfo("participant3", "192.168.1.203", 9003)
        };
        
        std::string transactionId1 = "2pc_transaction_1";
        if (dtm.beginTransaction(transactionId1, config2PC)) {
            // Add participants (in a real scenario, these might be added dynamically)
            dtm.addParticipant(transactionId1, ParticipantInfo("participant4", "192.168.1.204", 9004));
            
            // Execute the transaction
            bool success = dtm.executeTransaction(transactionId1);
            std::cout << "2PC Transaction " << transactionId1 << " result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
            
            // Check status
            auto status = dtm.getTransactionStatus(transactionId1);
            std::cout << "2PC Transaction " << transactionId1 << " status: " << 
                        ((status == DistributedTransactionStatus::COMMITTED) ? "COMMITTED" :
                         (status == DistributedTransactionStatus::ABORTED) ? "ABORTED" : "UNKNOWN") << std::endl;
        } else {
            std::cout << "Failed to begin 2PC transaction" << std::endl;
        }
        
        // Test 2: Saga Transaction
        std::cout << "Testing Saga Transaction" << std::endl;
        
        DistributedTransactionConfig configSaga;
        configSaga.type = DistributedTransactionType::SAGA;
        
        std::string transactionId2 = "saga_transaction_1";
        if (dtm.beginTransaction(transactionId2, configSaga)) {
            // Add steps to the saga
            dtm.addSagaStep(transactionId2, SagaStep("step1", "action1", "compensation1", "participant1", "data1"));
            dtm.addSagaStep(transactionId2, SagaStep("step2", "action2", "compensation2", "participant2", "data2"));
            dtm.addSagaStep(transactionId2, SagaStep("step3", "action3", "compensation3", "participant3", "data3"));
            
            // Execute the transaction
            bool success = dtm.executeTransaction(transactionId2);
            std::cout << "Saga Transaction " << transactionId2 << " result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
            
            // Check status
            auto status = dtm.getTransactionStatus(transactionId2);
            std::cout << "Saga Transaction " << transactionId2 << " status: " << 
                        ((status == DistributedTransactionStatus::COMMITTED) ? "COMMITTED" :
                         (status == DistributedTransactionStatus::ABORTED) ? "ABORTED" :
                         (status == DistributedTransactionStatus::COMPENSATED) ? "COMPENSATED" : "UNKNOWN") << std::endl;
        } else {
            std::cout << "Failed to begin Saga transaction" << std::endl;
        }
        
        // Test 3: Cross-shard query
        std::cout << "Testing Cross-Shard Query" << std::endl;
        
        std::string transactionId3 = "cross_shard_query_1";
        if (dtm.beginTransaction(transactionId3, config2PC)) {
            auto results = dtm.executeCrossShardQuery(transactionId3, "SELECT * FROM users");
            std::cout << "Cross-shard query returned " << results.size() << " results" << std::endl;
            
            for (const auto& result : results) {
                std::cout << "Shard " << result.shardId << " result: " << result.rows.size() << " rows, success: " << result.success << std::endl;
            }
        }
        
        std::cout << "Distributed Transaction Test completed successfully" << std::endl;
        
        // Shutdown distributed transaction manager
        dtm.shutdown();
        
    } catch (const std::exception& e) {
        std::cout << "Exception in distributed transaction test: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}