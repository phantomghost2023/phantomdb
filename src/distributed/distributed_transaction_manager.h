#ifndef PHANTOMDB_DISTRIBUTED_TRANSACTION_MANAGER_H
#define PHANTOMDB_DISTRIBUTED_TRANSACTION_MANAGER_H

#include "two_phase_commit.h"
#include "saga.h"
#include "cross_shard_query.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>

namespace phantomdb {
namespace distributed {

// Distributed transaction types
enum class DistributedTransactionType {
    TWO_PHASE_COMMIT,
    SAGA
};

// Distributed transaction configuration
struct DistributedTransactionConfig {
    DistributedTransactionType type;
    std::vector<ParticipantInfo> participants;
    std::chrono::milliseconds timeout;
    
    DistributedTransactionConfig() 
        : type(DistributedTransactionType::TWO_PHASE_COMMIT),
          timeout(std::chrono::milliseconds(30000)) {} // 30 seconds default
};

// Distributed transaction status
enum class DistributedTransactionStatus {
    ACTIVE,
    PREPARED,
    COMMITTED,
    ABORTED,
    COMPENSATED
};

class DistributedTransactionManager {
public:
    DistributedTransactionManager();
    ~DistributedTransactionManager();
    
    // Initialize the distributed transaction manager
    bool initialize();
    
    // Shutdown the distributed transaction manager
    void shutdown();
    
    // Begin a new distributed transaction
    bool beginTransaction(const std::string& transactionId, 
                         const DistributedTransactionConfig& config);
    
    // Execute a distributed transaction
    bool executeTransaction(const std::string& transactionId);
    
    // Add a step to a Saga transaction
    bool addSagaStep(const std::string& transactionId, const SagaStep& step);
    
    // Add a participant to a 2PC transaction
    bool addParticipant(const std::string& transactionId, const ParticipantInfo& participant);
    
    // Execute a cross-shard query within a transaction
    std::vector<QueryResult> executeCrossShardQuery(const std::string& transactionId, 
                                                   const std::string& query);
    
    // Get transaction status
    DistributedTransactionStatus getTransactionStatus(const std::string& transactionId) const;
    
    // Get transaction type
    DistributedTransactionType getTransactionType(const std::string& transactionId) const;
    
    // Register callbacks
    void registerPrepareCallback(const PrepareCallback& callback);
    void registerCommitCallback(const CommitCallback& callback);
    void registerAbortCallback(const AbortCallback& callback);
    void registerActionCallback(const SagaActionCallback& callback);
    void registerCompensationCallback(const SagaCompensationCallback& callback);
    void registerQueryExecutionCallback(const QueryExecutionCallback& callback);
    
    // Add a shard for cross-shard queries
    bool addShard(const ShardInfo& shard);
    
    // Remove a shard
    bool removeShard(const std::string& shardId);

private:
    // Transaction information
    struct TransactionInfo {
        std::string transactionId;
        DistributedTransactionType type;
        std::atomic<DistributedTransactionStatus> status;
        
        TransactionInfo(const std::string& id, DistributedTransactionType t)
            : transactionId(id), type(t), status(DistributedTransactionStatus::ACTIVE) {}
    };
    
    // Active transactions
    std::unordered_map<std::string, std::unique_ptr<TransactionInfo>> transactions_;
    
    // 2PC coordinator
    std::unique_ptr<TwoPhaseCommitCoordinator> twoPC_;
    
    // Saga coordinator
    std::unique_ptr<SagaCoordinator> saga_;
    
    // Cross-shard query processor
    std::unique_ptr<CrossShardQueryProcessor> crossShardQuery_;
    
    // Mutex for thread safety
    mutable std::mutex managerMutex_;
    
    // Internal methods
    bool executeTwoPhaseCommitTransaction(const std::string& transactionId);
    bool executeSagaTransaction(const std::string& transactionId);
};

} // namespace distributed
} // namespace phantomdb

#endif // PHANTOMDB_DISTRIBUTED_TRANSACTION_MANAGER_H