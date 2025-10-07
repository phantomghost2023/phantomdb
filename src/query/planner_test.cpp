#include "query_planner.h"
#include "sql_parser.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

void testBasicPlanGeneration() {
    std::cout << "Testing basic plan generation..." << std::endl;
    
    SQLParser parser;
    QueryPlanner planner;
    
    std::string errorMsg;
    
    // Parse a simple SELECT statement
    auto ast = parser.parse("SELECT * FROM users", errorMsg);
    assert(ast != nullptr);
    assert(errorMsg.empty());
    
    // Generate a plan
    auto plan = planner.generatePlan(ast.get(), errorMsg);
    assert(plan != nullptr);
    assert(errorMsg.empty());
    
    // Check that it's a table scan node
    auto tableScan = dynamic_cast<TableScanNode*>(plan.get());
    assert(tableScan != nullptr);
    assert(tableScan->getTableName() == "users");
    
    std::cout << "Basic plan generation test passed!" << std::endl;
}

void testPlanWithColumns() {
    std::cout << "Testing plan generation with specific columns..." << std::endl;
    
    SQLParser parser;
    QueryPlanner planner;
    
    std::string errorMsg;
    
    // Parse a SELECT statement with specific columns
    auto ast = parser.parse("SELECT id, name FROM users", errorMsg);
    assert(ast != nullptr);
    assert(errorMsg.empty());
    
    // Generate a plan
    auto plan = planner.generatePlan(ast.get(), errorMsg);
    assert(plan != nullptr);
    assert(errorMsg.empty());
    
    // Check that it's a table scan node
    auto tableScan = dynamic_cast<TableScanNode*>(plan.get());
    assert(tableScan != nullptr);
    assert(tableScan->getTableName() == "users");
    
    std::cout << "Plan generation with columns test passed!" << std::endl;
}

void testPlanGenerationError() {
    std::cout << "Testing plan generation error handling..." << std::endl;
    
    QueryPlanner planner;
    std::string errorMsg;
    
    // Try to generate a plan with null AST
    auto plan = planner.generatePlan(nullptr, errorMsg);
    assert(plan == nullptr);
    assert(!errorMsg.empty());
    
    std::cout << "Plan generation error handling test passed!" << std::endl;
}

int main() {
    std::cout << "Running Query Planner tests..." << std::endl;
    
    testBasicPlanGeneration();
    testPlanWithColumns();
    testPlanGenerationError();
    
    std::cout << "All Query Planner tests passed!" << std::endl;
    return 0;
}