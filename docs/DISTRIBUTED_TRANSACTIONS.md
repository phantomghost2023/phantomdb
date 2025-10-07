# Distributed Transactions Implementation

## Overview

This document describes the distributed transaction components implemented for Phantom-DB's distributed architecture. These components form the third major milestone of Phase 3 implementation, providing distributed transaction capabilities through Two-Phase Commit (2PC) and Saga patterns, along with cross-shard query processing.

## Components

### 1. Two-Phase Commit Coordinator

The [TwoPhaseCommitCoordinator](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/two_phase_commit.h#L50-L153) class implements the Two-Phase Commit protocol for distributed transactions. 2PC is a fundamental protocol for ensuring atomicity across distributed systems.

#### Key Features:
- Classic two-phase commit implementation (prepare and commit/abort phases)
- Participant management (add/remove participants)
- Timeout handling for both prepare and commit/abort phases
- Callback-based participant communication
- Transaction state tracking

#### Public Interface:
```cpp
class TwoPhaseCommitCoordinator {
public:
    TwoPhaseCommitCoordinator();
    ~TwoPhaseCommitCoordinator();
    
    bool initialize();
    void shutdown();
    
    bool beginTransaction(const std::string& transactionId, const std::vector<ParticipantInfo>& participants);
    bool addParticipant(const std::string& transactionId, const ParticipantInfo& participant);
    bool executeTwoPhaseCommit(const std::string& transactionId);
    
    TransactionState getTransactionState(const std::string& transactionId) const;
    std::vector<ParticipantInfo> getParticipants(const std::string& transactionId) const;
    
    void registerPrepareCallback(const PrepareCallback& callback);
    void registerCommitCallback(const CommitCallback& callback);
    void registerAbortCallback(const AbortCallback& callback);
    
    void setPrepareTimeout(const std::chrono::milliseconds& timeout);
    void setCommitAbortTimeout(const std::chrono::milliseconds& timeout);
    
    std::chrono::milliseconds getPrepareTimeout() const;
    std::chrono::milliseconds getCommitAbortTimeout() const;
};
```

### 2. Saga Coordinator

The [SagaCoordinator](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/saga.h#L54-L136) class implements the Saga pattern for distributed transactions. The Saga pattern is an alternative to 2PC that provides better availability by using compensating transactions.

#### Key Features:
- Saga pattern implementation with action and compensation steps
- Sequential step execution with compensation on failure
- Timeout handling for saga execution
- Callback-based participant communication
- Saga and step state tracking

#### Public Interface:
```cpp
class SagaCoordinator {
public:
    SagaCoordinator();
    ~SagaCoordinator();
    
    bool initialize();
    void shutdown();
    
    bool beginSaga(const std::string& sagaId);
    bool addStep(const std::string& sagaId, const SagaStep& step);
    bool executeSaga(const std::string& sagaId);
    
    SagaStatus getSagaStatus(const std::string& sagaId) const;
    std::vector<SagaStep> getSteps(const std::string& sagaId) const;
    
    void registerActionCallback(const SagaActionCallback& callback);
    void registerCompensationCallback(const SagaCompensationCallback& callback);
    
    void setSagaTimeout(const std::chrono::milliseconds& timeout);
    std::chrono::milliseconds getSagaTimeout() const;
};
```

### 3. Cross-Shard Query Processor

The [CrossShardQueryProcessor](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/cross_shard_query.h#L42-L101) class handles queries that span multiple shards in a distributed database.

#### Key Features:
- Cross-shard query execution
- Parallel execution across multiple shards
- Shard management (add/remove shards)
- Table-to-shard mapping
- Query result merging
- Timeout handling

#### Public Interface:
```cpp
class CrossShardQueryProcessor {
public:
    CrossShardQueryProcessor();
    ~CrossShardQueryProcessor();
    
    bool initialize();
    void shutdown();
    
    bool addShard(const ShardInfo& shard);
    bool removeShard(const std::string& shardId);
    
    std::vector<QueryResult> executeCrossShardQuery(const std::string& query);
    std::vector<QueryResult> executeQueryOnShards(const std::string& query, 
                                                const std::vector<std::string>& shardIds);
    
    std::vector<ShardInfo> getShards() const;
    std::vector<ShardInfo> getShardsForTable(const std::string& tableName) const;
    
    void registerQueryExecutionCallback(const QueryExecutionCallback& callback);
    
    void setQueryTimeout(const std::chrono::milliseconds& timeout);
    std::chrono::milliseconds getQueryTimeout() const;
};
```

### 4. Distributed Transaction Manager

The [DistributedTransactionManager](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/distributed_transaction_manager.h#L54-L123) class integrates all distributed transaction components into a unified interface.

#### Key Features:
- Unified interface for both 2PC and Saga transactions
- Cross-shard query integration
- Transaction lifecycle management
- Participant and shard management
- Callback registration for all components

#### Public Interface:
```cpp
class DistributedTransactionManager {
public:
    DistributedTransactionManager();
    ~DistributedTransactionManager();
    
    bool initialize();
    void shutdown();
    
    bool beginTransaction(const std::string& transactionId, 
                         const DistributedTransactionConfig& config);
    bool executeTransaction(const std::string& transactionId);
    
    bool addSagaStep(const std::string& transactionId, const SagaStep& step);
    bool addParticipant(const std::string& transactionId, const ParticipantInfo& participant);
    
    std::vector<QueryResult> executeCrossShardQuery(const std::string& transactionId, 
                                                   const std::string& query);
    
    DistributedTransactionStatus getTransactionStatus(const std::string& transactionId) const;
    DistributedTransactionType getTransactionType(const std::string& transactionId) const;
    
    void registerPrepareCallback(const PrepareCallback& callback);
    void registerCommitCallback(const CommitCallback& callback);
    void registerAbortCallback(const AbortCallback& callback);
    void registerActionCallback(const SagaActionCallback& callback);
    void registerCompensationCallback(const SagaCompensationCallback& callback);
    void registerQueryExecutionCallback(const QueryExecutionCallback& callback);
    
    bool addShard(const ShardInfo& shard);
    bool removeShard(const std::string& shardId);
};
```

## Integration

These components work together to provide a complete distributed transaction solution:

1. **TwoPhaseCommitCoordinator** handles traditional 2PC transactions with strong consistency
2. **SagaCoordinator** handles Saga transactions with better availability and eventual consistency
3. **CrossShardQueryProcessor** handles queries that span multiple shards
4. **DistributedTransactionManager** orchestrates all these services through a unified interface

## Usage Example

```cpp
#include "distributed_transaction_manager.h"

using namespace phantomdb::distributed;

int main() {
    // Create and initialize distributed transaction manager
    DistributedTransactionManager dtm;
    dtm.initialize();
    
    // Register callbacks
    dtm.registerPrepareCallback(prepareCallback);
    dtm.registerCommitCallback(commitCallback);
    dtm.registerAbortCallback(abortCallback);
    
    // Add shards for cross-shard queries
    dtm.addShard(ShardInfo("shard1", "192.168.1.101", 8001));
    dtm.addShard(ShardInfo("shard2", "192.168.1.102", 8002));
    
    // Example 1: Two-Phase Commit Transaction
    DistributedTransactionConfig config2PC;
    config2PC.type = DistributedTransactionType::TWO_PHASE_COMMIT;
    config2PC.participants = {
        ParticipantInfo("participant1", "192.168.1.201", 9001),
        ParticipantInfo("participant2", "192.168.1.202", 9002)
    };
    
    std::string transactionId1 = "2pc_transaction_1";
    dtm.beginTransaction(transactionId1, config2PC);
    dtm.addParticipant(transactionId1, ParticipantInfo("participant3", "192.168.1.203", 9003));
    dtm.executeTransaction(transactionId1);
    
    // Example 2: Saga Transaction
    DistributedTransactionConfig configSaga;
    configSaga.type = DistributedTransactionType::SAGA;
    
    std::string transactionId2 = "saga_transaction_1";
    dtm.beginTransaction(transactionId2, configSaga);
    dtm.addSagaStep(transactionId2, SagaStep("step1", "action1", "compensation1", "participant1"));
    dtm.addSagaStep(transactionId2, SagaStep("step2", "action2", "compensation2", "participant2"));
    dtm.executeTransaction(transactionId2);
    
    // Example 3: Cross-shard query
    auto results = dtm.executeCrossShardQuery(transactionId1, "SELECT * FROM users");
    
    // Shutdown
    dtm.shutdown();
    
    return 0;
}
```

## Implementation Details

### Two-Phase Commit Protocol

The Two-Phase Commit implementation follows the standard 2PC protocol:

1. **Phase 1 (Prepare)**: The coordinator asks all participants to prepare for the transaction
2. **Phase 2 (Commit/Abort)**: Based on all participants' responses, the coordinator decides to commit or abort

Key aspects:
- All participants must vote YES in the prepare phase for the transaction to commit
- If any participant votes NO or times out, the transaction is aborted
- The coordinator handles timeouts for both phases

### Saga Pattern

The Saga pattern implementation provides an alternative to 2PC with better availability:

1. **Sequential Execution**: Saga steps are executed sequentially
2. **Compensation**: If any step fails, previously executed steps are compensated in reverse order
3. **Eventual Consistency**: Provides eventual consistency rather than immediate consistency

Key aspects:
- Each step has an associated compensation action
- Compensation is performed in reverse order of execution
- Sagas can be long-running transactions

### Cross-Shard Queries

The cross-shard query processor handles queries that span multiple shards:

1. **Shard Determination**: Determines which shards contain the relevant data
2. **Parallel Execution**: Executes the query on all relevant shards in parallel
3. **Result Merging**: Merges results from different shards

Key aspects:
- Supports both broadcast queries (to all shards) and targeted queries (to specific shards)
- Parallel execution for improved performance
- Timeout handling for query execution

## Build and Test

To build and test the distributed transaction components:

1. Ensure the distributed module is included in the main CMakeLists.txt
2. Build the project with CMake
3. Execute the distributed transaction test: `build/src/distributed/distributed_transaction_test.exe`

## Future Enhancements

Planned improvements for the distributed transaction system:

1. **Global Timestamp Ordering**: Implement global timestamp ordering for better consistency
2. **Advanced Compensation**: Implement more sophisticated compensation strategies
3. **Performance Optimization**: Optimize network communication and reduce latency
4. **Security Integration**: Add authentication and encryption for transaction coordination
5. **Monitoring and Metrics**: Enhanced monitoring and performance metrics
6. **Hybrid Transaction Strategy**: Implement a hybrid approach that selects between 2PC and Saga based on transaction characteristics