# PhantomDB Security Documentation

This document provides comprehensive documentation for the security features in PhantomDB, including Role-Based Access Control (RBAC) and audit logging capabilities.

## Overview

PhantomDB implements enterprise-grade security features to protect data and ensure compliance with regulatory requirements. The security architecture includes:

1. **Role-Based Access Control (RBAC)** - Fine-grained access control based on user roles
2. **Audit Logging** - Comprehensive tracking of all database operations
3. **Authentication** - Secure user authentication mechanisms
4. **Authorization** - Permission-based access control
5. **Data Protection** - Encryption and secure data handling

## Role-Based Access Control (RBAC)

PhantomDB implements a flexible RBAC system that allows administrators to define roles and assign permissions to users.

### User Roles

PhantomDB defines three primary user roles:

1. **ADMIN** - Full access to all database operations
2. **WRITER** - Read and write access to data
3. **READER** - Read-only access to data

### Permissions

The RBAC system supports the following permissions:

- `CREATE_DATABASE` - Create new databases
- `DROP_DATABASE` - Drop existing databases
- `CREATE_TABLE` - Create new tables
- `DROP_TABLE` - Drop existing tables
- `INSERT` - Insert data into tables
- `SELECT` - Select data from tables
- `UPDATE` - Update existing data
- `DELETE` - Delete data from tables
- `ALTER_TABLE` - Modify table structure
- `CREATE_INDEX` - Create indexes
- `DROP_INDEX` - Drop indexes
- `EXECUTE_QUERY` - Execute custom queries
- `MANAGE_USERS` - Manage user accounts
- `MANAGE_ROLES` - Manage user roles

### Role Permissions

Each role has a predefined set of permissions:

- **ADMIN**: All permissions
- **WRITER**: SELECT, INSERT, UPDATE, DELETE, EXECUTE_QUERY
- **READER**: SELECT, EXECUTE_QUERY

### Using RBAC

```cpp
#include "rbac.h"

using namespace phantomdb::security;

// Create RBAC manager
RBACManager rbac;

// Initialize the RBAC system
rbac.initialize();

// Create a new user
rbac.createUser("john_doe", "secure_password");

// Assign a role to a user
rbac.assignRole("john_doe", UserRole::WRITER);

// Check if a user has a specific permission
if (rbac.hasPermission("john_doe", Permission::INSERT)) {
    // User can insert data
}

// List all users
std::vector<std::string> users = rbac.listUsers();

// List permissions for a specific user
std::vector<Permission> permissions = rbac.listUserPermissions("john_doe");
```

## Audit Logging

PhantomDB provides comprehensive audit logging to track all database operations for security, compliance, and troubleshooting purposes.

### Audit Event Types

The audit system tracks the following event types:

- `USER_LOGIN` - User authentication
- `USER_LOGOUT` - User logout
- `DATABASE_CREATE` - Database creation
- `DATABASE_DROP` - Database deletion
- `TABLE_CREATE` - Table creation
- `TABLE_DROP` - Table deletion
- `TABLE_ALTER` - Table modification
- `INDEX_CREATE` - Index creation
- `INDEX_DROP` - Index deletion
- `DATA_INSERT` - Data insertion
- `DATA_SELECT` - Data selection
- `DATA_UPDATE` - Data update
- `DATA_DELETE` - Data deletion
- `TRANSACTION_START` - Transaction initiation
- `TRANSACTION_COMMIT` - Transaction commit
- `TRANSACTION_ROLLBACK` - Transaction rollback
- `QUERY_EXECUTE` - Query execution
- `PERMISSION_GRANTED` - Permission assignment
- `PERMISSION_REVOKED` - Permission removal
- `ROLE_ASSIGNED` - Role assignment
- `ROLE_REVOKED` - Role removal

### Using Audit Logging

```cpp
#include "audit_logger.h"

using namespace phantomdb::audit;

// Create audit logger
AuditLogger logger;

// Initialize the audit logger
logger.initialize("/var/log/phantomdb/audit.log");

// Log a user login
logger.logUserLogin("john_doe", "192.168.1.100");

// Log a database creation
logger.logDatabaseCreate("john_doe", "myapp");

// Log a data insert operation
logger.logDataInsert("john_doe", "myapp", "users", "user_123");

// Log a query execution
logger.logQueryExecute("john_doe", "myapp", "SELECT * FROM users WHERE id = 1", 2.5);

// Get recent audit events
std::vector<AuditEvent> recentEvents = logger.getRecentEvents(100);

// Get events for a specific user
std::vector<AuditEvent> userEvents = logger.getEventsForUser("john_doe");

// Get events for a specific time range
auto start = std::chrono::system_clock::now() - std::chrono::hours(24);
auto end = std::chrono::system_clock::now();
std::vector<AuditEvent> timeRangeEvents = logger.getEventsForTimeRange(start, end);
```

## Integration with Database Operations

Security features are integrated throughout the database system to ensure consistent access control and auditing.

### Query Processor Integration

The query processor checks user permissions before executing operations:

```cpp
class SecureQueryProcessor {
public:
    bool executeQuery(const std::string& user, const std::string& query) {
        // Check if user has permission to execute queries
        if (!rbac_.hasPermission(user, Permission::EXECUTE_QUERY)) {
            auditLogger_.logQueryExecute(user, "", query, 0.0);
            return false; // Permission denied
        }
        
        // Process the query
        // ...
        
        return true;
    }
    
private:
    RBACManager rbac_;
    AuditLogger auditLogger_;
};
```

