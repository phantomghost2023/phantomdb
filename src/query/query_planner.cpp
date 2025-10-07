#include "query_planner.h"
#include <iostream>
#include <sstream>

namespace phantomdb {
namespace query {

// PlanNode implementation
PlanNode::PlanNode(PlanNodeType type) : type_(type), cost_(0.0) {}

PlanNodeType PlanNode::getType() const {
    return type_;
}

void PlanNode::setCost(double cost) {
    cost_ = cost;
}

double PlanNode::getCost() const {
    return cost_;
}

// TableScanNode implementation
TableScanNode::TableScanNode(const std::string& tableName) 
    : PlanNode(PlanNodeType::TABLE_SCAN), tableName_(tableName) {
    // Set a default cost based on table size (simplified)
    setCost(100.0);
}

std::string TableScanNode::toString() const {
    std::ostringstream oss;
    oss << "TableScan(table=" << tableName_ << ", cost=" << getCost() << ")";
    return oss.str();
}

const std::string& TableScanNode::getTableName() const {
    return tableName_;
}

// JoinNode implementation
JoinNode::JoinNode(std::unique_ptr<PlanNode> left, std::unique_ptr<PlanNode> right, const std::string& condition)
    : PlanNode(PlanNodeType::JOIN), left_(std::move(left)), right_(std::move(right)), condition_(condition) {
    // Set a default cost (simplified)
    setCost(200.0);
}

std::string JoinNode::toString() const {
    std::ostringstream oss;
    oss << "Join(condition=" << condition_ << ", cost=" << getCost() << ")";
    return oss.str();
}

const PlanNode* JoinNode::getLeft() const {
    return left_.get();
}

const PlanNode* JoinNode::getRight() const {
    return right_.get();
}

const std::string& JoinNode::getCondition() const {
    return condition_;
}

// SubqueryNode implementation
SubqueryNode::SubqueryNode(std::unique_ptr<PlanNode> subPlan, const std::string& alias)
    : PlanNode(PlanNodeType::SUBQUERY), subPlan_(std::move(subPlan)), alias_(alias) {
    // Set cost based on subplan cost
    setCost(subPlan_->getCost() * 1.5); // Subquery typically has some overhead
}

std::string SubqueryNode::toString() const {
    std::ostringstream oss;
    oss << "Subquery(alias=" << alias_ << ", cost=" << getCost() << ")";
    return oss.str();
}

const PlanNode* SubqueryNode::getSubPlan() const {
    return subPlan_.get();
}

const std::string& SubqueryNode::getAlias() const {
    return alias_;
}

// InsertNode implementation
InsertNode::InsertNode(const std::string& tableName, const std::vector<std::string>& columns, const std::vector<std::vector<std::string>>& values)
    : PlanNode(PlanNodeType::INSERT), tableName_(tableName), columns_(columns), values_(values) {
    // Set a default cost based on number of rows (simplified)
    setCost(10.0 * values.size());
}

std::string InsertNode::toString() const {
    std::ostringstream oss;
    oss << "Insert(table=" << tableName_ << ", rows=" << values_.size() << ", cost=" << getCost() << ")";
    return oss.str();
}

const std::string& InsertNode::getTableName() const {
    return tableName_;
}

const std::vector<std::string>& InsertNode::getColumns() const {
    return columns_;
}

const std::vector<std::vector<std::string>>& InsertNode::getValues() const {
    return values_;
}

// UpdateNode implementation
UpdateNode::UpdateNode(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& setClauses, const std::string& whereClause)
    : PlanNode(PlanNodeType::UPDATE), tableName_(tableName), setClauses_(setClauses), whereClause_(whereClause) {
    // Set a default cost (simplified)
    setCost(50.0);
}

std::string UpdateNode::toString() const {
    std::ostringstream oss;
    oss << "Update(table=" << tableName_ << ", setClauses=" << setClauses_.size() << ", cost=" << getCost() << ")";
    return oss.str();
}

const std::string& UpdateNode::getTableName() const {
    return tableName_;
}

const std::vector<std::pair<std::string, std::string>>& UpdateNode::getSetClauses() const {
    return setClauses_;
}

const std::string& UpdateNode::getWhereClause() const {
    return whereClause_;
}

// DeleteNode implementation
DeleteNode::DeleteNode(const std::string& tableName, const std::string& whereClause)
    : PlanNode(PlanNodeType::DELETE), tableName_(tableName), whereClause_(whereClause) {
    // Set a default cost (simplified)
    setCost(50.0);
}

std::string DeleteNode::toString() const {
    std::ostringstream oss;
    oss << "Delete(table=" << tableName_ << ", cost=" << getCost() << ")";
    return oss.str();
}

const std::string& DeleteNode::getTableName() const {
    return tableName_;
}

const std::string& DeleteNode::getWhereClause() const {
    return whereClause_;
}

// QueryPlanner implementation
class QueryPlanner::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    std::unique_ptr<PlanNode> generatePlan(const ASTNode* ast, std::string& errorMsg) {
        if (!ast) {
            errorMsg = "Invalid AST";
            return nullptr;
        }
        
