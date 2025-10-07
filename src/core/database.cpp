#include "database.h"
#include "utils.h"
#include "persistence.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <mutex>

namespace phantomdb {
namespace core {

class Database::Impl {
public:
    Impl() : persistenceManager(std::make_unique<PersistenceManager>()) {}
    ~Impl() = default;
    
    struct Table {
        std::vector<std::pair<std::string, std::string>> columns;
        std::vector<std::unordered_map<std::string, std::string>> rows;
    };

    // Database storage
    std::unordered_map<std::string, std::unordered_map<std::string, Table>> databases;
    
    std::unique_ptr<PersistenceManager> persistenceManager;
    
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
    
    // Parse condition
    auto conditionMap = utils::parseCondition(condition);
    
    // Filter data based on condition
    std::vector<std::unordered_map<std::string, std::string>> result;
    const auto& allData = tableIt->second.rows;
    
    for (const auto& row : allData) {
        if (utils::matchesCondition(row, conditionMap)) {
            result.push_back(row);
        }
    }
    
    std::cout << "Selected " << result.size() << " rows from table " << tableName 
              << " in database " << dbName << std::endl;
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
    
    // Parse condition
    auto conditionMap = utils::parseCondition(condition);
    
    // Update matching rows
    auto& tableData = tableIt->second.rows;
    int updatedRows = 0;
    
    for (auto& row : tableData) {
        if (utils::matchesCondition(row, conditionMap)) {
            // Update the row with new data
            for (const auto& pair : data) {
                row[pair.first] = pair.second;
            }
            updatedRows++;
        }
    }
    
    std::cout << "Updated " << updatedRows << " rows in table " << tableName 
              << " in database " << dbName << std::endl;
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
    
    // Parse condition
    auto conditionMap = utils::parseCondition(condition);
    
    // Remove matching rows
    auto& tableData = tableIt->second.rows;
    auto newEnd = std::remove_if(tableData.begin(), tableData.end(), 
        [&conditionMap](const std::unordered_map<std::string, std::string>& row) {
            return utils::matchesCondition(row, conditionMap);
        });
    
    int deletedRows = std::distance(newEnd, tableData.end());
    tableData.erase(newEnd, tableData.end());
    
    std::cout << "Deleted " << deletedRows << " rows from table " << tableName 
              << " in database " << dbName << std::endl;
    return true;
}

bool Database::saveToDisk(const std::string& dbName, const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    return pImpl->persistenceManager->saveDatabase(dbName, dbIt->second, filename);
}

bool Database::loadFromDisk(const std::string& dbName, const std::string& filename) {
    std::lock_guard<std::mutex> lock(pImpl->db_mutex);
    auto& tables = pImpl->databases[dbName]; // Create entry if doesn't exist
    return pImpl->persistenceManager->loadDatabase(dbName, tables, filename);
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