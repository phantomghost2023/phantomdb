#include "enhanced_mvcc_manager.h"
#include <iostream>
#include <algorithm>
#include <shared_mutex>

namespace phantomdb {
namespace transaction {

// EnhancedMVCCManager implementation
class EnhancedMVCCManager::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "Initializing Enhanced MVCC Manager..." << std::endl;
        return true;
    }
    
    void shutdown() {
        std::cout << "Shutting down Enhanced MVCC Manager..." << std::endl;
    }
    
    bool createVersion(int transactionId, const std::string& key, const std::string& data) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        
        // Create a new version with current timestamp
        EnhancedTimestamp timestamp = getCurrentTimestamp();
        EnhancedDataVersion version(transactionId, timestamp, data);
        
        // Add the version to the version chain for this key
        versionChains_[key].push_back(std::move(version));
        
        std::cout << "Created enhanced version for key " << key << " in transaction " << transactionId << std::endl;
        return true;
    }
    
    bool readData(int transactionId, const std::string& key, std::string& data, IsolationLevel isolation) {
        std::shared_lock<std::shared_mutex> lock(rwMutex_);
        
        // Register the read operation
        registerReadOperation(transactionId, key);
        
        // Prevent phantom reads for SERIALIZABLE isolation
        if (isolation == IsolationLevel::SERIALIZABLE) {
            preventPhantomReads(transactionId, key);
        }
        
        // Find the version chain for this key
        auto it = versionChains_.find(key);
        if (it == versionChains_.end() || it->second.empty()) {
            // No versions exist for this key
            return false;
        }
        
        const auto& versions = it->second;
        
        // Find the most recent version that is visible to this transaction
        for (auto rit = versions.rbegin(); rit != versions.rend(); ++rit) {
            if (isVisible(transactionId, *rit, isolation)) {
                data = rit->data;
                
                // Register this read with the version
                registerRead(transactionId, key, *rit);
                
                return true;
            }
        }
        
        // No visible version found
        return false;
    }
    
    bool writeData(int transactionId, const std::string& key, const std::string& data, IsolationLevel isolation) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        
        // Register the write operation
        registerWriteOperation(transactionId, key);
        
        // Check for write conflicts based on isolation level
        if (hasWriteConflict(transactionId, key, isolation)) {
            std::cerr << "Write conflict detected for transaction " << transactionId 
                      << " on key " << key << std::endl;
            return false;
        }
        
        // Create a new version
        EnhancedTimestamp timestamp = getCurrentTimestamp();
        EnhancedDataVersion version(transactionId, timestamp, data);
        
        // Add the version to the version chain for this key
        versionChains_[key].push_back(std::move(version));
        
        std::cout << "Wrote enhanced version for key " << key << " in transaction " << transactionId << std::endl;
        return true;
    }
    
    bool commitTransaction(int transactionId) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        
        // Validate snapshot consistency for SNAPSHOT isolation
        auto snapshot = getSnapshot(transactionId);
        if (snapshot && !validateSnapshot(transactionId)) {
            std::cerr << "Snapshot validation failed for transaction " << transactionId << std::endl;
            return false;
        }
        
        // Detect write skew for SERIALIZABLE isolation
        if (detectWriteSkew(transactionId)) {
            std::cerr << "Write skew detected for transaction " << transactionId << std::endl;
            return false;
        }
        
        // Mark all versions created by this transaction as committed
        EnhancedTimestamp commitTimestamp = getCurrentTimestamp();
        for (auto& pair : versionChains_) {
            for (auto& version : pair.second) {
                if (version.transactionId == transactionId) {
                    version.isCommitted = true;
                    version.commitTimestamp = commitTimestamp;
                }
            }
        }
        
        // Update transaction statistics
        updateTransactionStats(transactionId, true);
        
        std::cout << "Committed enhanced versions for transaction " << transactionId << std::endl;
        return true;
    }
    
    bool abortTransaction(int transactionId) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        
        // Mark all versions created by this transaction as aborted
        for (auto& pair : versionChains_) {
            for (auto& version : pair.second) {
                if (version.transactionId == transactionId) {
                    version.isAborted = true;
                }
            }
        }
        
        // Update transaction statistics
        updateTransactionStats(transactionId, false);
        
        std::cout << "Aborted enhanced versions for transaction " << transactionId << std::endl;
        return true;
    }
    
    bool hasConflicts(int transactionId, IsolationLevel isolation) {
        std::shared_lock<std::shared_mutex> lock(rwMutex_);
        
        // For READ_UNCOMMITTED and READ_COMMITTED, no conflict detection is needed
        if (isolation == IsolationLevel::READ_UNCOMMITTED || 
            isolation == IsolationLevel::READ_COMMITTED) {
            return false;
        }
        
        // Check for write conflicts
        return hasWriteConflict(transactionId, "", isolation);
    }
    
    EnhancedTimestamp getCurrentTimestamp() const {
        return std::chrono::high_resolution_clock::now();
    }
    
    bool createSnapshot(int transactionId) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        EnhancedTimestamp timestamp = getCurrentTimestamp();
        snapshots_[transactionId] = std::make_unique<EnhancedTransactionSnapshot>(transactionId, timestamp);
        return true;
    }
    
    EnhancedTransactionSnapshot* getSnapshot(int transactionId) {
        std::shared_lock<std::shared_mutex> lock(rwMutex_);
        auto it = snapshots_.find(transactionId);
        if (it != snapshots_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    void registerRead(int transactionId, const std::string& key, const EnhancedDataVersion& version) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        auto snapshot = getSnapshot(transactionId);
        if (snapshot) {
            snapshot->readVersions[key] = version;
        }
    }
    
    void registerWrite(int transactionId, const std::string& key) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        auto snapshot = getSnapshot(transactionId);
        if (snapshot) {
            snapshot->writeSet.insert(key);
        }
    }
    
    bool isVisible(int transactionId, const EnhancedDataVersion& version, IsolationLevel isolation) {
        switch (isolation) {
            case IsolationLevel::READ_UNCOMMITTED:
                // In READ_UNCOMMITTED, all versions are visible (except aborted ones)
                return !version.isAborted;
                
            case IsolationLevel::READ_COMMITTED:
                // In READ_COMMITTED, only committed versions are visible
                return version.isCommitted && !version.isAborted;
                
            case IsolationLevel::REPEATABLE_READ:
                // In REPEATABLE_READ, committed versions are visible
                // Additional logic would be needed for snapshot-based consistency
                return version.isCommitted && !version.isAborted;
                
            case IsolationLevel::SERIALIZABLE:
                // In SERIALIZABLE, committed versions are visible
                // Additional locking would be needed for full serializability
                return version.isCommitted && !version.isAborted;
                
            case IsolationLevel::SNAPSHOT:
                // In SNAPSHOT, versions visible at transaction start are visible
                {
                    auto snapshot = getSnapshot(transactionId);
                    if (snapshot) {
                        // Version is visible if it was committed before the transaction started
                        // or if it was created by this transaction
                        return (version.transactionId == transactionId) || 
                               (version.isCommitted && version.commitTimestamp <= snapshot->timestamp);
                    } else {
                        // Fallback to READ_COMMITTED if no snapshot
                        return version.isCommitted && !version.isAborted;
                    }
                }
                
            default:
                return false;
        }
    }
    
    bool preventPhantomReads(int transactionId, const std::string& keyPattern) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        // For SERIALIZABLE isolation, we need to prevent phantom reads
        // This would typically involve range locking or predicate locking
        // For now, we'll just track the keys accessed by this transaction
        serializableReads_[transactionId].insert(keyPattern);
        return true;
    }
    
    bool detectWriteSkew(int transactionId) {
        // Write skew detection would involve checking if two transactions
        // have read a set of data, then each written to a different item
        // in a way that would be inconsistent if run serially
        // This is a simplified implementation
        return false;
    }
    
    bool validateSnapshot(int transactionId) {
        // Validate that no other transaction has committed changes
        // that would affect this transaction's snapshot
        auto snapshot = getSnapshot(transactionId);
        if (!snapshot) {
            return true; // No snapshot to validate
        }
        
        // Check if any committed transactions have written to keys
        // that this transaction has read
        for (const auto& readPair : snapshot->readVersions) {
            const std::string& key = readPair.first;
            const EnhancedDataVersion& readVersion = readPair.second;
            
            // Check if there's a newer committed version of this key
            auto it = versionChains_.find(key);
            if (it != versionChains_.end()) {
                for (const auto& version : it->second) {
                    // If there's a committed version that was committed after
                    // this transaction started and is different from what
                    // this transaction read, we have a conflict
                    if (version.isCommitted && 
                        version.commitTimestamp > snapshot->timestamp &&
                        version.transactionId != transactionId &&
                        version.data != readVersion.data) {
                        return false; // Snapshot validation failed
                    }
                }
            }
        }
        
        return true; // Snapshot is consistent
    }
    
    EnhancedMVCCManager::TransactionStats getTransactionStats(int transactionId) const {
        std::shared_lock<std::shared_mutex> lock(rwMutex_);
        auto it = transactionStats_.find(transactionId);
        if (it != transactionStats_.end()) {
            return it->second;
        }
        return EnhancedMVCCManager::TransactionStats(transactionId);
    }
    
