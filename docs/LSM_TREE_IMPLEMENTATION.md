# LSM-Tree Implementation in PhantomDB

## Overview

The LSM-tree (Log-Structured Merge-Tree) is a data structure designed to optimize write-heavy workloads. It's particularly effective in database systems where writes significantly outnumber reads.

## Design Principles

Our LSM-tree implementation follows these key principles:

1. **Write Optimization**: Minimize disk I/O for write operations by batching updates in memory
2. **Sequential Writes**: Convert random writes to sequential writes for better performance
3. **Background Compaction**: Merge sorted runs in the background to maintain read performance
4. **Tiered Storage**: Organize data in levels to balance write amplification and read performance

## Components

### MemTable
- In-memory component that stores recent writes
- Implemented as a sorted map for fast lookups
- Flushed to disk when it reaches a size threshold

### SSTable (Sorted String Table)
- Immutable, sorted files stored on disk
- Organized in levels for efficient merging
- Support for compression and bloom filters (future enhancement)

### Compaction Process
- Merges SSTables in the background
- Reduces space amplification by removing duplicates
- Maintains sorted order across levels

## Implementation Details

### Data Structure

```cpp
template<typename Key, typename Value>
class LSMTREE {
public:
    explicit LSMTREE(int memtableSize = DEFAULT_MEMTABLE_SIZE);
    ~LSMTREE();
    
    void insert(const Key& key, const Value& value);
    bool search(const Key& key, Value& value) const;
    bool remove(const Key& key);
    int getSize() const;
    int getCount() const;
    
private:
    struct SSTable {
        std::map<Key, Value> data;
        int level;
    };
    
    mutable std::mutex mtx_;
    std::map<Key, Value> memtable_;  // In-memory table
    std::deque<std::shared_ptr<SSTable>> sstables_;  // Sorted String Tables on disk
    int memtableSize_;
    int count_;
    mutable int size_;
    
    void flushMemtable();
    std::shared_ptr<SSTable> mergeSSTables(const std::shared_ptr<SSTable>& sst1, 
                                           const std::shared_ptr<SSTable>& sst2) const;
    void compact();
};
```

### Key Features

1. **Thread Safety**: All operations are protected by mutexes
2. **Template-Based**: Generic implementation supporting different key/value types
3. **Automatic Flushing**: MemTable automatically flushes when reaching capacity
4. **Level-Based Compaction**: Simple tiered compaction strategy

## Performance Characteristics

| Operation | Complexity | Notes |
|-----------|------------|-------|
| Insert | O(log N) | Where N is the size of the memtable |
| Search | O(log N + M*log K) | Where M is the number of SSTables and K is the average size |
| Delete | O(log N + M*log K) | Marked with tombstones in this implementation |

## Integration with Index Manager

The LSM-tree implementation integrates seamlessly with the Index Manager:

1. **Index Creation**: Supports LSM_TREE index type in createIndex()
2. **Data Operations**: Fully supports insert and search operations
3. **Resource Management**: Automatic cleanup on index drop

## Future Enhancements

1. **Persistent Storage**: Currently SSTables are kept in memory; future versions will persist to disk
2. **Bloom Filters**: Add probabilistic data structures for faster negative lookups
3. **Advanced Compaction**: Implement more sophisticated compaction strategies (Leveled, Size-Tiered)
4. **Compression**: Add data compression for SSTables
5. **Concurrent Access**: Improve concurrent read/write performance

## Usage Example

```cpp
#include "lsm_tree.h"

// Create an LSM-tree index
LSMTREE<int, std::string> lsmTree(1000); // 1000 item capacity

// Insert data
lsmTree.insert(1, "value1");
lsmTree.insert(2, "value2");

// Search for data
std::string value;
if (lsmTree.search(1, value)) {
    std::cout << "Found: " << value << std::endl;
}
```

## Testing

Comprehensive tests are provided in `lsm_tree_test.cpp` and `lsm_tree_index_test.cpp` covering:
- Basic operations (insert, search, delete)
- MemTable flushing
- Update operations
- Size and count tracking
- Integration with the Index Manager