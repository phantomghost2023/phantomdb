#include <iostream>
#include <memory>
#include <string>

// Include our headers directly to test compilation
#include "src/query/sql_parser.h"
#include "src/query/query_planner.h"
#include "src/query/execution_engine.h"

using namespace phantomdb::query;

int main() {
    std::cout << "Verifying DELETE statement implementation..." << std::endl;
    
    // Test 1: Check if DeleteStatement class exists and is accessible
    try {
        SQLParser parser;
        std::string errorMsg;
        
        // Test DELETE parsing
        std::string deleteSql = "DELETE FROM users WHERE id = 1";
        auto ast = parser.parse(deleteSql, errorMsg);
        
        if (ast) {
            std::cout << "✓ DELETE statement parsing successful" << std::endl;
            std::cout << "  Parsed AST: " << ast->toString() << std::endl;
        } else {
            std::cout << "✗ DELETE statement parsing failed: " << errorMsg << std::endl;
            return 1;
        }
        
        // Test 2: Check if DeleteNode plan generation works
        QueryPlanner planner;
        auto plan = planner.generatePlan(ast.get(), errorMsg);
        
        if (plan) {
            std::cout << "✓ DELETE plan generation successful" << std::endl;
            std::cout << "  Generated plan: " << plan->toString() << std::endl;
            
            // Verify it's a DELETE plan
            if (plan->getType() == PlanNodeType::DELETE) {
                std::cout << "✓ Plan type correctly identified as DELETE" << std::endl;
            } else {
                std::cout << "✗ Plan type is not DELETE: " << static_cast<int>(plan->getType()) << std::endl;
                return 1;
            }
        } else {
            std::cout << "✗ DELETE plan generation failed: " << errorMsg << std::endl;
            return 1;
        }
        
        std::cout << "\nAll verification tests passed!" << std::endl;
        std::cout << "DELETE statement implementation is working correctly." << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Exception occurred during verification: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}