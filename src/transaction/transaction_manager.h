#ifndef PHANTOMDB_TRANSACTION_MANAGER_H
#define PHANTOMDB_TRANSACTION_MANAGER_H

#include <string>
#include <memory>
#include <vector>

namespace phantomdb {
namespace transaction {

enum class IsolationLevel {
    READ_UNCOMMITTED,
    READ_COMMITTED,
    REPEATABLE_READ,
    SERIALIZABLE,
    SNAPSHOT
};

enum class TransactionState {
    ACTIVE,
    PARTIALLY_COMMITTED,
    COMMITTED,
    FAILED,
    ABORTED,
    TERMINATED
};

class Transaction {
public:
    Transaction(int id, IsolationLevel isolation = IsolationLevel::READ_COMMITTED);
    ~Transaction();
    
    int getId() const;
    IsolationLevel getIsolationLevel() const;
    TransactionState getState() const;
    
    void setState(TransactionState state);
    
private:
    int id_;
    IsolationLevel isolationLevel_;
    TransactionState state_;
};

class TransactionManager {
public:
    TransactionManager();
    ~TransactionManager();
    
    // Initialize the transaction manager
    bool initialize();
    
    // Shutdown the transaction manager
    void shutdown();
    
    // Begin a new transaction
    std::shared_ptr<Transaction> beginTransaction(IsolationLevel isolation = IsolationLevel::READ_COMMITTED);
    
    // Commit a transaction
    bool commitTransaction(std::shared_ptr<Transaction> transaction);
    
    // Rollback a transaction
    bool rollbackTransaction(std::shared_ptr<Transaction> transaction);
    
    // Get transaction by ID
    std::shared_ptr<Transaction> getTransaction(int id) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace transaction
} // namespace phantomdb

#endif // PHANTOMDB_TRANSACTION_MANAGER_H