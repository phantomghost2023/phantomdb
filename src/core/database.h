#ifndef PHANTOMDB_DATABASE_H
#define PHANTOMDB_DATABASE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace phantomdb {
namespace core {

// Forward declaration
class EnhancedPersistenceManager;

class Database {
public:
    Database();
    ~Database();
    
    // Database operations
    bool createDatabase(const std::string& dbName);
    bool dropDatabase(const std::string& dbName);
    std::vector<std::string> listDatabases() const;
    
    // Table operations
    bool createTable(const std::string& dbName, const std::string& tableName, 
                     const std::vector<std::pair<std::string, std::string>>& columns);
    bool dropTable(const std::string& dbName, const std::string& tableName);
    std::vector<std::string> listTables(const std::string& dbName) const;
    std::vector<std::pair<std::string, std::string>> getTableSchema(const std::string& dbName, const std::string& tableName) const;
    
    // Data operations
    bool insertData(const std::string& dbName, const std::string& tableName,
                   const std::unordered_map<std::string, std::string>& data);
    std::vector<std::unordered_map<std::string, std::string>> selectData(
        const std::string& dbName, const std::string& tableName,
        const std::unordered_map<std::string, std::string>& condition = {});
    bool updateData(const std::string& dbName, const std::string& tableName,
                   const std::unordered_map<std::string, std::string>& data,
                   const std::unordered_map<std::string, std::string>& condition = {});
    bool deleteData(const std::string& dbName, const std::string& tableName,
                   const std::unordered_map<std::string, std::string>& condition = {});
    
    // Persistence operations
    bool saveToDisk(const std::string& dbName, const std::string& filename = "");
    bool loadFromDisk(const std::string& dbName, const std::string& filename = "");
    
    // Enhanced persistence operations
    bool appendTransactionLog(const std::string& dbName, const std::string& operation,
                             const std::unordered_map<std::string, std::string>& data);
    bool createSnapshot(const std::string& dbName);
    
    // Configuration
    void setDataDirectory(const std::string& directory);
    std::string getDataDirectory() const;
    void setSnapshotEnabled(bool enabled);
    bool isSnapshotEnabled() const;
    void setSnapshotInterval(size_t interval);
    size_t getSnapshotInterval() const;
    
    // Status and health checks
    bool isHealthy() const;
    std::string getStats() const;

private:
    // Private implementation details
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace core
} // namespace phantomdb

#endif // PHANTOMDB_DATABASE_H