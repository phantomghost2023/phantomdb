#include <iostream>
#include <memory>
#include <string>

// Include the necessary headers
#include "src/query/sql_parser.h"
#include "src/query/query_planner.h"

using namespace phantomdb::query;

int main() {
    std::cout << "Testing DELETE statement implementation..." << std::endl;
    
    try {
        // Test DELETE statement parsing
        std::cout << "\n1. Testing DELETE statement parsing..." << std::endl;
        SQLParser parser;
        std::string errorMsg;
        
        // Test basic DELETE statement
        std::string sql1 = "DELETE FROM users WHERE id = 1";
        auto ast1 = parser.parse(sql1, errorMsg);
        
        if (ast1) {
            std::cout << "✓ DELETE statement parsed successfully: " << ast1->toString() << std::endl;
        } else {
            std::cout << "✗ Failed to parse DELETE statement: " << errorMsg << std::endl;
            return 1;
        }
        
        // Test DELETE statement without WHERE clause
        std::string sql2 = "DELETE FROM users";
        auto ast2 = parser.parse(sql2, errorMsg);
        
        if (ast2) {
            std::cout << "✓ DELETE statement without WHERE parsed successfully: " << ast2->toString() << std::endl;
        } else {
            std::cout << "✗ Failed to parse DELETE statement without WHERE: " << errorMsg << std::endl;
            return 1;
        }
        
        // Test DELETE plan generation
        std::cout << "\n2. Testing DELETE plan generation..." << std::endl;
        QueryPlanner planner;
        
        auto plan1 = planner.generatePlan(ast1.get(), errorMsg);
        
        if (plan1) {
            std::cout << "✓ DELETE plan generated successfully: " << plan1->toString() << std::endl;
            
            // Verify it's a DELETE plan
            if (plan1->getType() == PlanNodeType::DELETE) {
                std::cout << "✓ Plan type correctly identified as DELETE" << std::endl;
            } else {
                std::cout << "✗ Plan type is not DELETE: " << static_cast<int>(plan1->getType()) << std::endl;
                return 1;
            }
        } else {
            std::cout << "✗ Failed to generate DELETE plan: " << errorMsg << std::endl;
            return 1;
        }
        
        auto plan2 = planner.generatePlan(ast2.get(), errorMsg);
        
        if (plan2) {
            std::cout << "✓ DELETE plan without WHERE generated successfully: " << plan2->toString() << std::endl;
            
            // Verify it's a DELETE plan
            if (plan2->getType() == PlanNodeType::DELETE) {
                std::cout << "✓ Plan type correctly identified as DELETE" << std::endl;
            } else {
                std::cout << "✗ Plan type is not DELETE: " << static_cast<int>(plan2->getType()) << std::endl;
                return 1;
            }
        } else {
            std::cout << "✗ Failed to generate DELETE plan without WHERE: " << errorMsg << std::endl;
            return 1;
        }
        
        std::cout << "\nAll tests passed! DELETE statement implementation is working correctly." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Exception occurred during testing: " << e.what() << std::endl;
        return 1;
    }
}