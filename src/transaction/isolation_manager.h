#ifndef PHANTOMDB_ISOLATION_MANAGER_H
#define PHANTOMDB_ISOLATION_MANAGER_H

#include "transaction_manager.h"
#include "mvcc_manager.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <chrono>

namespace phantomdb {
namespace transaction {

// Forward declarations
class Transaction;
enum class IsolationLevel;

// Snapshot structure for SNAPSHOT isolation level
struct TransactionSnapshot {
    int transactionId;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::unordered_set<std::string> readKeys;
    
    TransactionSnapshot(int id, const std::chrono::time_point<std::chrono::high_resolution_clock>& ts)
        : transactionId(id), timestamp(ts) {}
};

// Isolation level manager class
class IsolationManager {
public:
    IsolationManager();
    ~IsolationManager();
    
    // Initialize the isolation manager
    bool initialize();
    
    // Shutdown the isolation manager
    void shutdown();
    
    // Check if a read operation is allowed under the given isolation level
    bool isReadAllowed(IsolationLevel level, const std::string& key) const;
    
    // Check if a write operation is allowed under the given isolation level
    bool isWriteAllowed(IsolationLevel level, const std::string& key) const;
    
    // Get the visibility predicate for the given isolation level
    bool isVisible(IsolationLevel level, int transactionId, const DataVersion& version) const;
    
    // Handle phantom read prevention for SERIALIZABLE isolation
    bool preventPhantomReads(IsolationLevel level, int transactionId, const std::string& key);
    
    // Create a snapshot for the transaction (for SNAPSHOT isolation)
    bool createSnapshot(int transactionId);
    
    // Get transaction snapshot
    TransactionSnapshot* getSnapshot(int transactionId) const;
    
    // Check for write conflicts
    bool hasWriteConflict(int transactionId, const std::string& key) const;
    
    // Register a read operation
    void registerRead(int transactionId, const std::string& key);
    
    // Register a write operation
    void registerWrite(int transactionId, const std::string& key);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace transaction
} // namespace phantomdb

#endif // PHANTOMDB_ISOLATION_MANAGER_H