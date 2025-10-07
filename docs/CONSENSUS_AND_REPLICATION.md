# Consensus and Replication Implementation

## Overview

This document describes the consensus and replication components implemented for Phantom-DB's distributed architecture. These components form the second major milestone of Phase 3 implementation, providing distributed consensus through the Raft protocol, multi-region replication capabilities, and conflict resolution mechanisms.

## Components

### 1. Raft Consensus

The [RaftConsensus](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/raft.h#L64-L228) class implements the Raft consensus algorithm for distributed coordination. Raft is a consensus algorithm that is designed to be understandable and provides a foundation for fault-tolerant distributed systems.

#### Key Features:
- Leader election mechanism
- Log replication across cluster nodes
- Safety guarantees (election safety, leader completeness, state machine safety)
- Cluster membership changes
- Log compaction and snapshotting (planned)

#### Public Interface:
```cpp
class RaftConsensus {
public:
    RaftConsensus(const std::string& nodeId, const std::vector<RaftNodeInfo>& nodes);
    ~RaftConsensus();
    
    bool initialize();
    void shutdown();
    
    bool submitCommand(const std::string& command, const std::string& data);
    
    void registerApplyCommandCallback(const ApplyCommandCallback& callback);
    void registerSendRequestVoteCallback(const SendRequestVoteCallback& callback);
    void registerSendAppendEntriesCallback(const SendAppendEntriesCallback& callback);
    
    bool handleRequestVote(const RequestVoteRequest& request, RequestVoteResponse& response);
    bool handleAppendEntries(const AppendEntriesRequest& request, AppendEntriesResponse& response);
    
    std::string getLeader() const;
    uint64_t getCurrentTerm() const;
    RaftState getState() const;
    uint64_t getCommitIndex() const;
    uint64_t getLastApplied() const;
};
```

### 2. Replication Manager

The [ReplicationManager](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/replication_manager.h#L36-L138) class handles multi-region data replication, ensuring data availability and durability across different geographical regions.

#### Key Features:
- Multiple replication strategies (synchronous, asynchronous, semi-synchronous)
- Region management (add/remove regions)
- Heartbeat-based connectivity monitoring
- Primary region designation for semi-synchronous replication

#### Public Interface:
```cpp
class ReplicationManager {
public:
    ReplicationManager(ReplicationStrategy strategy = ReplicationStrategy::ASYNCHRONOUS);
    ~ReplicationManager();
    
    bool initialize();
    void shutdown();
    
    bool addRegion(const RegionInfo& region);
    bool removeRegion(const std::string& regionId);
    
    void setReplicationStrategy(ReplicationStrategy strategy);
    ReplicationStrategy getReplicationStrategy() const;
    
    bool replicateData(const std::string& key, const std::string& value);
    
    std::vector<ReplicationStatus> getReplicationStatus() const;
    std::vector<RegionInfo> getRegions() const;
    
    bool areAllRegionsConnected() const;
    std::string getPrimaryRegion() const;
    bool setPrimaryRegion(const std::string& regionId);
};
```

### 3. Conflict Resolver

The [ConflictResolver](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/conflict_resolver.h#L44-L121) class provides mechanisms for resolving data conflicts that may arise in distributed systems due to concurrent updates or network partitions.

#### Key Features:
- Multiple conflict resolution strategies (latest timestamp, LWW register, multi-value)
- Custom conflict resolution support
- Conflict resolution statistics tracking
- Thread-safe operations

#### Public Interface:
```cpp
class ConflictResolver {
public:
    ConflictResolver(ConflictResolutionStrategy strategy = ConflictResolutionStrategy::LATEST_TIMESTAMP);
    ~ConflictResolver();
    
    bool initialize();
    void shutdown();
    
    void setResolutionStrategy(ConflictResolutionStrategy strategy);
    ConflictResolutionStrategy getResolutionStrategy() const;
    
    void registerCustomResolver(const CustomConflictResolver& resolver);
    
    ResolvedConflict resolveConflict(const ConflictInfo& conflict);
    std::vector<ResolvedConflict> resolveConflicts(const std::vector<ConflictInfo>& conflicts);
    
    struct ResolutionStats {
        uint64_t totalConflicts;
        uint64_t resolvedByTimestamp;
        uint64_t resolvedByLWW;
        uint64_t resolvedByMultiValue;
        uint64_t resolvedByCustom;
        uint64_t unresolvedConflicts;
    };
    
    ResolutionStats getResolutionStats() const;
};
```

### 4. Consensus Manager

The [ConsensusManager](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/consensus_manager.h#L32-L104) class integrates all consensus and replication components into a unified interface for distributed coordination.

#### Key Features:
- Unified interface for all consensus and replication operations
- Integration of Raft consensus, replication management, and conflict resolution
- Command submission and application
- Status monitoring and statistics

#### Public Interface:
```cpp
class ConsensusManager {
public:
    ConsensusManager(const ConsensusConfig& config);
    ~ConsensusManager();
    
    bool initialize();
    void shutdown();
    
    bool submitCommand(const std::string& command, const std::string& data);
    
    void registerApplyCommandCallback(const ApplyCommandCallback& callback);
    void registerCustomConflictResolver(const CustomConflictResolver& resolver);
    
    bool addReplicationRegion(const RegionInfo& region);
    bool removeReplicationRegion(const std::string& regionId);
    
    bool replicateData(const std::string& key, const std::string& value);
    
    std::string getLeader() const;
    uint64_t getCurrentTerm() const;
    RaftState getNodeState() const;
    uint64_t getCommitIndex() const;
    uint64_t getLastApplied() const;
    
    std::vector<ReplicationStatus> getReplicationStatus() const;
    ConflictResolver::ResolutionStats getConflictResolutionStats() const;
};
```

## Integration

These components work together to provide a complete consensus and replication solution:

1. **RaftConsensus** handles leader election and log replication within a cluster
2. **ReplicationManager** manages data replication across geographical regions
3. **ConflictResolver** resolves data conflicts that may arise during replication
4. **ConsensusManager** orchestrates all these services through a unified interface

## Usage Example

```cpp
#include "consensus_manager.h"

using namespace phantomdb::distributed;

int main() {
    // Create Raft node information
    std::vector<RaftNodeInfo> raftNodes = {
        RaftNodeInfo("node1", "192.168.1.101", 8001),
        RaftNodeInfo("node2", "192.168.1.102", 8002),
        RaftNodeInfo("node3", "192.168.1.103", 8003)
    };
    
    // Create consensus configuration
    ConsensusConfig config;
    config.nodeId = "node1";
    config.raftNodes = raftNodes;
    config.replicationStrategy = ReplicationStrategy::ASYNCHRONOUS;
    config.conflictResolutionStrategy = ConflictResolutionStrategy::LATEST_TIMESTAMP;
    
    // Create and initialize consensus manager
    ConsensusManager consensusManager(config);
    consensusManager.initialize();
    
    // Add replication regions
    consensusManager.addReplicationRegion(RegionInfo("region1", "10.0.1.1", 9001, true));
    consensusManager.addReplicationRegion(RegionInfo("region2", "10.0.2.1", 9002));
    
    // Submit commands
    consensusManager.submitCommand("WRITE", "key1:value1");
    
    // Replicate data
    consensusManager.replicateData("test_key", "test_value");
    
    // Check status
    std::cout << "Current leader: " << consensusManager.getLeader() << std::endl;
    std::cout << "Commit index: " << consensusManager.getCommitIndex() << std::endl;
    
    // Shutdown
    consensusManager.shutdown();
    
    return 0;
}
```

## Implementation Details

### Raft Consensus Algorithm

The Raft implementation follows the standard Raft protocol with the following key aspects:

1. **Node States**: Each node can be in one of three states:
   - **Follower**: Receives RPCs from leaders and candidates
   - **Candidate**: Participates in leader elections
   - **Leader**: Handles client requests and replicates log entries

2. **Term Management**: Time is divided into terms, each beginning with an election. Terms are numbered consecutively and monotonically increase.

3. **Leader Election**: When a follower doesn't hear from a leader within an election timeout, it becomes a candidate and starts an election.

4. **Log Replication**: Leaders accept log entries from clients and replicate them to other servers.

5. **Safety**: Raft includes several mechanisms to ensure safety:
   - Election Safety: At most one leader can be elected in a term
   - Leader Completeness: If a log entry is committed in a given term, it will be present in the logs of leaders for all higher-numbered terms
   - State Machine Safety: If a server has applied a log entry at a given index to its state machine, no other server will ever apply a different log entry for the same index

### Replication Strategies

The replication manager supports three replication strategies:

1. **Synchronous**: Waits for acknowledgment from all regions before considering the operation complete
2. **Asynchronous**: Sends data to all regions without waiting for acknowledgment
3. **Semi-Synchronous**: Waits for acknowledgment from the primary region only

### Conflict Resolution Strategies

The conflict resolver provides four conflict resolution strategies:

1. **Latest Timestamp**: Resolves conflicts by selecting the value with the latest timestamp
2. **LWW Register**: Last Writer Wins approach based on timestamps
3. **Multi-Value**: Preserves all conflicting values in a multi-value structure
4. **Custom**: Allows users to provide custom conflict resolution logic

## Build and Test

To build and test the consensus and replication components:

1. Ensure the distributed module is included in the main CMakeLists.txt
2. Build the project with CMake
3. Execute the consensus test: `build/src/distributed/consensus_test.exe`

## Future Enhancements

Planned improvements for the consensus and replication system:

1. **Snapshotting**: Implement log compaction through snapshotting
2. **Membership Changes**: Support for dynamic cluster membership changes
3. **Performance Optimization**: Optimize network communication and reduce latency
4. **Security Integration**: Add authentication and encryption for RPCs
5. **Monitoring and Metrics**: Enhanced monitoring and performance metrics
6. **Advanced Conflict Resolution**: Implement more sophisticated conflict resolution algorithms