#include "cluster_manager.h"
#include "consensus_manager.h"
#include "distributed_transaction_manager.h"
// Commenting out elastic_scaling_manager.h to avoid NodeInfo redefinition
// #include "elastic_scaling_manager.h"
#include "data_rebalancer.h"
#include "cross_shard_query.h"
#include "node_discovery.h"
#include "membership_service.h"
// Commenting out sharding_strategy.h to avoid ShardInfo redefinition
// #include "sharding_strategy.h"
#include "load_balancer.h"
#include "raft.h"
#include "replication_manager.h"
#include "conflict_resolver.h"
#include "two_phase_commit.h"
#include "saga.h"
#include "node_manager.h"
#include "resource_scaler.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <atomic>
#include <future>
#include <iomanip>
#include <sstream>
#include <cfloat>
#include <mutex>

using namespace phantomdb::distributed;

// Performance test configuration
struct PerformanceConfig {
    int numNodes = 10;
    int numTransactions = 1000;
    int numQueries = 1000;
    int numConcurrentClients = 10;
    int testDurationSeconds = 30;
    int dataSizeKB = 1024; // 1MB data size
};

// Performance metrics structure
struct PerformanceMetrics {
    double avgLatencyMs = 0.0;
    double maxLatencyMs = 0.0;
    double minLatencyMs = 0.0;
    double throughputOpsPerSec = 0.0;
    double cpuUsagePercent = 0.0;
    double memoryUsageMB = 0.0;
    int totalOperations = 0;
    int failedOperations = 0;
    int successfulOperations = 0;
};

// Test result structure
struct TestResult {
    std::string testName;
    PerformanceMetrics metrics;
    bool passed = false;
    std::string details;
};

// Global performance counters
static std::atomic<int> g_totalOps(0);
static std::atomic<int> g_failedOps(0);
static std::atomic<int> g_successOps(0);
static std::atomic<long long> g_totalLatency(0); // Use long long for atomic operations
static std::atomic<double> g_maxLatency(0.0);
static std::atomic<double> g_minLatency(DBL_MAX);

// Random generator for test data
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dis(1, 1000000);

// Helper function to generate random strings
std::string generateRandomString(int length) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += chars[dis(gen) % chars.length()];
    }
    return result;
}

