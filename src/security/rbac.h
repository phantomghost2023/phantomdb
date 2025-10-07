#ifndef PHANTOMDB_RBAC_H
#define PHANTOMDB_RBAC_H

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace phantomdb {
namespace security {

// User roles
enum class UserRole {
    ADMIN,     // Full access
    READER,    // Read-only access
    WRITER     // Read and write access
};

// Permission types
enum class Permission {
    CREATE_DATABASE,
    DROP_DATABASE,
    CREATE_TABLE,
    DROP_TABLE,
    INSERT,
    SELECT,
    UPDATE,
    DELETE,
    ALTER_TABLE,
    CREATE_INDEX,
    DROP_INDEX,
    EXECUTE_QUERY,
    MANAGE_USERS,
    MANAGE_ROLES
};

class RBACManager {
public:
    RBACManager();
    ~RBACManager();
    
    // Initialize the RBAC manager
    bool initialize();
    
    // Shutdown the RBAC manager
    void shutdown();
    
    // Create a new user
    bool createUser(const std::string& username, const std::string& password);
    
    // Authenticate a user
    bool authenticateUser(const std::string& username, const std::string& password);
    
    // Assign a role to a user
    bool assignRole(const std::string& username, UserRole role);
    
    // Get user's role
    UserRole getUserRole(const std::string& username) const;
    
    // Check if a user has a specific permission
    bool hasPermission(const std::string& username, Permission permission) const;
    
    // Check if a role has a specific permission
    bool roleHasPermission(UserRole role, Permission permission) const;
    
    // Add a permission to a role
    bool addPermissionToRole(UserRole role, Permission permission);
    
    // Remove a permission from a role
    bool removePermissionFromRole(UserRole role, Permission permission);
    
    // List all users
    std::vector<std::string> listUsers() const;
    
    // List all permissions for a user
    std::vector<Permission> listUserPermissions(const std::string& username) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace security
} // namespace phantomdb

#endif // PHANTOMDB_RBAC_H