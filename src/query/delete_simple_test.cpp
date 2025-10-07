#include "sql_parser.h"
#include "query_planner.h"
#include "execution_engine.h"
#include "../transaction/transaction_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;
using namespace phantomdb::transaction;

int main() {
    std::cout << "Testing DELETE statement functionality..." << std::endl;
    
    // Test DELETE statement parsing
    std::cout << "\n1. Testing DELETE statement parsing..." << std::endl;
    SQLParser parser;
    std::string errorMsg;
    
    std::string sql = "DELETE FROM users WHERE id = 1";
    auto ast = parser.parse(sql, errorMsg);
    
    if (ast) {
        std::cout << "✓ DELETE statement parsed successfully: " << ast->toString() << std::endl;
    } else {
        std::cout << "✗ Failed to parse DELETE statement: " << errorMsg << std::endl;
        return 1;
    }
    
    // Test DELETE plan generation
    std::cout << "\n2. Testing DELETE plan generation..." << std::endl;
    QueryPlanner planner;
    
    auto plan = planner.generatePlan(ast.get(), errorMsg);
    
    if (plan) {
        std::cout << "✓ DELETE plan generated successfully: " << plan->toString() << std::endl;
    } else {
        std::cout << "✗ Failed to generate DELETE plan: " << errorMsg << std::endl;
        return 1;
    }
    
    // Test DELETE execution
    std::cout << "\n3. Testing DELETE execution..." << std::endl;
    ExecutionEngine engine;
    
    if (!engine.initialize()) {
        std::cout << "✗ Failed to initialize execution engine" << std::endl;
        return 1;
    }
    
    auto transaction = std::make_shared<Transaction>(1, IsolationLevel::READ_COMMITTED);
    std::vector<std::vector<std::string>> results;
    bool success = engine.executePlan(std::move(plan), transaction, results, errorMsg);
    
    if (success) {
        std::cout << "✓ DELETE executed successfully" << std::endl;
    } else {
        std::cout << "✗ Failed to execute DELETE: " << errorMsg << std::endl;
        engine.shutdown();
        return 1;
    }
    
    engine.shutdown();
    
    std::cout << "\nAll DELETE tests passed!" << std::endl;
    return 0;
}