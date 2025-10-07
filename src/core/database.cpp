#include "database.h"
#include "utils.h"
#include "persistence.h"
#include <iostream>
#include <algorithm>

namespace phantomdb {
namespace core {

class Database::Impl {
public:
    Impl() : persistenceManager(std::make_unique<PersistenceManager>()) {}
    ~Impl() = default;
    
    // Database storage
    // Structure: database_name -> table_name -> { column_definitions, table_data }
    std::unordered_map<std::string, 
        std::unordered_map<std::string, 
            std::pair<std::vector<std::pair<std::string, std::string>>, 
                     std::vector<std::unordered_map<std::string, std::string>>>>> databases;
    
    std::unique_ptr<PersistenceManager> persistenceManager;
};

Database::Database() : pImpl(std::make_unique<Impl>()) {
    std::cout << "PhantomDB Database initialized" << std::endl;
}

Database::~Database() {
    std::cout << "PhantomDB Database shutdown" << std::endl;
}

bool Database::createDatabase(const std::string& dbName) {
    if (pImpl->databases.find(dbName) != pImpl->databases.end()) {
        std::cout << "Database " << dbName << " already exists" << std::endl;
        return false;
    }
    
    pImpl->databases[dbName] = {};
    std::cout << "Created database " << dbName << std::endl;
    return true;
}

bool Database::dropDatabase(const std::string& dbName) {
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
    std::vector<std::string> result;
    result.reserve(pImpl->databases.size());
    
    for (const auto& pair : pImpl->databases) {
        result.push_back(pair.first);
    }
    
    return result;
}

bool Database::createTable(const std::string& dbName, const std::string& tableName, 
                          const std::vector<std::pair<std::string, std::string>>& columns) {
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
    
    // Store both column definitions and empty data
    tables[tableName] = std::make_pair(columns, std::vector<std::unordered_map<std::string, std::string>>{});
    std::cout << "Created table " << tableName << " in database " << dbName << std::endl;
    return true;
}

bool Database::dropTable(const std::string& dbName, const std::string& tableName) {
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
    
    return tableIt->second.first; // Return column definitions
}

bool Database::insertData(const std::string& dbName, const std::string& tableName,
                         const std::unordered_map<std::string, std::string>& data) {
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
    const auto& columnDefinitions = tableIt->second.first;
    
    // Validate data against schema
    std::string validationError;
    if (!columnDefinitions.empty() && !utils::validateData(data, 
        std::unordered_map<std::string, std::string>(columnDefinitions.begin(), columnDefinitions.end()), 
        validationError)) {
        std::cout << "Data validation failed: " << validationError << std::endl;
        return false;
    }
    
    tableIt->second.second.push_back(data); // Add to data vector
    std::cout << "Inserted data into table " << tableName << " in database " << dbName << std::endl;
    return true;
}

std::vector<std::unordered_map<std::string, std::string>> Database::selectData(
    const std::string& dbName, const std::string& tableName,
    const std::string& condition) {
    
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
    const auto& allData = tableIt->second.second;
    
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
                         const std::string& condition) {
    
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
    const auto& columnDefinitions = tableIt->second.first;
    
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
    auto& tableData = tableIt->second.second;
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
                         const std::string& condition) {
    
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
    auto& tableData = tableIt->second.second;
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
    auto dbIt = pImpl->databases.find(dbName);
    if (dbIt == pImpl->databases.end()) {
        std::cout << "Database " << dbName << " not found" << std::endl;
        return false;
    }
    
    return pImpl->persistenceManager->saveDatabase(dbName, dbIt->second, filename);
}

bool Database::loadFromDisk(const std::string& dbName, const std::string& filename) {
    auto& tables = pImpl->databases[dbName]; // Create entry if doesn't exist
    return pImpl->persistenceManager->loadDatabase(dbName, tables, filename);
}

bool Database::isHealthy() const {
    // For now, always return true
    return true;
}

std::string Database::getStats() const {
    // For now, return a simple stats string
    return "Database is healthy";
}

} // namespace core
} // namespace phantomdb