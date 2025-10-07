#include "sql_parser.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace phantomdb {
namespace query {

// SelectStatement implementation
SelectStatement::SelectStatement(std::vector<std::string> columns, std::string table)
    : columns_(std::move(columns)), table_(std::move(table)) {}

std::string SelectStatement::toString() const {
    std::ostringstream oss;
    oss << "SELECT ";
    
    if (columns_.empty()) {
        oss << "*";
    } else {
        for (size_t i = 0; i < columns_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << columns_[i];
        }
    }
    
    oss << " FROM " << table_;
    
    // Add JOIN clauses if any
    for (const auto& join : joins_) {
        oss << " JOIN " << join.table << " ON " << join.condition;
    }
    
    // Add subqueries if any
    for (const auto& subquery : subqueries_) {
        oss << " " << subquery->toString();
    }
    
    return oss.str();
}

const std::vector<std::string>& SelectStatement::getColumns() const {
    return columns_;
}

const std::string& SelectStatement::getTable() const {
    return table_;
}

const std::vector<JoinClause>& SelectStatement::getJoins() const {
    return joins_;
}

const std::vector<std::unique_ptr<Subquery>>& SelectStatement::getSubqueries() const {
    return subqueries_;
}

void SelectStatement::addJoin(const JoinClause& join) {
    joins_.push_back(join);
}

void SelectStatement::addSubquery(std::unique_ptr<Subquery> subquery) {
    subqueries_.push_back(std::move(subquery));
}

// Subquery implementation
Subquery::Subquery(std::unique_ptr<SelectStatement> selectStmt, std::string alias)
    : selectStmt_(std::move(selectStmt)), alias_(std::move(alias)) {}

std::string Subquery::toString() const {
    std::ostringstream oss;
    oss << "(" << selectStmt_->toString() << ") AS " << alias_;
    return oss.str();
}

const SelectStatement* Subquery::getSelectStatement() const {
    return selectStmt_.get();
}

const std::string& Subquery::getAlias() const {
    return alias_;
}

// InsertStatement implementation
InsertStatement::InsertStatement(std::string table, std::vector<std::string> columns, std::vector<std::vector<std::string>> values)
    : table_(std::move(table)), columns_(std::move(columns)), values_(std::move(values)) {}

std::string InsertStatement::toString() const {
    std::ostringstream oss;
    oss << "INSERT INTO " << table_ << " ";
    
    if (!columns_.empty()) {
        oss << "(";
        for (size_t i = 0; i < columns_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << columns_[i];
        }
        oss << ") ";
    }
    
    oss << "VALUES ";
    
    for (size_t i = 0; i < values_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "(";
        
        const auto& row = values_[i];
        for (size_t j = 0; j < row.size(); ++j) {
            if (j > 0) oss << ", ";
            oss << "'" << row[j] << "'";
        }
        oss << ")";
    }
    
    return oss.str();
}

const std::string& InsertStatement::getTable() const {
    return table_;
}

const std::vector<std::string>& InsertStatement::getColumns() const {
    return columns_;
}

const std::vector<std::vector<std::string>>& InsertStatement::getValues() const {
    return values_;
}

// UpdateStatement implementation
UpdateStatement::UpdateStatement(std::string table, std::vector<std::pair<std::string, std::string>> setClauses, std::string whereClause)
    : table_(std::move(table)), setClauses_(std::move(setClauses)), whereClause_(std::move(whereClause)) {}

std::string UpdateStatement::toString() const {
    std::ostringstream oss;
    oss << "UPDATE " << table_ << " SET ";
    
    for (size_t i = 0; i < setClauses_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << setClauses_[i].first << " = '" << setClauses_[i].second << "'";
    }
    
    if (!whereClause_.empty()) {
        oss << " WHERE " << whereClause_;
    }
    
    return oss.str();
}

const std::string& UpdateStatement::getTable() const {
    return table_;
}

const std::vector<std::pair<std::string, std::string>>& UpdateStatement::getSetClauses() const {
    return setClauses_;
}

const std::string& UpdateStatement::getWhereClause() const {
    return whereClause_;
}

// DeleteStatement implementation
DeleteStatement::DeleteStatement(std::string table, std::string whereClause)
    : table_(std::move(table)), whereClause_(std::move(whereClause)) {}

std::string DeleteStatement::toString() const {
    std::ostringstream oss;
    oss << "DELETE FROM " << table_;
    
    if (!whereClause_.empty()) {
        oss << " WHERE " << whereClause_;
    }
    
    return oss.str();
}

const std::string& DeleteStatement::getTable() const {
    return table_;
}

const std::string& DeleteStatement::getWhereClause() const {
    return whereClause_;
}

// SQLParser implementation
class SQLParser::Impl {
public:
    Impl() = default;
    ~Impl() = default;
    
