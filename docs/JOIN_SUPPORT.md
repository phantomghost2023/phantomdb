# JOIN Support

This document describes the implementation of JOIN support in PhantomDB.

## Overview

JOIN support allows users to combine rows from two or more tables based on a related column between them. The implementation follows the same pattern as other SQL operations, with components in the parser, query planner, and execution engine.

## Syntax

The basic syntax for JOIN statements is:

```sql
SELECT columns FROM table1 JOIN table2 ON condition;
```

Examples:
```sql
SELECT * FROM users JOIN orders ON users.id = orders.user_id;
SELECT users.name, orders.total FROM users JOIN orders ON users.id = orders.user_id;
SELECT users.name, orders.total, products.name FROM users JOIN orders ON users.id = orders.user_id JOIN products ON orders.product_id = products.id;
```

## Implementation Details

### 1. Abstract Syntax Tree (AST)

The `SelectStatement` class was extended to support JOIN clauses:

```cpp
// Join clause structure
struct JoinClause {
    std::string table;
    std::string condition;
};

// Select statement node
class SelectStatement : public ASTNode {
public:
    SelectStatement(std::vector<std::string> columns, std::string table);
    virtual ~SelectStatement() = default;
    
    std::string toString() const override;
    
    const std::vector<std::string>& getColumns() const;
    const std::string& getTable() const;
    const std::vector<JoinClause>& getJoins() const;
    
    void addJoin(const JoinClause& join);
    
private:
    std::vector<std::string> columns_;
    std::string table_;
    std::vector<JoinClause> joins_;
};
```

### 2. SQL Parser

The parser was extended to recognize JOIN statements:
- Added JOIN and ON token types
- Modified `parseSelectStatement()` method to parse JOIN clauses
- Added logic to handle multiple JOINs in a single SELECT statement

### 3. Query Planner

A new `JoinNode` plan node was added:

```cpp
class JoinNode : public PlanNode {
public:
    JoinNode(std::unique_ptr<PlanNode> left, std::unique_ptr<PlanNode> right, const std::string& condition);
    virtual ~JoinNode() = default;
    
    std::string toString() const override;
    const PlanNode* getLeft() const;
    const PlanNode* getRight() const;
    const std::string& getCondition() const;
    
private:
    std::unique_ptr<PlanNode> left_;
    std::unique_ptr<PlanNode> right_;
    std::string condition_;
};
```

The query planner was extended to generate JOIN plans from SELECT AST nodes with JOIN clauses.

### 4. Execution Engine

A new `ExecJoinNode` execution node was added:

```cpp
class ExecJoinNode : public ExecutionNode {
public:
    ExecJoinNode(const std::string& condition);
    virtual ~ExecJoinNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
    void setLeft(std::unique_ptr<ExecutionNode> left);
    void setRight(std::unique_ptr<ExecutionNode> right);
    
private:
    std::string condition_;
    std::unique_ptr<ExecutionNode> left_;
    std::unique_ptr<ExecutionNode> right_;
};
```

The execution engine was extended to convert JOIN plan nodes to execution nodes and execute them.

## Testing

Comprehensive tests were created for each component:
- Parser tests to verify correct parsing of JOIN statements
- Plan tests to verify correct plan generation
- Execution tests to verify correct execution
- Comprehensive tests for complex JOIN scenarios

## Build Integration

The JOIN support is integrated into the build system:
- Added new source files to CMakeLists.txt
- Created test executables for JOIN functionality

## Future Enhancements

Possible future enhancements for JOIN support:
- Support for different types of JOINs (LEFT, RIGHT, FULL OUTER)
- JOIN optimization algorithms
- Index-based JOIN optimization
- Parallel JOIN execution