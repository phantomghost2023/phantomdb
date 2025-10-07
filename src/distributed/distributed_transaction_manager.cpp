#include "distributed_transaction_manager.h"
#include <iostream>

namespace phantomdb {
namespace distributed {

DistributedTransactionManager::DistributedTransactionManager() {
    std::cout << "Creating DistributedTransactionManager" << std::endl;
}

DistributedTransactionManager::~DistributedTransactionManager() {
    if (twoPC_ || saga_ || crossShardQuery_) {
        shutdown();
    }
    std::cout << "Destroying DistributedTransactionManager" << std::endl;
}

bool DistributedTransactionManager::initialize() {
    std::cout << "Initializing DistributedTransactionManager" << std::endl;
    
    try {
        // Initialize 2PC coordinator
        twoPC_ = std::make_unique<TwoPhaseCommitCoordinator>();
        if (!twoPC_->initialize()) {
            std::cout << "Failed to initialize TwoPhaseCommitCoordinator" << std::endl;
            return false;
        }
        
        // Initialize Saga coordinator
        saga_ = std::make_unique<SagaCoordinator>();
        if (!saga_->initialize()) {
            std::cout << "Failed to initialize SagaCoordinator" << std::endl;
            return false;
        }
        
        // Initialize cross-shard query processor
        crossShardQuery_ = std::make_unique<CrossShardQueryProcessor>();
        if (!crossShardQuery_->initialize()) {
            std::cout << "Failed to initialize CrossShardQueryProcessor" << std::endl;
            return false;
        }
        
        std::cout << "DistributedTransactionManager initialized successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cout << "Exception during DistributedTransactionManager initialization: " << e.what() << std::endl;
        return false;
    }
}

void DistributedTransactionManager::shutdown() {
    std::cout << "Shutting down DistributedTransactionManager" << std::endl;
    
    // Shutdown components in reverse order
    if (crossShardQuery_) {
        crossShardQuery_->shutdown();
        crossShardQuery_.reset();
    }
    
    if (saga_) {
        saga_->shutdown();
        saga_.reset();
    }
    
    if (twoPC_) {
        twoPC_->shutdown();
        twoPC_.reset();
    }
    
    // Clear transactions
    {
        std::lock_guard<std::mutex> lock(managerMutex_);
        transactions_.clear();
    }
    
    std::cout << "DistributedTransactionManager shutdown completed" << std::endl;
}

bool DistributedTransactionManager::beginTransaction(const std::string& transactionId, 
                                                  const DistributedTransactionConfig& config) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    // Check if transaction already exists
    if (transactions_.find(transactionId) != transactions_.end()) {
        std::cout << "Transaction " << transactionId << " already exists" << std::endl;
        return false;
    }
    
    // Create transaction info
    auto transaction = std::make_unique<TransactionInfo>(transactionId, config.type);
    transactions_[transactionId] = std::move(transaction);
    
    // Initialize the transaction in the appropriate coordinator
    bool success = false;
    switch (config.type) {
        case DistributedTransactionType::TWO_PHASE_COMMIT:
            success = twoPC_->beginTransaction(transactionId, config.participants);
            if (success) {
                twoPC_->setPrepareTimeout(config.timeout);
                twoPC_->setCommitAbortTimeout(config.timeout);
            }
            break;
        case DistributedTransactionType::SAGA:
            success = saga_->beginSaga(transactionId);
            if (success) {
                saga_->setSagaTimeout(config.timeout);
            }
            break;
    }
    
    if (success) {
        std::cout << "Started " << ((config.type == DistributedTransactionType::TWO_PHASE_COMMIT) ? "2PC" : "Saga") 
                  << " transaction " << transactionId << std::endl;
    } else {
        std::cout << "Failed to start " << ((config.type == DistributedTransactionType::TWO_PHASE_COMMIT) ? "2PC" : "Saga") 
                  << " transaction " << transactionId << std::endl;
        // Remove the transaction if initialization failed
        transactions_.erase(transactionId);
    }
    
    return success;
}

bool DistributedTransactionManager::executeTransaction(const std::string& transactionId) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        std::cout << "Transaction " << transactionId << " not found" << std::endl;
        return false;
    }
    
    auto& transaction = it->second;
    
    // Execute based on transaction type
    bool success = false;
    switch (transaction->type) {
        case DistributedTransactionType::TWO_PHASE_COMMIT:
            success = executeTwoPhaseCommitTransaction(transactionId);
            break;
        case DistributedTransactionType::SAGA:
            success = executeSagaTransaction(transactionId);
            break;
    }
    
    if (success) {
        transaction->status.store(DistributedTransactionStatus::COMMITTED);
        std::cout << "Transaction " << transactionId << " executed successfully" << std::endl;
    } else {
        transaction->status.store(DistributedTransactionStatus::ABORTED);
        std::cout << "Transaction " << transactionId << " execution failed" << std::endl;
    }
    
    return success;
}

