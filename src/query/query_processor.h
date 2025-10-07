#ifndef PHANTOMDB_QUERY_PROCESSOR_H
#define PHANTOMDB_QUERY_PROCESSOR_H

#include <string>
#include <memory>
#include <vector>
#include "../transaction/transaction_manager.h"

namespace phantomdb {
namespace query {

class QueryProcessor {
public:
    QueryProcessor();
    ~QueryProcessor();
    
    // Initialize the query processor
    bool initialize();
    
    // Shutdown the query processor
    void shutdown();
    
    // Parse a SQL query and return the AST
    bool parseQuery(const std::string& sql, std::string& errorMsg);
    
    // Plan a query execution
    bool planQuery(const std::string& sql, std::string& plan, std::string& errorMsg);
    
    // Execute a query and return results
    bool executeQuery(const std::string& sql, std::vector<std::vector<std::string>>& results, std::string& errorMsg);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace query
} // namespace phantomdb

#endif // PHANTOMDB_QUERY_PROCESSOR_H