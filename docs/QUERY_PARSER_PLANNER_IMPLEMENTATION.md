# Query Parser and Planner Implementation

## Overview

This document describes the implementation of the query parser and planner components for Phantom-DB. These components are responsible for converting SQL text into an Abstract Syntax Tree (AST) and then generating execution plans from that AST.

## Architecture

The query processing pipeline consists of the following components:

1. **SQLParser**: Converts SQL text into an Abstract Syntax Tree (AST)
2. **QueryPlanner**: Generates execution plans from the AST
3. **QueryProcessor**: Coordinates the overall query processing workflow

## Query Parser Implementation

### SQLParser Class

The SQLParser class is responsible for lexical analysis and syntactic analysis of SQL queries.

#### Implementation Files:
- [sql_parser.h](../src/query/sql_parser.h)
- [sql_parser.cpp](../src/query/sql_parser.cpp)

#### Key Features:
- Tokenization of SQL text
- Lexical analysis with token recognition
- Syntactic analysis for SELECT statements
- AST generation for parsed queries
- Error handling and reporting

#### Supported SQL Constructs:
- SELECT statements with column lists or *
- FROM clauses with single table references
- Basic tokenization of identifiers, keywords, and literals

#### Public Interface:
```cpp
class SQLParser {
public:
    SQLParser();
    ~SQLParser();
    
    // Parse a SQL query and return the AST
    std::unique_ptr<ASTNode> parse(const std::string& sql, std::string& errorMsg);
};
```

### Tokenization

The parser implements a simple tokenizer that recognizes the following token types:
- Keywords (SELECT, FROM, WHERE, etc.)
- Identifiers (table names, column names)
- Literals (string literals, numeric literals)
- Symbols (operators, punctuation)

### AST Generation

The parser generates an Abstract Syntax Tree with the following node types:
- **ASTNode**: Base class for all AST nodes
- **SelectStatement**: Represents a SELECT statement with columns and table information

### Error Handling

The parser provides detailed error reporting with:
- Error messages describing the nature of parse failures
- Line and column information for error location
- Exception-safe implementation

## Query Planner Implementation

### QueryPlanner Class

The QueryPlanner class generates execution plans from AST nodes.

#### Implementation Files:
- [query_planner.h](../src/query/query_planner.h)
- [query_planner.cpp](../src/query/query_planner.cpp)

#### Key Features:
- Plan node generation from AST nodes
- Cost estimation for plan nodes
- Simple plan optimization strategies
- Plan serialization for debugging

#### Supported Plan Nodes:
- **TableScanNode**: Scans an entire table
- **PlanNode**: Base class for all plan nodes (extensible for future nodes)

#### Public Interface:
```cpp
class QueryPlanner {
public:
    QueryPlanner();
    ~QueryPlanner();
    
    // Generate an execution plan from an AST
    std::unique_ptr<PlanNode> generatePlan(const ASTNode* ast, std::string& errorMsg);
};
```

### Plan Generation

The planner currently generates simple table scan plans for SELECT statements. In a more advanced implementation, it would consider:
- Available indexes for optimized access
- Table statistics for cost-based optimization
- Join conditions for multi-table queries
- Filter conditions for selective scanning
- Projection requirements for column selection

### Cost Estimation

Plan nodes include cost estimation based on:
- Table size estimates
- Index availability
- Selectivity of filter conditions
- Join cardinality estimates

## Integration with Query Processor

The query processor has been updated to use the new parser and planner components:

### Updated Workflow:
1. **Parse**: Convert SQL text to AST using SQLParser
2. **Plan**: Generate execution plan from AST using QueryPlanner
3. **Execute**: Execute the plan (placeholder implementation)

### Implementation Details:
- Parser and planner instances are managed within the QueryProcessor
- AST nodes are cached between parse and plan phases
- Error messages are propagated through the processing pipeline

## Testing

Comprehensive tests have been created for both components:

### Parser Tests:
- [parser_test.cpp](../src/query/parser_test.cpp)

#### Test Coverage:
- Basic SELECT statement parsing
- SELECT with specific columns
- Whitespace handling
- Error condition handling

### Planner Tests:
- [planner_test.cpp](../src/query/planner_test.cpp)

#### Test Coverage:
- Basic plan generation
- Plan generation with specific columns
- Error condition handling

## Build System Integration

The new components are integrated into the CMake build system:

### Updated CMakeLists.txt:
- [src/query/CMakeLists.txt](../src/query/CMakeLists.txt)

#### Changes:
- Added sql_parser.cpp and query_planner.cpp to QUERY_SOURCES
- Added sql_parser.h and query_planner.h to QUERY_HEADERS
- Added parser_test and planner_test executables

## Design Decisions

### 1. Modular Architecture

The parser and planner are implemented as separate components to allow:
- Independent development and testing
- Reusability across different query processing paths
- Clear separation of concerns

### 2. AST Design

The AST design follows these principles:
- Extensible node hierarchy
- Type-safe node casting
- Memory-safe smart pointer usage
- Clear serialization interface

### 3. Plan Node Design

The plan node design follows these principles:
- Extensible node hierarchy for future operators
- Cost-based optimization support
- Clear string representation for debugging
- Type-safe node identification

### 4. Error Handling

Error handling follows these principles:
- Detailed error messages with context
- Exception-safe implementation
- Graceful degradation on parse errors
- Propagation of errors through the processing pipeline

## Current Limitations

This is a basic implementation with the following limitations:
- Only SELECT statements are supported
- No support for JOINs, subqueries, or complex expressions
- No actual query execution engine
- Simplified cost estimation
- No optimization beyond basic plan generation

## Next Steps

The next phase of query processor development will focus on:

### 1. Extended Parser Support
- INSERT, UPDATE, DELETE statements
- JOIN syntax support
- Subquery support
- Complex expression parsing
- DDL statement support

### 2. Advanced Planner Features
- Index-aware planning
- Join ordering optimization
- Cost-based optimization
- Plan caching

### 3. Query Optimizer Implementation
- Rule-based optimization
- Statistics collection
- Selectivity estimation
- Plan space exploration

### 4. Execution Engine Implementation
- Operator framework
- Vectorized execution
- Memory management
- Parallel execution support

## Performance Considerations

The design takes into account several performance considerations:

### 1. Memory Management
- Smart pointer usage for automatic memory management
- Object reuse where possible
- Efficient string handling

### 2. Parsing Performance
- Single-pass tokenization
- Minimal string copying
- Efficient data structures

### 3. Planning Performance
- Simple cost model for fast plan generation
- Caching of parsed ASTs
- Lightweight plan nodes

## Usage Example

```cpp
#include "query_processor.h"
#include "sql_parser.h"
#include "query_planner.h"

using namespace phantomdb::query;

int main() {
    // Parse a SQL query
    SQLParser parser;
    std::string errorMsg;
    auto ast = parser.parse("SELECT id, name FROM users", errorMsg);
    
    if (ast) {
        // Generate an execution plan
        QueryPlanner planner;
        auto plan = planner.generatePlan(ast.get(), errorMsg);
        
        if (plan) {
            std::cout << "Generated plan: " << plan->toString() << std::endl;
        } else {
            std::cerr << "Planning failed: " << errorMsg << std::endl;
        }
    } else {
        std::cerr << "Parsing failed: " << errorMsg << std::endl;
    }
    
    return 0;
}
```

## Conclusion

This implementation provides the foundation for the query parsing and planning components of Phantom-DB. The modular design and clear interfaces make it easy to extend with more sophisticated parsing, planning, and optimization capabilities in future development phases.