bool DistributedTransactionManager::addSagaStep(const std::string& transactionId, const SagaStep& step) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        std::cout << "Transaction " << transactionId << " not found" << std::endl;
        return false;
    }
    
    auto& transaction = it->second;
    
    // Check if this is a Saga transaction
    if (transaction->type != DistributedTransactionType::SAGA) {
        std::cout << "Transaction " << transactionId << " is not a Saga transaction" << std::endl;
        return false;
    }
    
    return saga_->addStep(transactionId, step);
}

bool DistributedTransactionManager::addParticipant(const std::string& transactionId, 
                                                 const ParticipantInfo& participant) {
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        std::cout << "Transaction " << transactionId << " not found" << std::endl;
        return false;
    }
    
    auto& transaction = it->second;
    
    // Check if this is a 2PC transaction
    if (transaction->type != DistributedTransactionType::TWO_PHASE_COMMIT) {
        std::cout << "Transaction " << transactionId << " is not a 2PC transaction" << std::endl;
        return false;
    }
    
    return twoPC_->addParticipant(transactionId, participant);
}

std::vector<QueryResult> DistributedTransactionManager::executeCrossShardQuery(
    const std::string& transactionId, 
    const std::string& query) {
    
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        std::cout << "Transaction " << transactionId << " not found" << std::endl;
        return {};
    }
    
    // Execute the cross-shard query
    return crossShardQuery_->executeCrossShardQuery(query);
}

DistributedTransactionStatus DistributedTransactionManager::getTransactionStatus(
    const std::string& transactionId) const {
    
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        return DistributedTransactionStatus::ABORTED; // Transaction not found
    }
    
    return it->second->status.load();
}

DistributedTransactionType DistributedTransactionManager::getTransactionType(
    const std::string& transactionId) const {
    
    std::lock_guard<std::mutex> lock(managerMutex_);
    
    auto it = transactions_.find(transactionId);
    if (it == transactions_.end()) {
        return DistributedTransactionType::TWO_PHASE_COMMIT; // Default type
    }
    
    return it->second->type;
}

void DistributedTransactionManager::registerPrepareCallback(const PrepareCallback& callback) {
    if (twoPC_) {
        twoPC_->registerPrepareCallback(callback);
    }
}

void DistributedTransactionManager::registerCommitCallback(const CommitCallback& callback) {
    if (twoPC_) {
        twoPC_->registerCommitCallback(callback);
    }
}

void DistributedTransactionManager::registerAbortCallback(const AbortCallback& callback) {
    if (twoPC_) {
        twoPC_->registerAbortCallback(callback);
    }
}

void DistributedTransactionManager::registerActionCallback(const SagaActionCallback& callback) {
    if (saga_) {
        saga_->registerActionCallback(callback);
    }
}

void DistributedTransactionManager::registerCompensationCallback(const SagaCompensationCallback& callback) {
    if (saga_) {
        saga_->registerCompensationCallback(callback);
    }
}

void DistributedTransactionManager::registerQueryExecutionCallback(const QueryExecutionCallback& callback) {
    if (crossShardQuery_) {
        crossShardQuery_->registerQueryExecutionCallback(callback);
    }
}

bool DistributedTransactionManager::addShard(const ShardInfo& shard) {
    if (crossShardQuery_) {
        return crossShardQuery_->addShard(shard);
    }
    return false;
}

bool DistributedTransactionManager::removeShard(const std::string& shardId) {
    if (crossShardQuery_) {
        return crossShardQuery_->removeShard(shardId);
    }
    return false;
}

bool DistributedTransactionManager::executeTwoPhaseCommitTransaction(const std::string& transactionId) {
    std::cout << "Executing 2PC transaction " << transactionId << std::endl;
    
    // Execute the two-phase commit protocol
    return twoPC_->executeTwoPhaseCommit(transactionId);
}

bool DistributedTransactionManager::executeSagaTransaction(const std::string& transactionId) {
    std::cout << "Executing Saga transaction " << transactionId << std::endl;
    
    // Execute the Saga
    return saga_->executeSaga(transactionId);
}

} // namespace distributed
} // namespace phantomdb