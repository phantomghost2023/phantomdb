#include "sql_parser.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing DELETE statement parsing..." << std::endl;
    
    SQLParser parser;
    std::string errorMsg;
    
    // Test basic DELETE statement
    std::string sql1 = "DELETE FROM users WHERE id = 1";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    assert(ast1 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast1->toString() << std::endl;
    
    // Test DELETE statement with complex WHERE clause
    std::string sql2 = "DELETE FROM users WHERE age > 18 AND active = 'true'";
    auto ast2 = parser.parse(sql2, errorMsg);
    
    assert(ast2 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast2->toString() << std::endl;
    
    // Test DELETE statement without WHERE clause (delete all)
    std::string sql3 = "DELETE FROM users";
    auto ast3 = parser.parse(sql3, errorMsg);
    
    assert(ast3 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast3->toString() << std::endl;
    
    std::cout << "All DELETE statement parsing tests passed!" << std::endl;
    
    return 0;
}