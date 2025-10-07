#include "../src/query/sql_parser.h"
#include "../src/query/query_planner.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    SQLParser parser;
    QueryPlanner planner;
    std::string errorMsg;
    
    // Test 1: Simple subquery planning
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
    
    std::cout << "Test 1 passed: " << plan1->toString() << std::endl;
    
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
    
    std::cout << "Test 2 passed: " << plan2->toString() << std::endl;
    
    std::cout << "All subquery planning tests passed!" << std::endl;
    return 0;
}