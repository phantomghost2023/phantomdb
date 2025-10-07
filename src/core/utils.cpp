#include "utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace phantomdb {
namespace core {
namespace utils {

// Helper function to validate integer values
bool isValidInteger(const std::string& value) {
    if (value.empty()) return false;
    
    size_t start = 0;
    if (value[0] == '-' || value[0] == '+') {
        if (value.length() == 1) return false;
        start = 1;
    }
    
    for (size_t i = start; i < value.length(); ++i) {
        if (!std::isdigit(value[i])) {
            return false;
        }
    }
    return true;
}

// Helper function to validate floating point values
bool isValidFloat(const std::string& value) {
    if (value.empty()) return false;
    
    // Use regex for floating point validation
    std::regex floatRegex("^[+-]?([0-9]*\\.?[0-9]+|[0-9]+\\.?[0-9]*)([eE][+-]?[0-9]+)?$");
    return std::regex_match(value, floatRegex);
}

// Helper function to validate boolean values
bool isValidBoolean(const std::string& value) {
    std::string lowerValue = value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    
    return (lowerValue == "true" || lowerValue == "false" || 
            lowerValue == "1" || lowerValue == "0" ||
            lowerValue == "yes" || lowerValue == "no" ||
            lowerValue == "on" || lowerValue == "off");
}

// Helper function to validate date values (YYYY-MM-DD format)
bool isValidDate(const std::string& value) {
    std::regex dateRegex("^\\d{4}-\\d{2}-\\d{2}$");
    if (!std::regex_match(value, dateRegex)) {
        return false;
    }
    
    // Additional validation for actual date values
    // This is a simplified validation - in a real implementation, you would check:
    // - Month is between 01-12
    // - Day is valid for the given month and year (considering leap years)
    // - Year is reasonable
    
    return true;
}

// Helper function to validate time values (HH:MM:SS format)
bool isValidTime(const std::string& value) {
    std::regex timeRegex("^\\d{2}:\\d{2}:\\d{2}$");
    if (!std::regex_match(value, timeRegex)) {
        return false;
    }
    
    // Additional validation for actual time values
    // This is a simplified validation - in a real implementation, you would check:
    // - Hours are between 00-23
    // - Minutes are between 00-59
    // - Seconds are between 00-59
    
    return true;
}

// Helper function to validate timestamp values (YYYY-MM-DD HH:MM:SS format)
bool isValidTimestamp(const std::string& value) {
    std::regex timestampRegex("^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}$");
    return std::regex_match(value, timestampRegex);
}

std::unordered_map<std::string, std::string> parseCondition(const std::string& condition) {
    std::unordered_map<std::string, std::string> result;
    
    if (condition.empty()) {
        return result;
    }
    
    // Simple parser for conditions like "field = 'value' AND field2 = 'value2'"
    std::string trimmed = condition;
    
    // Remove extra whitespace
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), trimmed.end());
    
    // Split by AND
    std::vector<std::string> conditions;
    std::string::size_type start = 0;
    std::string::size_type pos = 0;
    
    while ((pos = trimmed.find(" AND ", start)) != std::string::npos) {
        conditions.push_back(trimmed.substr(start, pos - start));
        start = pos + 5; // length of " AND "
    }
    conditions.push_back(trimmed.substr(start));
    
    // Parse each condition
    for (const auto& cond : conditions) {
        // Find the = sign
        std::string::size_type eqPos = cond.find(" = ");
        if (eqPos != std::string::npos) {
            std::string field = cond.substr(0, eqPos);
            std::string value = cond.substr(eqPos + 3);
            
            // Trim whitespace
            field.erase(field.begin(), std::find_if(field.begin(), field.end(), [](int ch) {
                return !std::isspace(ch);
            }));
            field.erase(std::find_if(field.rbegin(), field.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), field.end());
            
            value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](int ch) {
                return !std::isspace(ch);
            }));
            value.erase(std::find_if(value.rbegin(), value.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), value.end());
            
            // Remove quotes if present
            if (value.length() >= 2 && value.front() == '\'' && value.back() == '\'') {
                value = value.substr(1, value.length() - 2);
            }
            
            result[field] = value;
        }
    }
    
    return result;
}

bool matchesCondition(const std::unordered_map<std::string, std::string>& row, 
                     const std::unordered_map<std::string, std::string>& condition) {
    // If no condition, match all rows
    if (condition.empty()) {
        return true;
    }
    
    // Check if all conditions are met
    for (const auto& pair : condition) {
        auto it = row.find(pair.first);
        if (it == row.end() || it->second != pair.second) {
            return false;
        }
    }
    
    return true;
}

bool validateData(const std::unordered_map<std::string, std::string>& data,
                 const std::unordered_map<std::string, std::string>& columnDefinitions,
                 std::string& error) {
    
    // Check for required columns
    for (const auto& column : columnDefinitions) {
        // For simplicity, we're not enforcing required fields in this implementation
        // In a real implementation, you might have nullable/non-nullable columns
    }
    
    // Validate data types
    for (const auto& pair : data) {
        const std::string& fieldName = pair.first;
        const std::string& value = pair.second;
        
        // Check if field exists in schema
        auto it = columnDefinitions.find(fieldName);
        if (it == columnDefinitions.end()) {
            error = "Field '" + fieldName + "' does not exist in table schema";
            return false;
        }
        
        // Validate value type
        const std::string& expectedType = it->second;
        if (!validateValueType(value, expectedType)) {
            error = "Value '" + value + "' for field '" + fieldName + 
                    "' does not match expected type '" + expectedType + "'";
            return false;
        }
    }
    
    return true;
}

bool validateValueType(const std::string& value, const std::string& type) {
    // Convert type to lowercase for case-insensitive comparison
    std::string lowerType = type;
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    
    // String types - everything can be a string
    if (lowerType == "string" || lowerType == "text" || lowerType == "varchar" || 
        lowerType == "char" || lowerType == "nvarchar") {
        return true;
    }
    
    // Integer types
    if (lowerType == "integer" || lowerType == "int" || lowerType == "bigint" || 
        lowerType == "smallint" || lowerType == "tinyint") {
        return isValidInteger(value);
    }
    
    // Floating point types
    if (lowerType == "float" || lowerType == "double" || lowerType == "real" || 
        lowerType == "decimal" || lowerType == "numeric") {
        return isValidFloat(value);
    }
    
    // Boolean types
    if (lowerType == "boolean" || lowerType == "bool") {
        return isValidBoolean(value);
    }
    
    // Date types
    if (lowerType == "date") {
        return isValidDate(value);
    }
    
    // Time types
    if (lowerType == "time") {
        return isValidTime(value);
    }
    
    // Timestamp types
    if (lowerType == "timestamp" || lowerType == "datetime") {
        return isValidTimestamp(value);
    }
    
    // For unknown types, accept any string value for now
    return true;
}

} // namespace utils
} // namespace core
} // namespace phantomdb