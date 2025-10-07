#include "query_optimizer.h"
#include "query_planner.h"
#include "sql_parser.h"
#include <iostream>
#include <memory>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing enhanced Query Optimizer..." << std::endl;
    
    // Initialize the query optimizer
    QueryOptimizer optimizer;
    if (!optimizer.initialize()) {
        std::cerr << "Failed to initialize query optimizer" << std::endl;
        return 1;
    }
    
    // Create a simple SQL parser
    SQLParser parser;
    if (!parser.initialize()) {
        std::cerr << "Failed to initialize SQL parser" << std::endl;
        return 1;
    }
    
    // Create a query planner
    QueryPlanner planner;
    
    // Test 1: Simple SELECT query
    std::cout << "\n--- Test 1: Simple SELECT query ---" << std::endl;
    std::string sql1 = "SELECT * FROM users";
    std::string errorMsg;
    
    auto ast1 = parser.parse(sql1, errorMsg);
    if (!ast1) {
        std::cerr << "Failed to parse SQL: " << errorMsg << std::endl;
        return 1;
    }
    
    auto plan1 = planner.generatePlan(ast1.get(), errorMsg);
    if (!plan1) {
        std::cerr << "Failed to generate plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Original plan cost: " << plan1->getCost() << std::endl;
    
    auto optimizedPlan1 = optimizer.optimize(std::move(plan1), errorMsg);
    if (!optimizedPlan1) {
        std::cerr << "Failed to optimize plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Optimized plan cost: " << optimizedPlan1->getCost() << std::endl;
    std::cout << "Plan: " << optimizedPlan1->toString() << std::endl;
    
    // Test 2: SELECT with JOIN
    std::cout << "\n--- Test 2: SELECT with JOIN ---" << std::endl;
    std::string sql2 = "SELECT * FROM users u JOIN orders o ON u.id = o.user_id";
    errorMsg.clear();
    
    auto ast2 = parser.parse(sql2, errorMsg);
    if (!ast2) {
        std::cerr << "Failed to parse SQL: " << errorMsg << std::endl;
        return 1;
    }
    
    auto plan2 = planner.generatePlan(ast2.get(), errorMsg);
    if (!plan2) {
        std::cerr << "Failed to generate plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Original plan cost: " << plan2->getCost() << std::endl;
    
    auto optimizedPlan2 = optimizer.optimize(std::move(plan2), errorMsg);
    if (!optimizedPlan2) {
        std::cerr << "Failed to optimize plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Optimized plan cost: " << optimizedPlan2->getCost() << std::endl;
    std::cout << "Plan: " << optimizedPlan2->toString() << std::endl;
    
    // Test 3: INSERT query
    std::cout << "\n--- Test 3: INSERT query ---" << std::endl;
    std::string sql3 = "INSERT INTO users (id, name, email) VALUES (1, 'John Doe', 'john@example.com')";
    errorMsg.clear();
    
    auto ast3 = parser.parse(sql3, errorMsg);
    if (!ast3) {
        std::cerr << "Failed to parse SQL: " << errorMsg << std::endl;
        return 1;
    }
    
    auto plan3 = planner.generatePlan(ast3.get(), errorMsg);
    if (!plan3) {
        std::cerr << "Failed to generate plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Original plan cost: " << plan3->getCost() << std::endl;
    
    auto optimizedPlan3 = optimizer.optimize(std::move(plan3), errorMsg);
    if (!optimizedPlan3) {
        std::cerr << "Failed to optimize plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Optimized plan cost: " << optimizedPlan3->getCost() << std::endl;
    std::cout << "Plan: " << optimizedPlan3->toString() << std::endl;
    
    // Test 4: UPDATE query
    std::cout << "\n--- Test 4: UPDATE query ---" << std::endl;
    std::string sql4 = "UPDATE users SET name = 'Jane Doe' WHERE id = 1";
    errorMsg.clear();
    
    auto ast4 = parser.parse(sql4, errorMsg);
    if (!ast4) {
        std::cerr << "Failed to parse SQL: " << errorMsg << std::endl;
        return 1;
    }
    
    auto plan4 = planner.generatePlan(ast4.get(), errorMsg);
    if (!plan4) {
        std::cerr << "Failed to generate plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Original plan cost: " << plan4->getCost() << std::endl;
    
    auto optimizedPlan4 = optimizer.optimize(std::move(plan4), errorMsg);
    if (!optimizedPlan4) {
        std::cerr << "Failed to optimize plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Optimized plan cost: " << optimizedPlan4->getCost() << std::endl;
    std::cout << "Plan: " << optimizedPlan4->toString() << std::endl;
    
    // Test 5: DELETE query
    std::cout << "\n--- Test 5: DELETE query ---" << std::endl;
    std::string sql5 = "DELETE FROM users WHERE id = 1";
    errorMsg.clear();
    
    auto ast5 = parser.parse(sql5, errorMsg);
    if (!ast5) {
        std::cerr << "Failed to parse SQL: " << errorMsg << std::endl;
        return 1;
    }
    
    auto plan5 = planner.generatePlan(ast5.get(), errorMsg);
    if (!plan5) {
        std::cerr << "Failed to generate plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Original plan cost: " << plan5->getCost() << std::endl;
    
    auto optimizedPlan5 = optimizer.optimize(std::move(plan5), errorMsg);
    if (!optimizedPlan5) {
        std::cerr << "Failed to optimize plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Optimized plan cost: " << optimizedPlan5->getCost() << std::endl;
    std::cout << "Plan: " << optimizedPlan5->toString() << std::endl;
    
    // Shutdown components
    optimizer.shutdown();
    parser.shutdown();
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}