#ifndef PHANTOMDB_QUERY_EXECUTOR_H
#define PHANTOMDB_QUERY_EXECUTOR_H

#include "database.h"
#include <string>
#include <unordered_map>

// Only include nlohmann/json if available
#ifdef HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

namespace phantomdb {
namespace core {

class QueryExecutor {
public:
    QueryExecutor(Database& db);
    ~QueryExecutor() = default;
    
    // Execute a JSON-based query
    #ifdef HAS_NLOHMANN_JSON
    nlohmann::json executeQuery(const nlohmann::json& query);
    #else
    // Fallback implementation when JSON library is not available
    std::string executeQuery(const std::string& query);
    #endif
    
    // Helper functions for query building
    #ifdef HAS_NLOHMANN_JSON
    nlohmann::json createSelectQuery(const std::string& database, const std::string& table,
                                   const std::unordered_map<std::string, std::string>& conditions = {});
    nlohmann::json createInsertQuery(const std::string& database, const std::string& table,
                                   const std::unordered_map<std::string, std::string>& data);
    nlohmann::json createUpdateQuery(const std::string& database, const std::string& table,
                                   const std::unordered_map<std::string, std::string>& data,
                                   const std::unordered_map<std::string, std::string>& conditions = {});
    nlohmann::json createDeleteQuery(const std::string& database, const std::string& table,
                                   const std::unordered_map<std::string, std::string>& conditions = {});
    #endif

private:
    Database& database_;
    
    // Helper functions
    std::string buildConditionString(const std::unordered_map<std::string, std::string>& conditions);
};

} // namespace core
} // namespace phantomdb

#endif // PHANTOMDB_QUERY_EXECUTOR_H