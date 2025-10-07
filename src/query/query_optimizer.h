#ifndef PHANTOMDB_QUERY_OPTIMIZER_H
#define PHANTOMDB_QUERY_OPTIMIZER_H

#include "query_planner.h"
#include <string>
#include <memory>
#include <vector>

namespace phantomdb {
namespace query {

// Forward declarations
class PlanNode;
class TableStats;
class IndexStats;

// Statistics manager class
class StatisticsManager {
public:
    StatisticsManager();
    ~StatisticsManager();
    
    // Initialize the statistics manager
    bool initialize();
    
    // Shutdown the statistics manager
    void shutdown();
    
    // Get table statistics
    std::shared_ptr<TableStats> getTableStats(const std::string& tableName);
    
    // Get index statistics
    std::shared_ptr<IndexStats> getIndexStats(const std::string& indexName);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Table statistics
class TableStats {
public:
    TableStats(const std::string& name, size_t rowCount, size_t avgRowSize);
    ~TableStats() = default;
    
    const std::string& getName() const;
    size_t getRowCount() const;
    size_t getAvgRowSize() const;
    
private:
    std::string name_;
    size_t rowCount_;
    size_t avgRowSize_;
};

// Index statistics
class IndexStats {
public:
    IndexStats(const std::string& name, const std::string& tableName, size_t cardinality);
    ~IndexStats() = default;
    
    const std::string& getName() const;
    const std::string& getTableName() const;
    size_t getCardinality() const;
    
private:
    std::string name_;
    std::string tableName_;
    size_t cardinality_;
};

// Rule-based optimizer class
class RuleBasedOptimizer {
public:
    RuleBasedOptimizer();
    ~RuleBasedOptimizer();
    
    // Initialize the rule-based optimizer
    bool initialize();
    
    // Shutdown the rule-based optimizer
    void shutdown();
    
    // Apply optimization rules to a plan
    std::unique_ptr<PlanNode> optimize(std::unique_ptr<PlanNode> plan, std::string& errorMsg);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Cost-based optimizer class
class CostBasedOptimizer {
public:
    CostBasedOptimizer(std::shared_ptr<StatisticsManager> statsManager);
    ~CostBasedOptimizer();
    
    // Initialize the cost-based optimizer
    bool initialize();
    
    // Shutdown the cost-based optimizer
    void shutdown();
    
    // Estimate the cost of a plan
    double estimateCost(const PlanNode* plan);
    
    // Optimize a plan based on cost
    std::unique_ptr<PlanNode> optimize(std::unique_ptr<PlanNode> plan, std::string& errorMsg);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Query optimizer class
class QueryOptimizer {
public:
    QueryOptimizer();
    ~QueryOptimizer();
    
    // Initialize the query optimizer
    bool initialize();
    
    // Shutdown the query optimizer
    void shutdown();
    
    // Optimize a plan using both rule-based and cost-based optimization
    std::unique_ptr<PlanNode> optimize(std::unique_ptr<PlanNode> plan, std::string& errorMsg);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace query
} // namespace phantomdb

#endif // PHANTOMDB_QUERY_OPTIMIZER_H