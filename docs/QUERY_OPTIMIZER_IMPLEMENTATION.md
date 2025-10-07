# Query Optimizer Implementation

## Overview

This document describes the implementation of the query optimizer component for Phantom-DB's query processor. The query optimizer improves execution plans through rule-based and cost-based optimization techniques.

## Architecture

The query optimizer consists of the following components:

1. **StatisticsManager**: Manages table and index statistics for cost estimation
2. **RuleBasedOptimizer**: Applies transformation rules to improve query plans
3. **CostBasedOptimizer**: Uses cost models to select the best execution plan
4. **QueryOptimizer**: Coordinates the overall optimization process

## Statistics Manager Implementation

### StatisticsManager Class

The StatisticsManager class manages table and index statistics used for cost estimation.

#### Implementation Files:
- [query_optimizer.h](../src/query/query_optimizer.h)
- [query_optimizer.cpp](../src/query/query_optimizer.cpp)

#### Key Features:
- Table statistics management (row count, average row size)
- Index statistics management (cardinality)
- Dummy statistics for testing purposes
- Thread-safe operations

#### Public Interface:
```cpp
class StatisticsManager {
public:
    StatisticsManager();
    ~StatisticsManager();
    
    // Initialize the statistics manager
    bool initialize();
    
    // Shutdown the statistics manager
    void shutdown();
    
    // Get table statistics
    std::shared_ptr<TableStats> getTableStats(const std::string& tableName);
    
    // Get index statistics
    std::shared_ptr<IndexStats> getIndexStats(const std::string& indexName);
};
```

### TableStats Class

Represents statistics for a table.

#### Key Features:
- Table name
- Row count
- Average row size

### IndexStats Class

Represents statistics for an index.

#### Key Features:
- Index name
- Table name
- Cardinality (number of distinct values)

## Rule-Based Optimizer Implementation

### RuleBasedOptimizer Class

The RuleBasedOptimizer class applies transformation rules to improve query plans.

#### Implementation Files:
- [query_optimizer.h](../src/query/query_optimizer.h)
- [query_optimizer.cpp](../src/query/query_optimizer.cpp)

#### Key Features:
- Predicate pushdown
- Join reordering
- Subquery unnesting
- Expression simplification
- Redundancy elimination
- Thread-safe operations

#### Public Interface:
```cpp
class RuleBasedOptimizer {
public:
    RuleBasedOptimizer();
    ~RuleBasedOptimizer();
    
    // Initialize the rule-based optimizer
    bool initialize();
    
    // Shutdown the rule-based optimizer
    void shutdown();
    
    // Apply optimization rules to a plan
    std::unique_ptr<PlanNode> optimize(std::unique_ptr<PlanNode> plan, std::string& errorMsg);
};
```

## Cost-Based Optimizer Implementation

### CostBasedOptimizer Class

The CostBasedOptimizer class uses cost models to select the best execution plan.

#### Implementation Files:
- [query_optimizer.h](../src/query/query_optimizer.h)
- [query_optimizer.cpp](../src/query/query_optimizer.cpp)

#### Key Features:
- Cost estimation for different plan operations
- Alternative plan generation
- Plan selection based on cost comparison
- Integration with statistics manager
- Thread-safe operations

#### Public Interface:
```cpp
class CostBasedOptimizer {
public:
    CostBasedOptimizer(std::shared_ptr<StatisticsManager> statsManager);
    ~CostBasedOptimizer();
    
    // Initialize the cost-based optimizer
    bool initialize();
    
    // Shutdown the cost-based optimizer
    void shutdown();
    
    // Estimate the cost of a plan
    double estimateCost(const PlanNode* plan);
    
    // Optimize a plan based on cost
    std::unique_ptr<PlanNode> optimize(std::unique_ptr<PlanNode> plan, std::string& errorMsg);
};
```

## Query Optimizer Implementation

### QueryOptimizer Class

The QueryOptimizer class coordinates the overall optimization process.

#### Implementation Files:
- [query_optimizer.h](../src/query/query_optimizer.h)
- [query_optimizer.cpp](../src/query/query_optimizer.cpp)

#### Key Features:
- Integration of rule-based and cost-based optimization
- Sequential application of optimization phases
- Error handling and propagation
- Resource management
- Thread-safe operations

