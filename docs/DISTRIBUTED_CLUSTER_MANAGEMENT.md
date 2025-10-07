# Distributed Cluster Management

## Overview

This document describes the cluster management components implemented for Phantom-DB's distributed architecture. These components form the foundation of Phase 3 implementation, providing node discovery, membership management, data sharding, and load balancing capabilities.

## Components

### 1. Cluster Manager

The [ClusterManager](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/cluster_manager.h#L26-L99) class is the central component responsible for managing the entire cluster. It coordinates all other distributed services and maintains the overall state of the cluster.

#### Key Features:
- Node registration and management
- Cluster health monitoring
- Integration with all other distributed services
- Thread-safe operations

#### Public Interface:
```cpp
class ClusterManager {
public:
    ClusterManager();
    ~ClusterManager();
    
    bool initialize();
    void shutdown();
    
    bool addNode(const std::string& nodeId, const std::string& address, int port);
    bool removeNode(const std::string& nodeId);
    
    std::shared_ptr<NodeInfo> getNode(const std::string& nodeId) const;
    std::vector<std::shared_ptr<NodeInfo>> getAllNodes() const;
    std::vector<std::shared_ptr<NodeInfo>> getActiveNodes() const;
    
    bool updateNodeHeartbeat(const std::string& nodeId);
    bool isClusterHealthy() const;
    
    size_t getClusterSize() const;
    size_t getActiveClusterSize() const;
};
```

### 2. Node Discovery

The [NodeDiscovery](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/node_discovery.h#L22-L79) class handles automatic detection and registration of nodes in the cluster. It implements mechanisms for discovering new nodes and detecting node failures.

#### Key Features:
- Automatic node discovery
- Node failure detection
- Configurable discovery intervals
- Callback-based event system

#### Public Interface:
```cpp
class NodeDiscovery {
public:
    NodeDiscovery();
    ~NodeDiscovery();
    
    bool initialize();
    void shutdown();
    
    bool startDiscovery();
    void stopDiscovery();
    
    void registerNodeCallback(const NodeDiscoveryCallback& callback);
    void registerFailureCallback(const NodeDiscoveryCallback& callback);
    
    bool addNode(const std::string& nodeId, const std::string& address, int port);
    bool removeNode(const std::string& nodeId);
    
    void setDiscoveryInterval(int intervalMs);
    int getDiscoveryInterval() const;
};
```

### 3. Membership Service

The [MembershipService](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/membership_service.h#L21-L71) maintains the list of active members in the cluster and tracks their health status through heartbeat mechanisms.

#### Key Features:
- Membership list management
- Heartbeat-based health monitoring
- Automatic failure detection
- Thread-safe operations

#### Public Interface:
```cpp
class MembershipService {
public:
    MembershipService();
    ~MembershipService();
    
    bool initialize();
    void shutdown();
    
    bool addNode(const std::string& nodeId);
    bool removeNode(const std::string& nodeId);
    
    bool isMember(const std::string& nodeId) const;
    std::vector<std::string> getMembers() const;
    size_t getMemberCount() const;
    
    bool updateHeartbeat(const std::string& nodeId);
    std::vector<std::string> detectFailedNodes(int timeoutSeconds = 30);
    
    void setHeartbeatTimeout(int timeoutSeconds);
    int getHeartbeatTimeout() const;
};
```

### 4. Sharding Strategy

The [ShardingStrategy](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/sharding_strategy.h#L36-L91) class implements data partitioning mechanisms to distribute data across cluster nodes.

#### Key Features:
- Multiple sharding algorithms (hash-based, range-based)
- Dynamic shard creation and management
- Node assignment to shards
- Automatic rebalancing

#### Public Interface:
```cpp
class ShardingStrategy {
public:
    ShardingStrategy(ShardingStrategyType strategyType = ShardingStrategyType::HASH_BASED);
    virtual ~ShardingStrategy();
    
    bool initialize();
    void shutdown();
    
    bool createShards(const std::vector<std::string>& nodeIds, int shardCount);
    std::string getShardForKey(const std::string& key) const;
    
    std::vector<std::string> getNodesForShard(const std::string& shardId) const;
    std::vector<std::shared_ptr<ShardInfo>> getAllShards() const;
    
    bool addNodeToShard(const std::string& shardId, const std::string& nodeId);
    bool removeNodeFromShard(const std::string& shardId, const std::string& nodeId);
    
    bool rebalanceShards(const std::vector<std::string>& currentNodes);
    
    ShardingStrategyType getStrategyType() const;
};
```

### 5. Load Balancer

The [LoadBalancer](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/distributed/load_balancer.h#L44-L103) distributes workload across cluster nodes using various load balancing algorithms.

#### Key Features:
- Multiple load balancing algorithms (round-robin, random, least connections, weighted)
- Node health monitoring
- Performance-based node selection
- Statistics tracking

#### Public Interface:
```cpp
class LoadBalancer {
public:
    LoadBalancer(LoadBalancingAlgorithm algorithm = LoadBalancingAlgorithm::ROUND_ROBIN);
    ~LoadBalancer();
    
    bool initialize();
    void shutdown();
    
    bool addNode(const std::string& nodeId);
    bool removeNode(const std::string& nodeId);
    
    std::string getNextNode(const std::vector<std::string>& availableNodes);
    
    bool updateNodeStatistics(const std::string& nodeId, const NodeStatistics& stats);
    std::shared_ptr<NodeStatistics> getNodeStatistics(const std::string& nodeId) const;
    
    void setNodeHealth(const std::string& nodeId, bool isHealthy);
    bool isNodeHealthy(const std::string& nodeId) const;
    
    void setAlgorithm(LoadBalancingAlgorithm algorithm);
    LoadBalancingAlgorithm getAlgorithm() const;
};
```

## Integration

These components work together to provide a complete cluster management solution:

1. **Node Discovery** automatically detects new nodes and notifies the Cluster Manager
2. **Membership Service** maintains the list of active cluster members
3. **Sharding Strategy** distributes data across the cluster nodes
4. **Load Balancer** ensures optimal distribution of workload
5. **Cluster Manager** orchestrates all these services

## Usage Example

```cpp
#include "cluster_manager.h"

using namespace phantomdb::distributed;

int main() {
    // Create and initialize cluster manager
    ClusterManager clusterManager;
    clusterManager.initialize();
    
    // Add nodes to the cluster
    clusterManager.addNode("node1", "192.168.1.101", 8001);
    clusterManager.addNode("node2", "192.168.1.102", 8002);
    
    // Check cluster health
    if (clusterManager.isClusterHealthy()) {
        std::cout << "Cluster is healthy" << std::endl;
    }
    
    // Get cluster information
    std::cout << "Cluster size: " << clusterManager.getClusterSize() << std::endl;
    
    // Shutdown
    clusterManager.shutdown();
    
    return 0;
}
```

## Build and Test

To build and test the distributed components:

1. Run the build script: `build_distributed.bat`
2. Execute the cluster test: `build/Release/cluster_test.exe`

## Future Enhancements

Planned improvements for the distributed cluster management system:

1. **Advanced Discovery Protocols**: Implementation of gossip protocols or multicast discovery
2. **Dynamic Reconfiguration**: Support for runtime cluster reconfiguration
3. **Enhanced Sharding**: Consistent hashing and directory-based sharding strategies
4. **Security Integration**: Authentication and encryption for inter-node communication
5. **Monitoring and Metrics**: Comprehensive monitoring and performance metrics