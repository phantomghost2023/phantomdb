#ifndef PHANTOMDB_EXECUTION_ENGINE_H
#define PHANTOMDB_EXECUTION_ENGINE_H

#include "query_planner.h"
#include "../transaction/transaction_manager.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace phantomdb {
namespace query {

// Forward declarations
class ExecutionNode;
class ExecutionContext;

// Result row structure
struct ResultRow {
    std::vector<std::string> values;
};

// Execution context
class ExecutionContext {
public:
    ExecutionContext(std::shared_ptr<transaction::Transaction> transaction);
    ~ExecutionContext() = default;
    
    std::shared_ptr<transaction::Transaction> getTransaction() const;
    void setResult(const std::vector<ResultRow>& result);
    const std::vector<ResultRow>& getResult() const;
    
private:
    std::shared_ptr<transaction::Transaction> transaction_;
    std::vector<ResultRow> result_;
};

// Base execution node class
class ExecutionNode {
public:
    ExecutionNode();
    virtual ~ExecutionNode() = default;
    
    virtual bool execute(ExecutionContext& context) = 0;
    virtual std::string toString() const = 0;
    
    void addChild(std::unique_ptr<ExecutionNode> child);
    const std::vector<std::unique_ptr<ExecutionNode>>& getChildren() const;
    
protected:
    std::vector<std::unique_ptr<ExecutionNode>> children_;
};

// Table scan execution node
class ExecTableScanNode : public ExecutionNode {
public:
    ExecTableScanNode(const std::string& tableName);
    virtual ~ExecTableScanNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
private:
    std::string tableName_;
};

// Filter execution node
class ExecFilterNode : public ExecutionNode {
public:
    ExecFilterNode(const std::string& condition);
    virtual ~ExecFilterNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
private:
    std::string condition_;
};

// Project execution node
class ExecProjectNode : public ExecutionNode {
public:
    ExecProjectNode(const std::vector<std::string>& columns);
    virtual ~ExecProjectNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
private:
    std::vector<std::string> columns_;
};

// Join execution node
class ExecJoinNode : public ExecutionNode {
public:
    ExecJoinNode(const std::string& condition);
    virtual ~ExecJoinNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
    void setLeft(std::unique_ptr<ExecutionNode> left);
    void setRight(std::unique_ptr<ExecutionNode> right);
    
private:
    std::string condition_;
    std::unique_ptr<ExecutionNode> left_;
    std::unique_ptr<ExecutionNode> right_;
};

// Subquery execution node
class ExecSubqueryNode : public ExecutionNode {
public:
    ExecSubqueryNode(const std::string& alias);
    virtual ~ExecSubqueryNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
    void setSubPlan(std::unique_ptr<ExecutionNode> subPlan);
    const ExecutionNode* getSubPlan() const;
    
private:
    std::string alias_;
    std::unique_ptr<ExecutionNode> subPlan_;
};

// Insert execution node
class ExecInsertNode : public ExecutionNode {
public:
    ExecInsertNode(const std::string& tableName, const std::vector<std::string>& columns, const std::vector<std::vector<std::string>>& values);
    virtual ~ExecInsertNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
private:
    std::string tableName_;
    std::vector<std::string> columns_;
    std::vector<std::vector<std::string>> values_;
};

// Update execution node
class ExecUpdateNode : public ExecutionNode {
public:
    ExecUpdateNode(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& setClauses, const std::string& whereClause);
    virtual ~ExecUpdateNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
private:
    std::string tableName_;
    std::vector<std::pair<std::string, std::string>> setClauses_;
    std::string whereClause_;
};

// Delete execution node
class ExecDeleteNode : public ExecutionNode {
public:
    ExecDeleteNode(const std::string& tableName, const std::string& whereClause);
    virtual ~ExecDeleteNode() = default;
    
    bool execute(ExecutionContext& context) override;
    std::string toString() const override;
    
private:
    std::string tableName_;
    std::string whereClause_;
};

// Execution engine class
class ExecutionEngine {
public:
    ExecutionEngine();
    ~ExecutionEngine();
    
    // Initialize the execution engine
    bool initialize();
    
    // Shutdown the execution engine
    void shutdown();
    
    // Execute a plan
    bool executePlan(std::unique_ptr<PlanNode> plan, 
                    std::shared_ptr<transaction::Transaction> transaction,
                    std::vector<std::vector<std::string>>& results,
                    std::string& errorMsg);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace query
} // namespace phantomdb

#endif // PHANTOMDB_EXECUTION_ENGINE_H