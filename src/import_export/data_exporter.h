#ifndef PHANTOMDB_DATA_EXPORTER_H
#define PHANTOMDB_DATA_EXPORTER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace phantomdb {
namespace import_export {

// Forward declarations
class DatabaseConnection;

// Export format enumeration
enum class ExportFormat {
    CSV,
    JSON,
    SQLITE
};

// Export options structure
struct ExportOptions {
    bool includeHeader = true;
    char delimiter = ',';
    std::string nullValue = "NULL";
    bool prettyPrint = false;
    int batchSize = 1000;
};

// Export result structure
struct ExportResult {
    bool success = false;
    size_t rowsExported = 0;
    std::string errorMessage;
    std::vector<std::string> warnings;
};

// Data exporter class
class DataExporter {
public:
    DataExporter();
    ~DataExporter();
    
    // Export data to a file
    ExportResult exportToFile(const std::string& filePath,
                             const std::string& databaseName,
                             const std::string& tableName,
                             ExportFormat format,
                             const ExportOptions& options = ExportOptions());
    
    // Export data to a string
    ExportResult exportToString(const std::string& databaseName,
                               const std::string& tableName,
                               ExportFormat format,
                               const ExportOptions& options = ExportOptions());
    
    // Export data to SQLite database
    ExportResult exportToSQLite(const std::string& sqlitePath,
                               const std::string& databaseName,
                               const std::string& tableName,
                               const ExportOptions& options = ExportOptions());
    
    // Set database connection
    void setDatabaseConnection(std::shared_ptr<DatabaseConnection> connection);
    
private:
    // Internal export methods
    ExportResult exportCSV(const std::string& filePath,
                          const std::string& databaseName,
                          const std::string& tableName,
                          const ExportOptions& options);
    
    ExportResult exportJSON(const std::string& filePath,
                           const std::string& databaseName,
                           const std::string& tableName,
                           const ExportOptions& options);
    
    // Helper methods
    std::string escapeCSVField(const std::string& field, char delimiter);
    std::string formatJSONValue(const std::string& value);
    
    std::shared_ptr<DatabaseConnection> dbConnection_;
};

} // namespace import_export
} // namespace phantomdb

#endif // PHANTOMDB_DATA_EXPORTER_H