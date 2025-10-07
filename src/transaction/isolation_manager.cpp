#include "isolation_manager.h"
#include <iostream>
#include <unordered_set>

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
                // This would require tracking transaction start time
                return version.isCommitted;
                
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
    
private:
    mutable std::mutex mutex_;
    // Track reads for SERIALIZABLE isolation to prevent phantom reads
    std::unordered_map<int, std::unordered_set<std::string>> serializableReads_;
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

} // namespace transaction
} // namespace phantomdb