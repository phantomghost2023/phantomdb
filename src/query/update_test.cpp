#include "sql_parser.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing UPDATE statement parsing..." << std::endl;
    
    SQLParser parser;
    std::string errorMsg;
    
    // Test basic UPDATE statement
    std::string sql1 = "UPDATE users SET name = 'John Doe' WHERE id = 1";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    assert(ast1 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast1->toString() << std::endl;
    
    // Test UPDATE statement with multiple SET clauses
    std::string sql2 = "UPDATE users SET name = 'Jane Doe', age = '30' WHERE id = 2";
    auto ast2 = parser.parse(sql2, errorMsg);
    
    assert(ast2 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast2->toString() << std::endl;
    
    // Test UPDATE statement without WHERE clause
    std::string sql3 = "UPDATE users SET active = 'true'";
    auto ast3 = parser.parse(sql3, errorMsg);
    
    assert(ast3 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast3->toString() << std::endl;
    
    std::cout << "All UPDATE statement parsing tests passed!" << std::endl;
    
    return 0;
}