private:
    mutable std::shared_mutex rwMutex_;
    std::unordered_map<std::string, std::vector<EnhancedDataVersion>> versionChains_;
    std::unordered_map<int, std::unique_ptr<EnhancedTransactionSnapshot>> snapshots_;
    std::unordered_map<int, std::unordered_set<std::string>> serializableReads_;
    std::unordered_map<int, EnhancedMVCCManager::TransactionStats> transactionStats_;
    
    void registerReadOperation(int transactionId, const std::string& key) {
        auto it = transactionStats_.find(transactionId);
        if (it != transactionStats_.end()) {
            it->second.readOperations++;
        } else {
            EnhancedMVCCManager::TransactionStats stats(transactionId);
            stats.readOperations = 1;
            transactionStats_[transactionId] = stats;
        }
    }
    
    void registerWriteOperation(int transactionId, const std::string& key) {
        auto it = transactionStats_.find(transactionId);
        if (it != transactionStats_.end()) {
            it->second.writeOperations++;
        } else {
            EnhancedMVCCManager::TransactionStats stats(transactionId);
            stats.writeOperations = 1;
            transactionStats_[transactionId] = stats;
        }
    }
    
    void updateTransactionStats(int transactionId, bool committed) {
        auto it = transactionStats_.find(transactionId);
        if (it != transactionStats_.end()) {
            // Update duration and other stats as needed
            // This is a simplified implementation
        }
    }
    
    bool hasWriteConflict(int transactionId, const std::string& key, IsolationLevel isolation) {
        // For SERIALIZABLE and SNAPSHOT isolation, check for write conflicts
        if (isolation == IsolationLevel::SERIALIZABLE || isolation == IsolationLevel::SNAPSHOT) {
            // Check if another transaction has written to this key since our transaction started
            auto it = versionChains_.find(key);
            if (it != versionChains_.end()) {
                EnhancedTimestamp transactionStart = getCurrentTimestamp();
                auto snapshot = getSnapshot(transactionId);
                if (snapshot) {
                    transactionStart = snapshot->timestamp;
                }
                
                for (const auto& version : it->second) {
                    // If another committed transaction wrote to this key after our transaction started
                    if (version.transactionId != transactionId && 
                        version.isCommitted && 
                        version.commitTimestamp > transactionStart) {
                        return true; // Conflict detected
                    }
                }
            }
        }
        return false;
    }
};

