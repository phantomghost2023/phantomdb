#include "database.h"
#include "utils.h"
#include "enhanced_persistence.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <mutex>

namespace phantomdb {
namespace core {

class Database::Impl {
public:
    Impl() : persistenceManager(std::make_unique<EnhancedPersistenceManager>()) {}
    ~Impl() = default;
    
    struct Table {
        std::vector<std::pair<std::string, std::string>> columns;
        std::vector<std::unordered_map<std::string, std::string>> rows;
    };

    // Database storage
    std::unordered_map<std::string, std::unordered_map<std::string, Table>> databases;
    
    std::unique_ptr<EnhancedPersistenceManager> persistenceManager;
    
    // Concurrency control
    mutable std::mutex db_mutex;
};

Database::Database() : pImpl(std::make_unique<Impl>()) {
    std::cout << "PhantomDB Database initialized" << std::endl;
}

Database::~Database() {
    std::cout << "PhantomDB Database shutdown" << std::endl;
}

bool Database::createDatabase(const std::string& dbName) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    if (pImpl->databases.find(dbName) != pImpl->databases.end()) {
        std::cout << "Database " << dbName << " already exists" << std::endl;
        return false;
    }
    
    pImpl->databases[dbName] = {};
    std::cout << "Created database " << dbName << std::endl;
    
    // Log the operation
    std::unordered_map<std::string, std::string> logData = {{"database", dbName}};
    pImpl->persistenceManager->appendTransactionLog(dbName, "CREATE_DATABASE", logData);
    
    return true;
}

bool Database::dropDatabase(const std::string& dbName) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto it = pImpl->databases.find(dbName);
    if (it == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    pImpl->databases.erase(it);
    std::cout << "Dropped database " << dbName << std::endl;
    
    // Log the operation
    std::unordered_map<std::string, std::string> logData = {{"database", dbName}};
    pImpl->persistenceManager->appendTransactionLog(dbName, "DROP_DATABASE", logData);
    
    return true;
}

std::vector<std::string> Database::listDatabases() const {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    std::vector<std::string> result;
    result.reserve(pImpl->databases.size());
    
    for (const auto& pair : pImpl->databases) {
        result.push_back(pair.first);
    }
    
    return result;
}

bool Database::createTable(const std::string& dbName, const std::string& tableName, 
                          const std::vector<std::pair<std::string, std::string>>& columns) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    auto& tables = dbIt->second;
    if (tables.find(tableName) != tables.end()) {
        std::cout << "Table " << tableName << " already exists in database " << dbName << std::endl;
        return false;
    }
    
    tables[tableName] = {columns, {}};
    std::cout << "Created table " << tableName << " in database " << dbName << std::endl;
    
    // Log the operation
    std::unordered_map<std::string, std::string> logData = {
        {"database", dbName},
        {"table", tableName}
    };
    pImpl->persistenceManager->appendTransactionLog(dbName, "CREATE_TABLE", logData);
    
    return true;
}

bool Database::dropTable(const std::string& dbName, const std::string& tableName) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    auto& tables = dbIt->second;
    auto tableIt = tables.find(tableName);
    if (tableIt == tables.end()) {
        std::cout << "Table " << tableName << " not found in database " << dbName << std::endl;
        return false;
    }
    
    tables.erase(tableIt);
    std::cout << "Dropped table " << tableName << " from database " << dbName << std::endl;
    
    // Log the operation
    std::unordered_map<std::string, std::string> logData = {
        {"database", dbName},
        {"table", tableName}
    };
    pImpl->persistenceManager->appendTransactionLog(dbName, "DROP_TABLE", logData);
    
    return true;
}

std::vector<std::string> Database::listTables(const std::string& dbName) const {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return {};
    }
    
    std::vector<std::string> result;
    result.reserve(dbIt->second.size());
    
    for (const auto& pair : dbIt->second) {
        result.push_back(pair.first);
    }
    
    return result;
}

std::vector<std::pair<std::string, std::string>> Database::getTableSchema(const std::string& dbName, const std::string& tableName) const {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return {};
    }
    
    auto& tables = dbIt->second;
    auto tableIt = tables.find(tableName);
    if (tableIt == tables.end()) {
        std::cout << "Table " << tableName << " not found in database " << dbName << std::endl;
        return {};
    }
    
    return tableIt->second.columns; // Return column definitions
}

