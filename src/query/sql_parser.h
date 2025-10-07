#ifndef PHANTOMDB_SQL_PARSER_H
#define PHANTOMDB_SQL_PARSER_H

#include <string>
#include <memory>
#include <vector>

namespace phantomdb {
namespace query {

// Forward declarations
class ASTNode;
class SelectStatement;
class InsertStatement;
class UpdateStatement;
class DeleteStatement;
class FromClause;
class WhereClause;

// Token types
enum class TokenType {
    SELECT,
    FROM,
    WHERE,
    INSERT,
    UPDATE,
    DELETE,
    CREATE,
    DROP,
    TABLE,
    INDEX,
    INTO,
    VALUES,
    SET,
    JOIN,
    ON,
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER,
    COMMA,
    SEMICOLON,
    EQUALS,
    NOT_EQUALS,
    LESS_THAN,
    GREATER_THAN,
    LESS_EQUAL,
    GREATER_EQUAL,
    AND,
    OR,
    NOT,
    LPAREN,
    RPAREN,
    ASTERISK,
    DOT,
    END_OF_FILE,
    UNKNOWN
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
};

// SQL Parser class
class SQLParser {
public:
    SQLParser();
    ~SQLParser();
    
    // Parse a SQL query and return the AST
    std::unique_ptr<ASTNode> parse(const std::string& sql, std::string& errorMsg);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// AST Node base class
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
};

// Forward declaration for Subquery
class Subquery;

// Join clause structure
struct JoinClause {
    std::string table;
    std::string condition;
};

// Select statement node
class SelectStatement : public ASTNode {
public:
    SelectStatement(std::vector<std::string> columns, std::string table);
    virtual ~SelectStatement() = default;
    
    std::string toString() const override;
    
    const std::vector<std::string>& getColumns() const;
    const std::string& getTable() const;
    const std::vector<JoinClause>& getJoins() const;
    const std::vector<std::unique_ptr<Subquery>>& getSubqueries() const;
    
    void addJoin(const JoinClause& join);
    void addSubquery(std::unique_ptr<Subquery> subquery);
    
private:
    std::vector<std::string> columns_;
    std::string table_;
    std::vector<JoinClause> joins_;
    std::vector<std::unique_ptr<Subquery>> subqueries_;
};

// Subquery structure
class Subquery : public ASTNode {
public:
    Subquery(std::unique_ptr<SelectStatement> selectStmt, std::string alias);
    virtual ~Subquery() = default;
    
    std::string toString() const override;
    
    const SelectStatement* getSelectStatement() const;
    const std::string& getAlias() const;
    
private:
    std::unique_ptr<SelectStatement> selectStmt_;
    std::string alias_;
};

// Insert statement node
class InsertStatement : public ASTNode {
public:
    InsertStatement(std::string table, std::vector<std::string> columns, std::vector<std::vector<std::string>> values);
    virtual ~InsertStatement() = default;
    
    std::string toString() const override;
    
    const std::string& getTable() const;
    const std::vector<std::string>& getColumns() const;
    const std::vector<std::vector<std::string>>& getValues() const;
    
private:
    std::string table_;
    std::vector<std::string> columns_;
    std::vector<std::vector<std::string>> values_;
};

// Update statement node
class UpdateStatement : public ASTNode {
public:
    UpdateStatement(std::string table, std::vector<std::pair<std::string, std::string>> setClauses, std::string whereClause);
    virtual ~UpdateStatement() = default;
    
    std::string toString() const override;
    
    const std::string& getTable() const;
    const std::vector<std::pair<std::string, std::string>>& getSetClauses() const;
    const std::string& getWhereClause() const;
    
private:
    std::string table_;
    std::vector<std::pair<std::string, std::string>> setClauses_;
    std::string whereClause_;
};

// Delete statement node
class DeleteStatement : public ASTNode {
public:
    DeleteStatement(std::string table, std::string whereClause);
    virtual ~DeleteStatement() = default;
    
    std::string toString() const override;
    
    const std::string& getTable() const;
    const std::string& getWhereClause() const;
    
private:
    std::string table_;
    std::string whereClause_;
};

} // namespace query
} // namespace phantomdb

#endif // PHANTOMDB_SQL_PARSER_H