        // Try to cast to SelectStatement
        const SelectStatement* selectStmt = dynamic_cast<const SelectStatement*>(ast);
        if (selectStmt) {
            return generateSelectPlan(selectStmt);
        }
        
        // Try to cast to InsertStatement
        const InsertStatement* insertStmt = dynamic_cast<const InsertStatement*>(ast);
        if (insertStmt) {
            return generateInsertPlan(insertStmt);
        }
        
        // Try to cast to UpdateStatement
        const UpdateStatement* updateStmt = dynamic_cast<const UpdateStatement*>(ast);
        if (updateStmt) {
            return generateUpdatePlan(updateStmt);
        }
        
        // Try to cast to DeleteStatement
        const DeleteStatement* deleteStmt = dynamic_cast<const DeleteStatement*>(ast);
        if (deleteStmt) {
            return generateDeletePlan(deleteStmt);
        }
        
        errorMsg = "Unsupported AST node type";
        return nullptr;
    }
    
private:
    std::unique_ptr<PlanNode> generateSelectPlan(const SelectStatement* selectStmt) {
        // Check if this is a subquery (no table name but has subqueries)
        const auto& subqueries = selectStmt->getSubqueries();
        if (!selectStmt->getTable().empty()) {
            // Regular table scan
            const std::string& tableName = selectStmt->getTable();
            
            // Create a table scan node for the main table
            auto tableScan = std::make_unique<TableScanNode>(tableName);
            
            // Check for JOINs
            const auto& joins = selectStmt->getJoins();
            if (!joins.empty()) {
                // For simplicity, we'll create a chain of JOIN nodes
                // In a real implementation, we would optimize the join order
                std::unique_ptr<PlanNode> currentPlan = std::move(tableScan);
                
                for (const auto& join : joins) {
                    // Create a table scan for the join table
                    auto joinTableScan = std::make_unique<TableScanNode>(join.table);
                    
                    // Create a JOIN node
                    auto joinNode = std::make_unique<JoinNode>(
                        std::move(currentPlan),
                        std::move(joinTableScan),
                        join.condition
                    );
                    
                    currentPlan = std::move(joinNode);
                }
                
                return std::move(currentPlan);
            }
            
            return std::move(tableScan);
        } else if (!subqueries.empty()) {
            // This is a query with subqueries
            // For now, we'll handle the first subquery
            // In a more advanced implementation, we would handle all subqueries
            if (subqueries.size() == 1) {
                const Subquery* subquery = subqueries[0].get();
                const SelectStatement* subSelect = subquery->getSelectStatement();
                
                // Generate plan for subquery
                auto subPlan = generateSelectPlan(subSelect);
                
                // Create subquery node
                auto subqueryNode = std::make_unique<SubqueryNode>(
                    std::move(subPlan),
                    subquery->getAlias()
                );
                
                return std::move(subqueryNode);
            } else {
                // Multiple subqueries - create a more complex plan
                // For now, we'll just use the first one
                const Subquery* subquery = subqueries[0].get();
                const SelectStatement* subSelect = subquery->getSelectStatement();
                
                // Generate plan for subquery
                auto subPlan = generateSelectPlan(subSelect);
                
                // Create subquery node
                auto subqueryNode = std::make_unique<SubqueryNode>(
                    std::move(subPlan),
                    subquery->getAlias()
                );
                
                return std::move(subqueryNode);
            }
        } else {
            // No table and no subqueries - this shouldn't happen
            return nullptr;
        }
    }
    
    std::unique_ptr<PlanNode> generateInsertPlan(const InsertStatement* insertStmt) {
        // Create an insert node
        auto insertNode = std::make_unique<InsertNode>(
            insertStmt->getTable(),
            insertStmt->getColumns(),
            insertStmt->getValues()
        );
        
        return std::move(insertNode);
    }
    
    std::unique_ptr<PlanNode> generateUpdatePlan(const UpdateStatement* updateStmt) {
        // Create an update node
        auto updateNode = std::make_unique<UpdateNode>(
            updateStmt->getTable(),
            updateStmt->getSetClauses(),
            updateStmt->getWhereClause()
        );
        
        return std::move(updateNode);
    }
    
    std::unique_ptr<PlanNode> generateDeletePlan(const DeleteStatement* deleteStmt) {
        // Create a delete node
        auto deleteNode = std::make_unique<DeleteNode>(
            deleteStmt->getTable(),
            deleteStmt->getWhereClause()
        );
        
        return std::move(deleteNode);
    }
};

QueryPlanner::QueryPlanner() : pImpl(std::make_unique<Impl>()) {}

QueryPlanner::~QueryPlanner() = default;

std::unique_ptr<PlanNode> QueryPlanner::generatePlan(const ASTNode* ast, std::string& errorMsg) {
    return pImpl->generatePlan(ast, errorMsg);
}

} // namespace query
} // namespace phantomdb