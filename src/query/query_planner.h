#ifndef PHANTOMDB_QUERY_PLANNER_H
#define PHANTOMDB_QUERY_PLANNER_H

#include "sql_parser.h"
#include <string>
#include <memory>
#include <vector>

namespace phantomdb {
namespace query {

// Forward declarations
class PlanNode;

// Plan node types
enum class PlanNodeType {
    TABLE_SCAN,
    INDEX_SCAN,
    FILTER,
    PROJECT,
    JOIN,
    AGGREGATE,
    SORT,
    INSERT,
    UPDATE,
    DELETE,
    SUBQUERY
};

// Base plan node class
class PlanNode {
public:
    PlanNode(PlanNodeType type);
    virtual ~PlanNode() = default;
    
    PlanNodeType getType() const;
    virtual std::string toString() const = 0;
    
    void setCost(double cost);
    double getCost() const;
    
protected:
    PlanNodeType type_;
    double cost_;
};

// Table scan plan node
class TableScanNode : public PlanNode {
public:
    TableScanNode(const std::string& tableName);
    virtual ~TableScanNode() = default;
    
    std::string toString() const override;
    const std::string& getTableName() const;
    
private:
    std::string tableName_;
};

// Subquery plan node
class SubqueryNode : public PlanNode {
public:
    SubqueryNode(std::unique_ptr<PlanNode> subPlan, const std::string& alias);
    virtual ~SubqueryNode() = default;
    
    std::string toString() const override;
    const PlanNode* getSubPlan() const;
    const std::string& getAlias() const;
    
private:
    std::unique_ptr<PlanNode> subPlan_;
    std::string alias_;
};

// Join plan node
class JoinNode : public PlanNode {
public:
    JoinNode(std::unique_ptr<PlanNode> left, std::unique_ptr<PlanNode> right, const std::string& condition);
    virtual ~JoinNode() = default;
    
    std::string toString() const override;
    const PlanNode* getLeft() const;
    const PlanNode* getRight() const;
    const std::string& getCondition() const;
    
private:
    std::unique_ptr<PlanNode> left_;
    std::unique_ptr<PlanNode> right_;
    std::string condition_;
};

// Insert plan node
class InsertNode : public PlanNode {
public:
    InsertNode(const std::string& tableName, const std::vector<std::string>& columns, const std::vector<std::vector<std::string>>& values);
    virtual ~InsertNode() = default;
    
    std::string toString() const override;
    const std::string& getTableName() const;
    const std::vector<std::string>& getColumns() const;
    const std::vector<std::vector<std::string>>& getValues() const;
    
private:
    std::string tableName_;
    std::vector<std::string> columns_;
    std::vector<std::vector<std::string>> values_;
};

// Update plan node
class UpdateNode : public PlanNode {
public:
    UpdateNode(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& setClauses, const std::string& whereClause);
    virtual ~UpdateNode() = default;
    
    std::string toString() const override;
    const std::string& getTableName() const;
    const std::vector<std::pair<std::string, std::string>>& getSetClauses() const;
    const std::string& getWhereClause() const;
    
private:
    std::string tableName_;
    std::vector<std::pair<std::string, std::string>> setClauses_;
    std::string whereClause_;
};

// Delete plan node
class DeleteNode : public PlanNode {
public:
    DeleteNode(const std::string& tableName, const std::string& whereClause);
    virtual ~DeleteNode() = default;
    
    std::string toString() const override;
    const std::string& getTableName() const;
    const std::string& getWhereClause() const;
    
private:
    std::string tableName_;
    std::string whereClause_;
};

// Query planner class
class QueryPlanner {
public:
    QueryPlanner();
    ~QueryPlanner();
    
    // Generate an execution plan from an AST
    std::unique_ptr<PlanNode> generatePlan(const ASTNode* ast, std::string& errorMsg);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace query
} // namespace phantomdb

#endif // PHANTOMDB_QUERY_PLANNER_H