#include "../src/query/sql_parser.h"
#include "../src/query/query_planner.h"
#include "../src/query/execution_engine.h"
#include "../src/transaction/transaction_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;
using namespace phantomdb::transaction;

int main() {
    // Initialize components
    SQLParser parser;
    QueryPlanner planner;
    ExecutionEngine engine;
    
    // Initialize the execution engine
    if (!engine.initialize()) {
        std::cout << "Failed to initialize execution engine" << std::endl;
        return 1;
    }
    
    // Create a mock transaction manager and transaction
    TransactionManager txnManager;
    auto transaction = txnManager.beginTransaction();
    
    std::string errorMsg;
    
    // Test 1: Simple subquery execution
    std::string sql1 = "SELECT * FROM (SELECT id, name FROM users) AS subquery";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    if (!ast1) {
        std::cout << "Test 1 failed to parse: " << errorMsg << std::endl;
        return 1;
    }
    
    auto plan1 = planner.generatePlan(ast1.get(), errorMsg);
    if (!plan1) {
        std::cout << "Test 1 failed to generate plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::vector<std::vector<std::string>> results1;
    if (!engine.executePlan(std::move(plan1), transaction, results1, errorMsg)) {
        std::cout << "Test 1 failed to execute: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Test 1 passed. Results:" << std::endl;
    for (const auto& row : results1) {
        for (const auto& value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    
    // Test 2: Subquery with specific columns
    std::string sql2 = "SELECT id, name FROM (SELECT id, name, age FROM users WHERE age > 18) AS adults";
    auto ast2 = parser.parse(sql2, errorMsg);
    
    if (!ast2) {
        std::cout << "Test 2 failed to parse: " << errorMsg << std::endl;
        return 1;
    }
    
    auto plan2 = planner.generatePlan(ast2.get(), errorMsg);
    if (!plan2) {
        std::cout << "Test 2 failed to generate plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::vector<std::vector<std::string>> results2;
    if (!engine.executePlan(std::move(plan2), transaction, results2, errorMsg)) {
        std::cout << "Test 2 failed to execute: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Test 2 passed. Results:" << std::endl;
    for (const auto& row : results2) {
        for (const auto& value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    
    // Shutdown the execution engine
    engine.shutdown();
    
    std::cout << "All subquery execution tests passed!" << std::endl;
    return 0;
}