#include "query_optimizer.h"
#include "query_planner.h"
#include <iostream>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing Query Optimizer compilation..." << std::endl;
    
    // Create a QueryOptimizer instance
    QueryOptimizer optimizer;
    if (!optimizer.initialize()) {
        std::cout << "Failed to initialize QueryOptimizer!" << std::endl;
        return 1;
    }
    
    // Create a simple plan
    auto tableScan = std::make_unique<TableScanNode>("users");
    
    // Optimize the plan
    std::string errorMsg;
    auto optimizedPlan = optimizer.optimize(std::move(tableScan), errorMsg);
    
    if (!optimizedPlan) {
        std::cout << "Failed to optimize plan: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "Query Optimizer compilation test passed!" << std::endl;
    return 0;
}