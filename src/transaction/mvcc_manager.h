#ifndef PHANTOMDB_MVCC_MANAGER_H
#define PHANTOMDB_MVCC_MANAGER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace phantomdb {
namespace transaction {

// Forward declarations
class Transaction;
enum class IsolationLevel;

// Timestamp type
using Timestamp = std::chrono::time_point<std::chrono::high_resolution_clock>;

// Data version structure
struct DataVersion {
    int transactionId;
    Timestamp timestamp;
    std::string data;
    bool isCommitted;
    
    DataVersion(int tid, const Timestamp& ts, const std::string& d, bool committed = false)
        : transactionId(tid), timestamp(ts), data(d), isCommitted(committed) {}
};

// MVCC Manager class
class MVCCManager {
public:
    MVCCManager();
    ~MVCCManager();
    
    // Initialize the MVCC manager
    bool initialize();
    
    // Shutdown the MVCC manager
    void shutdown();
    
    // Create a new version of data
    bool createVersion(int transactionId, const std::string& key, const std::string& data);
    
    // Read data with MVCC semantics
    bool readData(int transactionId, const std::string& key, std::string& data, IsolationLevel isolation);
    
    // Commit a transaction's versions
    bool commitTransaction(int transactionId);
    
    // Abort a transaction's versions
    bool abortTransaction(int transactionId);
    
    // Get current timestamp
    Timestamp getCurrentTimestamp() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace transaction
} // namespace phantomdb

#endif // PHANTOMDB_MVCC_MANAGER_H