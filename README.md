# PhantomDB

A modern, distributed database system built with C++17, featuring clustering, consensus protocols, replication, and distributed transactions.

## Features

- **Distributed Architecture**: Multi-node clustering with automatic discovery
- **Consensus Protocol**: Raft-based consensus for data consistency
- **Data Replication**: Automatic data replication across cluster nodes
- **Distributed Transactions**: Support for ACID transactions across nodes
- **Horizontal Scaling**: Elastic scaling and load balancing
- **Sharding**: Cross-shard query processing
- **Storage Engine**: B+Tree, LSM-Tree, and Hash Table implementations
- **Query Processing**: SQL parser, query planner, and execution engine
- **Transaction Management**: MVCC, isolation levels, and locking
- **Schema Enforcement**: Column type validation and schema compliance
- **Condition Processing**: Advanced WHERE clause parsing and evaluation
- **Observability**: Metrics, tracing, and monitoring

## Architecture

PhantomDB follows a modular architecture with the following components:

1. **Core Module**: Basic database functionality and components
2. **Storage Module**: Storage engines (B+Tree, LSM-Tree, Hash Table) with WAL
3. **Query Module**: SQL parser, query planner, optimizer, and execution engine
4. **Transaction Module**: Transaction manager with MVCC and isolation control
5. **Distributed Module**: Clustering, consensus, replication, and distributed transactions
6. **API Module**: REST API and client interfaces
7. **Observability Module**: Metrics, tracing, and monitoring

## Prerequisites

- C++17 compatible compiler (Visual Studio 2022, GCC 7+, Clang 5+)
- CMake 3.15+
- [Optional] Conan package manager for dependency management

## Building PhantomDB

### Windows (Visual Studio)

```bash
# Configure the project
cmake -G "Visual Studio 17 2022" -A x64 .

# Build the project
cmake --build . --config Release
```

### Linux/macOS

```bash
# Configure the project
cmake -DCMAKE_BUILD_TYPE=Release .

# Build the project
make -j$(nproc)
```

## Running PhantomDB

After building, you can run the demo application:

```bash
# Windows
.\Release\phantomdb.exe

# Linux/macOS
./phantomdb
```

Or use the provided batch script:

```bash
# Windows
run_phantomdb.bat
```

## Enhanced Features

### Schema Enforcement

PhantomDB now enforces schema compliance for all data operations:

```cpp
// Define table schema
std::vector<std::pair<std::string, std::string>> columns = {
    {"id", "integer"},
    {"name", "string"},
    {"email", "string"},
    {"age", "integer"}
};

// Create table with schema
db.createTable("testdb", "users", columns);

// Valid data insertion
std::unordered_map<std::string, std::string> validData = {
    {"id", "1"},
    {"name", "John Doe"},
    {"email", "john@example.com"},
    {"age", "30"}
};
db.insertData("testdb", "users", validData); // Success

// Invalid data (wrong type)
std::unordered_map<std::string, std::string> invalidData = {
    {"id", "not_a_number"}, // Should be integer
    {"name", "John Doe"},
    {"email", "john@example.com"},
    {"age", "30"}
};
db.insertData("testdb", "users", invalidData); // Rejected

// Unknown field
std::unordered_map<std::string, std::string> unknownFieldData = {
    {"id", "1"},
    {"name", "John Doe"},
    {"email", "john@example.com"},
    {"age", "30"},
    {"unknown_field", "value"} // Not in schema
};
db.insertData("testdb", "users", unknownFieldData); // Rejected
```

### Condition Processing

PhantomDB now supports advanced condition processing in SELECT, UPDATE, and DELETE operations:

```cpp
// Simple condition
auto results = db.selectData("testdb", "users", "id = '1'");

// Complex condition
auto complexResults = db.selectData("testdb", "users", "age = '30' AND name = 'John Doe'");

// Update with condition
std::unordered_map<std::string, std::string> updateData = {
    {"age", "31"},
    {"email", "john.updated@example.com"}
};
db.updateData("testdb", "users", updateData, "id = '1'");

// Delete with condition
db.deleteData("testdb", "users", "id = '2'");
```

## Project Structure

```
PhantomDB/
├── src/
│   ├── core/          # Core database components
│   ├── storage/       # Storage engines and WAL
│   ├── query/         # SQL parser and query execution
│   ├── transaction/   # Transaction management
│   ├── distributed/   # Distributed components
│   ├── api/           # REST API and clients
│   └── observability/ # Metrics and monitoring
├── tests/             # Unit and integration tests
├── docs/              # Documentation
└── CMakeLists.txt     # Build configuration
```

## Key Components

### Core Module
- Database management with schema enforcement
- Storage engine with WAL for durability
- Garbage collection for memory management
- Basic database operations with validation

### Storage Engine
- B+Tree implementation for indexed storage
- LSM-Tree for write-optimized workloads
- Hash Table for key-value storage
- Write-Ahead Logging (WAL) for durability
- Garbage collection for memory management

### Query Processor
- SQL parser supporting DDL and DML operations
- Query planner with cost-based optimization
- Execution engine for query processing
- Support for SELECT, INSERT, UPDATE, DELETE operations

### Transaction Manager
- Multi-Version Concurrency Control (MVCC)
- Isolation levels (Read Uncommitted, Read Committed, Repeatable Read, Serializable)
- Lock manager for conflict resolution
- Distributed transaction support with Two-Phase Commit

### Distributed Components
- Cluster management with automatic node discovery
- Raft consensus protocol implementation
- Data replication across nodes
- Elastic scaling with automatic rebalancing
- Load balancing mechanisms
- Cross-shard query processing
- Distributed transaction support with Two-Phase Commit
- Saga pattern for distributed transactions

### API and Observability
- REST API implementation
- Metrics collection and monitoring
- Tracing capabilities
- Health checks and status reporting

## Testing

The project includes comprehensive tests for all components:

```bash
# Run specific tests
./build/src/core/test_enhanced_database
./build/src/storage/btree_test
./build/src/transaction/simple_transaction_test
./build/src/distributed/performance_test
```

## Documentation

Detailed documentation can be found in the [docs](docs/) directory, including:

- [Architecture Design](docs/architecture.md)
- [API Reference](docs/api.md)
- [Deployment Guide](docs/deployment.md)
- [Performance Tuning](docs/performance.md)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by modern distributed database systems
- Built with performance and scalability in mind
- Designed for cloud-native deployment