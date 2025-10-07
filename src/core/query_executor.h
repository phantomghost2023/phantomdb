#ifndef PHANTOMDB_QUERY_EXECUTOR_H
#define PHANTOMDB_QUERY_EXECUTOR_H

#include "database.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

namespace phantomdb {
namespace core {

class QueryExecutor {
public:
    QueryExecutor(Database& db);
    ~QueryExecutor() = default;
    
    /**
     * @brief Execute a JSON-based query
     * 
     * @param queryJson The JSON query to execute
     * @return nlohmann::json The result of the query execution
     */
    nlohmann::json executeQuery(const nlohmann::json& queryJson);
    
private:
    Database& database_;
    
    /**
     * @brief Execute a SELECT query
     */
    nlohmann::json executeSelect(const nlohmann::json& query);
    
    /**
     * @brief Execute an INSERT query
     */
    nlohmann::json executeInsert(const nlohmann::json& query);
    
    /**
     * @brief Execute an UPDATE query
     */
    nlohmann::json executeUpdate(const nlohmann::json& query);
    
    /**
     * @brief Execute a DELETE query
     */
    nlohmann::json executeDelete(const nlohmann::json& query);
    
    /**
     * @brief Convert conditions to string format
     */
    std::string conditionsToString(const nlohmann::json& conditions);
    
    /**
     * @brief Build error response
     */
    nlohmann::json buildErrorResponse(const std::string& message);
    
    /**
     * @brief Build success response
     */
    nlohmann::json buildSuccessResponse(const nlohmann::json& data = nlohmann::json::array(), 
                                       int count = 0, 
                                       const std::string& message = "Operation completed successfully");
};

} // namespace core
} // namespace phantomdb

#endif // PHANTOMDB_QUERY_EXECUTOR_H