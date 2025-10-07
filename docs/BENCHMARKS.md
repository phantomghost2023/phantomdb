# PhantomDB Benchmark Suite

This document describes the benchmark suite for PhantomDB, which measures performance across various operations and workloads.

## Overview

The benchmark suite provides quantitative measurements of PhantomDB's performance characteristics including:
- Database creation and deletion
- Table creation and schema operations
- Data insertion rates
- Query execution times
- Transaction throughput
- Concurrent operation performance
- Distributed operation performance

## Benchmark Categories

### 1. Core Database Operations
- Database creation/deletion performance
- Table creation/deletion performance
- Index creation performance

### 2. Data Operations
- Insert performance (single/multi-row)
- Update performance
- Delete performance
- Bulk insert performance

### 3. Query Operations
- Simple SELECT queries
- Complex SELECT with WHERE clauses
- JOIN performance
- Aggregation performance

### 4. Transaction Operations
- Single transaction throughput
- Concurrent transaction performance
- Transaction commit/rollback times

### 5. Concurrent Operations
- Multi-threaded insert performance
- Multi-threaded query performance
- Lock contention scenarios

### 6. Distributed Operations
- Cluster formation time
- Data replication performance
- Cross-shard query performance
- Distributed transaction performance

## Running Benchmarks

### Building Benchmark Executables

```bash
# Build with benchmarks enabled
cmake -DBENCHMARKS=ON .
make
```

### Running All Benchmarks

```bash
# Run all benchmarks
./run_benchmarks.sh
```

### Running Specific Benchmarks

```bash
# Run core database benchmarks
./benchmarks/core_benchmarks

# Run query benchmarks
./benchmarks/query_benchmarks

# Run transaction benchmarks
./benchmarks/transaction_benchmarks
```

## Benchmark Output

Each benchmark produces:
1. Execution time measurements
2. Throughput metrics (operations/second)
3. Resource utilization statistics
4. Comparative results against baseline

Results are output in both human-readable format and machine-readable JSON for automated analysis.

## Performance Goals

### Latency Targets
- Simple query execution: < 10ms
- Complex query execution: < 100ms
- Transaction commit: < 50ms
- Bulk insert (1000 rows): < 100ms

### Throughput Targets
- Single-row inserts: > 10,000 ops/sec
- Query execution: > 5,000 ops/sec
- Concurrent transactions: > 1,000 ops/sec
- Distributed operations: > 500 ops/sec

### Scalability Targets
- Linear scaling up to 16 cores
- Near-linear scaling up to 8 nodes
- < 10% degradation under moderate load
- Graceful degradation under heavy load

## Continuous Benchmarking

Benchmarks are run automatically as part of the CI pipeline and results are tracked over time to detect performance regressions.