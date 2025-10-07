#ifndef PHANTOMDB_ISOLATION_MANAGER_H
#define PHANTOMDB_ISOLATION_MANAGER_H

#include "transaction_manager.h"
#include "mvcc_manager.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace phantomdb {
namespace transaction {

// Forward declarations
class Transaction;
enum class IsolationLevel;

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
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace transaction
} // namespace phantomdb

#endif // PHANTOMDB_ISOLATION_MANAGER_H