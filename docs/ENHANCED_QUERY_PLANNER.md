# Enhanced Query Planner in PhantomDB

This document describes the enhanced query planner features implemented in PhantomDB, including rule-based optimizations for filter reordering and index leveraging.

## Features

### 1. Rule-Based Optimizations
The enhanced query planner implements several rule-based optimizations:

- **Predicate Pushdown**: Moves filter conditions as close to the data source as possible
- **Join Reordering**: Reorders JOIN operations to minimize intermediate result sizes
- **Subquery Unnesting**: Converts subqueries to JOINs when beneficial
- **Expression Simplification**: Simplifies complex expressions
- **Redundancy Elimination**: Removes duplicate operations

### 2. Index-Aware Planning
The query planner can leverage index information to:

- Select the most appropriate index for filter conditions
- Reorder filter conditions to use the most selective indexes first
- Transform table scans into index scans when beneficial
- Estimate costs more accurately using index statistics

### 3. Enhanced Cost Estimation
Improved cost estimation based on:

- Table statistics (row counts, average row sizes)
- Column statistics (cardinalities, selectivities)
- Index statistics (types, performance characteristics)
- Historical query performance data

### 4. Statistics Management
Comprehensive statistics management for accurate cost estimation:

- Table-level statistics
- Column-level statistics
- Index-level statistics
- Automatic statistics collection and updates

## API Usage

### Basic Query Planning

```cpp
#include "enhanced_query_planner.h"
#include "sql_parser.h"

// Create components
phantomdb::query::EnhancedStatisticsManager statsManager;
phantomdb::query::EnhancedQueryPlanner queryPlanner;

// Initialize components
statsManager.initialize();
queryPlanner.initialize();

// Set up dependencies
queryPlanner.setStatisticsManager(&statsManager);

// Parse a SQL query
phantomdb::query::SQLParser parser;
std::string errorMsg;
auto ast = parser.parse("SELECT * FROM users WHERE id = 123;", errorMsg);

// Generate optimized plan
auto plan = queryPlanner.generateOptimizedPlan(ast.get(), errorMsg);
std::cout << "Generated plan: " << plan->toString() << std::endl;
std::cout << "Plan cost: " << plan->getCost() << std::endl;

// Shutdown components
queryPlanner.shutdown();
statsManager.shutdown();
```

### Statistics Management

```cpp
// Update table statistics
statsManager.updateTableStats("users", 10000, 100);

// Update column statistics
statsManager.updateColumnStats("users", "id", 10000, 0.0001);
statsManager.updateColumnStats("users", "email", 10000, 0.0001);

// Update index statistics
statsManager.updateIndexStats("users_id_idx", "users", "id", 
                             phantomdb::storage::IndexType::B_TREE,
                             10000, 0.0001, 0.1);

// Estimate selectivity of a condition
double selectivity = statsManager.estimateSelectivity("users", "id = 123");
```

### Index-Aware Planning

```cpp
// Set up index manager for index-aware planning
phantomdb::storage::EnhancedIndexManager indexManager;
queryPlanner.setIndexManager(&indexManager);

// The planner will now consider available indexes when generating plans
auto plan = queryPlanner.generateOptimizedPlan(ast.get(), errorMsg);
```

## Optimization Techniques

### Predicate Pushdown
Filters are moved as close to the data source as possible to reduce the amount of data processed at higher levels:

```sql
-- Original query
SELECT * FROM users u JOIN orders o ON u.id = o.user_id WHERE u.id = 123;

-- Optimized plan applies filter before join
-- TableScan(users) with filter (id = 123) -> Join -> Result
```

### Join Reordering
JOIN operations are reordered based on table sizes and filter selectivities to minimize intermediate result sizes:

```sql
-- Original query
SELECT * FROM large_table lt 
JOIN medium_table mt ON lt.id = mt.large_table_id
JOIN small_table st ON mt.id = st.medium_table_id
WHERE st.id = 123;

-- Optimized plan reorders joins
-- TableScan(small_table) with filter -> Join -> Join -> Result
```

### Index Selection
The planner selects the most appropriate index based on:

1. **Selectivity**: More selective indexes are preferred
2. **Performance**: Faster indexes are preferred
3. **Coverage**: Indexes that cover more query columns are preferred

```sql
-- Query with multiple filter conditions
SELECT * FROM users WHERE id = 123 AND email = 'user@example.com';

-- If both id and email have indexes, the planner chooses based on:
-- 1. Which index is more selective for this query
-- 2. Which index has better performance characteristics
-- 3. Which index can satisfy more conditions
```

## Cost Estimation Model

The enhanced query planner uses a sophisticated cost estimation model that considers:

### I/O Costs
- Disk reads and writes
- Sequential vs. random access patterns
- Buffer pool effects

### CPU Costs
- Processing time for operations
- Expression evaluation costs
- Sorting and aggregation costs

### Memory Costs
- Buffer usage
- Intermediate result sizes
- Working set sizes

### Network Costs (for distributed queries)
- Data transfer costs
- Latency considerations
- Bandwidth limitations

## Implementation Details

### EnhancedQueryPlanner
The core of the enhanced query planning system is the `EnhancedQueryPlanner` class, which provides:

1. **Plan Generation**: Creates initial query plans from AST
2. **Rule-Based Optimization**: Applies transformation rules
3. **Cost-Based Optimization**: Selects the lowest-cost plan
4. **Index Awareness**: Leverages index information for better plans

### EnhancedStatisticsManager
Manages statistics for accurate cost estimation:

1. **Table Statistics**: Row counts, average row sizes
2. **Column Statistics**: Cardinalities, selectivities
3. **Index Statistics**: Types, performance characteristics
4. **Selectivity Estimation**: Estimates filter effectiveness

### Optimization Pipeline
The query planner applies optimizations in a specific order:

1. **Parse SQL**: Convert SQL to AST
2. **Generate Basic Plan**: Create initial plan from AST
3. **Apply Rule-Based Optimizations**: Transform plan using rules
4. **Apply Cost-Based Optimizations**: Select best plan based on costs
5. **Return Optimized Plan**: Provide final execution plan

## Performance Considerations

1. **Statistics Freshness**: Regular statistics updates for accurate cost estimation
2. **Index Maintenance**: Proper index maintenance for optimal performance
3. **Query Complexity**: Complex queries may require more planning time
4. **Memory Usage**: Statistics and plan generation require memory resources

## Future Enhancements

Planned improvements include:
- Machine learning-based cost estimation
- Advanced join algorithms (hash joins, sort-merge joins)
- Materialized view support
- Query plan caching
- Distributed query optimization
- Adaptive query processing based on runtime feedback