    std::unique_ptr<ASTNode> parse(const std::string& sql, std::string& errorMsg) {
        sql_ = sql;
        position_ = 0;
        line_ = 1;
        column_ = 1;
        
        try {
            // Skip leading whitespace
            skipWhitespace();
            
            // Check for SELECT statement
            if (position_ < sql_.length() && 
                (sql_[position_] == 'S' || sql_[position_] == 's')) {
                if (position_ + 5 < sql_.length()) {
                    std::string prefix = sql_.substr(position_, 6);
                    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
                    if (prefix == "SELECT") {
                        return parseSelectStatement();
                    }
                }
            }
            
            // Check for INSERT statement
            if (position_ < sql_.length() && 
                (sql_[position_] == 'I' || sql_[position_] == 'i')) {
                if (position_ + 5 < sql_.length()) {
                    std::string prefix = sql_.substr(position_, 6);
                    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
                    if (prefix == "INSERT") {
                        return parseInsertStatement();
                    }
                }
            }
            
            // Check for UPDATE statement
            if (position_ < sql_.length() && 
                (sql_[position_] == 'U' || sql_[position_] == 'u')) {
                if (position_ + 5 < sql_.length()) {
                    std::string prefix = sql_.substr(position_, 6);
                    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
                    if (prefix == "UPDATE") {
                        return parseUpdateStatement();
                    }
                }
            }
            
            // Check for DELETE statement
            if (position_ < sql_.length() && 
                (sql_[position_] == 'D' || sql_[position_] == 'd')) {
                if (position_ + 5 < sql_.length()) {
                    std::string prefix = sql_.substr(position_, 6);
                    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
                    if (prefix == "DELETE") {
                        return parseDeleteStatement();
                    }
                }
            }
            
            errorMsg = "Unsupported SQL statement";
            return nullptr;
        } catch (const std::exception& e) {
            errorMsg = std::string("Parse error: ") + e.what();
            return nullptr;
        }
    }
    
private:
    std::string sql_;
    size_t position_;
    int line_;
    int column_;
    
    void skipWhitespace() {
        while (position_ < sql_.length() && std::isspace(sql_[position_])) {
            if (sql_[position_] == '\n') {
                line_++;
                column_ = 1;
            } else {
                column_++;
            }
            position_++;
        }
    }
    
