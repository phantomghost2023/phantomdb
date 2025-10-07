#include "sql_parser.h"
#include "query_planner.h"
#include <iostream>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing SQL Parser and Query Planner compilation..." << std::endl;
    
    // Create a SQLParser instance
    SQLParser parser;
    
    // Parse a simple SELECT statement
    std::string errorMsg;
    auto ast = parser.parse("SELECT * FROM users", errorMsg);
    
    if (!ast) {
        std::cout << "Failed to parse SQL: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "SQL parsing successful!" << std::endl;
    
    // Create a QueryPlanner instance
    QueryPlanner planner;
    
    // Generate a plan
    auto plan = planner.generatePlan(ast.get(), errorMsg);
    
    if (!plan) {
        std::cout << "Failed to generate plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Plan generation successful!" << std::endl;
    std::cout << "Generated plan: " << plan->toString() << std::endl;
    
    std::cout << "SQL Parser and Query Planner compilation test passed!" << std::endl;
    return 0;
}