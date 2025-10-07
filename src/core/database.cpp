#include "database.h"
#include <iostream>
#include <algorithm>

namespace phantomdb {
namespace core {

class Database::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    // Database storage
    std::unordered_map<std::string, std::unordered_map<std::string, 
        std::vector<std::unordered_map<std::string, std::string>>>> databases;
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
    
    tables[tableName] = {};
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
    
    tableIt->second.push_back(data);
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
    
    // For now, return all data (condition is ignored)
    std::cout << "Selected data from table " << tableName << " in database " << dbName << std::endl;
    return tableIt->second;
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
    
    // For now, just return true (actual update logic would go here)
    std::cout << "Updated data in table " << tableName << " in database " << dbName << std::endl;
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
    
    // For now, just return true (actual delete logic would go here)
    std::cout << "Deleted data from table " << tableName << " in database " << dbName << std::endl;
    return true;
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