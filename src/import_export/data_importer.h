#ifndef PHANTOMDB_DATA_IMPORTER_H
#define PHANTOMDB_DATA_IMPORTER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace phantomdb {
namespace import_export {

// Forward declarations
class DatabaseConnection;

// Data format enumeration
enum class DataFormat {
    CSV,
    JSON,
    SQLITE
};

// Import options structure
struct ImportOptions {
    bool hasHeader = true;
    char delimiter = ',';
    std::string nullValue = "NULL";
    bool ignoreDuplicates = false;
    int batchSize = 1000;
};

// Import result structure
struct ImportResult {
    bool success = false;
    size_t rowsImported = 0;
    size_t rowsFailed = 0;
    std::string errorMessage;
    std::vector<std::string> warnings;
};

// Data importer class
class DataImporter {
public:
    DataImporter();
    ~DataImporter();
    
    // Import data from a file
    ImportResult importFromFile(const std::string& filePath, 
                               const std::string& databaseName,
                               const std::string& tableName,
                               DataFormat format,
                               const ImportOptions& options = ImportOptions());
    
    // Import data from a string
    ImportResult importFromString(const std::string& data,
                                 const std::string& databaseName,
                                 const std::string& tableName,
                                 DataFormat format,
                                 const ImportOptions& options = ImportOptions());
    
    // Import data from SQLite database
    ImportResult importFromSQLite(const std::string& sqlitePath,
                                 const std::string& databaseName,
                                 const std::string& tableName,
                                 const ImportOptions& options = ImportOptions());
    
    // Set database connection
    void setDatabaseConnection(std::shared_ptr<DatabaseConnection> connection);
    
private:
    // Internal import methods
    ImportResult importCSV(const std::string& filePath,
                          const std::string& databaseName,
                          const std::string& tableName,
                          const ImportOptions& options);
    
    ImportResult importJSON(const std::string& filePath,
                           const std::string& databaseName,
                           const std::string& tableName,
                           const ImportOptions& options);
    
    // Helper methods
    std::vector<std::string> parseCSVLine(const std::string& line, char delimiter);
    std::string escapeString(const std::string& str);
    
    std::shared_ptr<DatabaseConnection> dbConnection_;
};

} // namespace import_export
} // namespace phantomdb

#endif // PHANTOMDB_DATA_IMPORTER_H