### Transaction Manager Integration

The transaction manager enforces security during transaction operations:

```cpp
class SecureTransactionManager {
public:
    std::shared_ptr<Transaction> beginTransaction(const std::string& user) {
        // Check if user has permission to begin transactions
        if (!rbac_.hasPermission(user, Permission::EXECUTE_QUERY)) {
            auditLogger_.logTransactionStart(user, "");
            return nullptr; // Permission denied
        }
        
        // Begin the transaction
        auto transaction = transactionManager_.beginTransaction();
        
        // Log the transaction start
        auditLogger_.logTransactionStart(user, std::to_string(transaction->getId()));
        
        return transaction;
    }
    
private:
    RBACManager rbac_;
    AuditLogger auditLogger_;
    TransactionManager transactionManager_;
};
```

## Security Best Practices

### User Management

1. **Strong Passwords**: Enforce strong password policies
2. **Regular Audits**: Periodically review user accounts and permissions
3. **Least Privilege**: Grant users only the minimum permissions they need
4. **Account Expiration**: Implement account expiration for temporary users

### Audit Logging

1. **Log Retention**: Maintain audit logs for the required period
2. **Log Protection**: Protect audit logs from unauthorized access
3. **Log Analysis**: Regularly analyze audit logs for suspicious activity
4. **Alerting**: Set up alerts for critical security events

### Data Protection

1. **Encryption**: Encrypt sensitive data at rest and in transit
2. **Backup Security**: Secure database backups
3. **Network Security**: Use secure network protocols
4. **Access Controls**: Implement network-level access controls

## Configuration

### RBAC Configuration

RBAC can be configured through the database configuration:

```json
{
    "security": {
        "rbac": {
            "enabled": true,
            "default_role": "READER",
            "roles": {
                "ADMIN": {
                    "permissions": ["*"]
                },
                "WRITER": {
                    "permissions": ["SELECT", "INSERT", "UPDATE", "DELETE", "EXECUTE_QUERY"]
                },
                "READER": {
                    "permissions": ["SELECT", "EXECUTE_QUERY"]
                }
            }
        }
    }
}
```

### Audit Logging Configuration

Audit logging can be configured through the database configuration:

```json
{
    "security": {
        "audit": {
            "enabled": true,
            "log_file": "/var/log/phantomdb/audit.log",
            "max_file_size": "100MB",
            "max_files": 10,
            "events": {
                "USER_LOGIN": true,
                "USER_LOGOUT": true,
                "DATABASE_CREATE": true,
                "DATABASE_DROP": true,
                "TABLE_CREATE": true,
                "TABLE_DROP": true,
                "DATA_INSERT": true,
                "DATA_SELECT": false,
                "DATA_UPDATE": true,
                "DATA_DELETE": true
            }
        }
    }
}
```

## API Reference

### RBACManager

- `bool initialize()` - Initialize the RBAC system
- `void shutdown()` - Shutdown the RBAC system
- `bool createUser(const std::string& username, const std::string& password)` - Create a new user
- `bool authenticateUser(const std::string& username, const std::string& password)` - Authenticate a user
- `bool assignRole(const std::string& username, UserRole role)` - Assign a role to a user
- `UserRole getUserRole(const std::string& username) const` - Get a user's role
- `bool hasPermission(const std::string& username, Permission permission) const` - Check if a user has a permission
- `std::vector<std::string> listUsers() const` - List all users
- `std::vector<Permission> listUserPermissions(const std::string& username) const` - List a user's permissions

### AuditLogger

- `bool initialize(const std::string& logFilePath)` - Initialize the audit logger
- `void shutdown()` - Shutdown the audit logger
- `void logEvent(const AuditEvent& event)` - Log a custom audit event
- `void logUserLogin(const std::string& username, const std::string& ipAddress)` - Log a user login
- `void logUserLogout(const std::string& username, const std::string& ipAddress)` - Log a user logout
- `void logDatabaseCreate(const std::string& username, const std::string& databaseName)` - Log database creation
- `void logDatabaseDrop(const std::string& username, const std::string& databaseName)` - Log database deletion
- `std::vector<AuditEvent> getEventsForUser(const std::string& username) const` - Get events for a user
- `std::vector<AuditEvent> getEventsForTimeRange(const std::chrono::system_clock::time_point& start, const std::chrono::system_clock::time_point& end) const` - Get events for a time range
- `std::vector<AuditEvent> getRecentEvents(size_t count) const` - Get recent events

## Troubleshooting

### Common Issues

1. **Permission Denied**: Check that the user has the required permissions
2. **Audit Log Not Writing**: Verify that the log file path is writable
3. **Authentication Failure**: Check username and password
4. **Role Assignment Failure**: Verify that the user exists

### Security Monitoring

1. **Failed Login Attempts**: Monitor for excessive failed login attempts
2. **Privilege Escalation**: Monitor for unusual permission changes
3. **Data Access Patterns**: Monitor for unusual data access patterns
4. **Configuration Changes**: Monitor for security configuration changes

## Compliance

PhantomDB's security features help meet compliance requirements for:

- **GDPR**: Data protection and privacy
- **HIPAA**: Healthcare data security
- **SOX**: Financial data integrity
- **PCI DSS**: Payment card security

## Future Enhancements

Planned security enhancements include:

- Multi-factor authentication
- LDAP/Active Directory integration
- Advanced encryption options
- Real-time threat detection
- Automated compliance reporting
- Security incident response