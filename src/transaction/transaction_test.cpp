#include "transaction_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::transaction;

void testTransactionManagerCreation() {
    std::cout << "Testing TransactionManager creation..." << std::endl;
    
    TransactionManager manager;
    assert(manager.initialize());
    
    std::cout << "TransactionManager creation test passed!" << std::endl;
}

void testBeginTransaction() {
    std::cout << "Testing begin transaction..." << std::endl;
    
    TransactionManager manager;
    manager.initialize();
    
    auto transaction = manager.beginTransaction();
    assert(transaction != nullptr);
    assert(transaction->getId() > 0);
    assert(transaction->getState() == TransactionState::ACTIVE);
    
    std::cout << "Begin transaction test passed!" << std::endl;
}

void testCommitTransaction() {
    std::cout << "Testing commit transaction..." << std::endl;
    
    TransactionManager manager;
    manager.initialize();
    
    auto transaction = manager.beginTransaction();
    assert(transaction != nullptr);
    
    bool result = manager.commitTransaction(transaction);
    assert(result);
    assert(transaction->getState() == TransactionState::COMMITTED);
    
    std::cout << "Commit transaction test passed!" << std::endl;
}

void testRollbackTransaction() {
    std::cout << "Testing rollback transaction..." << std::endl;
    
    TransactionManager manager;
    manager.initialize();
    
    auto transaction = manager.beginTransaction();
    assert(transaction != nullptr);
    
    bool result = manager.rollbackTransaction(transaction);
    assert(result);
    assert(transaction->getState() == TransactionState::ABORTED);
    
    std::cout << "Rollback transaction test passed!" << std::endl;
}

void testGetTransaction() {
    std::cout << "Testing get transaction..." << std::endl;
    
    TransactionManager manager;
    manager.initialize();
    
    auto transaction = manager.beginTransaction();
    assert(transaction != nullptr);
    
    int transactionId = transaction->getId();
    auto retrievedTransaction = manager.getTransaction(transactionId);
    assert(retrievedTransaction != nullptr);
    assert(retrievedTransaction->getId() == transactionId);
    
    std::cout << "Get transaction test passed!" << std::endl;
}

int main() {
    std::cout << "Running TransactionManager tests..." << std::endl;
    
    testTransactionManagerCreation();
    testBeginTransaction();
    testCommitTransaction();
    testRollbackTransaction();
    testGetTransaction();
    
    std::cout << "All TransactionManager tests passed!" << std::endl;
    return 0;
}