bool Database::insertData(const std::string& dbName, const std::string& tableName,
                         const std::unordered_map<std::string, std::string>& data) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    auto& tables = dbIt->second;
    auto tableIt = tables.find(tableName);
    if (tableIt == tables.end()) {
        std::cout << "Table " << tableName << " not found in database " << dbName << std::endl;
        return false;
    }
    
    // Get column definitions for schema validation
    const auto& columnDefinitions = tableIt->second.columns;
    
    // Validate data against schema
    std::string validationError;
    if (!columnDefinitions.empty() && !utils::validateData(data, 
        std::unordered_map<std::string, std::string>(columnDefinitions.begin(), columnDefinitions.end()), 
        validationError)) {
        std::cout << "Data validation failed: " << validationError << std::endl;
        return false;
    }
    
    tableIt->second.rows.push_back(data);
    std::cout << "Inserted data into table " << tableName << " in database " << dbName << std::endl;
    
    // Log the operation
    std::unordered_map<std::string, std::string> logData = {
        {"database", dbName},
        {"table", tableName}
    };
    // Add data fields to log
    for (const auto& pair : data) {
        logData[pair.first] = pair.second;
    }
    pImpl->persistenceManager->appendTransactionLog(dbName, "INSERT", logData);
    
    return true;
}

std::vector<std::unordered_map<std::string, std::string>> Database::selectData(
    const std::string& dbName, const std::string& tableName,
    const std::unordered_map<std::string, std::string>& condition) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return {};
    }
    
    auto& tables = dbIt->second;
    auto tableIt = tables.find(tableName);
    if (tableIt == tables.end()) {
        std::cout << "Table " << tableName << " not found in database " << dbName << std::endl;
        return {};
    }
    
    // For now, we'll treat the condition map as a simple key-value filter
    // In a more advanced implementation, we would parse a condition string
    
    // Filter data based on condition
    std::vector<std::unordered_map<std::string, std::string>> result;
    const auto& allData = tableIt->second.rows;
    
    for (const auto& row : allData) {
        bool matches = true;
        for (const auto& cond : condition) {
            auto it = row.find(cond.first);
            if (it == row.end() || it->second != cond.second) {
                matches = false;
                break;
            }
        }
        if (matches) {
            result.push_back(row);
        }
    }
    
    std::cout << "Selected " << result.size() << " rows from table " << tableName 
              << " in database " << dbName << std::endl;
    
    // Log the operation
    std::unordered_map<std::string, std::string> logData = {
        {"database", dbName},
        {"table", tableName},
        {"result_count", std::to_string(result.size())}
    };
    pImpl->persistenceManager->appendTransactionLog(dbName, "SELECT", logData);
    
    return result;
}

bool Database::updateData(const std::string& dbName, const std::string& tableName,
                         const std::unordered_map<std::string, std::string>& data,
                         const std::unordered_map<std::string, std::string>& condition) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    auto& tables = dbIt->second;
    auto tableIt = tables.find(tableName);
    if (tableIt == tables.end()) {
        std::cout << "Table " << tableName << " not found in database " << dbName << std::endl;
        return false;
    }
    
    // Get column definitions for schema validation
    const auto& columnDefinitions = tableIt->second.columns;
    
    // Validate update data against schema
    std::string validationError;
    if (!columnDefinitions.empty() && !utils::validateData(data, 
        std::unordered_map<std::string, std::string>(columnDefinitions.begin(), columnDefinitions.end()), 
        validationError)) {
        std::cout << "Update data validation failed: " << validationError << std::endl;
        return false;
    }
    
    // For now, we'll treat the condition map as a simple key-value filter
    // In a more advanced implementation, we would parse a condition string
    
    // Update matching rows
    auto& tableData = tableIt->second.rows;
    int updatedRows = 0;
    
    for (auto& row : tableData) {
        bool matches = true;
        if (!condition.empty()) {
            for (const auto& cond : condition) {
                auto it = row.find(cond.first);
                if (it == row.end() || it->second != cond.second) {
                    matches = false;
                    break;
                }
            }
        }
        
        if (matches) {
            // Update the row with new data
            for (const auto& pair : data) {
                row[pair.first] = pair.second;
            }
            updatedRows++;
        }
    }
    
    std::cout << "Updated " << updatedRows << " rows in table " << tableName 
              << " in database " << dbName << std::endl;
    
    // Log the operation
    std::unordered_map<std::string, std::string> logData = {
        {"database", dbName},
        {"table", tableName},
        {"updated_rows", std::to_string(updatedRows)}
    };
    pImpl->persistenceManager->appendTransactionLog(dbName, "UPDATE", logData);
    
    return true;
}

