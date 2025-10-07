# PhantomDB

A modern, distributed database system built with C++17, featuring clustering, consensus protocols, replication, and distributed transactions. Enhanced with enterprise-grade features including persistent storage, advanced indexing, query optimization, ACID semantics, security, and developer tooling.

## Features

### Core Database Features
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

### Enterprise Features
- **Persistent Storage**: File-backed tables with snapshot/append-only files
- **Advanced Indexing**: B-tree, Hash, and LSM-tree indexes with automatic configuration
- **Query Optimization**: Rule-based and cost-based query optimizer with index awareness
- **Full ACID Semantics**: MVCC implementation with snapshot isolation
- **Security**: Role-Based Access Control (RBAC) and comprehensive audit logging
- **Developer Tooling**: Interactive CLI/REPL, import/export utilities, and plugin interface
- **Monitoring**: Prometheus/Grafana integration with pre-built dashboards
- **Performance**: Benchmark suite and code coverage reporting

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

## CLI Tools

PhantomDB provides two command-line interfaces for database administration and interaction:

### Traditional CLI
```bash
# Check server health
phantomdb-cli health

# List databases
phantomdb-cli list-databases

# Create a database
phantomdb-cli create-database myapp

# Execute a custom SQL query
phantomdb-cli execute-query myapp "SELECT * FROM users"

# Import data from a file
phantomdb-cli import myapp users data.csv --format csv

# Export data to a file
phantomdb-cli export myapp users data.json --format json
```

### Interactive REPL
```bash
# Launch the interactive REPL
phantomdb-repl

# Within the REPL:
phantomdb> use myapp
phantomdb [myapp]> SELECT * FROM users;
phantomdb [myapp]> INSERT INTO users (id, name, email) VALUES (1, 'John Doe', 'john@example.com');
phantomdb [myapp]> help
```

The interactive REPL features:
- Command auto-completion
- SQL keyword auto-completion
- Persistent command history
- Database context management
- Real-time query execution

## Plugin Architecture

PhantomDB features a flexible plugin architecture that allows extending functionality without modifying the core codebase:

### Plugin Types
1. **Function Extension Plugins** - Add custom SQL functions
2. **Storage Backend Plugins** - Implement alternative storage engines
3. **Query Processor Plugins** - Provide custom query optimization
4. **Authentication Provider Plugins** - Implement custom authentication
5. **Custom Data Type Plugins** - Add support for new data types

### Creating a Plugin
```cpp
#include "base_plugin.h"

class MyCustomPlugin : public BasePlugin {
public:
    MyCustomPlugin() 
        : BasePlugin("MyCustomPlugin", "1.0.0", "My custom plugin", PluginType::FUNCTION_EXTENSION) {
    }
    
    bool initialize() override {
        // Custom initialization
        return BasePlugin::initialize();
    }
    
    void* getInterface(const std::string& interfaceName) override {
        if (interfaceName == "MyCustomInterface") {
            // Return interface pointer
            return &myCustomInterface_;
        }
        return BasePlugin::getInterface(interfaceName);
    }
    
private:
    MyCustomInterface myCustomInterface_;
};
```

### Loading Plugins
```cpp
#include "plugin_manager.h"

using namespace phantomdb::plugin;

// Get the plugin manager instance
PluginManager& manager = PluginManager::getInstance();

// Load a plugin from a shared library
manager.loadPlugin("./plugins/my_custom_plugin.so");

// Get a specific plugin
Plugin* plugin = manager.getPlugin("MyCustomPlugin");
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

## Security Features

PhantomDB implements enterprise-grade security features to protect data and ensure compliance:

### Role-Based Access Control (RBAC)
```cpp
#include "rbac.h"

using namespace phantomdb::security;

// Create RBAC manager
RBACManager rbac;

// Initialize the RBAC system
rbac.initialize();

// Create a new user
rbac.createUser("john_doe", "secure_password");

// Assign a role to a user
rbac.assignRole("john_doe", UserRole::WRITER);

// Check if a user has a specific permission
if (rbac.hasPermission("john_doe", Permission::INSERT)) {
    // User can insert data
}
```

### Audit Logging
```cpp
#include "audit_logger.h"

using namespace phantomdb::audit;

// Create audit logger
AuditLogger logger;

// Initialize the audit logger
logger.initialize("/var/log/phantomdb/audit.log");

// Log a user login
logger.logUserLogin("john_doe", "192.168.1.100");

// Log a database creation
logger.logDatabaseCreate("john_doe", "myapp");

// Log a data insert operation
logger.logDataInsert("john_doe", "myapp", "users", "user_123");
```

### Security Configuration
```json
{
    "security": {
        "rbac": {
            "enabled": true,
            "default_role": "READER",
            "roles": {
                "ADMIN": {
                    "permissions": ["*"]
                },
                "WRITER": {
                    "permissions": ["SELECT", "INSERT", "UPDATE", "DELETE", "EXECUTE_QUERY"]
                },
                "READER": {
                    "permissions": ["SELECT", "EXECUTE_QUERY"]
                }
            }
        },
        "audit": {
            "enabled": true,
            "log_file": "/var/log/phantomdb/audit.log",
            "max_file_size": "100MB",
            "max_files": 10
        }
    }
}
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