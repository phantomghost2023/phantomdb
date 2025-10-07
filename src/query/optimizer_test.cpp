#include "query_optimizer.h"
#include "query_planner.h"
#include "sql_parser.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

void testStatisticsManagerCreation() {
    std::cout << "Testing StatisticsManager creation..." << std::endl;
    
    StatisticsManager manager;
    assert(manager.initialize());
    
    std::cout << "StatisticsManager creation test passed!" << std::endl;
}

void testRuleBasedOptimizerCreation() {
    std::cout << "Testing RuleBasedOptimizer creation..." << std::endl;
    
    RuleBasedOptimizer optimizer;
    assert(optimizer.initialize());
    
    std::cout << "RuleBasedOptimizer creation test passed!" << std::endl;
}

void testCostBasedOptimizerCreation() {
    std::cout << "Testing CostBasedOptimizer creation..." << std::endl;
    
    auto statsManager = std::make_shared<StatisticsManager>();
    statsManager->initialize();
    
    CostBasedOptimizer optimizer(statsManager);
    assert(optimizer.initialize());
    
    std::cout << "CostBasedOptimizer creation test passed!" << std::endl;
}

void testQueryOptimizerCreation() {
    std::cout << "Testing QueryOptimizer creation..." << std::endl;
    
    QueryOptimizer optimizer;
    assert(optimizer.initialize());
    
    std::cout << "QueryOptimizer creation test passed!" << std::endl;
}

void testQueryOptimization() {
    std::cout << "Testing query optimization..." << std::endl;
    
    // Create a simple plan
    auto tableScan = std::make_unique<TableScanNode>("users");
    
    // Create optimizer
    QueryOptimizer optimizer;
    optimizer.initialize();
    
    // Optimize the plan
    std::string errorMsg;
    auto optimizedPlan = optimizer.optimize(std::move(tableScan), errorMsg);
    assert(optimizedPlan != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Query optimization test passed!" << std::endl;
}

int main() {
    std::cout << "Running QueryOptimizer tests..." << std::endl;
    
    testStatisticsManagerCreation();
    testRuleBasedOptimizerCreation();
    testCostBasedOptimizerCreation();
    testQueryOptimizerCreation();
    testQueryOptimization();
    
    std::cout << "All QueryOptimizer tests passed!" << std::endl;
    return 0;
}