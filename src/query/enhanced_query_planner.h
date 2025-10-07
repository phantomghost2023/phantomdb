#ifndef PHANTOMDB_ENHANCED_QUERY_PLANNER_H
#define PHANTOMDB_ENHANCED_QUERY_PLANNER_H

#include "query_planner.h"
#include "../storage/enhanced_index_manager.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace phantomdb {
namespace query {

// Forward declarations
class EnhancedStatisticsManager;
class IndexUsageInfo;

// Enhanced query planner with rule-based optimizations
class EnhancedQueryPlanner {
public:
    EnhancedQueryPlanner();
    ~EnhancedQueryPlanner();
    
    // Initialize the query planner
    bool initialize();
    
    // Shutdown the query planner
    void shutdown();
    
    // Generate an optimized query plan
    std::unique_ptr<PlanNode> generateOptimizedPlan(const ASTNode* ast, std::string& errorMsg);
    
    // Set the index manager for index-aware planning
    void setIndexManager(storage::EnhancedIndexManager* indexManager);
    
    // Set the statistics manager for cost-based optimization
    void setStatisticsManager(EnhancedStatisticsManager* statsManager);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced statistics manager for better cost estimation
class EnhancedStatisticsManager {
public:
    EnhancedStatisticsManager();
    ~EnhancedStatisticsManager();
    
    // Initialize the statistics manager
    bool initialize();
    
    // Shutdown the statistics manager
    void shutdown();
    
    // Get table statistics
    struct TableStats {
        std::string tableName;
        size_t rowCount;
        size_t avgRowSize;
        std::unordered_map<std::string, size_t> columnCardinalities;
        std::unordered_map<std::string, double> columnSelectivities;
        
        TableStats(const std::string& name) : tableName(name), rowCount(0), avgRowSize(0) {}
    };
    
    std::shared_ptr<TableStats> getTableStats(const std::string& tableName);
    
    // Get index statistics
    struct IndexStats {
        std::string indexName;
        std::string tableName;
        std::string columnName;
        storage::IndexType indexType;
        size_t cardinality;
        double selectivity;
        double avgLookupTime;
        
        IndexStats(const std::string& name, const std::string& table, const std::string& column)
            : indexName(name), tableName(table), columnName(column), 
              indexType(storage::IndexType::B_TREE), cardinality(0), selectivity(1.0), avgLookupTime(0.0) {}
    };
    
    std::shared_ptr<IndexStats> getIndexStats(const std::string& indexName);
    
    // Update table statistics
    void updateTableStats(const std::string& tableName, size_t rowCount, size_t avgRowSize);
    
    // Update column statistics
    void updateColumnStats(const std::string& tableName, const std::string& columnName, 
                          size_t cardinality, double selectivity);
    
    // Update index statistics
    void updateIndexStats(const std::string& indexName, const std::string& tableName, 
                         const std::string& columnName, storage::IndexType type,
                         size_t cardinality, double selectivity, double avgLookupTime);
    
    // Estimate selectivity of a condition
    double estimateSelectivity(const std::string& tableName, const std::string& condition);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Information about index usage in a query plan
struct IndexUsageInfo {
    std::string indexName;
    std::string tableName;
    std::string columnName;
    storage::IndexType indexType;
    double estimatedCost;
    double selectivity;
    
    IndexUsageInfo(const std::string& idxName, const std::string& tblName, const std::string& colName)
        : indexName(idxName), tableName(tblName), columnName(colName), 
          indexType(storage::IndexType::B_TREE), estimatedCost(0.0), selectivity(1.0) {}
};

} // namespace query
} // namespace phantomdb

#endif // PHANTOMDB_ENHANCED_QUERY_PLANNER_H