#include "execution_engine.h"
#include "../transaction/transaction_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;
using namespace phantomdb::transaction;

int main() {
    std::cout << "Testing Execution Engine..." << std::endl;
    
    // Create execution engine
    ExecutionEngine engine;
    assert(engine.initialize());
    
    // Create a dummy transaction
    auto transaction = std::make_shared<Transaction>(1, IsolationLevel::READ_COMMITTED);
    
    // Create execution nodes
    auto tableScan = std::make_unique<ExecTableScanNode>("users");
    auto filter = std::make_unique<ExecFilterNode>("age > 25");
    auto project = std::make_unique<ExecProjectNode>(std::vector<std::string>{"id", "name"});
    
    // Build execution tree: Project -> Filter -> TableScan
    filter->addChild(std::move(tableScan));
    project->addChild(std::move(filter));
    
    // Create a simple execution context
    ExecutionContext context(transaction);
    
    // Execute the tree
    bool result = project->execute(context);
    assert(result);
    
    // Check results
    const auto& results = context.getResult();
    assert(!results.empty());
    
    std::cout << "Execution Engine test passed!" << std::endl;
    
    // Shutdown execution engine
    engine.shutdown();
    
    return 0;
}