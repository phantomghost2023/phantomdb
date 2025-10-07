#include "sql_parser.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing JOIN clause parsing..." << std::endl;
    
    SQLParser parser;
    std::string errorMsg;
    
    // Test basic SELECT with JOIN
    std::string sql1 = "SELECT * FROM users JOIN orders ON users.id = orders.user_id";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    assert(ast1 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast1->toString() << std::endl;
    
    // Test SELECT with multiple JOINs
    std::string sql2 = "SELECT users.name, orders.total FROM users JOIN orders ON users.id = orders.user_id JOIN products ON orders.product_id = products.id";
    auto ast2 = parser.parse(sql2, errorMsg);
    
    assert(ast2 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast2->toString() << std::endl;
    
    // Test SELECT with columns and JOIN
    std::string sql3 = "SELECT u.name, o.total FROM users u JOIN orders o ON u.id = o.user_id";
    auto ast3 = parser.parse(sql3, errorMsg);
    
    assert(ast3 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast3->toString() << std::endl;
    
    std::cout << "All JOIN parsing tests passed!" << std::endl;
    
    return 0;
}