    Token getNextToken() {
        skipWhitespace();
        
        if (position_ >= sql_.length()) {
            return Token(TokenType::END_OF_FILE, "", line_, column_);
        }
        
        char ch = sql_[position_];
        int startLine = line_;
        int startColumn = column_;
        
        // Handle single character tokens
        switch (ch) {
            case ',':
                position_++;
                column_++;
                return Token(TokenType::COMMA, ",", startLine, startColumn);
            case ';':
                position_++;
                column_++;
                return Token(TokenType::SEMICOLON, ";", startLine, startColumn);
            case '(':
                position_++;
                column_++;
                return Token(TokenType::LPAREN, "(", startLine, startColumn);
            case ')':
                position_++;
                column_++;
                return Token(TokenType::RPAREN, ")", startLine, startColumn);
            case '*':
                position_++;
                column_++;
                return Token(TokenType::ASTERISK, "*", startLine, startColumn);
            case '.':
                position_++;
                column_++;
                return Token(TokenType::DOT, ".", startLine, startColumn);
            case '=':
                position_++;
                column_++;
                return Token(TokenType::EQUALS, "=", startLine, startColumn);
        }
        
        // Handle multi-character tokens
        if (std::isalpha(ch)) {
            std::string identifier;
            while (position_ < sql_.length() && 
                   (std::isalnum(sql_[position_]) || sql_[position_] == '_')) {
                identifier += sql_[position_];
                position_++;
                column_++;
            }
            
            // Convert to uppercase for comparison
            std::string upperId = identifier;
            std::transform(upperId.begin(), upperId.end(), upperId.begin(), ::toupper);
            
            // Check for keywords
            if (upperId == "SELECT") {
                return Token(TokenType::SELECT, identifier, startLine, startColumn);
            } else if (upperId == "FROM") {
                return Token(TokenType::FROM, identifier, startLine, startColumn);
            } else if (upperId == "WHERE") {
                return Token(TokenType::WHERE, identifier, startLine, startColumn);
            } else if (upperId == "AND") {
                return Token(TokenType::AND, identifier, startLine, startColumn);
            } else if (upperId == "OR") {
                return Token(TokenType::OR, identifier, startLine, startColumn);
            } else if (upperId == "NOT") {
                return Token(TokenType::NOT, identifier, startLine, startColumn);
            } else if (upperId == "INSERT") {
                return Token(TokenType::INSERT, identifier, startLine, startColumn);
            } else if (upperId == "INTO") {
                return Token(TokenType::INTO, identifier, startLine, startColumn);
            } else if (upperId == "VALUES") {
                return Token(TokenType::VALUES, identifier, startLine, startColumn);
            } else if (upperId == "UPDATE") {
                return Token(TokenType::UPDATE, identifier, startLine, startColumn);
            } else if (upperId == "SET") {
                return Token(TokenType::SET, identifier, startLine, startColumn);
            } else if (upperId == "DELETE") {
                return Token(TokenType::DELETE, identifier, startLine, startColumn);
            } else if (upperId == "JOIN") {
                return Token(TokenType::JOIN, identifier, startLine, startColumn);
            } else if (upperId == "ON") {
                return Token(TokenType::ON, identifier, startLine, startColumn);
            } else if (upperId == "AS") {
                return Token(TokenType::IDENTIFIER, identifier, startLine, startColumn);
            } else {
                return Token(TokenType::IDENTIFIER, identifier, startLine, startColumn);
            }
        }
        
        // Handle string literals
        if (ch == '\'') {
            position_++;
            column_++;
            std::string literal;
            while (position_ < sql_.length() && sql_[position_] != '\'') {
                literal += sql_[position_];
                position_++;
                column_++;
            }
            if (position_ < sql_.length()) {
                position_++; // Skip closing quote
                column_++;
            }
            return Token(TokenType::STRING_LITERAL, literal, startLine, startColumn);
        }
        
        // Handle numbers
        if (std::isdigit(ch)) {
            std::string number;
            while (position_ < sql_.length() && std::isdigit(sql_[position_])) {
                number += sql_[position_];
                position_++;
                column_++;
            }
            return Token(TokenType::NUMBER, number, startLine, startColumn);
        }
        
        // Unknown token
        position_++;
        column_++;
        return Token(TokenType::UNKNOWN, std::string(1, ch), startLine, startColumn);
    }
    
