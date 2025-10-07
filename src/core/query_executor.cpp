#include "query_executor.h"
#include <iostream>
#include <sstream>

namespace phantomdb {
namespace core {

QueryExecutor::QueryExecutor(Database& db) : database_(db) {
}

nlohmann::json QueryExecutor::executeQuery(const nlohmann::json& queryJson) {
    try {
        // Validate required fields
        if (!queryJson.contains("operation")) {
            return buildErrorResponse("Missing required field: operation");
        }
        
        if (!queryJson.contains("database")) {
            return buildErrorResponse("Missing required field: database");
        }
        
        if (!queryJson.contains("table")) {
            return buildErrorResponse("Missing required field: table");
        }
        
        std::string operation = queryJson["operation"];
        
        if (operation == "select") {
            return executeSelect(queryJson);
        } else if (operation == "insert") {
            return executeInsert(queryJson);
        } else if (operation == "update") {
            return executeUpdate(queryJson);
        } else if (operation == "delete") {
            return executeDelete(queryJson);
        } else {
            return buildErrorResponse("Unsupported operation: " + operation);
        }
    } catch (const std::exception& e) {
        return buildErrorResponse("Error executing query: " + std::string(e.what()));
    }
}

nlohmann::json QueryExecutor::executeSelect(const nlohmann::json& query) {
    std::string database = query["database"];
    std::string table = query["table"];
    
    // Build condition string
    std::string condition;
    if (query.contains("conditions")) {
        condition = conditionsToString(query["conditions"]);
    }
    
    // Execute select operation
    auto results = database_.selectData(database, table, condition);
    
    // Convert results to JSON
    nlohmann::json jsonData = nlohmann::json::array();
    for (const auto& row : results) {
        nlohmann::json jsonRow;
        for (const auto& field : row) {
            jsonRow[field.first] = field.second;
        }
        jsonData.push_back(jsonRow);
    }
    
    return buildSuccessResponse(jsonData, static_cast<int>(results.size()));
}

nlohmann::json QueryExecutor::executeInsert(const nlohmann::json& query) {
    std::string database = query["database"];
    std::string table = query["table"];
    
    if (!query.contains("data")) {
        return buildErrorResponse("Missing required field: data");
    }
    
    // Convert JSON data to unordered_map
    std::unordered_map<std::string, std::string> data;
    for (auto it = query["data"].begin(); it != query["data"].end(); ++it) {
        data[it.key()] = it.value().get<std::string>();
    }
    
    // Execute insert operation
    bool success = database_.insertData(database, table, data);
    
    if (success) {
        return buildSuccessResponse(nlohmann::json::array(), 1, "Data inserted successfully");
    } else {
        return buildErrorResponse("Failed to insert data");
    }
}

nlohmann::json QueryExecutor::executeUpdate(const nlohmann::json& query) {
    std::string database = query["database"];
    std::string table = query["table"];
    
    if (!query.contains("data")) {
        return buildErrorResponse("Missing required field: data");
    }
    
    // Convert JSON data to unordered_map
    std::unordered_map<std::string, std::string> data;
    for (auto it = query["data"].begin(); it != query["data"].end(); ++it) {
        data[it.key()] = it.value().get<std::string>();
    }
    
    // Build condition string
    std::string condition;
    if (query.contains("conditions")) {
        condition = conditionsToString(query["conditions"]);
    }
    
    // Execute update operation
    bool success = database_.updateData(database, table, data, condition);
    
    if (success) {
        return buildSuccessResponse(nlohmann::json::array(), 1, "Data updated successfully");
    } else {
        return buildErrorResponse("Failed to update data");
    }
}

nlohmann::json QueryExecutor::executeDelete(const nlohmann::json& query) {
    std::string database = query["database"];
    std::string table = query["table"];
    
    // Build condition string
    std::string condition;
    if (query.contains("conditions")) {
        condition = conditionsToString(query["conditions"]);
    }
    
    // Execute delete operation
    bool success = database_.deleteData(database, table, condition);
    
    if (success) {
        return buildSuccessResponse(nlohmann::json::array(), 1, "Data deleted successfully");
    } else {
        return buildErrorResponse("Failed to delete data");
    }
}

std::string QueryExecutor::conditionsToString(const nlohmann::json& conditions) {
    if (conditions.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    bool first = true;
    
    for (auto it = conditions.begin(); it != conditions.end(); ++it) {
        if (!first) {
            oss << " AND ";
        }
        
        oss << it.key() << " = '" << it.value().get<std::string>() << "'";
        first = false;
    }
    
    return oss.str();
}

nlohmann::json QueryExecutor::buildErrorResponse(const std::string& message) {
    nlohmann::json response;
    response["success"] = false;
    response["error"] = message;
    return response;
}

nlohmann::json QueryExecutor::buildSuccessResponse(const nlohmann::json& data, 
                                                  int count, 
                                                  const std::string& message) {
    nlohmann::json response;
    response["success"] = true;
    response["data"] = data;
    response["count"] = count;
    response["message"] = message;
    return response;
}

} // namespace core
} // namespace phantomdb