bool Database::deleteData(const std::string& dbName, const std::string& tableName,
                         const std::unordered_map<std::string, std::string>& condition) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    auto& tables = dbIt->second;
    auto tableIt = tables.find(tableName);
    if (tableIt == tables.end()) {
        std::cout << "Table " << tableName << " not found in database " << dbName << std::endl;
        return false;
    }
    
    // For now, we'll treat the condition map as a simple key-value filter
    // In a more advanced implementation, we would parse a condition string
    
    // Remove matching rows
    auto& tableData = tableIt->second.rows;
    auto newEnd = std::remove_if(tableData.begin(), tableData.end(), 
        [&condition](const std::unordered_map<std::string, std::string>& row) {
            if (condition.empty()) return false; // Don't remove if no condition
            for (const auto& cond : condition) {
                auto it = row.find(cond.first);
                if (it == row.end() || it->second != cond.second) {
                    return false; // Don't remove if condition doesn't match
                }
            }
            return true; // Remove if all conditions match
        });
    
    int deletedRows = std::distance(newEnd, tableData.end());
    tableData.erase(newEnd, tableData.end());
    
    std::cout << "Deleted " << deletedRows << " rows from table " << tableName 
              << " in database " << dbName << std::endl;
    
    // Log the operation
    std::unordered_map<std::string, std::string> logData = {
        {"database", dbName},
        {"table", tableName},
        {"deleted_rows", std::to_string(deletedRows)}
    };
    pImpl->persistenceManager->appendTransactionLog(dbName, "DELETE", logData);
    
    return true;
}

bool Database::saveToDisk(const std::string& dbName, const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    // Convert to the format expected by EnhancedPersistenceManager
    std::unordered_map<std::string, TableData> tables;
    for (const auto& tablePair : dbIt->second) {
        TableData tableData;
        tableData.columns = tablePair.second.columns;
        tableData.rows = tablePair.second.rows;
        tables[tablePair.first] = tableData;
    }
    
    return pImpl->persistenceManager->saveDatabase(dbName, tables, filename);
}

bool Database::loadFromDisk(const std::string& dbName, const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto& tables = pImpl->databases[dbName]; // Create entry if doesn't exist
    
    // Load using EnhancedPersistenceManager
    std::unordered_map<std::string, TableData> loadedTables;
    bool result = pImpl->persistenceManager->loadDatabase(dbName, loadedTables, filename);
    
    if (result) {
        // Convert back to internal format
        for (const auto& tablePair : loadedTables) {
            Impl::Table table;
            table.columns = tablePair.second.columns;
            table.rows = tablePair.second.rows;
            tables[tablePair.first] = table;
        }
    }
    
    return result;
}

bool Database::appendTransactionLog(const std::string& dbName, const std::string& operation,
                                   const std::unordered_map<std::string, std::string>& data) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    return pImpl->persistenceManager->appendTransactionLog(dbName, operation, data);
}

bool Database::createSnapshot(const std::string& dbName) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    // Convert to the format expected by EnhancedPersistenceManager
    std::unordered_map<std::string, TableData> tables;
    for (const auto& tablePair : dbIt->second) {
        TableData tableData;
        tableData.columns = tablePair.second.columns;
        tableData.rows = tablePair.second.rows;
        tables[tablePair.first] = tableData;
    }
    
    return pImpl->persistenceManager->createSnapshot(dbName, tables);
}

void Database::setDataDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    pImpl->persistenceManager->setDataDirectory(directory);
}

std::string Database::getDataDirectory() const {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    return pImpl->persistenceManager->getDataDirectory();
}

void Database::setSnapshotEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    pImpl->persistenceManager->setSnapshotEnabled(enabled);
}

bool Database::isSnapshotEnabled() const {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    return pImpl->persistenceManager->isSnapshotEnabled();
}

void Database::setSnapshotInterval(size_t interval) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    pImpl->persistenceManager->setSnapshotInterval(interval);
}

size_t Database::getSnapshotInterval() const {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    return pImpl->persistenceManager->getSnapshotInterval();
}

bool Database::isHealthy() const {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    return true;
}

std::string Database::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    return "Database is healthy";
}

} // namespace core
} // namespace phantomdb