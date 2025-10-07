#ifndef PHANTOMDB_PERSISTENCE_H
#define PHANTOMDB_PERSISTENCE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

namespace phantomdb {
namespace core {

class PersistenceManager {
public:
    PersistenceManager();
    ~PersistenceManager() = default;
    
    /**
     * @brief Save database to file
     * 
     * @param databaseName The name of the database to save
     * @param tables The table data to save
     * @param filename The file to save to (optional, defaults to database name)
     * @return true if successful, false otherwise
     */
    bool saveDatabase(const std::string& databaseName,
                     const std::unordered_map<std::string, 
                         std::pair<std::vector<std::pair<std::string, std::string>>, 
                                  std::vector<std::unordered_map<std::string, std::string>>>>& tables,
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
                     std::unordered_map<std::string, 
                         std::pair<std::vector<std::pair<std::string, std::string>>, 
                                  std::vector<std::unordered_map<std::string, std::string>>>>& tables,
                     const std::string& filename = "");
    
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
    
private:
    std::string dataDirectory_;
    
    /**
     * @brief Get the full path for a database file
     */
    std::string getDatabaseFilePath(const std::string& databaseName, const std::string& filename = "") const;
    
    /**
     * @brief Escape special characters in a string for CSV
     */
    std::string escapeCSV(const std::string& str) const;
    
    /**
     * @brief Unescape special characters from CSV
     */
    std::string unescapeCSV(const std::string& str) const;
};

} // namespace core
} // namespace phantomdb

#endif // PHANTOMDB_PERSISTENCE_H