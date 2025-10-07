#include "execution_engine.h"
#include <iostream>
#include <algorithm>

namespace phantomdb {
namespace query {

// ExecutionContext implementation
ExecutionContext::ExecutionContext(std::shared_ptr<transaction::Transaction> transaction)
    : transaction_(transaction) {
}

std::shared_ptr<transaction::Transaction> ExecutionContext::getTransaction() const {
    return transaction_;
}

void ExecutionContext::setResult(const std::vector<ResultRow>& result) {
    result_ = result;
}

const std::vector<ResultRow>& ExecutionContext::getResult() const {
    return result_;
}

// ExecutionNode implementation
ExecutionNode::ExecutionNode() = default;

void ExecutionNode::addChild(std::unique_ptr<ExecutionNode> child) {
    children_.push_back(std::move(child));
}

const std::vector<std::unique_ptr<ExecutionNode>>& ExecutionNode::getChildren() const {
    return children_;
}

// ExecTableScanNode implementation
ExecTableScanNode::ExecTableScanNode(const std::string& tableName)
    : tableName_(tableName) {
}

bool ExecTableScanNode::execute(ExecutionContext& context) {
    std::cout << "Executing table scan on: " << tableName_ << std::endl;
    
    // In a real implementation, this would:
    // 1. Access the storage engine to read data from the table
    // 2. Apply any filters or projections
    // 3. Return the results
    
    // For now, we'll simulate some results
    std::vector<ResultRow> results;
    results.push_back({{"id", "name", "age"}});
    results.push_back({{"1", "John", "25"}});
    results.push_back({{"2", "Jane", "30"}});
    results.push_back({{"3", "Bob", "35"}});
    
    context.setResult(results);
    
    // Execute child nodes if any
    for (auto& child : children_) {
        if (!child->execute(context)) {
            return false;
        }
    }
    
    return true;
}

std::string ExecTableScanNode::toString() const {
    return "TableScan(" + tableName_ + ")";
}

// ExecFilterNode implementation
ExecFilterNode::ExecFilterNode(const std::string& condition)
    : condition_(condition) {
}

bool ExecFilterNode::execute(ExecutionContext& context) {
    std::cout << "Executing filter with condition: " << condition_ << std::endl;
    
    // In a real implementation, this would:
    // 1. Apply the filter condition to the input data
    // 2. Return only rows that match the condition
    
    // For now, we'll just pass through the data
    // Execute child nodes if any
    for (auto& child : children_) {
        if (!child->execute(context)) {
            return false;
        }
    }
    
    return true;
}

std::string ExecFilterNode::toString() const {
    return "Filter(" + condition_ + ")";
}

// ExecProjectNode implementation
ExecProjectNode::ExecProjectNode(const std::vector<std::string>& columns)
    : columns_(columns) {
}

bool ExecProjectNode::execute(ExecutionContext& context) {
    std::cout << "Executing projection on columns: ";
    for (const auto& col : columns_) {
        std::cout << col << " ";
    }
    std::cout << std::endl;
    
    // In a real implementation, this would:
    // 1. Select only the specified columns from the input data
    // 2. Return a result set with only those columns
    
    // For now, we'll just pass through the data
    // Execute child nodes if any
    for (auto& child : children_) {
        if (!child->execute(context)) {
            return false;
        }
    }
    
    return true;
}

std::string ExecProjectNode::toString() const {
    std::string result = "Project(";
    for (size_t i = 0; i < columns_.size(); ++i) {
        if (i > 0) result += ", ";
        result += columns_[i];
    }
    result += ")";
    return result;
}

// ExecJoinNode implementation
ExecJoinNode::ExecJoinNode(const std::string& condition)
    : condition_(condition) {
}

bool ExecJoinNode::execute(ExecutionContext& context) {
    std::cout << "Executing join with condition: " << condition_ << std::endl;
    
    // In a real implementation, this would:
    // 1. Execute the left child to get the left table data
    // 2. Execute the right child to get the right table data
    // 3. Perform the join operation based on the condition
    // 4. Return the joined results
    
    // For now, we'll just simulate a join operation
    std::vector<ResultRow> results;
    results.push_back({{"user_id", "user_name", "order_id", "order_total"}});
    results.push_back({{"1", "John", "101", "25.99"}});
    results.push_back({{"2", "Jane", "102", "30.50"}});
    
    context.setResult(results);
    
    return true;
}

std::string ExecJoinNode::toString() const {
    return "Join(" + condition_ + ")";
}

void ExecJoinNode::setLeft(std::unique_ptr<ExecutionNode> left) {
    left_ = std::move(left);
}

void ExecJoinNode::setRight(std::unique_ptr<ExecutionNode> right) {
    right_ = std::move(right);
}

// ExecSubqueryNode implementation
ExecSubqueryNode::ExecSubqueryNode(const std::string& alias)
    : alias_(alias) {
}

bool ExecSubqueryNode::execute(ExecutionContext& context) {
    std::cout << "Executing subquery: " << alias_ << std::endl;
    
    // In a real implementation, this would:
    // 1. Execute the subquery plan
    // 2. Store the results for use in the parent query
    // 3. Handle correlation with the parent query
    
    // For now, we'll just simulate some results
    std::vector<ResultRow> results;
    results.push_back({{"id", "name"}});
    results.push_back({{"1", "John"}});
    results.push_back({{"2", "Jane"}});
    
    context.setResult(results);
    
    // Execute the subquery plan if it exists
    if (subPlan_ && !subPlan_->execute(context)) {
        return false;
    }
    
    return true;
}

std::string ExecSubqueryNode::toString() const {
    return "Subquery(" + alias_ + ")";
}

void ExecSubqueryNode::setSubPlan(std::unique_ptr<ExecutionNode> subPlan) {
    subPlan_ = std::move(subPlan);
}

const ExecutionNode* ExecSubqueryNode::getSubPlan() const {
    return subPlan_.get();
}

// ExecInsertNode implementation
ExecInsertNode::ExecInsertNode(const std::string& tableName, const std::vector<std::string>& columns, const std::vector<std::vector<std::string>>& values)
    : tableName_(tableName), columns_(columns), values_(values) {
}

bool ExecInsertNode::execute(ExecutionContext& context) {
    std::cout << "Executing insert into: " << tableName_ << std::endl;
    
    // In a real implementation, this would:
    // 1. Access the storage engine to insert data into the table
    // 2. Use the transaction context for ACID compliance
    // 3. Handle column mapping if columns are specified
    // 4. Insert all rows
    
    // For now, we'll just simulate the insertion
    std::cout << "Inserting " << values_.size() << " rows into " << tableName_ << std::endl;
    
    // Set empty results since INSERT doesn't return data
    std::vector<ResultRow> results;
    context.setResult(results);
    
    // Execute child nodes if any
    for (auto& child : children_) {
        if (!child->execute(context)) {
            return false;
        }
    }
    
    return true;
}

std::string ExecInsertNode::toString() const {
    return "Insert(" + tableName_ + ")";
}

// ExecUpdateNode implementation
ExecUpdateNode::ExecUpdateNode(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& setClauses, const std::string& whereClause)
    : tableName_(tableName), setClauses_(setClauses), whereClause_(whereClause) {
}

bool ExecUpdateNode::execute(ExecutionContext& context) {
    std::cout << "Executing update on: " << tableName_ << std::endl;
    
    // In a real implementation, this would:
    // 1. Access the storage engine to update data in the table
    // 2. Use the transaction context for ACID compliance
    // 3. Apply the SET clauses to matching rows
    // 4. Filter rows using the WHERE clause
    
    // For now, we'll just simulate the update
    std::cout << "Updating " << setClauses_.size() << " columns";
    if (!whereClause_.empty()) {
        std::cout << " WHERE " << whereClause_;
    }
    std::cout << std::endl;
    
    // Set empty results since UPDATE doesn't return data
    std::vector<ResultRow> results;
    context.setResult(results);
    
    // Execute child nodes if any
    for (auto& child : children_) {
        if (!child->execute(context)) {
            return false;
        }
    }
    
    return true;
}

std::string ExecUpdateNode::toString() const {
    return "Update(" + tableName_ + ")";
}

// ExecDeleteNode implementation
ExecDeleteNode::ExecDeleteNode(const std::string& tableName, const std::string& whereClause)
    : tableName_(tableName), whereClause_(whereClause) {
}

bool ExecDeleteNode::execute(ExecutionContext& context) {
    std::cout << "Executing delete from: " << tableName_ << std::endl;
    
    // In a real implementation, this would:
    // 1. Access the storage engine to delete data from the table
    // 2. Use the transaction context for ACID compliance
    // 3. Filter rows using the WHERE clause
    
    // For now, we'll just simulate the deletion
    std::cout << "Deleting rows from " << tableName_;
    if (!whereClause_.empty()) {
        std::cout << " WHERE " << whereClause_;
    }
    std::cout << std::endl;
    
    // Set empty results since DELETE doesn't return data
    std::vector<ResultRow> results;
    context.setResult(results);
    
    // Execute child nodes if any
    for (auto& child : children_) {
        if (!child->execute(context)) {
            return false;
        }
    }
    
    return true;
}

std::string ExecDeleteNode::toString() const {
    return "Delete(" + tableName_ + ")";
}

// ExecutionEngine::Impl implementation
class ExecutionEngine::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "Initializing Execution Engine..." << std::endl;
        return true;
    }
    