    std::unique_ptr<ASTNode> parseSelectStatement() {
        // Save current position to backtrack if needed
        size_t savedPosition = position_;
        int savedLine = line_;
        int savedColumn = column_;
        
        // Skip SELECT keyword
        Token token = getNextToken();
        
        // Parse columns
        std::vector<std::string> columns;
        token = getNextToken();
        
        if (token.type == TokenType::ASTERISK) {
            // SELECT * - no specific columns
            token = getNextToken();
        } else if (token.type == TokenType::IDENTIFIER) {
            columns.push_back(token.value);
            token = getNextToken();
            
            // Parse additional columns
            while (token.type == TokenType::COMMA) {
                token = getNextToken();
                if (token.type == TokenType::IDENTIFIER) {
                    columns.push_back(token.value);
                    token = getNextToken();
                } else {
                    throw std::runtime_error("Expected identifier after comma");
                }
            }
        } else if (token.type == TokenType::LPAREN) {
            // This might be a subquery in the column list
            // For now, we'll handle simple cases and throw for complex ones
            throw std::runtime_error("Subqueries in column list not yet supported");
        } else {
            throw std::runtime_error("Expected * or identifier after SELECT");
        }
        
        // Parse FROM clause
        if (token.type != TokenType::FROM) {
            throw std::runtime_error("Expected FROM clause");
        }
        
        // Check what comes after FROM - could be a table name or a subquery
        token = getNextToken();
        std::unique_ptr<SelectStatement> selectStmt;
        
        if (token.type == TokenType::LPAREN) {
            // This is a subquery in the FROM clause
            // Parse the subquery by recursively calling parseSelectStatement
            auto subquerySelect = parseSelectStatement();
            if (!subquerySelect) {
                throw std::runtime_error("Failed to parse subquery");
            }
            
            // Expect closing parenthesis
            Token closingParen = getNextToken();
            if (closingParen.type != TokenType::RPAREN) {
                throw std::runtime_error("Expected closing parenthesis after subquery");
            }
            
            // Expect AS keyword and alias
            Token asToken = getNextToken();
            if (asToken.type != TokenType::IDENTIFIER) {
                throw std::runtime_error("Expected alias after subquery");
            }
            
            std::string alias = asToken.value;
            
            // Create the main select statement with empty table name
            selectStmt = std::make_unique<SelectStatement>(std::move(columns), "");
            
            // Create subquery object
            auto subquery = std::make_unique<Subquery>(
                std::unique_ptr<SelectStatement>(static_cast<SelectStatement*>(subquerySelect.release())), 
                std::move(alias)
            );
            
            // Add subquery to the main select statement
            selectStmt->addSubquery(std::move(subquery));
            
            // Get next token
            token = getNextToken();
        } else if (token.type == TokenType::IDENTIFIER) {
            // Regular table name
            std::string tableName = token.value;
            selectStmt = std::make_unique<SelectStatement>(std::move(columns), std::move(tableName));
            
            // Get next token
            token = getNextToken();
        } else {
            throw std::runtime_error("Expected table name or subquery after FROM");
        }
        
        // Parse JOIN clauses and other parts
        while (token.type == TokenType::JOIN) {
            // Parse table name for JOIN
            token = getNextToken();
            if (token.type != TokenType::IDENTIFIER) {
                throw std::runtime_error("Expected table name after JOIN");
            }
            
            std::string joinTable = token.value;
            
            // Parse ON keyword
            token = getNextToken();
            if (token.type != TokenType::ON) {
                throw std::runtime_error("Expected ON after JOIN table name");
            }
            
            // For simplicity, we'll just capture the rest of the condition as the JOIN condition
            std::string condition;
            while (position_ < sql_.length() && 
                   sql_[position_] != ';' && 
                   sql_[position_] != 'J' &&  // Next JOIN
                   sql_[position_] != 'W') {  // WHERE clause
                condition += sql_[position_];
                position_++;
            }
            
            // Trim whitespace from condition
            condition.erase(0, condition.find_first_not_of(" \t\n\r"));
            condition.erase(condition.find_last_not_of(" \t\n\r") + 1);
            
            // Add the JOIN clause
            JoinClause join;
            join.table = std::move(joinTable);
            join.condition = std::move(condition);
            selectStmt->addJoin(join);
            
            // Get next token to check for more JOINs
            skipWhitespace();
            if (position_ < sql_.length()) {
                // Check for next keyword
                if (position_ + 3 < sql_.length()) {
                    std::string prefix = sql_.substr(position_, 4);
                    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
                    if (prefix == "JOIN") {
                        token = getNextToken();
                        continue;
                    }
                }
                if (position_ + 4 < sql_.length()) {
                    std::string prefix = sql_.substr(position_, 5);
                    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);
                    if (prefix == "WHERE") {
                        token = getNextToken();
                        break;
                    }
                }
                token = getNextToken();
            } else {
                break;
            }
        }
        
        // Return the SELECT statement AST node
        return std::move(selectStmt);
    }
    
    std::unique_ptr<ASTNode> parseInsertStatement() {
        // Skip INSERT keyword
        Token token = getNextToken();
        
        // Parse INTO keyword
        token = getNextToken();
        if (token.type != TokenType::INTO) {
            throw std::runtime_error("Expected INTO after INSERT");
        }
        
        // Parse table name
        token = getNextToken();
        if (token.type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected table name after INTO");
        }
        
        std::string tableName = token.value;
        
        // Parse columns (optional)
        std::vector<std::string> columns;
        token = getNextToken();
        
        if (token.type == TokenType::LPAREN) {
            // Parse column list
            token = getNextToken();
            
            if (token.type == TokenType::IDENTIFIER) {
                columns.push_back(token.value);
                token = getNextToken();
                
                // Parse additional columns
                while (token.type == TokenType::COMMA) {
                    token = getNextToken();
                    if (token.type == TokenType::IDENTIFIER) {
                        columns.push_back(token.value);
                        token = getNextToken();
                    } else {
                        throw std::runtime_error("Expected identifier after comma");
                    }
                }
            }
            
            // Expect closing parenthesis
            if (token.type != TokenType::RPAREN) {
                throw std::runtime_error("Expected closing parenthesis after column list");
            }
            
            token = getNextToken();
        }
        
        // Parse VALUES keyword
        if (token.type != TokenType::VALUES) {
            throw std::runtime_error("Expected VALUES keyword");
        }
        
        // Parse value lists
        std::vector<std::vector<std::string>> values;
        
        do {
            token = getNextToken();
            
            // Expect opening parenthesis
            if (token.type != TokenType::LPAREN) {
                throw std::runtime_error("Expected opening parenthesis for value list");
            }
            
            // Parse values in this row
            std::vector<std::string> rowValues;
            token = getNextToken();
            
            // Parse first value
            if (token.type == TokenType::STRING_LITERAL || token.type == TokenType::NUMBER) {
                rowValues.push_back(token.value);
                token = getNextToken();
                
                // Parse additional values
                while (token.type == TokenType::COMMA) {
                    token = getNextToken();
                    if (token.type == TokenType::STRING_LITERAL || token.type == TokenType::NUMBER) {
                        rowValues.push_back(token.value);
                        token = getNextToken();
                    } else {
                        throw std::runtime_error("Expected string literal or number after comma");
                    }
                }
            }
            
            // Expect closing parenthesis
            if (token.type != TokenType::RPAREN) {
                throw std::runtime_error("Expected closing parenthesis after value list");
            }
            
            values.push_back(std::move(rowValues));
            
            // Check if there are more value lists
            token = getNextToken();
        } while (token.type == TokenType::COMMA);
        
        // Create and return the INSERT statement AST node
        return std::make_unique<InsertStatement>(std::move(tableName), std::move(columns), std::move(values));
    }
    
