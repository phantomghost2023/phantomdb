#include "../src/query/sql_parser.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    SQLParser parser;
    std::string errorMsg;
    
    // Test 1: Simple subquery parsing
    std::string sql1 = "SELECT * FROM (SELECT id, name FROM users) AS subquery";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    if (ast1) {
        std::cout << "Test 1 passed: " << ast1->toString() << std::endl;
    } else {
        std::cout << "Test 1 failed: " << errorMsg << std::endl;
        return 1;
    }
    
    // Test 2: Subquery with specific columns
    std::string sql2 = "SELECT id, name FROM (SELECT id, name, age FROM users WHERE age > 18) AS adults";
    auto ast2 = parser.parse(sql2, errorMsg);
    
    if (ast2) {
        std::cout << "Test 2 passed: " << ast2->toString() << std::endl;
    } else {
        std::cout << "Test 2 failed: " << errorMsg << std::endl;
        return 1;
    }
    
    // Test 3: Subquery with JOIN
    std::string sql3 = "SELECT u.id, u.name, p.title FROM (SELECT id, name FROM users) AS u JOIN posts AS p ON u.id = p.user_id";
    auto ast3 = parser.parse(sql3, errorMsg);
    
    if (ast3) {
        std::cout << "Test 3 passed: " << ast3->toString() << std::endl;
    } else {
        std::cout << "Test 3 failed: " << errorMsg << std::endl;
        return 1;
    }
    
    std::cout << "All subquery parsing tests passed!" << std::endl;
    return 0;
}