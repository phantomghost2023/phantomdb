#include "sql_parser.h"
#include "query_planner.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing INSERT plan generation..." << std::endl;
    
    SQLParser parser;
    QueryPlanner planner;
    std::string errorMsg;
    
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
    
    // Verify it's an INSERT plan
    assert(plan->getType() == PlanNodeType::INSERT);
    
    std::cout << "INSERT plan generation test passed!" << std::endl;
    
    return 0;
}