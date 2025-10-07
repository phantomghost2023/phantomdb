#include "sql_parser.h"
#include "query_planner.h"
#include "execution_engine.h"
#include "../transaction/transaction_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;
using namespace phantomdb::transaction;

int main() {
    std::cout << "Testing comprehensive JOIN functionality..." << std::endl;
    
    SQLParser parser;
    QueryPlanner planner;
    ExecutionEngine engine;
    std::string errorMsg;
    
    // Initialize the execution engine
    assert(engine.initialize());
    
    // Test 1: Basic SELECT with JOIN
    std::cout << "\n1. Testing basic SELECT with JOIN..." << std::endl;
    std::string sql1 = "SELECT * FROM users JOIN orders ON users.id = orders.user_id";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    assert(ast1 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast1->toString() << std::endl;
    
    auto plan1 = planner.generatePlan(ast1.get(), errorMsg);
    
    assert(plan1 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Generated plan: " << plan1->toString() << std::endl;
    
    auto transaction1 = std::make_shared<Transaction>(1, IsolationLevel::READ_COMMITTED);
    std::vector<std::vector<std::string>> results1;
    bool success1 = engine.executePlan(std::move(plan1), transaction1, results1, errorMsg);
    
    assert(success1);
    assert(errorMsg.empty());
    
    std::cout << "✓ Basic JOIN execution successful" << std::endl;
    
    // Test 2: SELECT with multiple JOINs
    std::cout << "\n2. Testing SELECT with multiple JOINs..." << std::endl;
    std::string sql2 = "SELECT users.name, orders.total, products.name FROM users JOIN orders ON users.id = orders.user_id JOIN products ON orders.product_id = products.id";
    auto ast2 = parser.parse(sql2, errorMsg);
    
    assert(ast2 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast2->toString() << std::endl;
    
    auto plan2 = planner.generatePlan(ast2.get(), errorMsg);
    
    assert(plan2 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Generated plan: " << plan2->toString() << std::endl;
    
    auto transaction2 = std::make_shared<Transaction>(2, IsolationLevel::READ_COMMITTED);
    std::vector<std::vector<std::string>> results2;
    bool success2 = engine.executePlan(std::move(plan2), transaction2, results2, errorMsg);
    
    assert(success2);
    assert(errorMsg.empty());
    
    std::cout << "✓ Multiple JOINs execution successful" << std::endl;
    
    // Test 3: SELECT with JOIN and WHERE clause
    std::cout << "\n3. Testing SELECT with JOIN and WHERE clause..." << std::endl;
    std::string sql3 = "SELECT users.name, orders.total FROM users JOIN orders ON users.id = orders.user_id WHERE users.age > 18";
    auto ast3 = parser.parse(sql3, errorMsg);
    
    assert(ast3 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast3->toString() << std::endl;
    
    auto plan3 = planner.generatePlan(ast3.get(), errorMsg);
    
    assert(plan3 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Generated plan: " << plan3->toString() << std::endl;
    
    auto transaction3 = std::make_shared<Transaction>(3, IsolationLevel::READ_COMMITTED);
    std::vector<std::vector<std::string>> results3;
    bool success3 = engine.executePlan(std::move(plan3), transaction3, results3, errorMsg);
    
    assert(success3);
    assert(errorMsg.empty());
    
    std::cout << "✓ JOIN with WHERE clause execution successful" << std::endl;
    
    std::cout << "\nAll comprehensive JOIN tests passed!" << std::endl;
    
    // Shutdown the execution engine
    engine.shutdown();
    
    return 0;
}