    std::unique_ptr<ASTNode> parseUpdateStatement() {
        // Skip UPDATE keyword
        Token token = getNextToken();
        
        // Parse table name
        token = getNextToken();
        if (token.type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected table name after UPDATE");
        }
        
        std::string tableName = token.value;
        
        // Parse SET keyword
        token = getNextToken();
        if (token.type != TokenType::SET) {
            throw std::runtime_error("Expected SET after table name");
        }
        
        // Parse SET clauses
        std::vector<std::pair<std::string, std::string>> setClauses;
        
        do {
            // Parse column name
            token = getNextToken();
            if (token.type != TokenType::IDENTIFIER) {
                throw std::runtime_error("Expected column name in SET clause");
            }
            
            std::string columnName = token.value;
            
            // Parse equals sign
            token = getNextToken();
            if (token.type != TokenType::EQUALS) {
                throw std::runtime_error("Expected = in SET clause");
            }
            
            // Parse value
            token = getNextToken();
            if (token.type != TokenType::STRING_LITERAL && token.type != TokenType::NUMBER) {
                throw std::runtime_error("Expected value in SET clause");
            }
            
            std::string value = token.value;
            
            // Add to set clauses
            setClauses.emplace_back(std::move(columnName), std::move(value));
            
            // Check if there are more SET clauses
            token = getNextToken();
        } while (token.type == TokenType::COMMA);
        
        // Parse WHERE clause (optional)
        std::string whereClause;
        if (token.type == TokenType::WHERE) {
            // For simplicity, we'll just capture the rest of the statement as the WHERE clause
            // In a more advanced implementation, we would parse the WHERE clause properly
            while (position_ < sql_.length() && sql_[position_] != ';') {
                whereClause += sql_[position_];
                position_++;
            }
            
            // Skip trailing semicolon if present
            if (position_ < sql_.length() && sql_[position_] == ';') {
                position_++;
            }
        }
        
        // Create and return the UPDATE statement AST node
        return std::make_unique<UpdateStatement>(std::move(tableName), std::move(setClauses), std::move(whereClause));
    }
    
    std::unique_ptr<ASTNode> parseDeleteStatement() {
        // Skip DELETE keyword
        Token token = getNextToken();
        
        // Parse FROM keyword
        token = getNextToken();
        if (token.type != TokenType::FROM) {
            throw std::runtime_error("Expected FROM after DELETE");
        }
        
        // Parse table name
        token = getNextToken();
        if (token.type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected table name after FROM");
        }
        
        std::string tableName = token.value;
        
        // Parse WHERE clause (optional)
        std::string whereClause;
        token = getNextToken();
        if (token.type == TokenType::WHERE) {
            // For simplicity, we'll just capture the rest of the statement as the WHERE clause
            // In a more advanced implementation, we would parse the WHERE clause properly
            while (position_ < sql_.length() && sql_[position_] != ';') {
                whereClause += sql_[position_];
                position_++;
            }
            
            // Skip trailing semicolon if present
            if (position_ < sql_.length() && sql_[position_] == ';') {
                position_++;
            }
        }
        
        // Create and return the DELETE statement AST node
        return std::make_unique<DeleteStatement>(std::move(tableName), std::move(whereClause));
    }
};

SQLParser::SQLParser() : pImpl(std::make_unique<Impl>()) {}

SQLParser::~SQLParser() = default;

std::unique_ptr<ASTNode> SQLParser::parse(const std::string& sql, std::string& errorMsg) {
    return pImpl->parse(sql, errorMsg);
}

} // namespace query
} // namespace phantomdb