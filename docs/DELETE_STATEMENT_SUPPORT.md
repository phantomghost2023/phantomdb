# DELETE Statement Support

This document describes the implementation of DELETE statement support in PhantomDB.

## Overview

The DELETE statement allows users to remove rows from a table based on specified conditions. The implementation follows the same pattern as INSERT and UPDATE statements, with components in the parser, query planner, and execution engine.

## Syntax

The basic syntax for DELETE statements is:

```sql
DELETE FROM table_name WHERE condition;
```

The WHERE clause is optional. If omitted, all rows in the table will be deleted.

Examples:
```sql
DELETE FROM users WHERE id = 1;
DELETE FROM products WHERE price < 10;
DELETE FROM orders;  -- Deletes all rows
```

## Implementation Details

### 1. Abstract Syntax Tree (AST)

A new `DeleteStatement` class was added to represent DELETE operations in the AST:

```cpp
class DeleteStatement : public ASTNode {
public:
    DeleteStatement(std::string table, std::string whereClause);
    virtual ~DeleteStatement() = default;
    
    std::string toString() const override;
    
    const std::string& getTable() const;
    const std::string& getWhereClause() const;
    
private:
    std::string table_;
    std::string whereClause_;
};
```

### 2. SQL Parser

The parser was extended to recognize DELETE statements:
- Added DELETE token type recognition
- Implemented `parseDeleteStatement()` method to parse DELETE syntax
- Added logic to handle the FROM keyword and optional WHERE clause

### 3. Query Planner

A new `DeleteNode` plan node was added:

```cpp
class DeleteNode : public PlanNode {
public:
    DeleteNode(const std::string& tableName, const std::string& whereClause);
    virtual ~DeleteNode() = default;
    
    std::string toString() const override;
    const std::string& getTableName() const;
    const std::string& getWhereClause() const;
    
private:
    std::string tableName_;
    std::string whereClause_;
};
```

The query planner was extended to generate DELETE plans from DELETE AST nodes.

### 4. Execution Engine

A new `ExecDeleteNode` execution node was added:

```cpp
class ExecDeleteNode : public ExecutionNode {
public:
    ExecDeleteNode(const std::string& tableName, const std::string& whereClause);
    virtual ~ExecDeleteNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
private:
    std::string tableName_;
    std::string whereClause_;
};
```

The execution engine was extended to convert DELETE plan nodes to execution nodes and execute them.

## Testing

Comprehensive tests were created for each component:
- Parser tests to verify correct parsing of DELETE statements
- Plan tests to verify correct plan generation
- Execution tests to verify correct execution

## Build Integration

The DELETE statement support is integrated into the build system:
- Added new source files to CMakeLists.txt
- Created test executables for DELETE functionality
- Added build script for testing DELETE support

## Future Enhancements

Possible future enhancements for DELETE statement support:
- More sophisticated WHERE clause parsing
- Support for JOINs in DELETE statements
- Batch deletion optimizations
- Foreign key constraint handling