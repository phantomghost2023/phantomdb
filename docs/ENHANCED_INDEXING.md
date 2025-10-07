# Enhanced Indexing in PhantomDB

This document describes the enhanced indexing features implemented in PhantomDB, including support for multiple index types with advanced configuration options and performance monitoring.

## Features

### 1. Multiple Index Types
PhantomDB now supports multiple index types optimized for different workloads:

1. **B-tree Indexes**: Balanced tree structure, excellent for range queries and ordered data access
2. **Hash Indexes**: Hash table implementation, optimal for exact match queries
3. **LSM-tree Indexes**: Log-structured merge-tree, designed for write-heavy workloads
4. **Bitmap Indexes**: Efficient for low-cardinality columns (planned)
5. **Full-text Indexes**: Specialized for text search operations (planned)

### 2. Advanced Configuration
Each index can be configured with:

- **Cache Size**: Control memory usage for index caching
- **Compression**: Enable/disable data compression to reduce storage footprint
- **Size Limits**: Configure maximum key and value sizes
- **Duplicate Handling**: Control whether duplicates are allowed

### 3. Performance Monitoring
Comprehensive statistics tracking for:

- Lookup, insert, and delete performance
- Cache hit/miss ratios
- Memory and disk usage
- Key count and distribution

### 4. Bulk Operations
Optimized bulk insert operations for better performance when loading large datasets.

### 5. Auto-Indexing
Automatic index creation based on table schema and usage patterns.

## API Usage

### Basic Index Operations

```cpp
#include "enhanced_index_manager.h"

phantomdb::storage::EnhancedIndexManager indexManager;

// Initialize the index manager
indexManager.initialize();

// Create a B-tree index with custom configuration
phantomdb::storage::IndexConfig config;
config.cacheSize = 2000;
config.useCompression = true;
indexManager.createIndex("users", "id", phantomdb::storage::IndexType::B_TREE, config);

// Create a Hash index
indexManager.createIndex("users", "email", phantomdb::storage::IndexType::HASH);

// Insert data
indexManager.insertIntoIndex("users_id_idx", "1001", "John Doe");

// Search for data
std::string value;
if (indexManager.searchInIndex("users_id_idx", "1001", value)) {
    std::cout << "Found: " << value << std::endl;
}

// Range search (B-tree only)
std::vector<std::pair<std::string, std::string>> results;
indexManager.rangeSearch("users_id_idx", "1001", "1010", results);
```

### Bulk Operations

```cpp
// Bulk insert for better performance
std::vector<std::pair<std::string, std::string>> data = {
    {"1001", "John Doe"},
    {"1002", "Jane Smith"},
    {"1003", "Bob Johnson"}
};
indexManager.bulkInsert("users_id_idx", data);
```

### Configuration Management

```cpp
// Get current index configuration
auto config = indexManager.getIndexConfig("users_id_idx");
std::cout << "Cache size: " << config.cacheSize << std::endl;

// Update index configuration
phantomdb::storage::IndexConfig newConfig = config;
newConfig.cacheSize = 5000;
indexManager.updateIndexConfig("users_id_idx", newConfig);
```

### Performance Monitoring

```cpp
// Get index statistics
auto stats = indexManager.getIndexStats("users_id_idx");
std::cout << "Average lookup time: " << stats.avgLookupTime << " microseconds" << std::endl;
std::cout << "Cache hit rate: " << (double)stats.cacheHits / (stats.cacheHits + stats.cacheMisses) * 100 << "%" << std::endl;

// Analyze index for optimization suggestions
indexManager.analyzeIndex("users_id_idx");
```

### Auto-Indexing

```cpp
// Enable auto-indexing for a table
std::vector<std::string> columns = {"name", "email"};
phantomdb::storage::IndexConfig autoConfig;
autoConfig.cacheSize = 1000;
indexManager.enableAutoIndexing("products", columns, phantomdb::storage::IndexType::HASH, autoConfig);

// Check if auto-indexing is enabled
if (indexManager.isAutoIndexingEnabled("products")) {
    std::cout << "Auto-indexing is enabled for products table" << std::endl;
}
```

## Index Types and Use Cases

### B-tree Indexes
**Best for**: Range queries, ordered data access, balanced read/write workloads
**Characteristics**:
- O(log n) lookup, insert, and delete performance
- Maintains data in sorted order
- Efficient for range scans
- Good cache locality

### Hash Indexes
**Best for**: Exact match queries, read-heavy workloads with random access patterns
**Characteristics**:
- O(1) average case lookup performance
- No ordering guarantees
- Excellent for point queries
- Higher memory overhead than B-trees

### LSM-tree Indexes
**Best for**: Write-heavy workloads, time-series data, logging applications
**Characteristics**:
- Optimized for high write throughput
- Uses sequential I/O for better disk performance
- Periodic compaction for space efficiency
- Higher read latency due to multiple storage levels

## Performance Considerations

1. **Memory vs. Disk Trade-offs**: Indexes can be configured to balance memory usage with performance
2. **Cache Efficiency**: Larger cache sizes generally improve performance but consume more memory
3. **Compression**: Reduces storage footprint but may impact performance
4. **Bulk Operations**: Use bulk insert for loading large datasets rather than individual inserts
5. **Index Selection**: Choose the appropriate index type based on query patterns

## Implementation Details

### EnhancedIndexManager
The core of the enhanced indexing system is the `EnhancedIndexManager` class, which provides:

1. **Index Lifecycle Management**: Creation, configuration, and deletion of indexes
2. **Performance Monitoring**: Statistics collection and analysis
3. **Auto-Indexing**: Automatic index creation based on usage patterns
4. **Bulk Operations**: Optimized operations for large datasets

### Index Statistics
Each index maintains detailed statistics including:
- Operation counts and timing
- Cache performance metrics
- Memory and storage usage
- Key distribution information

### Configuration Management
Indexes can be dynamically reconfigured without rebuilding, allowing for runtime optimization.

## Future Enhancements

Planned improvements include:
- Bitmap index implementation for low-cardinality columns
- Full-text search index for text data
- Distributed index support for clustered deployments
- Advanced query optimization based on index statistics
- Machine learning-based index selection and configuration