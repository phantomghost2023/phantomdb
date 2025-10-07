#include "database.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <mutex>

namespace phantomdb {
namespace core {

class Database::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    struct Table {
        std::vector<std::pair<std::string, std::string>> columns;
        std::vector<std::unordered_map<std::string, std::string>> rows;
    };

    // Database storage
    std::unordered_map<std::string, std::unordered_map<std::string, Table>> databases;

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
    
    if (condition.empty()) {
        return tableIt->second.rows;
    }

    std::vector<std::unordered_map<std::string, std::string>> results;
    for (const auto& row : tableIt->second.rows) {
        bool match = true;
        for (const auto& cond : condition) {
            auto it = row.find(cond.first);
            if (it == row.end() || it->second != cond.second) {
                match = false;
                break;
            }
        }
        if (match) {
            results.push_back(row);
        }
    }

    return results;
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

    for (auto& row : tableIt->second.rows) {
        bool match = true;
        if (!condition.empty()) {
            for (const auto& cond : condition) {
                auto it = row.find(cond.first);
                if (it == row.end() || it->second != cond.second) {
                    match = false;
                    break;
                }
            }
        }

        if (match) {
            for (const auto& updatePair : data) {
                row[updatePair.first] = updatePair.second;
            }
        }
    }

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

    if (condition.empty()) {
        tableIt->second.rows.clear();
        return true;
    }

    auto& rows = tableIt->second.rows;
    rows.erase(std::remove_if(rows.begin(), rows.end(),
        [&](const auto& row) {
            bool match = true;
            for (const auto& cond : condition) {
                auto it = row.find(cond.first);
                if (it == row.end() || it->second != cond.second) {
                    match = false;
                    break;
                }
            }
            return match;
        }),
        rows.end());

    return true;
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