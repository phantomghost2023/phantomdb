#include "sql_parser.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing INSERT statement parsing..." << std::endl;
    
    SQLParser parser;
    std::string errorMsg;
    
    // Test basic INSERT statement
    std::string sql1 = "INSERT INTO users (id, name, age) VALUES ('1', 'John', '25')";
    auto ast1 = parser.parse(sql1, errorMsg);
    
    assert(ast1 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast1->toString() << std::endl;
    
    // Test INSERT statement without columns
    std::string sql2 = "INSERT INTO users VALUES ('2', 'Jane', '30')";
    auto ast2 = parser.parse(sql2, errorMsg);
    
    assert(ast2 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast2->toString() << std::endl;
    
    // Test INSERT statement with multiple rows
    std::string sql3 = "INSERT INTO users (id, name, age) VALUES ('3', 'Bob', '35'), ('4', 'Alice', '28')";
    auto ast3 = parser.parse(sql3, errorMsg);
    
    assert(ast3 != nullptr);
    assert(errorMsg.empty());
    
    std::cout << "Parsed AST: " << ast3->toString() << std::endl;
    
    std::cout << "All INSERT statement parsing tests passed!" << std::endl;
    
    return 0;
}