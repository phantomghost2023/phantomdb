#include "transaction_manager.h"
#include "mvcc_manager.h"
#include "lock_manager.h"
#include "isolation_manager.h"
#include <iostream>
#include <atomic>
#include <unordered_map>
#include <mutex>

namespace phantomdb {
namespace transaction {

// Transaction implementation
Transaction::Transaction(int id, IsolationLevel isolation) 
    : id_(id), isolationLevel_(isolation), state_(TransactionState::ACTIVE) {
    std::cout << "Created transaction " << id_ << " with isolation level " 
              << static_cast<int>(isolationLevel_) << std::endl;
}

Transaction::~Transaction() {
    std::cout << "Destroyed transaction " << id_ << std::endl;
}

int Transaction::getId() const {
    return id_;
}

IsolationLevel Transaction::getIsolationLevel() const {
    return isolationLevel_;
}

TransactionState Transaction::getState() const {
    return state_;
}

void Transaction::setState(TransactionState state) {
    state_ = state;
}

// TransactionManager implementation
class TransactionManager::Impl {
public:
    Impl() : nextTransactionId_(1), isolationManager_(std::make_unique<IsolationManager>()) {}
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "Initializing Transaction Manager..." << std::endl;
        // Initialize concurrency control and lock manager
        mvccManager_ = std::make_unique<MVCCManager>();
        lockManager_ = std::make_unique<LockManager>();
        
        if (!mvccManager_->initialize() || !lockManager_->initialize() || !isolationManager_->initialize()) {
            return false;
        }
        
        return true;
    }
    
    void shutdown() {
        std::cout << "Shutting down Transaction Manager..." << std::endl;
        // Clean up resources
        if (isolationManager_) {
            isolationManager_->shutdown();
            isolationManager_.reset();
        }
        
        if (mvccManager_) {
            mvccManager_->shutdown();
            mvccManager_.reset();
        }
        
        if (lockManager_) {
            lockManager_->shutdown();
            lockManager_.reset();
        }
    }
    
    std::shared_ptr<Transaction> beginTransaction(IsolationLevel isolation) {
        std::lock_guard<std::mutex> lock(mutex_);
        int transactionId = nextTransactionId_++;
        auto transaction = std::make_shared<Transaction>(transactionId, isolation);
        transactions_[transactionId] = transaction;
        std::cout << "Started transaction " << transactionId << std::endl;
        return transaction;
    }
    
    bool commitTransaction(std::shared_ptr<Transaction> transaction) {
        if (!transaction) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        int transactionId = transaction->getId();
        
        // Check if transaction exists
        auto it = transactions_.find(transactionId);
        if (it == transactions_.end()) {
            std::cerr << "Transaction " << transactionId << " not found" << std::endl;
            return false;
        }
        
        // Commit the transaction using MVCC
        if (mvccManager_ && !mvccManager_->commitTransaction(transactionId)) {
            std::cerr << "Failed to commit transaction " << transactionId << " in MVCC manager" << std::endl;
            return false;
        }
        
        // Release all locks
        if (lockManager_ && !lockManager_->releaseAllLocks(transactionId)) {
            std::cerr << "Failed to release locks for transaction " << transactionId << std::endl;
            // Continue anyway as the commit was successful
        }
        
        transaction->setState(TransactionState::COMMITTED);
        std::cout << "Committed transaction " << transactionId << std::endl;
        return true;
    }
    
    bool rollbackTransaction(std::shared_ptr<Transaction> transaction) {
        if (!transaction) {
            return false;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        int transactionId = transaction->getId();
        
        // Check if transaction exists
        auto it = transactions_.find(transactionId);
        if (it == transactions_.end()) {
            std::cerr << "Transaction " << transactionId << " not found" << std::endl;
            return false;
        }
        
        // Abort the transaction using MVCC
        if (mvccManager_ && !mvccManager_->abortTransaction(transactionId)) {
            std::cerr << "Failed to abort transaction " << transactionId << " in MVCC manager" << std::endl;
            return false;
        }
        
        // Release all locks
        if (lockManager_ && !lockManager_->releaseAllLocks(transactionId)) {
            std::cerr << "Failed to release locks for transaction " << transactionId << std::endl;
            // Continue anyway as the rollback was successful
        }
        
        transaction->setState(TransactionState::ABORTED);
        std::cout << "Rolled back transaction " << transactionId << std::endl;
        return true;
    }
    
    std::shared_ptr<Transaction> getTransaction(int id) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = transactions_.find(id);
        if (it != transactions_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    MVCCManager* getMVCCManager() const {
        return mvccManager_.get();
    }
    
    LockManager* getLockManager() const {
        return lockManager_.get();
    }
    
    IsolationManager* getIsolationManager() const {
        return isolationManager_.get();
    }
    
private:
    mutable std::mutex mutex_;
    std::atomic<int> nextTransactionId_;
    std::unordered_map<int, std::shared_ptr<Transaction>> transactions_;
    std::unique_ptr<MVCCManager> mvccManager_;
    std::unique_ptr<LockManager> lockManager_;
    std::unique_ptr<IsolationManager> isolationManager_;
};

TransactionManager::TransactionManager() : pImpl(std::make_unique<Impl>()) {
    std::cout << "PhantomDB Transaction Manager created" << std::endl;
}

TransactionManager::~TransactionManager() {
    std::cout << "PhantomDB Transaction Manager destroyed" << std::endl;
}

bool TransactionManager::initialize() {
    return pImpl->initialize();
}

void TransactionManager::shutdown() {
    pImpl->shutdown();
}

std::shared_ptr<Transaction> TransactionManager::beginTransaction(IsolationLevel isolation) {
    return pImpl->beginTransaction(isolation);
}

bool TransactionManager::commitTransaction(std::shared_ptr<Transaction> transaction) {
    return pImpl->commitTransaction(transaction);
}

bool TransactionManager::rollbackTransaction(std::shared_ptr<Transaction> transaction) {
    return pImpl->rollbackTransaction(transaction);
}

std::shared_ptr<Transaction> TransactionManager::getTransaction(int id) const {
    return pImpl->getTransaction(id);
}

} // namespace transaction
} // namespace phantomdb