#### Public Interface:
```cpp
class QueryOptimizer {
public:
    QueryOptimizer();
    ~QueryOptimizer();
    
    // Initialize the query optimizer
    bool initialize();
    
    // Shutdown the query optimizer
    void shutdown();
    
    // Optimize a plan using both rule-based and cost-based optimization
    std::unique_ptr<PlanNode> optimize(std::unique_ptr<PlanNode> plan, std::string& errorMsg);
};
```

## Integration with Query Processor

The query processor has been updated to use the new optimizer component:

### Updated Workflow:
1. **Parse**: Convert SQL text to AST using SQLParser
2. **Plan**: Generate execution plan using QueryPlanner
3. **Optimize**: Improve plan using QueryOptimizer
4. **Execute**: Execute the optimized plan

### Implementation Details:
- QueryOptimizer is managed within the QueryProcessor
- Optimization is applied between planning and execution phases
- Error messages are propagated through the processing pipeline

## Testing

Comprehensive tests have been created for the optimizer components:

### Optimizer Tests:
- [optimizer_test.cpp](../src/query/optimizer_test.cpp)

#### Test Coverage:
- Statistics manager creation and initialization
- Rule-based optimizer creation and initialization
- Cost-based optimizer creation and initialization
- Query optimizer creation and initialization
- Basic query optimization

## Build System Integration

The new component is integrated into the CMake build system:

### Updated CMakeLists.txt:
- [src/query/CMakeLists.txt](../src/query/CMakeLists.txt)

#### Changes:
- Added query_optimizer.cpp to QUERY_SOURCES
- Added query_optimizer.h to QUERY_HEADERS
- Added optimizer_test executable

## Design Decisions

### 1. Modular Architecture

The optimizer is implemented as separate components to allow:
- Independent development and testing
- Clear separation of concerns
- Reusability across different optimization scenarios

### 2. Two-Phase Optimization

The optimizer follows a two-phase approach:
- **Rule-Based Optimization**: Apply transformation rules first
- **Cost-Based Optimization**: Use cost models for plan selection

### 3. Statistics Management

Statistics are managed through a dedicated component to:
- Centralize statistics access
- Provide dummy statistics for testing
- Enable future statistics collection mechanisms

### 4. Thread Safety

Thread safety is ensured through:
- Mutex protection for shared data structures
- RAII principles for resource management
- Exception-safe implementation

## Current Limitations

This is a basic implementation with the following limitations:
- Simplified cost estimation
- Limited optimization rules
- Dummy statistics instead of real collection
- No plan caching
- No advanced optimization techniques

## Next Steps

The next phase of query optimizer development will focus on:

### 1. Enhanced Optimization Rules
- Advanced predicate pushdown
- Complex join reordering algorithms
- Subquery optimization
- Aggregation optimization

### 2. Improved Cost Models
- I/O cost estimation
- CPU cost estimation
- Memory cost estimation
- Network cost estimation (for distributed queries)

### 3. Statistics Collection
- Automatic statistics collection
- Histogram-based statistics
- Sampling-based statistics collection
- Statistics maintenance and updating

### 4. Plan Caching
- Optimized plan caching
- Plan invalidation mechanisms
- Cost-based plan selection from cache

## Performance Considerations

The design takes into account several performance considerations:

### 1. Memory Management
- Smart pointer usage for automatic memory management
- Efficient data structures for statistics storage
- Minimal memory copying

### 2. Optimization Performance
- Early pruning of unpromising plans
- Efficient cost estimation
- Caching of intermediate results

### 3. Scalability
- Modular design for easy extension
- Thread-safe operations for concurrent optimization
- Efficient algorithms for large query plans

## Usage Example

```cpp
#include "query_optimizer.h"
#include "query_planner.h"

using namespace phantomdb::query;

int main() {
    // Create and initialize optimizer
    QueryOptimizer optimizer;
    optimizer.initialize();
    
    // Create a plan (typically from QueryPlanner)
    auto plan = std::make_unique<TableScanNode>("users");
    
    // Optimize the plan
    std::string errorMsg;
    auto optimizedPlan = optimizer.optimize(std::move(plan), errorMsg);
    
    if (optimizedPlan) {
        std::cout << "Optimized plan: " << optimizedPlan->toString() << std::endl;
    } else {
        std::cerr << "Optimization failed: " << errorMsg << std::endl;
    }
    
    return 0;
}
```

## Conclusion

This implementation provides the foundation for query optimization in Phantom-DB's query processor. The modular design and clear interfaces make it easy to extend with more sophisticated optimization techniques in future development phases.