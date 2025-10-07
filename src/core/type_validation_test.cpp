#include "utils.h"
#include <iostream>
#include <unordered_map>

using namespace phantomdb::core::utils;

int main() {
    std::cout << "Testing enhanced type validation..." << std::endl;
    
    // Test 1: Valid integer values
    std::cout << "\n--- Test 1: Integer validation ---" << std::endl;
    std::cout << "isValidInteger('123'): " << (isValidInteger("123") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidInteger('-456'): " << (isValidInteger("-456") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidInteger('0'): " << (isValidInteger("0") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidInteger('abc'): " << (isValidInteger("abc") ? "FAIL" : "PASS") << std::endl;
    std::cout << "isValidInteger(''): " << (isValidInteger("") ? "FAIL" : "PASS") << std::endl;
    
    // Test 2: Valid float values
    std::cout << "\n--- Test 2: Float validation ---" << std::endl;
    std::cout << "isValidFloat('123.45'): " << (isValidFloat("123.45") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidFloat('-67.89'): " << (isValidFloat("-67.89") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidFloat('1.23e10'): " << (isValidFloat("1.23e10") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidFloat('abc'): " << (isValidFloat("abc") ? "FAIL" : "PASS") << std::endl;
    std::cout << "isValidFloat(''): " << (isValidFloat("") ? "FAIL" : "PASS") << std::endl;
    
    // Test 3: Valid boolean values
    std::cout << "\n--- Test 3: Boolean validation ---" << std::endl;
    std::cout << "isValidBoolean('true'): " << (isValidBoolean("true") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidBoolean('false'): " << (isValidBoolean("false") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidBoolean('1'): " << (isValidBoolean("1") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidBoolean('0'): " << (isValidBoolean("0") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidBoolean('yes'): " << (isValidBoolean("yes") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidBoolean('no'): " << (isValidBoolean("no") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidBoolean('invalid'): " << (isValidBoolean("invalid") ? "FAIL" : "PASS") << std::endl;
    
    // Test 4: Valid date values
    std::cout << "\n--- Test 4: Date validation ---" << std::endl;
    std::cout << "isValidDate('2023-12-25'): " << (isValidDate("2023-12-25") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidDate('2023-02-29'): " << (isValidDate("2023-02-29") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidDate('invalid'): " << (isValidDate("invalid") ? "FAIL" : "PASS") << std::endl;
    std::cout << "isValidDate('2023/12/25'): " << (isValidDate("2023/12/25") ? "FAIL" : "PASS") << std::endl;
    
    // Test 5: Valid time values
    std::cout << "\n--- Test 5: Time validation ---" << std::endl;
    std::cout << "isValidTime('12:34:56'): " << (isValidTime("12:34:56") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidTime('00:00:00'): " << (isValidTime("00:00:00") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidTime('invalid'): " << (isValidTime("invalid") ? "FAIL" : "PASS") << std::endl;
    std::cout << "isValidTime('12:34'): " << (isValidTime("12:34") ? "FAIL" : "PASS") << std::endl;
    
    // Test 6: Valid timestamp values
    std::cout << "\n--- Test 6: Timestamp validation ---" << std::endl;
    std::cout << "isValidTimestamp('2023-12-25 12:34:56'): " << (isValidTimestamp("2023-12-25 12:34:56") ? "PASS" : "FAIL") << std::endl;
    std::cout << "isValidTimestamp('invalid'): " << (isValidTimestamp("invalid") ? "FAIL" : "PASS") << std::endl;
    
    // Test 7: Type validation against schema
    std::cout << "\n--- Test 7: Schema validation ---" << std::endl;
    std::unordered_map<std::string, std::string> schema = {
        {"id", "int"},
        {"name", "string"},
        {"age", "integer"},
        {"salary", "float"},
        {"active", "boolean"},
        {"created", "timestamp"}
    };
    
    std::unordered_map<std::string, std::string> validData = {
        {"id", "123"},
        {"name", "John Doe"},
        {"age", "30"},
        {"salary", "50000.50"},
        {"active", "true"},
        {"created", "2023-12-25 12:34:56"}
    };
    
    std::string error;
    bool result = validateData(validData, schema, error);
    std::cout << "Valid data validation: " << (result ? "PASS" : "FAIL") << std::endl;
    if (!result) {
        std::cout << "Error: " << error << std::endl;
    }
    
    std::unordered_map<std::string, std::string> invalidData = {
        {"id", "abc"},  // Invalid integer
        {"name", "John Doe"},
        {"age", "30"}
    };
    
    result = validateData(invalidData, schema, error);
    std::cout << "Invalid data validation: " << (result ? "FAIL" : "PASS") << std::endl;
    if (!result) {
        std::cout << "Error: " << error << std::endl;
    }
    
    std::cout << "\nAll tests completed!" << std::endl;
    return 0;
}