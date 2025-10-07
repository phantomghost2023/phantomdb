# Subquery Support

## Overview

This document describes the implementation of subquery support in Phantom-DB's query processor. The implementation includes extending the AST, parser, query planner, and execution engine to handle subqueries in SELECT statements.

## Implementation Details

### AST Extension

The AST has been extended with a new `Subquery` class that represents subqueries and modifications to the `SelectStatement` class to support subqueries:

```cpp
class Subquery : public ASTNode {
public:
    Subquery(std::unique_ptr<SelectStatement> selectStmt, std::string alias);
    // ... other methods
};

class SelectStatement : public ASTNode {
public:
    // ... existing methods
    const std::vector<std::unique_ptr<Subquery>>& getSubqueries() const;
    void addSubquery(std::unique_ptr<Subquery> subquery);
    // ... other methods
};
```

### Parser Extension

The SQL parser has been enhanced to recognize and parse subqueries with the following syntax:

```sql
SELECT * FROM (SELECT column1, column2 FROM table_name) AS alias;
SELECT column1, column2 FROM (SELECT column1, column2, column3 FROM table_name WHERE condition) AS alias;
```

The parser supports:
- Subqueries in the FROM clause
- Subquery aliases
- Nested SELECT statements within parentheses
- Column selection from subquery results

### Query Planner Extension

The query planner has been extended with a new `SubqueryNode` plan node type:

```cpp
class SubqueryNode : public PlanNode {
public:
    SubqueryNode(std::unique_ptr<PlanNode> subPlan, const std::string& alias);
    // ... other methods
};
```

The planner now recognizes the `PlanNodeType::SUBQUERY` enum value for subquery plan nodes.

### Execution Engine Extension

The execution engine has been extended with a new `ExecSubqueryNode` execution node:

```cpp
class ExecSubqueryNode : public ExecutionNode {
public:
    ExecSubqueryNode(const std::string& alias);
    // ... other methods
};
```

## Files Modified

### New Files
- `src/query/test_subquery_parsing.cpp` - Parser test for subquery statements
- `src/query/test_subquery_planning.cpp` - Plan generation test for subquery statements
- `src/query/test_subquery_execution.cpp` - Execution test for subquery statements
- `src/query/test_subquery_functionality.cpp` - Comprehensive test for subquery functionality
- `docs/SUBQUERY_SUPPORT.md` - This documentation file

### Modified Files
- `src/query/sql_parser.h` - Added Subquery class and modified SelectStatement class
- `src/query/sql_parser.cpp` - Added parsing logic for subqueries
- `src/query/query_planner.h` - Added SubqueryNode class and PlanNodeType::SUBQUERY
- `src/query/query_planner.cpp` - Added plan generation logic for subqueries
- `src/query/execution_engine.h` - Added ExecSubqueryNode class
- `src/query/execution_engine.cpp` - Added execution logic for subqueries
- `tests/CMakeLists.txt` - Added new test executables

## Testing

Comprehensive tests have been created to verify the subquery functionality:

1. **Parser Test** (`test_subquery_parsing.cpp`) - Verifies that subquery statements are correctly parsed into AST nodes
2. **Plan Test** (`test_subquery_planning.cpp`) - Verifies that subquery AST nodes are correctly converted to plan nodes
3. **Execution Test** (`test_subquery_execution.cpp`) - Verifies that subquery plan nodes are correctly executed
4. **Functionality Test** (`test_subquery_functionality.cpp`) - Comprehensive end-to-end test of subquery functionality

## Future Enhancements

Planned improvements for subquery support include:
- Support for correlated subqueries
- Support for subqueries in WHERE clauses
- Support for subqueries in SELECT lists
- Support for EXISTS and IN subqueries
- Integration with the optimizer for subquery unnesting
- Performance optimization for subquery execution

## Usage Example

The following subquery statements are now supported:

```sql
-- Simple subquery
SELECT * FROM (SELECT id, name FROM users) AS subquery;

-- Subquery with column selection
SELECT id, name FROM (SELECT id, name, age FROM users WHERE age > 18) AS adults;

-- Subquery with more complex inner query
SELECT user_id, order_count FROM 
    (SELECT user_id, COUNT(*) as order_count FROM orders GROUP BY user_id) AS user_orders
WHERE order_count > 5;
```