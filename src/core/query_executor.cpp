#include "query_executor.h"
#include "utils.h"
#include <iostream>
#include <sstream>

#ifdef HAS_NLOHMANN_JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#endif

namespace phantomdb {
namespace core {

QueryExecutor::QueryExecutor(Database& db) : database_(db) {
}

#ifdef HAS_NLOHMANN_JSON
json QueryExecutor::executeQuery(const json& query) {
    try {
        std::string operation = query["operation"];
        
        if (operation == "select") {
            std::string database = query["database"];
            std::string table = query["table"];
            
            std::unordered_map<std::string, std::string> conditions;
            if (query.contains("conditions")) {
                for (auto& [key, value] : query["conditions"].items()) {
                    conditions[key] = value;
                }
            }
            
            auto results = database_.selectData(database, table, buildConditionString(conditions));
            
            json response;
            response["success"] = true;
            response["data"] = results;
            response["count"] = results.size();
            
            return response;
        }
        else if (operation == "insert") {
            std::string database = query["database"];
            std::string table = query["table"];
            
            std::unordered_map<std::string, std::string> data;
            for (auto& [key, value] : query["data"].items()) {
                data[key] = value;
            }
            
            bool success = database_.insertData(database, table, data);
            
            json response;
            response["success"] = success;
            response["message"] = success ? "Data inserted successfully" : "Failed to insert data";
            
            return response;
        }
        else if (operation == "update") {
            std::string database = query["database"];
            std::string table = query["table"];
            
            std::unordered_map<std::string, std::string> data;
            for (auto& [key, value] : query["data"].items()) {
                data[key] = value;
            }
            
            std::unordered_map<std::string, std::string> conditions;
            if (query.contains("conditions")) {
                for (auto& [key, value] : query["conditions"].items()) {
                    conditions[key] = value;
                }
            }
            
            bool success = database_.updateData(database, table, data, buildConditionString(conditions));
            
            json response;
            response["success"] = success;
            response["message"] = success ? "Data updated successfully" : "Failed to update data";
            
            return response;
        }
        else if (operation == "delete") {
            std::string database = query["database"];
            std::string table = query["table"];
            
            std::unordered_map<std::string, std::string> conditions;
            if (query.contains("conditions")) {
                for (auto& [key, value] : query["conditions"].items()) {
                    conditions[key] = value;
                }
            }
            
            bool success = database_.deleteData(database, table, buildConditionString(conditions));
            
            json response;
            response["success"] = success;
            response["message"] = success ? "Data deleted successfully" : "Failed to delete data";
            
            return response;
        }
        else {
            json response;
            response["success"] = false;
            response["error"] = "Unknown operation: " + operation;
            return response;
        }
    } catch (const std::exception& e) {
        json response;
        response["success"] = false;
        response["error"] = std::string("Exception: ") + e.what();
        return response;
    }
}
#else
std::string QueryExecutor::executeQuery(const std::string& query) {
    // Simple fallback implementation without JSON support
    return "{\"success\": true, \"message\": \"JSON support not available\"}";
}
#endif

#ifdef HAS_NLOHMANN_JSON
json QueryExecutor::createSelectQuery(const std::string& database, const std::string& table,
                                    const std::unordered_map<std::string, std::string>& conditions) {
    json query;
    query["operation"] = "select";
    query["database"] = database;
    query["table"] = table;
    
    if (!conditions.empty()) {
        query["conditions"] = json::object();
        for (const auto& pair : conditions) {
            query["conditions"][pair.first] = pair.second;
        }
    }
    
    return query;
}

json QueryExecutor::createInsertQuery(const std::string& database, const std::string& table,
                                    const std::unordered_map<std::string, std::string>& data) {
    json query;
    query["operation"] = "insert";
    query["database"] = database;
    query["table"] = table;
    query["data"] = json::object();
    
    for (const auto& pair : data) {
        query["data"][pair.first] = pair.second;
    }
    
    return query;
}

json QueryExecutor::createUpdateQuery(const std::string& database, const std::string& table,
                                    const std::unordered_map<std::string, std::string>& data,
                                    const std::unordered_map<std::string, std::string>& conditions) {
    json query;
    query["operation"] = "update";
    query["database"] = database;
    query["table"] = table;
    query["data"] = json::object();
    
    for (const auto& pair : data) {
        query["data"][pair.first] = pair.second;
    }
    
    if (!conditions.empty()) {
        query["conditions"] = json::object();
        for (const auto& pair : conditions) {
            query["conditions"][pair.first] = pair.second;
        }
    }
    
    return query;
}

json QueryExecutor::createDeleteQuery(const std::string& database, const std::string& table,
                                    const std::unordered_map<std::string, std::string>& conditions) {
    json query;
    query["operation"] = "delete";
    query["database"] = database;
    query["table"] = table;
    
    if (!conditions.empty()) {
        query["conditions"] = json::object();
        for (const auto& pair : conditions) {
            query["conditions"][pair.first] = pair.second;
        }
    }
    
    return query;
}
#endif

std::string QueryExecutor::buildConditionString(const std::unordered_map<std::string, std::string>& conditions) {
    if (conditions.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& pair : conditions) {
        if (!first) {
            oss << " AND ";
        }
        
        // Escape single quotes in values
        std::string escapedValue = pair.second;
        size_t pos = 0;
        while ((pos = escapedValue.find("'", pos)) != std::string::npos) {
            escapedValue.replace(pos, 1, "''");
            pos += 2;
        }
        
        oss << pair.first << " = '" << escapedValue << "'";
        first = false;
    }
    
    return oss.str();
}

} // namespace core
} // namespace phantomdb