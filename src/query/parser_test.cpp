#include "sql_parser.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

void testBasicSelect() {
    std::cout << "Testing basic SELECT statement..." << std::endl;
    
    SQLParser parser;
    std::string errorMsg;
    
    // Test SELECT * FROM table
    auto ast = parser.parse("SELECT * FROM users", errorMsg);
    assert(ast != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Basic SELECT test passed!" << std::endl;
}

void testSelectWithColumns() {
    std::cout << "Testing SELECT with specific columns..." << std::endl;
    
    SQLParser parser;
    std::string errorMsg;
    
    // Test SELECT column1, column2 FROM table
    auto ast = parser.parse("SELECT id, name FROM users", errorMsg);
    assert(ast != nullptr);
    assert(errorMsg.empty());
    
    // Cast to SelectStatement to check details
    auto selectStmt = dynamic_cast<SelectStatement*>(ast.get());
    assert(selectStmt != nullptr);
    
    const auto& columns = selectStmt->getColumns();
    assert(columns.size() == 2);
    assert(columns[0] == "id");
    assert(columns[1] == "name");
    
    assert(selectStmt->getTable() == "users");
    
    std::cout << "SELECT with columns test passed!" << std::endl;
}

void testSelectWithWhitespace() {
    std::cout << "Testing SELECT with extra whitespace..." << std::endl;
    
    SQLParser parser;
    std::string errorMsg;
    
    // Test SELECT with extra whitespace
    auto ast = parser.parse("  SELECT   id , name   FROM   users  ", errorMsg);
    assert(ast != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "SELECT with whitespace test passed!" << std::endl;
}

void testParseError() {
    std::cout << "Testing parse error handling..." << std::endl;
    
    SQLParser parser;
    std::string errorMsg;
    
    // Test invalid SQL
    auto ast = parser.parse("INVALID SQL STATEMENT", errorMsg);
    assert(ast == nullptr);
    assert(!errorMsg.empty());
    
    std::cout << "Parse error handling test passed!" << std::endl;
}

int main() {
    std::cout << "Running SQL Parser tests..." << std::endl;
    
    testBasicSelect();
    testSelectWithColumns();
    testSelectWithWhitespace();
    testParseError();
    
    std::cout << "All SQL Parser tests passed!" << std::endl;
    return 0;
}