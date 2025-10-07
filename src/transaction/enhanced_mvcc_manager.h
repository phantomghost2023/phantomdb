#ifndef PHANTOMDB_ENHANCED_MVCC_MANAGER_H
#define PHANTOMDB_ENHANCED_MVCC_MANAGER_H

#include "transaction_manager.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <chrono>

namespace phantomdb {
namespace transaction {

// Enhanced timestamp type
using EnhancedTimestamp = std::chrono::time_point<std::chrono::high_resolution_clock>;

// Enhanced data version with more detailed information
struct EnhancedDataVersion {
    int transactionId;
    EnhancedTimestamp timestamp;
    EnhancedTimestamp commitTimestamp;
    std::string data;
    bool isCommitted;
    bool isAborted;
    std::vector<std::string> readBy;  // Transactions that have read this version
    
    EnhancedDataVersion(int txId, EnhancedTimestamp ts, const std::string& d)
        : transactionId(txId), timestamp(ts), commitTimestamp(ts), 
          data(d), isCommitted(false), isAborted(false) {}
};

// Transaction snapshot with read and write sets
struct EnhancedTransactionSnapshot {
    int transactionId;
    EnhancedTimestamp timestamp;
    std::unordered_set<std::string> readSet;
    std::unordered_set<std::string> writeSet;
    std::unordered_map<std::string, EnhancedDataVersion> readVersions;
    
    EnhancedTransactionSnapshot(int txId, EnhancedTimestamp ts)
        : transactionId(txId), timestamp(ts) {}
};

// Enhanced MVCC manager with full ACID semantics
class EnhancedMVCCManager {
public:
    EnhancedMVCCManager();
    ~EnhancedMVCCManager();
    
    // Initialize the MVCC manager
    bool initialize();
    
    // Shutdown the MVCC manager
    void shutdown();
    
    // Create a new version of data
    bool createVersion(int transactionId, const std::string& key, const std::string& data);
    
    // Read data with full isolation support
    bool readData(int transactionId, const std::string& key, std::string& data, IsolationLevel isolation);
    
    // Write data with full isolation support
    bool writeData(int transactionId, const std::string& key, const std::string& data, IsolationLevel isolation);
    
    // Commit a transaction
    bool commitTransaction(int transactionId);
    
    // Abort a transaction
    bool abortTransaction(int transactionId);
    
    // Check for conflicts before committing
    bool hasConflicts(int transactionId, IsolationLevel isolation);
    
    // Get current timestamp
    EnhancedTimestamp getCurrentTimestamp() const;
    
    // Create a snapshot for a transaction
    bool createSnapshot(int transactionId);
    
    // Get snapshot for a transaction
    EnhancedTransactionSnapshot* getSnapshot(int transactionId);
    
    // Register a read operation
    void registerRead(int transactionId, const std::string& key, const EnhancedDataVersion& version);
    
    // Register a write operation
    void registerWrite(int transactionId, const std::string& key);
    
    // Check if a version is visible to a transaction
    bool isVisible(int transactionId, const EnhancedDataVersion& version, IsolationLevel isolation);
    
    // Prevent phantom reads
    bool preventPhantomReads(int transactionId, const std::string& keyPattern);
    
    // Detect write skew
    bool detectWriteSkew(int transactionId);
    
    // Validate snapshot consistency
    bool validateSnapshot(int transactionId);
    
    // Get transaction statistics
    struct TransactionStats {
        int transactionId;
        size_t readOperations;
        size_t writeOperations;
        size_t conflictsDetected;
        std::chrono::milliseconds duration;
        
        TransactionStats(int txId) : transactionId(txId), readOperations(0), writeOperations(0), 
                                   conflictsDetected(0), duration(0) {}
    };
    
    TransactionStats getTransactionStats(int transactionId) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace transaction
} // namespace phantomdb

#endif // PHANTOMDB_ENHANCED_MVCC_MANAGER_H