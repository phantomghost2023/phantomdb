#include "sql_parser.h"
#include "query_planner.h"
#include "execution_engine.h"
#include "../transaction/transaction_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;
using namespace phantomdb::transaction;

int main() {
    std::cout << "Testing INSERT execution..." << std::endl;
    
    SQLParser parser;
    QueryPlanner planner;
    ExecutionEngine engine;
    std::string errorMsg;
    
    // Initialize the execution engine
    assert(engine.initialize());
    
    // Test basic INSERT statement
    std::string sql = "INSERT INTO users (id, name, age) VALUES ('1', 'John', '25')";
    auto ast = parser.parse(sql, errorMsg);
    
    assert(ast != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast->toString() << std::endl;
    
    // Generate plan
    auto plan = planner.generatePlan(ast.get(), errorMsg);
    
    assert(plan != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Generated plan: " << plan->toString() << std::endl;
    
    // Create a transaction
    auto transaction = std::make_shared<Transaction>(1, IsolationLevel::READ_COMMITTED);
    
    // Execute the plan
    std::vector<std::vector<std::string>> results;
    bool success = engine.executePlan(std::move(plan), transaction, results, errorMsg);
    
    assert(success);
    assert(errorMsg.empty());
    
    std::cout << "INSERT execution test passed!" << std::endl;
    
    // Shutdown the execution engine
    engine.shutdown();
    
    return 0;
}