    void shutdown() {
        std::cout << "Shutting down Execution Engine..." << std::endl;
    }
    
    std::unique_ptr<ExecutionNode> convertPlanToExecutionNode(const PlanNode* planNode) {
        if (!planNode) {
            return nullptr;
        }
        
        std::unique_ptr<ExecutionNode> execNode;
        
        switch (planNode->getType()) {
            case PlanNodeType::TABLE_SCAN: {
                const auto* tableScan = static_cast<const query::TableScanNode*>(planNode);
                execNode = std::make_unique<ExecTableScanNode>(tableScan->getTableName());
                break;
            }
            case PlanNodeType::FILTER: {
                // For now, we'll create a simple filter node
                execNode = std::make_unique<ExecFilterNode>("condition");
                break;
            }
            case PlanNodeType::PROJECT: {
                // For now, we'll create a simple project node
                execNode = std::make_unique<ExecProjectNode>(std::vector<std::string>{"*"});
                break;
            }
            case PlanNodeType::JOIN: {
                const auto* joinNode = static_cast<const query::JoinNode*>(planNode);
                auto execJoinNode = std::make_unique<ExecJoinNode>(joinNode->getCondition());
                
                // Convert left and right children
                if (joinNode->getLeft()) {
                    auto leftExecNode = convertPlanToExecutionNode(joinNode->getLeft());
                    execJoinNode->setLeft(std::move(leftExecNode));
                }
                
                if (joinNode->getRight()) {
                    auto rightExecNode = convertPlanToExecutionNode(joinNode->getRight());
                    execJoinNode->setRight(std::move(rightExecNode));
                }
                
                execNode = std::move(execJoinNode);
                break;
            }
            case PlanNodeType::SUBQUERY: {
                const auto* subqueryNode = static_cast<const query::SubqueryNode*>(planNode);
                auto execSubqueryNode = std::make_unique<ExecSubqueryNode>(subqueryNode->getAlias());
                
                // Convert the subquery plan
                if (subqueryNode->getSubPlan()) {
                    auto subPlanExecNode = convertPlanToExecutionNode(subqueryNode->getSubPlan());
                    execSubqueryNode->setSubPlan(std::move(subPlanExecNode));
                }
                
                execNode = std::move(execSubqueryNode);
                break;
            }
            case PlanNodeType::INSERT: {
                const auto* insertNode = static_cast<const query::InsertNode*>(planNode);
                execNode = std::make_unique<ExecInsertNode>(
                    insertNode->getTableName(),
                    insertNode->getColumns(),
                    insertNode->getValues()
                );
                break;
            }
            case PlanNodeType::UPDATE: {
                const auto* updateNode = static_cast<const query::UpdateNode*>(planNode);
                execNode = std::make_unique<ExecUpdateNode>(
                    updateNode->getTableName(),
                    updateNode->getSetClauses(),
                    updateNode->getWhereClause()
                );
                break;
            }
            case PlanNodeType::DELETE: {
                const auto* deleteNode = static_cast<const query::DeleteNode*>(planNode);
                execNode = std::make_unique<ExecDeleteNode>(
                    deleteNode->getTableName(),
                    deleteNode->getWhereClause()
                );
                break;
            }
            default:
                // For unsupported node types, return nullptr
                return nullptr;
        }
        
        return execNode;
    }
    
