#include "transaction_manager.h"
#include <iostream>

using namespace phantomdb::transaction;

int main() {
    std::cout << "Testing TransactionManager compilation..." << std::endl;
    
    // Create a TransactionManager instance
    TransactionManager manager;
    
    // Initialize the manager
    if (!manager.initialize()) {
        std::cout << "Failed to initialize TransactionManager!" << std::endl;
        return 1;
    }
    
    // Begin a transaction
    auto transaction = manager.beginTransaction();
    
    if (transaction) {
        std::cout << "Started transaction " << transaction->getId() << std::endl;
        
        // Commit the transaction
        bool success = manager.commitTransaction(transaction);
        
        if (success) {
            std::cout << "TransactionManager compilation test passed!" << std::endl;
            return 0;
        } else {
            std::cout << "Failed to commit transaction!" << std::endl;
            return 1;
        }
    } else {
        std::cout << "Failed to begin transaction!" << std::endl;
        return 1;
    }
}