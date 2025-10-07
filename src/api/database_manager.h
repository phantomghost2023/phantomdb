#ifndef PHANTOMDB_DATABASE_MANAGER_H
#define PHANTOMDB_DATABASE_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../core/database.h"
#include "../query/query_processor.h"
#include "../transaction/transaction_manager.h"
#include "../observability/observability.h"

namespace phantomdb {
namespace api {

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();
    
    // Database operations
    bool createDatabase(const std::string& dbName);
    bool dropDatabase(const std::string& dbName);
    std::vector<std::string> listDatabases() const;
    
    // Table operations
    bool createTable(const std::string& dbName, const std::string& tableName, 
                     const std::vector<std::pair<std::string, std::string>>& columns);
    bool dropTable(const std::string& dbName, const std::string& tableName);
    std::vector<std::string> listTables(const std::string& dbName) const;
    
    // Data operations
    std::string insertData(const std::string& dbName, const std::string& tableName,
                          const std::unordered_map<std::string, std::string>& data);
    std::vector<std::unordered_map<std::string, std::string>> selectData(
        const std::string& dbName, const std::string& tableName,
        const std::string& condition = "");
    bool updateData(const std::string& dbName, const std::string& tableName,
                   const std::unordered_map<std::string, std::string>& data,
                   const std::string& condition = "");
    bool deleteData(const std::string& dbName, const std::string& tableName,
                   const std::string& condition = "");
    
    // Query execution
    std::string executeQuery(const std::string& dbName, const std::string& query);
    
    // Transaction operations
    std::string beginTransaction();
    bool commitTransaction(const std::string& txnId);
    bool rollbackTransaction(const std::string& txnId);
    
    // Status and health checks
    bool isHealthy() const;
    std::string getStats() const;
    
    // Metrics
    std::string getMetrics() const;
    void recordQuery(const std::string& queryType, double durationMs);

private:
    std::unique_ptr<core::Database> database_;
    std::unique_ptr<query::QueryProcessor> queryProcessor_;
    std::unique_ptr<transaction::TransactionManager> transactionManager_;
    std::shared_ptr<observability::DatabaseMetricsCollector> metricsCollector_;
    
    // Helper methods
    std::string toJson(const std::unordered_map<std::string, std::string>& data) const;
    std::string toJsonArray(const std::vector<std::unordered_map<std::string, std::string>>& data) const;
    std::string createErrorJson(const std::string& message) const;
    std::string createSuccessJson(const std::string& message) const;
};

} // namespace api
} // namespace phantomdb

#endif // PHANTOMDB_DATABASE_MANAGER_H