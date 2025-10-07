#include "../src/query/sql_parser.h"
#include "../src/query/query_planner.h"
#include "../src/query/execution_engine.h"
#include "../src/transaction/transaction_manager.h"
#include <iostream>
#include <cassert>
#include <vector>

using namespace phantomdb::query;
using namespace phantomdb::transaction;

void testSubqueryParsing() {
    std::cout << "Testing subquery parsing..." << std::endl;
    
    SQLParser parser;
    std::string errorMsg;
    
    // Test 1: Simple subquery parsing
    std::string sql1 = "SELECT * FROM (SELECT id, name FROM users) AS subquery";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    assert(ast1 != nullptr && "Test 1 failed to parse");
    std::cout << "Test 1 passed: " << ast1->toString() << std::endl;
    
    // Test 2: Subquery with specific columns
    std::string sql2 = "SELECT id, name FROM (SELECT id, name, age FROM users WHERE age > 18) AS adults";
    auto ast2 = parser.parse(sql2, errorMsg);
    
    assert(ast2 != nullptr && "Test 2 failed to parse");
    std::cout << "Test 2 passed: " << ast2->toString() << std::endl;
    
    std::cout << "Subquery parsing tests passed!" << std::endl << std::endl;
}

void testSubqueryPlanning() {
    std::cout << "Testing subquery planning..." << std::endl;
    
    SQLParser parser;
    QueryPlanner planner;
    std::string errorMsg;
    
    // Test 1: Simple subquery planning
    std::string sql1 = "SELECT * FROM (SELECT id, name FROM users) AS subquery";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    assert(ast1 != nullptr && "Test 1 failed to parse");
    
    auto plan1 = planner.generatePlan(ast1.get(), errorMsg);
    assert(plan1 != nullptr && "Test 1 failed to generate plan");
    std::cout << "Test 1 passed: " << plan1->toString() << std::endl;
    
    // Test 2: Subquery with specific columns
    std::string sql2 = "SELECT id, name FROM (SELECT id, name, age FROM users WHERE age > 18) AS adults";
    auto ast2 = parser.parse(sql2, errorMsg);
    
    assert(ast2 != nullptr && "Test 2 failed to parse");
    
    auto plan2 = planner.generatePlan(ast2.get(), errorMsg);
    assert(plan2 != nullptr && "Test 2 failed to generate plan");
    std::cout << "Test 2 passed: " << plan2->toString() << std::endl;
    
    std::cout << "Subquery planning tests passed!" << std::endl << std::endl;
}

void testSubqueryExecution() {
    std::cout << "Testing subquery execution..." << std::endl;
    
    // Initialize components
    SQLParser parser;
    QueryPlanner planner;
    ExecutionEngine engine;
    
    // Initialize the execution engine
    assert(engine.initialize() && "Failed to initialize execution engine");
    
    // Create a mock transaction manager and transaction
    TransactionManager txnManager;
    auto transaction = txnManager.beginTransaction();
    
    std::string errorMsg;
    
    // Test 1: Simple subquery execution
    std::string sql1 = "SELECT * FROM (SELECT id, name FROM users) AS subquery";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    assert(ast1 != nullptr && "Test 1 failed to parse");
    
    auto plan1 = planner.generatePlan(ast1.get(), errorMsg);
    assert(plan1 != nullptr && "Test 1 failed to generate plan");
    
    std::vector<std::vector<std::string>> results1;
    assert(engine.executePlan(std::move(plan1), transaction, results1, errorMsg) && "Test 1 failed to execute");
    
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
    
    assert(ast2 != nullptr && "Test 2 failed to parse");
    
    auto plan2 = planner.generatePlan(ast2.get(), errorMsg);
    assert(plan2 != nullptr && "Test 2 failed to generate plan");
    
    std::vector<std::vector<std::string>> results2;
    assert(engine.executePlan(std::move(plan2), transaction, results2, errorMsg) && "Test 2 failed to execute");
    
    std::cout << "Test 2 passed. Results:" << std::endl;
    for (const auto& row : results2) {
        for (const auto& value : row) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    
    // Shutdown the execution engine
    engine.shutdown();
    
    std::cout << "Subquery execution tests passed!" << std::endl << std::endl;
}

int main() {
    try {
        testSubqueryParsing();
        testSubqueryPlanning();
        testSubqueryExecution();
        
        std::cout << "All subquery functionality tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}