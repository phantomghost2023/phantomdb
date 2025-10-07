#include "sql_parser.h"
#include "query_planner.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing DELETE plan generation..." << std::endl;
    
    SQLParser parser;
    QueryPlanner planner;
    std::string errorMsg;
    
    // Test basic DELETE statement
    std::string sql = "DELETE FROM users WHERE id = 1";
    auto ast = parser.parse(sql, errorMsg);
    
    assert(ast != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast->toString() << std::endl;
    
    // Generate plan
    auto plan = planner.generatePlan(ast.get(), errorMsg);
    
    assert(plan != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Generated plan: " << plan->toString() << std::endl;
    
    // Verify it's a DELETE plan
    assert(plan->getType() == PlanNodeType::DELETE);
    
    std::cout << "DELETE plan generation test passed!" << std::endl;
    
    return 0;
}