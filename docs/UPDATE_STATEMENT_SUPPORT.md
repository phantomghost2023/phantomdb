# UPDATE Statement Support

## Overview

This document describes the implementation of UPDATE statement support in Phantom-DB's query processor. The implementation includes extending the AST, parser, query planner, and execution engine to handle UPDATE operations.

## Implementation Details

### AST Extension

The AST has been extended with a new `UpdateStatement` class that represents UPDATE operations:

```cpp
class UpdateStatement : public ASTNode {
public:
    UpdateStatement(std::string table, std::vector<std::pair<std::string, std::string>> setClauses, std::string whereClause);
    // ... other methods
};
```

### Parser Extension

The SQL parser has been enhanced to recognize and parse UPDATE statements with the following syntax:

```sql
UPDATE table_name SET column1 = value1, column2 = value2, ... WHERE condition;
```

The parser supports:
- UPDATE statements with single or multiple SET clauses
- Optional WHERE clauses
- String literal and numeric values in SET clauses

### Query Planner Extension

The query planner has been extended with a new `UpdateNode` plan node type:

```cpp
class UpdateNode : public PlanNode {
public:
    UpdateNode(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& setClauses, const std::string& whereClause);
    // ... other methods
};
```

### Execution Engine Extension

The execution engine has been extended with a new `ExecUpdateNode` execution node:

```cpp
class ExecUpdateNode : public ExecutionNode {
public:
    ExecUpdateNode(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& setClauses, const std::string& whereClause);
    // ... other methods
};
```

## Files Modified

### New Files
- `src/query/update_test.cpp` - Parser test for UPDATE statements
- `src/query/update_plan_test.cpp` - Plan generation test for UPDATE statements
- `src/query/update_execution_test.cpp` - Execution test for UPDATE statements
- `docs/UPDATE_STATEMENT_SUPPORT.md` - This documentation file

### Modified Files
- `src/query/sql_parser.h` - Added UpdateStatement class and TokenType::UPDATE
- `src/query/sql_parser.cpp` - Added parsing logic for UPDATE statements
- `src/query/query_planner.h` - Added UpdateNode class and PlanNodeType::UPDATE
- `src/query/query_planner.cpp` - Added plan generation logic for UPDATE statements
- `src/query/execution_engine.h` - Added ExecUpdateNode class
- `src/query/execution_engine.cpp` - Added execution logic for UPDATE statements
- `src/query/CMakeLists.txt` - Added new test executables

## Testing

Comprehensive tests have been created to verify the UPDATE statement functionality:

1. **Parser Test** (`update_test.cpp`) - Verifies that UPDATE statements are correctly parsed into AST nodes
2. **Plan Test** (`update_plan_test.cpp`) - Verifies that UPDATE AST nodes are correctly converted to plan nodes
3. **Execution Test** (`update_execution_test.cpp`) - Verifies that UPDATE plan nodes are correctly executed

## Future Enhancements

Planned improvements for UPDATE statement support include:
- Support for more complex WHERE clauses with logical operators
- Support for subqueries in SET clauses
- Integration with the storage engine for actual data updates
- Constraint validation (primary key, foreign key, etc.)
- Type checking and conversion

## Usage Example

The following UPDATE statements are now supported:

```sql
-- UPDATE with single SET clause
UPDATE users SET name = 'John Doe' WHERE id = 1;

-- UPDATE with multiple SET clauses
UPDATE users SET name = 'Jane Doe', age = '30' WHERE id = 2;

-- UPDATE without WHERE clause (updates all rows)
UPDATE users SET active = 'true';
```