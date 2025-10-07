#include "sql_parser.h"
#include "query_planner.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing UPDATE plan generation..." << std::endl;
    
    SQLParser parser;
    QueryPlanner planner;
    std::string errorMsg;
    
    // Test basic UPDATE statement
    std::string sql = "UPDATE users SET name = 'John Doe' WHERE id = 1";
    auto ast = parser.parse(sql, errorMsg);
    
    assert(ast != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast->toString() << std::endl;
    
    // Generate plan
    auto plan = planner.generatePlan(ast.get(), errorMsg);
    
    assert(plan != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Generated plan: " << plan->toString() << std::endl;
    
    // Verify it's an UPDATE plan
    assert(plan->getType() == PlanNodeType::UPDATE);
    
    std::cout << "UPDATE plan generation test passed!" << std::endl;
    
    return 0;
}