    bool executePlan(std::unique_ptr<PlanNode> plan, 
                    std::shared_ptr<transaction::Transaction> transaction,
                    std::vector<std::vector<std::string>>& results,
                    std::string& errorMsg) {
        std::cout << "Executing plan..." << std::endl;
        
        if (!plan) {
            errorMsg = "Invalid execution plan";
            return false;
        }
        
        // Convert the plan to an execution tree
        auto execNode = convertPlanToExecutionNode(plan.get());
        if (!execNode) {
            errorMsg = "Failed to convert plan to execution nodes";
            return false;
        }
        
        // Create execution context
        ExecutionContext context(transaction);
        
        // Execute the plan
        if (!execNode->execute(context)) {
            errorMsg = "Failed to execute plan";
            return false;
        }
        
        // Convert results to the expected format
        const auto& execResults = context.getResult();
        results.clear();
        for (const auto& row : execResults) {
            results.push_back(row.values);
        }
        
        return true;
    }
};

// ExecutionEngine implementation
ExecutionEngine::ExecutionEngine() : pImpl_(std::make_unique<Impl>()) {
    std::cout << "PhantomDB Execution Engine created" << std::endl;
}

ExecutionEngine::~ExecutionEngine() {
    std::cout << "PhantomDB Execution Engine destroyed" << std::endl;
}

bool ExecutionEngine::initialize() {
    return pImpl_->initialize();
}

void ExecutionEngine::shutdown() {
    pImpl_->shutdown();
}

bool ExecutionEngine::executePlan(std::unique_ptr<PlanNode> plan, 
                                 std::shared_ptr<transaction::Transaction> transaction,
                                 std::vector<std::vector<std::string>>& results,
                                 std::string& errorMsg) {
    return pImpl_->executePlan(std::move(plan), transaction, results, errorMsg);
}

} // namespace query
} // namespace phantomdb