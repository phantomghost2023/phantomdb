#ifndef PHANTOMDB_ENHANCED_PERSISTENCE_H
#define PHANTOMDB_ENHANCED_PERSISTENCE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <memory>
#include <mutex>

namespace phantomdb {
namespace core {

// Forward declaration
struct TableData {
    std::vector<std::pair<std::string, std::string>> columns;
    std::vector<std::unordered_map<std::string, std::string>> rows;
};

class EnhancedPersistenceManager {
public:
    EnhancedPersistenceManager();
    ~EnhancedPersistenceManager() = default;
    
    /**
     * @brief Save database to file with snapshot functionality
     * 
     * @param databaseName The name of the database to save
     * @param tables The table data to save
     * @param filename The file to save to (optional, defaults to database name)
     * @return true if successful, false otherwise
     */
    bool saveDatabase(const std::string& databaseName,
                     const std::unordered_map<std::string, TableData>& tables,
                     const std::string& filename = "");
    
    /**
     * @brief Load database from file
     * 
     * @param databaseName The name of the database to load
     * @param tables The table data to load into
     * @param filename The file to load from (optional, defaults to database name)
     * @return true if successful, false otherwise
     */
    bool loadDatabase(const std::string& databaseName,
                     std::unordered_map<std::string, TableData>& tables,
                     const std::string& filename = "");
    
    /**
     * @brief Append transaction log entry
     * 
     * @param databaseName The name of the database
     * @param operation The operation to log
     * @param data The data associated with the operation
     * @return true if successful, false otherwise
     */
    bool appendTransactionLog(const std::string& databaseName,
                             const std::string& operation,
                             const std::unordered_map<std::string, std::string>& data);
    
    /**
     * @brief Create a snapshot of the database
     * 
     * @param databaseName The name of the database to snapshot
     * @param tables The table data to snapshot
     * @return true if successful, false otherwise
     */
    bool createSnapshot(const std::string& databaseName,
                       const std::unordered_map<std::string, TableData>& tables);
    
    /**
     * @brief Set the data directory for persistence
     * 
     * @param directory The directory to store data files
     */
    void setDataDirectory(const std::string& directory);
    
    /**
     * @brief Get the current data directory
     * 
     * @return The current data directory
     */
    std::string getDataDirectory() const;
    
    /**
     * @brief Enable or disable snapshot functionality
     * 
     * @param enabled Whether to enable snapshots
     */
    void setSnapshotEnabled(bool enabled);
    
    /**
     * @brief Check if snapshot functionality is enabled
     * 
     * @return true if enabled, false otherwise
     */
    bool isSnapshotEnabled() const;
    
    /**
     * @brief Set the snapshot interval (in operations)
     * 
     * @param interval Number of operations between snapshots
     */
    void setSnapshotInterval(size_t interval);
    
    /**
     * @brief Get the snapshot interval
     * 
     * @return The snapshot interval
     */
    size_t getSnapshotInterval() const;

private:
    std::string dataDirectory_;
    bool snapshotEnabled_;
    size_t snapshotInterval_;
    size_t operationCount_;
    mutable std::mutex mutex_;
    
    /**
     * @brief Get the full path for a database file
     */
    std::string getDatabaseFilePath(const std::string& databaseName, const std::string& filename = "") const;
    
    /**
     * @brief Get the full path for a transaction log file
     */
    std::string getTransactionLogPath(const std::string& databaseName) const;
    
    /**
     * @brief Get the full path for a snapshot file
     */
    std::string getSnapshotPath(const std::string& databaseName) const;
    
    /**
     * @brief Escape special characters in a string for CSV
     */
    std::string escapeCSV(const std::string& str) const;
    
    /**
     * @brief Unescape special characters from CSV
     */
    std::string unescapeCSV(const std::string& str) const;
    
    /**
     * @brief Check if a snapshot is needed based on operation count
     */
    bool isSnapshotNeeded() const;
};

} // namespace core
} // namespace phantomdb

#endif // PHANTOMDB_ENHANCED_PERSISTENCE_H