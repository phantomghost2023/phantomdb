# Storage Engine Technical Specification

This document provides the technical specification for the Phantom-DB storage engine, which implements a hybrid approach combining LSM-tree and B+tree structures.

## Overview

The storage engine is the core component responsible for data persistence, retrieval, and management. It implements a hybrid approach that combines the write optimization of LSM-trees with the read optimization of B+trees, providing optimal performance across different workload patterns.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Storage Engine API                       │
├─────────────────────────────────────────────────────────────┤
│          Write-Ahead Log (WAL) Manager                      │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐    ┌─────────────────┐                │
│  │   LSM Store     │    │   B+Tree Store  │                │
│  │                 │    │                 │                │
│  │ ┌─────────────┐ │    │ ┌─────────────┐ │                │
│  │ │   MemTable  │ │    │ │    Root     │ │                │
│  │ ├─────────────┤ │    │ ├─────────────┤ │                │
│  │ │   SSTables  │ │    │ │   Internal  │ │                │
│  │ │             │ │    │ │   Nodes     │ │                │
│  │ │   Levels    │ │    │ ├─────────────┤ │                │
│  │ │             │ │    │ │   Leaf      │ │                │
│  │ │             │ │    │ │   Nodes     │ │                │
│  │ └─────────────┘ │    │ └─────────────┘ │                │
│  └─────────────────┘    └─────────────────┘                │
├─────────────────────────────────────────────────────────────┤
│              Index Management System                        │
├─────────────────────────────────────────────────────────────┤
│              Garbage Collection Manager                     │
├─────────────────────────────────────────────────────────────┤
│              Backup/Restore Manager                         │
└─────────────────────────────────────────────────────────────┘
```

## Components

### 1. Write-Ahead Log (WAL) Manager

#### Purpose
The WAL manager ensures data durability and enables crash recovery by logging all changes before they are applied to the main storage structures.

#### Key Features
- **Log-Structured Storage**: Efficient sequential writes
- **Log Entry Format**: Compact binary format with compression
- **Log Rotation**: Automatic rotation based on size or time
- **Crash Recovery**: Fast recovery using WAL during startup
- **Sync Options**: Configurable fsync policies for performance/durability trade-off

#### Implementation Details
- **Entry Format**: 
  ```
  [Timestamp][Transaction ID][Operation Type][Key Length][Key][Value Length][Value]
  ```
- **Compression**: Snappy compression for log entries
- **Batching**: Group multiple operations for efficiency
- **Retention**: Configurable retention policy for old logs

#### Performance Targets
- Write latency: < 1ms average
- Throughput: > 100K entries/sec
- Recovery time: < 1 second per 10K entries

### 2. LSM Store

#### Purpose
The LSM store optimizes for write-heavy workloads by using a log-structured merge-tree approach.

#### Key Features
- **MemTable**: In-memory sorted data structure for recent writes
- **SSTables**: Immutable, sorted files on disk
- **Leveling**: Multiple levels with size-tiered compaction
- **Bloom Filters**: Probabilistic data structures for fast lookups
- **Compression**: Per-level compression strategies

#### Implementation Details
- **MemTable**: Skip list implementation for O(log n) operations
- **SSTable Format**: 
  ```
  [Header][Index Block][Filter Block][Data Blocks][Footer]
  ```
- **Compaction Strategy**: Size-tiered compaction with leveled merging
- **Concurrency**: Lock-free reads, write batching

#### Performance Targets
- Write throughput: > 100K operations/sec
- Read latency: < 5ms average for point queries
- Space amplification: < 2x

### 3. B+Tree Store

#### Purpose
The B+Tree store optimizes for read-heavy workloads and range queries by using a traditional B+tree structure.

#### Key Features
- **Balanced Tree**: Guaranteed logarithmic access times
- **Leaf Node Linking**: Efficient range scans
- **Bulk Loading**: Optimized insertion for sorted data
- **Node Rebalancing**: Automatic tree balancing
- **Cache Friendly**: Sequential node layout

#### Implementation Details
- **Node Size**: Configurable page size (default: 4KB)
- **Fanout**: Variable based on key size (typically 100-500 children)
- **Split Policy**: 50-50 split for balanced trees
- **Concurrency**: Fine-grained locking with intention locks

#### Performance Targets
- Point query latency: < 1ms average
- Range query throughput: > 50K ranges/sec
- Insert latency: < 2ms average

### 4. Index Management System

#### Purpose
The index management system handles creation, maintenance, and optimization of secondary indexes.

#### Key Features
- **Index Types**: B+tree, Hash, and Full-text indexes
- **Index Creation**: Online index creation with minimal impact
- **Index Maintenance**: Automatic updates during data modifications
- **Index Statistics**: Collection and maintenance for query optimization
- **Index Drop**: Efficient removal of unused indexes

#### Implementation Details
- **Metadata Storage**: System tables for index metadata
- **Background Building**: Asynchronous index construction
- **Consistency**: Atomic index updates with data changes
- **Resource Management**: Limits on concurrent index operations

#### Performance Targets
- Index creation time: < 1 hour per 10M records
- Index update overhead: < 10% of data modification time
- Memory usage: < 100MB per index

### 5. Garbage Collection Manager

#### Purpose
The garbage collection manager reclaims space from deleted or obsolete data versions.

#### Key Features
- **MVCC Cleanup**: Removal of old transaction versions
- **Space Reclamation**: Return freed space to the storage pool
- **Background Processing**: Non-blocking cleanup operations
- **Tombstone Management**: Efficient handling of deleted records
- **Compaction Triggering**: Automatic compaction based on space usage

#### Implementation Details
- **Version Tracking**: Timestamp-based version identification
- **Reference Counting**: Track active references to data
- **Sweep Algorithm**: Periodic scanning for garbage
- **Concurrency**: Safe concurrent operation with active transactions

#### Performance Targets
- Space reclamation: > 90% of deleted space recovered
- Sweep overhead: < 5% of system resources
- Cleanup latency: < 100ms for typical operations

### 6. Backup/Restore Manager

#### Purpose
The backup/restore manager provides data protection and recovery capabilities.

#### Key Features
- **Consistent Snapshots**: Point-in-time consistent backups
- **Incremental Backup**: Efficient delta-based backups
- **Parallel Processing**: Concurrent backup operations
- **Compression**: Space-efficient backup storage
- **Encryption**: Secure backup transmission and storage

#### Implementation Details
- **Snapshot Isolation**: Use MVCC for consistent snapshots
- **Backup Format**: 
  ```
  [Header][Metadata][Data Chunks][Index][Footer]
  ```
- **Restore Process**: Validate and apply backup data
- **Recovery Testing**: Automated verification of backup integrity

#### Performance Targets
- Backup throughput: > 1GB/sec
- Restore time: < 1 hour per 100GB
- Incremental backup size: < 10% of full backup for typical workloads

## Hybrid Storage Approach

### Adaptive Data Placement
- **Hot Data**: Recent, frequently accessed data in B+tree store
- **Cold Data**: Older, infrequently accessed data in LSM store
- **Write-Heavy Tables**: Automatically routed to LSM store
- **Read-Heavy Tables**: Automatically routed to B+tree store

### Unified Interface
- **Single API**: Transparent access to both storage types
- **Automatic Optimization**: Runtime optimization based on access patterns
- **Migration**: Seamless data movement between storage types
- **Statistics**: Collection of access patterns for optimization

## Concurrency Control

### Reader-Writer Coordination
- **Lock-Free Reads**: MVCC for read operations
- **Write Batching**: Group writes for efficiency
- **Isolation Levels**: Support for all standard isolation levels
- **Deadlock Prevention**: Timeout-based deadlock handling

### Performance Optimization
- **Cache Coherence**: Consistent view across storage types
- **Memory Management**: Efficient buffer pool utilization
- **I/O Scheduling**: Prioritized I/O operations
- **Resource Limits**: Configurable resource usage caps

## Recovery Procedures

### Crash Recovery
1. **WAL Replay**: Reapply logged operations
2. **Consistency Check**: Verify data structure integrity
3. **Index Rebuild**: Reconstruct indexes if needed
4. **Service Restoration**: Resume normal operations

### Data Corruption Handling
1. **Detection**: Checksums and validation
2. **Isolation**: Prevent corruption spread
3. **Recovery**: Use backups or replicas
4. **Analysis**: Root cause investigation

## Configuration Options

### Performance Tuning
- **Buffer Pool Size**: Memory allocated for caching
- **Compaction Settings**: Trigger thresholds and strategies
- **WAL Policy**: Sync frequency and retention
- **Compression Levels**: Trade-off between CPU and space

### Operational Settings
- **Backup Schedule**: Automated backup timing
- **Retention Policy**: Data lifecycle management
- **Logging Level**: Verbosity of operational logs
- **Security Settings**: Encryption and access controls

## Monitoring and Metrics

### Key Performance Indicators
- **Throughput**: Operations per second
- **Latency**: Response time percentiles
- **Space Usage**: Storage efficiency metrics
- **Cache Hit Rate**: Memory utilization effectiveness

### Health Metrics
- **Error Rates**: Operation failure statistics
- **Resource Usage**: CPU, memory, disk, and network
- **Recovery Time**: Time to restore from failures
- **Backup Status**: Success and timing of backup operations

## Integration Points

### Query Processor Interface
- **Data Access API**: Standard methods for data retrieval
- **Transaction Integration**: Coordination with transaction manager
- **Statistics Interface**: Performance data collection
- **Metadata Access**: Schema and index information

### Administration Tools
- **Backup Interface**: Trigger and monitor backup operations
- **Maintenance API**: Manual compaction and optimization
- **Monitoring Hooks**: Real-time performance data
- **Configuration Management**: Runtime parameter adjustment

## Future Enhancements

### Planned Features
- **Columnar Storage**: Optimized analytical query support
- **Compression Improvements**: Advanced algorithms for specific data types
- **Tiered Storage**: Automatic data movement to different storage tiers
- **Machine Learning Optimization**: Adaptive tuning based on workload patterns

### Research Areas
- **Hardware-Aware Optimization**: Specialized implementations for NVM, SSD, etc.
- **Distributed Storage**: Extension to multi-node environments
- **Security Enhancements**: Advanced encryption and access controls
- **Cloud Integration**: Seamless cloud storage integration

This specification provides the foundation for implementing the storage engine component of Phantom-DB, ensuring it meets the performance, reliability, and scalability requirements while supporting the hybrid storage approach.