// Helper function to measure time
class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsedMilliseconds() const {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
        return duration.count() / 1000.0;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

// Helper function to update performance metrics
void updateMetrics(double latencyMs) {
    g_totalOps.fetch_add(1);
    g_totalLatency.fetch_add(static_cast<long long>(latencyMs * 1000)); // Convert to microseconds
    
    double currentMax = g_maxLatency.load();
    while (latencyMs > currentMax) {
        if (g_maxLatency.compare_exchange_weak(currentMax, latencyMs)) {
            break;
        }
    }
    
    double currentMin = g_minLatency.load();
    while (latencyMs < currentMin) {
        if (g_minLatency.compare_exchange_weak(currentMin, latencyMs)) {
            break;
        }
    }
    
    g_successOps.fetch_add(1);
}

// Helper function to log test results
void logTestResult(const std::string& testName, bool passed, const std::string& message = "") {
    if (passed) {
        std::cout << "✓ " << testName << " PASSED" << std::endl;
    } else {
        std::cout << "✗ " << testName << " FAILED: " << message << std::endl;
    }
}

// Test: Cluster Management Performance
TestResult testClusterManagementPerformance(const PerformanceConfig& config) {
    std::cout << "Running Cluster Management Performance Test" << std::endl;
    
    TestResult result;
    result.testName = "ClusterManagementPerformance";
    
    try {
        Timer totalTimer;
        
        // Create cluster manager
        ClusterManager clusterManager;
        if (!clusterManager.initialize()) {
            result.passed = false;
            result.details = "Failed to initialize ClusterManager";
            logTestResult(result.testName, result.passed, result.details);
            return result;
        }
        
        // Add nodes and measure performance
        Timer addTimer;
        for (int i = 0; i < config.numNodes; ++i) {
            std::string nodeId = "perf_node_" + std::to_string(i);
            std::string address = "192.168.1." + std::to_string(100 + i);
            clusterManager.addNode(nodeId, address, 8000 + i);
        }
        double addTime = addTimer.elapsedMilliseconds();
        
        // Update heartbeats and measure performance
        Timer heartbeatTimer;
        for (int i = 0; i < config.numNodes; ++i) {
            std::string nodeId = "perf_node_" + std::to_string(i);
            clusterManager.updateNodeHeartbeat(nodeId);
        }
        double heartbeatTime = heartbeatTimer.elapsedMilliseconds();
        
        // Get cluster status and measure performance
        Timer statusTimer;
        auto allNodes = clusterManager.getAllNodes();
        auto activeNodes = clusterManager.getActiveNodes();
        bool isHealthy = clusterManager.isClusterHealthy();
        double statusTime = statusTimer.elapsedMilliseconds();
        
        double totalTime = totalTimer.elapsedMilliseconds();
        
        // Calculate metrics
        result.metrics.totalOperations = config.numNodes * 2 + 1; // add + heartbeat + status
        result.metrics.successfulOperations = result.metrics.totalOperations;
        result.metrics.avgLatencyMs = totalTime / result.metrics.totalOperations;
        result.metrics.maxLatencyMs = std::max({addTime, heartbeatTime, statusTime});
        result.metrics.minLatencyMs = std::min({addTime / config.numNodes, heartbeatTime / config.numNodes, statusTime});
        result.metrics.throughputOpsPerSec = (result.metrics.totalOperations / totalTime) * 1000;
        
        result.passed = true;
        result.details = "Processed " + std::to_string(result.metrics.totalOperations) + 
                        " operations in " + std::to_string(totalTime) + " ms";
        
        // Shutdown cluster manager
        clusterManager.shutdown();
        
        logTestResult(result.testName, result.passed, result.details);
        return result;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.details = "Exception: " + std::string(e.what());
        logTestResult(result.testName, result.passed, result.details);
        return result;
    }
}

// Test: Consensus Performance
TestResult testConsensusPerformance(const PerformanceConfig& config) {
    std::cout << "Running Consensus Performance Test" << std::endl;
    
    TestResult result;
    result.testName = "ConsensusPerformance";
    
    try {
        Timer totalTimer;
        
        // Create consensus configuration
        std::vector<RaftNodeInfo> raftNodes;
        for (int i = 0; i < config.numNodes; ++i) {
            raftNodes.emplace_back(
                "node" + std::to_string(i),
                "192.168.1." + std::to_string(100 + i),
                8000 + i
            );
        }
        
        ConsensusConfig consensusConfig;
        consensusConfig.nodeId = "node0";
        consensusConfig.raftNodes = raftNodes;
        consensusConfig.replicationStrategy = ReplicationStrategy::ASYNCHRONOUS;
        consensusConfig.conflictResolutionStrategy = ConflictResolutionStrategy::LATEST_TIMESTAMP;
        
        // Create consensus manager
        ConsensusManager consensusManager(consensusConfig);
        if (!consensusManager.initialize()) {
            result.passed = false;
            result.details = "Failed to initialize ConsensusManager";
            logTestResult(result.testName, result.passed, result.details);
            return result;
        }
        
        // Add replication regions
        consensusManager.addReplicationRegion(RegionInfo("region1", "10.0.1.1", 9001, true));
        consensusManager.addReplicationRegion(RegionInfo("region2", "10.0.2.1", 9002));
        
        // Submit commands and measure performance
        Timer commandTimer;
        int commandsSubmitted = 0;
        for (int i = 0; i < config.numTransactions; ++i) {
            std::string key = "key" + std::to_string(i);
            std::string value = generateRandomString(100); // 100 byte values
            consensusManager.submitCommand("WRITE", key + ":" + value);
            commandsSubmitted++;
        }
        double commandTime = commandTimer.elapsedMilliseconds();
        
        // Replicate data and measure performance
        Timer replicateTimer;
        int dataReplicated = 0;
        for (int i = 0; i < config.numTransactions / 10; ++i) {
            std::string key = "replica_key" + std::to_string(i);
            std::string value = generateRandomString(1000); // 1KB values
            consensusManager.replicateData(key, value);
            dataReplicated++;
        }
        double replicateTime = replicateTimer.elapsedMilliseconds();
        
        double totalTime = totalTimer.elapsedMilliseconds();
        
        // Calculate metrics
        result.metrics.totalOperations = commandsSubmitted + dataReplicated;
        result.metrics.successfulOperations = result.metrics.totalOperations;
        result.metrics.avgLatencyMs = totalTime / result.metrics.totalOperations;
        result.metrics.maxLatencyMs = std::max(commandTime / commandsSubmitted, replicateTime / dataReplicated) * 1000;
        result.metrics.minLatencyMs = std::min(commandTime / commandsSubmitted, replicateTime / dataReplicated) * 1000;
        result.metrics.throughputOpsPerSec = (result.metrics.totalOperations / totalTime) * 1000;
        
        result.passed = true;
        result.details = "Processed " + std::to_string(result.metrics.totalOperations) + 
                        " operations in " + std::to_string(totalTime) + " ms";
        
        // Shutdown consensus manager
        consensusManager.shutdown();
        
        logTestResult(result.testName, result.passed, result.details);
        return result;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.details = "Exception: " + std::string(e.what());
        logTestResult(result.testName, result.passed, result.details);
        return result;
    }
}

// Test: Distributed Transaction Performance
TestResult testDistributedTransactionPerformance(const PerformanceConfig& config) {
    std::cout << "Running Distributed Transaction Performance Test" << std::endl;
    
    TestResult result;
    result.testName = "DistributedTransactionPerformance";
    
    try {
        Timer totalTimer;
        
        // Create distributed transaction manager
        DistributedTransactionManager txnManager;
        if (!txnManager.initialize()) {
            result.passed = false;
            result.details = "Failed to initialize DistributedTransactionManager";
            logTestResult(result.testName, result.passed, result.details);
            return result;
        }
        
        // Process transactions and measure performance
        Timer txnTimer;
        int txnsProcessed = 0;
        int txnsCommitted = 0;
        int txnsRolledBack = 0;
        
        for (int i = 0; i < config.numTransactions; ++i) {
            // Begin transaction
            DistributedTransactionConfig txnConfig;
            txnConfig.type = DistributedTransactionType::TWO_PHASE_COMMIT;
            std::string txnId = "txn_" + std::to_string(i);
            
            if (!txnManager.beginTransaction(txnId, txnConfig)) {
                g_failedOps.fetch_add(1);
                continue;
            }
            
            // Add participants to transaction
            ParticipantInfo participant1("node1", "192.168.1.101", 8001);
            ParticipantInfo participant2("node2", "192.168.1.102", 8002);
            ParticipantInfo participant3("node3", "192.168.1.103", 8003);
            
            txnManager.addParticipant(txnId, participant1);
            txnManager.addParticipant(txnId, participant2);
            txnManager.addParticipant(txnId, participant3);
            
            // Execute transaction
            bool executed = txnManager.executeTransaction(txnId);
            if (executed) {
                txnsCommitted++;
            } else {
                g_failedOps.fetch_add(1);
                txnsRolledBack++;
            }
            
            txnsProcessed++;
        }
        double txnTime = txnTimer.elapsedMilliseconds();
        
        double totalTime = totalTimer.elapsedMilliseconds();
        
        // Calculate metrics
        result.metrics.totalOperations = txnsProcessed;
        result.metrics.successfulOperations = txnsCommitted;
        result.metrics.failedOperations = txnsRolledBack;
        result.metrics.avgLatencyMs = totalTime / result.metrics.totalOperations;
        result.metrics.maxLatencyMs = txnTime / txnsProcessed * 1000; // Convert to ms
        result.metrics.minLatencyMs = txnTime / txnsProcessed * 1000; // Convert to ms
        result.metrics.throughputOpsPerSec = (result.metrics.totalOperations / totalTime) * 1000;
        
        result.passed = true;
        result.details = "Processed " + std::to_string(result.metrics.totalOperations) + 
                        " transactions in " + std::to_string(totalTime) + " ms";
        
        // Shutdown transaction manager
        txnManager.shutdown();
        
        logTestResult(result.testName, result.passed, result.details);
        return result;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.details = "Exception: " + std::string(e.what());
        logTestResult(result.testName, result.passed, result.details);
        return result;
    }
}

// Test: Cross-Shard Query Performance
TestResult testCrossShardQueryPerformance(const PerformanceConfig& config) {
    std::cout << "Running Cross-Shard Query Performance Test" << std::endl;
    
    TestResult result;
    result.testName = "CrossShardQueryPerformance";
    
    try {
        Timer totalTimer;
        
        // Create cross-shard query processor
        CrossShardQueryProcessor queryProcessor;
        if (!queryProcessor.initialize()) {
            result.passed = false;
            result.details = "Failed to initialize CrossShardQueryProcessor";
            logTestResult(result.testName, result.passed, result.details);
            return result;
        }
        
        // Process queries and measure performance
        Timer queryTimer;
        int queriesProcessed = 0;
        
        for (int i = 0; i < config.numQueries; ++i) {
            // Generate complex query
            std::string query = 
                "SELECT u.name, o.order_date, p.product_name "
                "FROM users u "
                "JOIN orders o ON u.id = o.user_id "
                "JOIN products p ON o.product_id = p.id "
                "WHERE u.age > " + std::to_string(i % 100) + 
                " AND o.order_date > '2023-01-01'";
            
            // Execute query
            auto resultData = queryProcessor.executeCrossShardQuery(query);
            queriesProcessed++;
        }
        double queryTime = queryTimer.elapsedMilliseconds();
        
        double totalTime = totalTimer.elapsedMilliseconds();
        
        // Calculate metrics
        result.metrics.totalOperations = queriesProcessed;
        result.metrics.successfulOperations = queriesProcessed;
        result.metrics.avgLatencyMs = totalTime / result.metrics.totalOperations;
        result.metrics.maxLatencyMs = queryTime / queriesProcessed * 1000; // Convert to ms
        result.metrics.minLatencyMs = queryTime / queriesProcessed * 1000; // Convert to ms
        result.metrics.throughputOpsPerSec = (result.metrics.totalOperations / totalTime) * 1000;
        
        result.passed = true;
        result.details = "Processed " + std::to_string(result.metrics.totalOperations) + 
                        " queries in " + std::to_string(totalTime) + " ms";
        
        // Shutdown query processor
        queryProcessor.shutdown();
        
        logTestResult(result.testName, result.passed, result.details);
        return result;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.details = "Exception: " + std::string(e.what());
        logTestResult(result.testName, result.passed, result.details);
        return result;
    }
}

// Test: Concurrent Operations Performance
TestResult testConcurrentOperationsPerformance(const PerformanceConfig& config) {
    std::cout << "Running Concurrent Operations Performance Test" << std::endl;
    
    TestResult result;
    result.testName = "ConcurrentOperationsPerformance";
    
    try {
        Timer totalTimer;
        
        // Create cluster manager
        ClusterManager clusterManager;
        if (!clusterManager.initialize()) {
            result.passed = false;
            result.details = "Failed to initialize ClusterManager";
            logTestResult(result.testName, result.passed, result.details);
            return result;
        }
        
        // Add initial nodes
        for (int i = 0; i < config.numNodes; ++i) {
            std::string nodeId = "concurrent_node_" + std::to_string(i);
            std::string address = "192.168.1." + std::to_string(100 + i);
            clusterManager.addNode(nodeId, address, 8000 + i);
        }
        
        // Launch concurrent client threads
        std::vector<std::future<void>> futures;
        
        auto startTime = std::chrono::steady_clock::now();
        auto endTime = startTime + std::chrono::seconds(config.testDurationSeconds);
        
        for (int client = 0; client < config.numConcurrentClients; ++client) {
            futures.push_back(std::async(std::launch::async, [&clusterManager, &endTime, client, &config]() {
                int localOps = 0;
                while (std::chrono::steady_clock::now() < endTime) {
                    // Random operations
                    int opType = dis(gen) % 4;
                    
                    switch (opType) {
                        case 0: // Add node
                            {
                                std::string nodeId = "client" + std::to_string(client) + "_node" + std::to_string(localOps);
                                std::string address = "192.168.2." + std::to_string(100 + (localOps % 255));
                                clusterManager.addNode(nodeId, address, 9000 + (localOps % 1000));
                                break;
                            }
                        case 1: // Update heartbeat
                            {
                                int nodeIdx = localOps % config.numNodes;
                                std::string nodeId = "concurrent_node_" + std::to_string(nodeIdx);
                                clusterManager.updateNodeHeartbeat(nodeId);
                                break;
                            }
                        case 2: // Get node status
                            {
                                auto nodes = clusterManager.getAllNodes();
                                bool healthy = clusterManager.isClusterHealthy();
                                break;
                            }
                        case 3: // Remove node
                            {
                                int nodeIdx = localOps % config.numNodes;
                                std::string nodeId = "concurrent_node_" + std::to_string(nodeIdx);
                                clusterManager.removeNode(nodeId);
                                break;
                            }
                    }
                    
                    localOps++;
                    g_totalOps.fetch_add(1);
                }
            }));
        }
        
        // Wait for all threads to complete
        for (auto& future : futures) {
            future.wait();
        }
        
        double totalTime = totalTimer.elapsedMilliseconds();
        
        // Calculate metrics
        result.metrics.totalOperations = g_totalOps.load();
        result.metrics.successfulOperations = result.metrics.totalOperations;
        result.metrics.avgLatencyMs = totalTime / result.metrics.totalOperations;
        result.metrics.throughputOpsPerSec = (result.metrics.totalOperations / totalTime) * 1000;
        
        result.passed = true;
        result.details = "Processed " + std::to_string(result.metrics.totalOperations) + 
                        " concurrent operations in " + std::to_string(totalTime) + " ms";
        
        // Shutdown cluster manager
        clusterManager.shutdown();
        
        logTestResult(result.testName, result.passed, result.details);
        return result;
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.details = "Exception: " + std::string(e.what());
        logTestResult(result.testName, result.passed, result.details);
        return result;
    }
}

// Print performance metrics
void printPerformanceMetrics(const std::vector<TestResult>& results) {
    std::cout << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "Performance Test Results Summary" << std::endl;
    std::cout << "==============================================" << std::endl;
    
    for (const auto& result : results) {
        std::cout << "Test: " << result.testName << std::endl;
        if (result.passed) {
            std::cout << "  Status: PASSED" << std::endl;
            std::cout << "  Total Operations: " << result.metrics.totalOperations << std::endl;
            std::cout << "  Successful Operations: " << result.metrics.successfulOperations << std::endl;
            std::cout << "  Failed Operations: " << result.metrics.failedOperations << std::endl;
            std::cout << "  Average Latency: " << std::fixed << std::setprecision(2) << result.metrics.avgLatencyMs << " ms" << std::endl;
            std::cout << "  Max Latency: " << std::fixed << std::setprecision(2) << result.metrics.maxLatencyMs << " ms" << std::endl;
            std::cout << "  Min Latency: " << std::fixed << std::setprecision(2) << result.metrics.minLatencyMs << " ms" << std::endl;
            std::cout << "  Throughput: " << std::fixed << std::setprecision(2) << result.metrics.throughputOpsPerSec << " ops/sec" << std::endl;
        } else {
            std::cout << "  Status: FAILED - " << result.details << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Overall summary
    int totalOps = 0;
    int successOps = 0;
    int failedOps = 0;
    double totalThroughput = 0.0;
    int passedTests = 0;
    
    for (const auto& result : results) {
        if (result.passed) {
            totalOps += result.metrics.totalOperations;
            successOps += result.metrics.successfulOperations;
            failedOps += result.metrics.failedOperations;
            totalThroughput += result.metrics.throughputOpsPerSec;
            passedTests++;
        }
    }
    
    std::cout << "Overall Performance Summary:" << std::endl;
    std::cout << "  Passed Tests: " << passedTests << "/" << results.size() << std::endl;
    std::cout << "  Total Operations: " << totalOps << std::endl;
    std::cout << "  Successful Operations: " << successOps << std::endl;
    std::cout << "  Failed Operations: " << failedOps << std::endl;
    std::cout << "  Average Throughput: " << std::fixed << std::setprecision(2) << (totalThroughput / passedTests) << " ops/sec" << std::endl;
    std::cout << "==============================================" << std::endl;
}

// Main performance test runner
bool runPerformanceTest() {
    std::cout << "==============================================" << std::endl;
    std::cout << "Starting PhantomDB Distributed Performance Test" << std::endl;
    std::cout << "==============================================" << std::endl;
    
    // Performance test configuration
    PerformanceConfig config;
    config.numNodes = 20;
    config.numTransactions = 5000;
    config.numQueries = 5000;
    config.numConcurrentClients = 20;
    config.testDurationSeconds = 60;
    config.dataSizeKB = 1024; // 1MB
    
    // Reset counters
    g_totalOps = 0;
    g_failedOps = 0;
    g_successOps = 0;
    g_totalLatency = 0;
    g_maxLatency = 0.0;
    g_minLatency = DBL_MAX;
    
    // Define performance tests
    std::vector<std::function<TestResult(const PerformanceConfig&)>> testFunctions = {
        testClusterManagementPerformance,
        testConsensusPerformance,
        testDistributedTransactionPerformance,
        testCrossShardQueryPerformance,
        testConcurrentOperationsPerformance
    };
    
    std::vector<std::string> testNames = {
        "Cluster Management Performance",
        "Consensus Performance",
        "Distributed Transaction Performance",
        "Cross-Shard Query Performance",
        "Concurrent Operations Performance"
    };
    
    // Run all performance tests
    std::vector<TestResult> results;
    
    for (size_t i = 0; i < testFunctions.size(); ++i) {
        std::cout << std::endl;
        std::cout << "--- Running Performance Test: " << testNames[i] << " ---" << std::endl;
        
        TestResult result = testFunctions[i](config);
        results.push_back(result);
    }
    
    // Print performance metrics
    printPerformanceMetrics(results);
    
    // Check if all tests passed
    bool allPassed = std::all_of(results.begin(), results.end(), [](const TestResult& r) { return r.passed; });
    
    if (allPassed) {
        std::cout << "🎉 All performance tests passed!" << std::endl;
    } else {
        std::cout << "⚠️  Some performance tests failed. Please review the results above." << std::endl;
    }
    
    return allPassed;
}

int main() {
    // No equivalent to spdlog::set_level in iostream, so we'll just output all messages
    
    try {
        bool success = runPerformanceTest();
        return success ? 0 : 1;
    } catch (const std::exception& e) {
        std::cout << "Exception in performance test: " << e.what() << std::endl;
        return 1;
    }
}