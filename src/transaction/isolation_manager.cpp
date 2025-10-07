#include "isolation_manager.h"
#include <iostream>
#include <unordered_set>
#include <algorithm>

namespace phantomdb {
namespace transaction {

// IsolationManager implementation
class IsolationManager::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "Initializing Isolation Manager..." << std::endl;
        return true;
    }
    
    void shutdown() {
        std::cout << "Shutting down Isolation Manager..." << std::endl;
    }
    
    bool isReadAllowed(IsolationLevel level, const std::string& key) const {
        // All isolation levels allow read operations
        return true;
    }
    
    bool isWriteAllowed(IsolationLevel level, const std::string& key) const {
        // All isolation levels allow write operations
        return true;
    }
    
    bool isVisible(IsolationLevel level, int transactionId, const DataVersion& version) const {
        switch (level) {
            case IsolationLevel::READ_UNCOMMITTED:
                // In READ_UNCOMMITTED, all versions are visible
                return true;
                
            case IsolationLevel::READ_COMMITTED:
                // In READ_COMMITTED, only committed versions are visible
                return version.isCommitted;
                
            case IsolationLevel::REPEATABLE_READ:
                // In REPEATABLE_READ, committed versions are visible
                // Additional logic would be needed for snapshot-based consistency
                return version.isCommitted;
                
            case IsolationLevel::SERIALIZABLE:
                // In SERIALIZABLE, committed versions are visible
                // Additional locking would be needed for full serializability
                return version.isCommitted;
                
            case IsolationLevel::SNAPSHOT:
                // In SNAPSHOT, versions visible at transaction start are visible
                {
                    auto snapshot = getSnapshot(transactionId);
                    if (snapshot) {
                        // Version is visible if it was committed before the transaction started
                        // or if it was created by this transaction
                        return (version.transactionId == transactionId) || 
                               (version.isCommitted && version.timestamp <= snapshot->timestamp);
                    } else {
                        // Fallback to READ_COMMITTED if no snapshot
                        return version.isCommitted;
                    }
                }
                
            default:
                return false;
        }
    }
    
    bool preventPhantomReads(IsolationLevel level, int transactionId, const std::string& key) {
        if (level == IsolationLevel::SERIALIZABLE) {
            // For SERIALIZABLE isolation, we need to prevent phantom reads
            // This would typically involve range locking or predicate locking
            // For now, we'll just track the keys accessed by this transaction
            std::lock_guard<std::mutex> lock(mutex_);
            serializableReads_[transactionId].insert(key);
            return true;
        }
        return true;
    }
    
    bool createSnapshot(int transactionId) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto timestamp = std::chrono::high_resolution_clock::now();
        snapshots_[transactionId] = std::make_unique<TransactionSnapshot>(transactionId, timestamp);
        return true;
    }
    
    TransactionSnapshot* getSnapshot(int transactionId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = snapshots_.find(transactionId);
        if (it != snapshots_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    bool hasWriteConflict(int transactionId, const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        // Check if another transaction has written to this key since our transaction started
        auto it = activeWrites_.find(key);
        if (it != activeWrites_.end()) {
            for (const auto& writerId : it->second) {
                if (writerId != transactionId) {
                    return true; // Conflict detected
                }
            }
        }
        return false;
    }
    
    void registerRead(int transactionId, const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto snapshot = getSnapshot(transactionId);
        if (snapshot) {
            snapshot->readKeys.insert(key);
        }
    }
    
    void registerWrite(int transactionId, const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        activeWrites_[key].insert(transactionId);
    }
    
private:
    mutable std::mutex mutex_;
    // Track reads for SERIALIZABLE isolation to prevent phantom reads
    std::unordered_map<int, std::unordered_set<std::string>> serializableReads_;
    // Track transaction snapshots for SNAPSHOT isolation
    std::unordered_map<int, std::unique_ptr<TransactionSnapshot>> snapshots_;
    // Track active writes to detect conflicts
    std::unordered_map<std::string, std::unordered_set<int>> activeWrites_;
};

IsolationManager::IsolationManager() : pImpl(std::make_unique<Impl>()) {}

IsolationManager::~IsolationManager() = default;

bool IsolationManager::initialize() {
    return pImpl->initialize();
}

void IsolationManager::shutdown() {
    pImpl->shutdown();
}

bool IsolationManager::isReadAllowed(IsolationLevel level, const std::string& key) const {
    return pImpl->isReadAllowed(level, key);
}

bool IsolationManager::isWriteAllowed(IsolationLevel level, const std::string& key) const {
    return pImpl->isWriteAllowed(level, key);
}

bool IsolationManager::isVisible(IsolationLevel level, int transactionId, const DataVersion& version) const {
    return pImpl->isVisible(level, transactionId, version);
}

bool IsolationManager::preventPhantomReads(IsolationLevel level, int transactionId, const std::string& key) {
    return pImpl->preventPhantomReads(level, transactionId, key);
}

bool IsolationManager::createSnapshot(int transactionId) {
    return pImpl->createSnapshot(transactionId);
}

TransactionSnapshot* IsolationManager::getSnapshot(int transactionId) const {
    return pImpl->getSnapshot(transactionId);
}

bool IsolationManager::hasWriteConflict(int transactionId, const std::string& key) const {
    return pImpl->hasWriteConflict(transactionId, key);
}

void IsolationManager::registerRead(int transactionId, const std::string& key) {
    pImpl->registerRead(transactionId, key);
}

void IsolationManager::registerWrite(int transactionId, const std::string& key) {
    pImpl->registerWrite(transactionId, key);
}

} // namespace transaction
} // namespace phantomdb