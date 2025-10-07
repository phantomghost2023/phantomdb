#include "sql_parser.h"
#include "query_planner.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing JOIN plan generation..." << std::endl;
    
    SQLParser parser;
    QueryPlanner planner;
    std::string errorMsg;
    
    // Test basic SELECT with JOIN
    std::string sql = "SELECT * FROM users JOIN orders ON users.id = orders.user_id";
    auto ast = parser.parse(sql, errorMsg);
    
    assert(ast != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast->toString() << std::endl;
    
    // Generate plan
    auto plan = planner.generatePlan(ast.get(), errorMsg);
    
    assert(plan != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Generated plan: " << plan->toString() << std::endl;
    
    std::cout << "JOIN plan generation test passed!" << std::endl;
    
    return 0;
}