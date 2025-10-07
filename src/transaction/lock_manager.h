#ifndef PHANTOMDB_LOCK_MANAGER_H
#define PHANTOMDB_LOCK_MANAGER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <condition_variable>

namespace phantomdb {
namespace transaction {

// Lock types
enum class LockType {
    SHARED,      // Shared lock (read lock)
    EXCLUSIVE    // Exclusive lock (write lock)
};

// Lock request structure
struct LockRequest {
    int transactionId;
    LockType lockType;
    
    LockRequest(int tid, LockType type) : transactionId(tid), lockType(type) {}
};

// Lock manager class
class LockManager {
public:
    LockManager();
    ~LockManager();
    
    // Initialize the lock manager
    bool initialize();
    
    // Shutdown the lock manager
    void shutdown();
    
    // Acquire a lock
    bool acquireLock(int transactionId, const std::string& resourceId, LockType lockType);
    
    // Release a lock
    bool releaseLock(int transactionId, const std::string& resourceId);
    
    // Release all locks for a transaction
    bool releaseAllLocks(int transactionId);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace transaction
} // namespace phantomdb

#endif // PHANTOMDB_LOCK_MANAGER_H