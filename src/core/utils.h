#ifndef PHANTOMDB_UTILS_H
#define PHANTOMDB_UTILS_H

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace phantomdb {
namespace core {
namespace utils {

/**
 * @brief Parse a simple condition string into key-value pairs
 * 
 * This function parses simple conditions like "id = '1' AND name = 'John'"
 * into a map of field names to expected values.
 * 
 * @param condition The condition string to parse
 * @return A map of field names to expected values
 */
std::unordered_map<std::string, std::string> parseCondition(const std::string& condition);

/**
 * @brief Check if a row matches the given condition
 * 
 * @param row The row to check
 * @param condition The parsed condition map
 * @return true if the row matches the condition, false otherwise
 */
bool matchesCondition(const std::unordered_map<std::string, std::string>& row, 
                     const std::unordered_map<std::string, std::string>& condition);

/**
 * @brief Validate data against column definitions
 * 
 * @param data The data to validate
 * @param columnDefinitions The column definitions (name -> type)
 * @param error The error message if validation fails
 * @return true if data is valid, false otherwise
 */
bool validateData(const std::unordered_map<std::string, std::string>& data,
                 const std::unordered_map<std::string, std::string>& columnDefinitions,
                 std::string& error);

/**
 * @brief Validate a single value against a column type
 * 
 * @param value The value to validate
 * @param type The expected type
 * @return true if value matches type, false otherwise
 */
bool validateValueType(const std::string& value, const std::string& type);

// Helper functions for type validation
bool isValidInteger(const std::string& value);
bool isValidFloat(const std::string& value);
bool isValidBoolean(const std::string& value);
bool isValidDate(const std::string& value);
bool isValidTime(const std::string& value);
bool isValidTimestamp(const std::string& value);

} // namespace utils
} // namespace core
} // namespace phantomdb

#endif // PHANTOMDB_UTILS_H