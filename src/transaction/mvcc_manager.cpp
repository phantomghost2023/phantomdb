#include "mvcc_manager.h"
#include "isolation_manager.h"
#include "transaction_manager.h"
#include <iostream>
#include <algorithm>
#include <chrono>

namespace phantomdb {
namespace transaction {

// MVCCManager implementation
class MVCCManager::Impl {
public:
    Impl() : isolationManager_(std::make_unique<IsolationManager>()) {}
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "Initializing MVCC Manager..." << std::endl;
        return isolationManager_->initialize();
    }
    
    void shutdown() {
        std::cout << "Shutting down MVCC Manager..." << std::endl;
        isolationManager_->shutdown();
    }
    
    bool createVersion(int transactionId, const std::string& key, const std::string& data) {
        std::lock_guard<std::shared_mutex> lock(rwMutex_);
        
        // Create a new version with current timestamp
        Timestamp timestamp = getCurrentTimestamp();
        DataVersion version(transactionId, timestamp, data, false);
        
        // Add the version to the version chain for this key
        versionChains_[key].push_back(std::move(version));
        
        std::cout << "Created version for key " << key << " in transaction " << transactionId << std::endl;
        return true;
    }
    
    bool readData(int transactionId, const std::string& key, std::string& data, IsolationLevel isolation) {
        std::shared_lock<std::shared_mutex> lock(rwMutex_);
        
        // Check if read is allowed under this isolation level
        if (!isolationManager_->isReadAllowed(isolation, key)) {
            return false;
        }
        
        // Register the read operation
        isolationManager_->registerRead(transactionId, key);
        
        // Prevent phantom reads for SERIALIZABLE isolation
        isolationManager_->preventPhantomReads(isolation, transactionId, key);
        
        // Find the version chain for this key
        auto it = versionChains_.find(key);
        if (it == versionChains_.end() || it->second.empty()) {
            // No versions exist for this key
            return false;
        }
        
        const auto& versions = it->second;
        
        // Find the most recent version that is visible to this transaction
        for (auto rit = versions.rbegin(); rit != versions.rend(); ++rit) {
            if (isolationManager_->isVisible(isolation, transactionId, *rit)) {
                data = rit->data;
                return true;
            }
        }
        
        // No visible version found
        return false;
    }
    
    bool writeData(int transactionId, const std::string& key, const std::string& data, IsolationLevel isolation) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        
        // Check if write is allowed under this isolation level
        if (!isolationManager_->isWriteAllowed(isolation, key)) {
            return false;
        }
        
        // Register the write operation
        isolationManager_->registerWrite(transactionId, key);
        
        // Create a new version
        Timestamp timestamp = getCurrentTimestamp();
        DataVersion version(transactionId, timestamp, data, false);
        
        // Add the version to the version chain for this key
        versionChains_[key].push_back(std::move(version));
        
        std::cout << "Wrote version for key " << key << " in transaction " << transactionId << std::endl;
        return true;
    }
    
    bool commitTransaction(int transactionId) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        
        // Mark all versions created by this transaction as committed
        for (auto& pair : versionChains_) {
            for (auto& version : pair.second) {
                if (version.transactionId == transactionId) {
                    version.isCommitted = true;
                }
            }
        }
        
        std::cout << "Committed versions for transaction " << transactionId << std::endl;
        return true;
    }
    
    bool abortTransaction(int transactionId) {
        std::unique_lock<std::shared_mutex> lock(rwMutex_);
        
        // Remove all versions created by this transaction
        for (auto& pair : versionChains_) {
            auto& versions = pair.second;
            versions.erase(
                std::remove_if(versions.begin(), versions.end(),
                    [transactionId](const DataVersion& version) {
                        return version.transactionId == transactionId;
                    }),
                versions.end()
            );
        }
        
        std::cout << "Aborted versions for transaction " << transactionId << std::endl;
        return true;
    }
    
    Timestamp getCurrentTimestamp() const {
        return std::chrono::high_resolution_clock::now();
    }
    
    bool hasConflicts(int transactionId, IsolationLevel isolation) const {
        std::shared_lock<std::shared_mutex> lock(rwMutex_);
        
        // For READ_COMMITTED and below, no conflict detection is needed
        if (isolation == IsolationLevel::READ_UNCOMMITTED || 
            isolation == IsolationLevel::READ_COMMITTED) {
            return false;
        }
        
        // Check for write conflicts
        return isolationManager_->hasWriteConflict(transactionId, "");
    }
    
private:
    mutable std::shared_mutex rwMutex_;
    std::unordered_map<std::string, std::vector<DataVersion>> versionChains_;
    std::unique_ptr<IsolationManager> isolationManager_;
};

MVCCManager::MVCCManager() : pImpl(std::make_unique<Impl>()) {}

MVCCManager::~MVCCManager() = default;

bool MVCCManager::initialize() {
    return pImpl->initialize();
}

void MVCCManager::shutdown() {
    pImpl->shutdown();
}

bool MVCCManager::createVersion(int transactionId, const std::string& key, const std::string& data) {
    return pImpl->createVersion(transactionId, key, data);
}

bool MVCCManager::readData(int transactionId, const std::string& key, std::string& data, IsolationLevel isolation) {
    return pImpl->readData(transactionId, key, data, isolation);
}

bool MVCCManager::writeData(int transactionId, const std::string& key, const std::string& data, IsolationLevel isolation) {
    return pImpl->writeData(transactionId, key, data, isolation);
}

bool MVCCManager::commitTransaction(int transactionId) {
    return pImpl->commitTransaction(transactionId);
}

bool MVCCManager::abortTransaction(int transactionId) {
    return pImpl->abortTransaction(transactionId);
}

Timestamp MVCCManager::getCurrentTimestamp() const {
    return pImpl->getCurrentTimestamp();
}

bool MVCCManager::hasConflicts(int transactionId, IsolationLevel isolation) const {
    return pImpl->hasConflicts(transactionId, isolation);
}

} // namespace transaction
} // namespace phantomdb