#include "query_processor.h"
#include "sql_parser.h"
#include "query_planner.h"
#include "query_optimizer.h"
#include "execution_engine.h"
#include <iostream>
#include <string>

namespace phantomdb {
namespace query {

class QueryProcessor::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "Initializing Query Processor..." << std::endl;
        // Initialize parser, planner, optimizer, and execution engine
        parser_ = std::make_unique<SQLParser>();
        planner_ = std::make_unique<QueryPlanner>();
        optimizer_ = std::make_unique<QueryOptimizer>();
        executionEngine_ = std::make_unique<ExecutionEngine>();
        
        if (!optimizer_->initialize()) {
            return false;
        }
        
        if (!executionEngine_->initialize()) {
            return false;
        }
        
        return true;
    }
    
    void shutdown() {
        std::cout << "Shutting down Query Processor..." << std::endl;
        // Clean up resources
        if (executionEngine_) {
            executionEngine_->shutdown();
            executionEngine_.reset();
        }
        
        if (optimizer_) {
            optimizer_->shutdown();
            optimizer_.reset();
        }
        
        planner_.reset();
        parser_.reset();
    }
    
    bool parseQuery(const std::string& sql, std::string& errorMsg) {
        std::cout << "Parsing query: " << sql << std::endl;
        // Use the SQL parser to parse the query
        auto ast = parser_->parse(sql, errorMsg);
        if (!ast) {
            return false;
        }
        // Store the AST for later use
        lastAST_ = std::move(ast);
        return true;
    }
    
    bool planQuery(const std::string& sql, std::string& plan, std::string& errorMsg) {
        std::cout << "Planning query: " << sql << std::endl;
        
        // First parse the query if we haven't already
        std::unique_ptr<ASTNode> ast;
        if (lastAST_) {
            ast = std::move(lastAST_);
        } else {
            ast = parser_->parse(sql, errorMsg);
            if (!ast) {
                return false;
            }
        }
        
        // Generate the execution plan
        auto planNode = planner_->generatePlan(ast.get(), errorMsg);
        if (!planNode) {
            return false;
        }
        
        // Optimize the plan
        auto optimizedPlan = optimizer_->optimize(std::move(planNode), errorMsg);
        if (!optimizedPlan) {
            return false;
        }
        
        // Convert the plan to a string representation
        plan = optimizedPlan->toString();
        return true;
    }
    
    bool executeQuery(const std::string& sql, std::vector<std::vector<std::string>>& results, std::string& errorMsg) {
        std::cout << "Executing query: " << sql << std::endl;
        
        // Parse the query
        std::unique_ptr<ASTNode> ast = parser_->parse(sql, errorMsg);
        if (!ast) {
            return false;
        }
        
        // Generate the execution plan
        auto planNode = planner_->generatePlan(ast.get(), errorMsg);
        if (!planNode) {
            return false;
        }
        
        // Optimize the plan
        auto optimizedPlan = optimizer_->optimize(std::move(planNode), errorMsg);
        if (!optimizedPlan) {
            return false;
        }
        
        // For now, we'll create a dummy transaction for execution
        // In a real implementation, the transaction would be provided by the caller
        auto transaction = std::make_shared<transaction::Transaction>(1, transaction::IsolationLevel::READ_COMMITTED);
        
        // Execute the plan using the execution engine
        return executionEngine_->executePlan(std::move(optimizedPlan), transaction, results, errorMsg);
    }
    
private:
    std::unique_ptr<SQLParser> parser_;
    std::unique_ptr<QueryPlanner> planner_;
    std::unique_ptr<QueryOptimizer> optimizer_;
    std::unique_ptr<ExecutionEngine> executionEngine_;
    std::unique_ptr<ASTNode> lastAST_;
};

QueryProcessor::QueryProcessor() : pImpl(std::make_unique<Impl>()) {
    std::cout << "PhantomDB Query Processor created" << std::endl;
}

QueryProcessor::~QueryProcessor() {
    std::cout << "PhantomDB Query Processor destroyed" << std::endl;
}

bool QueryProcessor::initialize() {
    return pImpl->initialize();
}

void QueryProcessor::shutdown() {
    pImpl->shutdown();
}

bool QueryProcessor::parseQuery(const std::string& sql, std::string& errorMsg) {
    return pImpl->parseQuery(sql, errorMsg);
}

bool QueryProcessor::planQuery(const std::string& sql, std::string& plan, std::string& errorMsg) {
    return pImpl->planQuery(sql, plan, errorMsg);
}

bool QueryProcessor::executeQuery(const std::string& sql, std::vector<std::vector<std::string>>& results, std::string& errorMsg) {
    return pImpl->executeQuery(sql, results, errorMsg);
}

} // namespace query
} // namespace phantomdb