EnhancedMVCCManager::EnhancedMVCCManager() : pImpl(std::make_unique<Impl>()) {}

EnhancedMVCCManager::~EnhancedMVCCManager() = default;

bool EnhancedMVCCManager::initialize() {
    return pImpl->initialize();
}

void EnhancedMVCCManager::shutdown() {
    pImpl->shutdown();
}

bool EnhancedMVCCManager::createVersion(int transactionId, const std::string& key, const std::string& data) {
    return pImpl->createVersion(transactionId, key, data);
}

bool EnhancedMVCCManager::readData(int transactionId, const std::string& key, std::string& data, IsolationLevel isolation) {
    return pImpl->readData(transactionId, key, data, isolation);
}

bool EnhancedMVCCManager::writeData(int transactionId, const std::string& key, const std::string& data, IsolationLevel isolation) {
    return pImpl->writeData(transactionId, key, data, isolation);
}

bool EnhancedMVCCManager::commitTransaction(int transactionId) {
    return pImpl->commitTransaction(transactionId);
}

bool EnhancedMVCCManager::abortTransaction(int transactionId) {
    return pImpl->abortTransaction(transactionId);
}

bool EnhancedMVCCManager::hasConflicts(int transactionId, IsolationLevel isolation) {
    return pImpl->hasConflicts(transactionId, isolation);
}

EnhancedMVCCManager::EnhancedTimestamp EnhancedMVCCManager::getCurrentTimestamp() const {
    return pImpl->getCurrentTimestamp();
}

bool EnhancedMVCCManager::createSnapshot(int transactionId) {
    return pImpl->createSnapshot(transactionId);
}

EnhancedTransactionSnapshot* EnhancedMVCCManager::getSnapshot(int transactionId) {
    return pImpl->getSnapshot(transactionId);
}

void EnhancedMVCCManager::registerRead(int transactionId, const std::string& key, const EnhancedDataVersion& version) {
    pImpl->registerRead(transactionId, key, version);
}

void EnhancedMVCCManager::registerWrite(int transactionId, const std::string& key) {
    pImpl->registerWrite(transactionId, key);
}

bool EnhancedMVCCManager::isVisible(int transactionId, const EnhancedDataVersion& version, IsolationLevel isolation) {
    return pImpl->isVisible(transactionId, version, isolation);
}

bool EnhancedMVCCManager::preventPhantomReads(int transactionId, const std::string& keyPattern) {
    return pImpl->preventPhantomReads(transactionId, keyPattern);
}

bool EnhancedMVCCManager::detectWriteSkew(int transactionId) {
    return pImpl->detectWriteSkew(transactionId);
}

bool EnhancedMVCCManager::validateSnapshot(int transactionId) {
    return pImpl->validateSnapshot(transactionId);
}

EnhancedMVCCManager::TransactionStats EnhancedMVCCManager::getTransactionStats(int transactionId) const {
    return pImpl->getTransactionStats(transactionId);
}

} // namespace transaction
} // namespace phantomdb