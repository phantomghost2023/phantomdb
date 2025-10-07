# INSERT Statement Support

## Overview

This document describes the implementation of INSERT statement support in Phantom-DB's query processor. The implementation includes extending the AST, parser, query planner, and execution engine to handle INSERT operations.

## Implementation Details

### AST Extension

The AST has been extended with a new `InsertStatement` class that represents INSERT operations:

```cpp
class InsertStatement : public ASTNode {
public:
    InsertStatement(std::string table, std::vector<std::string> columns, std::vector<std::vector<std::string>> values);
    // ... other methods
};
```

### Parser Extension

The SQL parser has been enhanced to recognize and parse INSERT statements with the following syntax:

```sql
INSERT INTO table_name (column1, column2, ...) VALUES (value1, value2, ...), (value1, value2, ...), ...;
```

The parser supports:
- INSERT statements with explicit column lists
- INSERT statements without column lists (uses all columns)
- Multiple value rows in a single INSERT statement

### Query Planner Extension

The query planner has been extended with a new `InsertNode` plan node type:

```cpp
class InsertNode : public PlanNode {
public:
    InsertNode(const std::string& tableName, const std::vector<std::string>& columns, const std::vector<std::vector<std::string>>& values);
    // ... other methods
};
```

### Execution Engine Extension

The execution engine has been extended with a new `ExecInsertNode` execution node:

```cpp
class ExecInsertNode : public ExecutionNode {
public:
    ExecInsertNode(const std::string& tableName, const std::vector<std::string>& columns, const std::vector<std::vector<std::string>>& values);
    // ... other methods
};
```

## Files Modified

### New Files
- `src/query/insert_test.cpp` - Parser test for INSERT statements
- `src/query/insert_plan_test.cpp` - Plan generation test for INSERT statements
- `src/query/insert_execution_test.cpp` - Execution test for INSERT statements
- `docs/INSERT_STATEMENT_SUPPORT.md` - This documentation file

### Modified Files
- `src/query/sql_parser.h` - Added InsertStatement class and TokenType::INSERT
- `src/query/sql_parser.cpp` - Added parsing logic for INSERT statements
- `src/query/query_planner.h` - Added InsertNode class and PlanNodeType::INSERT
- `src/query/query_planner.cpp` - Added plan generation logic for INSERT statements
- `src/query/execution_engine.h` - Added ExecInsertNode class
- `src/query/execution_engine.cpp` - Added execution logic for INSERT statements
- `src/query/CMakeLists.txt` - Added new test executables

## Testing

Comprehensive tests have been created to verify the INSERT statement functionality:

1. **Parser Test** (`insert_test.cpp`) - Verifies that INSERT statements are correctly parsed into AST nodes
2. **Plan Test** (`insert_plan_test.cpp`) - Verifies that INSERT AST nodes are correctly converted to plan nodes
3. **Execution Test** (`insert_execution_test.cpp`) - Verifies that INSERT plan nodes are correctly executed

## Future Enhancements

Planned improvements for INSERT statement support include:
- Support for INSERT ... SELECT statements
- Support for INSERT ... ON DUPLICATE KEY UPDATE statements
- Integration with the storage engine for actual data insertion
- Constraint validation (primary key, foreign key, etc.)
- Type checking and conversion

## Usage Example

The following INSERT statements are now supported:

```sql
-- INSERT with explicit columns
INSERT INTO users (id, name, age) VALUES ('1', 'John', '25');

-- INSERT without columns (uses all columns)
INSERT INTO users VALUES ('2', 'Jane', '30');

-- INSERT with multiple rows
INSERT INTO users (id, name, age) VALUES ('3', 'Bob', '35'), ('4', 'Alice', '28');
```