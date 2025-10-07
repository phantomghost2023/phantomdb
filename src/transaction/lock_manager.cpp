#include "lock_manager.h"
#include <iostream>
#include <algorithm>

namespace phantomdb {
namespace transaction {

// LockManager implementation
class LockManager::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "Initializing Lock Manager..." << std::endl;
        return true;
    }
    
    void shutdown() {
        std::cout << "Shutting down Lock Manager..." << std::endl;
    }
    
    bool acquireLock(int transactionId, const std::string& resourceId, LockType lockType) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Check if this transaction already holds a lock on this resource
        auto transactionIt = transactionLocks_.find(transactionId);
        if (transactionIt != transactionLocks_.end()) {
            auto& lockedResources = transactionIt->second;
            if (lockedResources.find(resourceId) != lockedResources.end()) {
                // Transaction already holds a lock on this resource
                std::cout << "Transaction " << transactionId << " already holds lock on " << resourceId << std::endl;
                return true;
            }
        }
        
        // Check existing locks on this resource
        auto resourceIt = resourceLocks_.find(resourceId);
        if (resourceIt != resourceLocks_.end()) {
            const auto& existingLocks = resourceIt->second;
            
            // Check for conflicts
            for (const auto& request : existingLocks) {
                // Exclusive lock conflicts with any other lock
                if (lockType == LockType::EXCLUSIVE || request.lockType == LockType::EXCLUSIVE) {
                    if (request.transactionId != transactionId) {
                        // Conflict detected - wait for lock
                        std::cout << "Transaction " << transactionId << " waiting for lock on " << resourceId << std::endl;
                        
                        // For simplicity, we'll just fail the lock request
                        // In a real implementation, we would implement waiting
                        return false;
                    }
                }
            }
        }
        
        // No conflicts, acquire the lock
        LockRequest request(transactionId, lockType);
        resourceLocks_[resourceId].push_back(request);
        transactionLocks_[transactionId].insert(resourceId);
        
        std::cout << "Transaction " << transactionId << " acquired " 
                  << (lockType == LockType::SHARED ? "SHARED" : "EXCLUSIVE") 
                  << " lock on " << resourceId << std::endl;
        return true;
    }
    
    bool releaseLock(int transactionId, const std::string& resourceId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Remove the lock from resource locks
        auto resourceIt = resourceLocks_.find(resourceId);
        if (resourceIt != resourceLocks_.end()) {
            auto& requests = resourceIt->second;
            requests.erase(
                std::remove_if(requests.begin(), requests.end(),
                    [transactionId](const LockRequest& request) {
                        return request.transactionId == transactionId;
                    }),
                requests.end()
            );
            
            // If no more locks on this resource, remove the entry
            if (requests.empty()) {
                resourceLocks_.erase(resourceIt);
            }
        }
        
        // Remove from transaction locks
        auto transactionIt = transactionLocks_.find(transactionId);
        if (transactionIt != transactionLocks_.end()) {
            transactionIt->second.erase(resourceId);
            if (transactionIt->second.empty()) {
                transactionLocks_.erase(transactionIt);
            }
        }
        
        std::cout << "Transaction " << transactionId << " released lock on " << resourceId << std::endl;
        return true;
    }
    
    bool releaseAllLocks(int transactionId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Get all resources locked by this transaction
        auto transactionIt = transactionLocks_.find(transactionId);
        if (transactionIt == transactionLocks_.end()) {
            return true; // No locks to release
        }
        
        const auto& lockedResources = transactionIt->second;
        
        // Release each lock
        for (const auto& resourceId : lockedResources) {
            // Remove the lock from resource locks
            auto resourceIt = resourceLocks_.find(resourceId);
            if (resourceIt != resourceLocks_.end()) {
                auto& requests = resourceIt->second;
                requests.erase(
                    std::remove_if(requests.begin(), requests.end(),
                        [transactionId](const LockRequest& request) {
                            return request.transactionId == transactionId;
                        }),
                    requests.end()
                );
                
                // If no more locks on this resource, remove the entry
                if (requests.empty()) {
                    resourceLocks_.erase(resourceIt);
                }
            }
        }
        
        // Remove all locks from transaction locks
        transactionLocks_.erase(transactionIt);
        
        std::cout << "Transaction " << transactionId << " released all locks" << std::endl;
        return true;
    }
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::vector<LockRequest>> resourceLocks_;
    std::unordered_map<int, std::unordered_set<std::string>> transactionLocks_;
};

LockManager::LockManager() : pImpl(std::make_unique<Impl>()) {}

LockManager::~LockManager() = default;

bool LockManager::initialize() {
    return pImpl->initialize();
}

void LockManager::shutdown() {
    pImpl->shutdown();
}

bool LockManager::acquireLock(int transactionId, const std::string& resourceId, LockType lockType) {
    return pImpl->acquireLock(transactionId, resourceId, lockType);
}

bool LockManager::releaseLock(int transactionId, const std::string& resourceId) {
    return pImpl->releaseLock(transactionId, resourceId);
}

bool LockManager::releaseAllLocks(int transactionId) {
    return pImpl->releaseAllLocks(transactionId);
}

} // namespace transaction
} // namespace phantomdb