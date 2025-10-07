#include "enhanced_query_planner.h"
#include "sql_parser.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Enhanced Query Planner..." << std::endl;
    
    // Create components
    phantomdb::query::EnhancedStatisticsManager statsManager;
    phantomdb::query::EnhancedQueryPlanner queryPlanner;
    
    // Initialize components
    assert(statsManager.initialize());
    assert(queryPlanner.initialize());
    
    // Set up dependencies
    queryPlanner.setStatisticsManager(&statsManager);
    
    // Test statistics manager
    std::cout << "\n--- Testing Statistics Manager ---" << std::endl;
    
    auto usersStats = statsManager.getTableStats("users");
    assert(usersStats != nullptr);
    std::cout << "Users table has " << usersStats->rowCount << " rows" << std::endl;
    
    auto userIdxStats = statsManager.getIndexStats("users_id_idx");
    assert(userIdxStats != nullptr);
    std::cout << "Users ID index type: " << 
        (userIdxStats->indexType == phantomdb::storage::IndexType::B_TREE ? "B-tree" : "Other") << std::endl;
    
    // Test selectivity estimation
    double selectivity = statsManager.estimateSelectivity("users", "id = 123");
    std::cout << "Estimated selectivity for 'id = 123': " << selectivity << std::endl;
    
    // Test query planner with a simple SELECT statement
    std::cout << "\n--- Testing Query Planner ---" << std::endl;
    
    phantomdb::query::SQLParser parser;
    std::string errorMsg;
    
    // Parse a simple SELECT query
    auto ast = parser.parse("SELECT * FROM users WHERE id = 123;", errorMsg);
    assert(ast != nullptr);
    
    // Generate optimized plan
    auto plan = queryPlanner.generateOptimizedPlan(ast.get(), errorMsg);
    assert(plan != nullptr);
    
    std::cout << "Generated plan: " << plan->toString() << std::endl;
    std::cout << "Plan cost: " << plan->getCost() << std::endl;
    
    // Test query planner with a JOIN query
    std::cout << "\n--- Testing JOIN Query ---" << std::endl;
    
    auto joinAst = parser.parse("SELECT * FROM users u JOIN orders o ON u.id = o.user_id;", errorMsg);
    assert(joinAst != nullptr);
    
    auto joinPlan = queryPlanner.generateOptimizedPlan(joinAst.get(), errorMsg);
    assert(joinPlan != nullptr);
    
    std::cout << "Generated JOIN plan: " << joinPlan->toString() << std::endl;
    std::cout << "JOIN plan cost: " << joinPlan->getCost() << std::endl;
    
    // Test query planner with a subquery
    std::cout << "\n--- Testing Subquery ---" << std::endl;
    
    auto subqueryAst = parser.parse("SELECT * FROM (SELECT id, name FROM users) AS u;", errorMsg);
    assert(subqueryAst != nullptr);
    
    auto subqueryPlan = queryPlanner.generateOptimizedPlan(subqueryAst.get(), errorMsg);
    assert(subqueryPlan != nullptr);
    
    std::cout << "Generated subquery plan: " << subqueryPlan->toString() << std::endl;
    std::cout << "Subquery plan cost: " << subqueryPlan->getCost() << std::endl;
    
    // Test query planner with INSERT statement
    std::cout << "\n--- Testing INSERT Statement ---" << std::endl;
    
    auto insertAst = parser.parse("INSERT INTO users (id, name, email) VALUES (1, 'John Doe', 'john@example.com');", errorMsg);
    assert(insertAst != nullptr);
    
    auto insertPlan = queryPlanner.generateOptimizedPlan(insertAst.get(), errorMsg);
    assert(insertPlan != nullptr);
    
    std::cout << "Generated INSERT plan: " << insertPlan->toString() << std::endl;
    std::cout << "INSERT plan cost: " << insertPlan->getCost() << std::endl;
    
    // Test query planner with UPDATE statement
    std::cout << "\n--- Testing UPDATE Statement ---" << std::endl;
    
    auto updateAst = parser.parse("UPDATE users SET name = 'Jane Smith' WHERE id = 1;", errorMsg);
    assert(updateAst != nullptr);
    
    auto updatePlan = queryPlanner.generateOptimizedPlan(updateAst.get(), errorMsg);
    assert(updatePlan != nullptr);
    
    std::cout << "Generated UPDATE plan: " << updatePlan->toString() << std::endl;
    std::cout << "UPDATE plan cost: " << updatePlan->getCost() << std::endl;
    
    // Test query planner with DELETE statement
    std::cout << "\n--- Testing DELETE Statement ---" << std::endl;
    
    auto deleteAst = parser.parse("DELETE FROM users WHERE id = 1;", errorMsg);
    assert(deleteAst != nullptr);
    
    auto deletePlan = queryPlanner.generateOptimizedPlan(deleteAst.get(), errorMsg);
    assert(deletePlan != nullptr);
    
    std::cout << "Generated DELETE plan: " << deletePlan->toString() << std::endl;
    std::cout << "DELETE plan cost: " << deletePlan->getCost() << std::endl;
    
    // Shutdown components
    queryPlanner.shutdown();
    statsManager.shutdown();
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}