#include "rbac.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

namespace phantomdb {
namespace security {

// User information structure
struct UserInfo {
    std::string username;
    std::string passwordHash; // In a real implementation, this would be a hashed password
    UserRole role;
};

// RBACManager implementation
class RBACManager::Impl {
public:
    Impl() {
        // Initialize default role permissions
        initializeDefaultPermissions();
    }
    
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "Initializing RBAC Manager..." << std::endl;
        
        // Create default admin user
        createUser("admin", "admin123");
        assignRole("admin", UserRole::ADMIN);
        
        return true;
    }
    
    void shutdown() {
        std::cout << "Shutting down RBAC Manager..." << std::endl;
    }
    
    bool createUser(const std::string& username, const std::string& password) {
        // Check if user already exists
        if (users.find(username) != users.end()) {
            std::cerr << "User already exists: " << username << std::endl;
            return false;
        }
        
        // Create new user with default reader role
        UserInfo user;
        user.username = username;
        user.passwordHash = hashPassword(password); // Simple hash for demo
        user.role = UserRole::READER; // Default role
        
        users[username] = user;
        std::cout << "Created user: " << username << std::endl;
        return true;
    }
    
    bool authenticateUser(const std::string& username, const std::string& password) {
        auto it = users.find(username);
        if (it == users.end()) {
            std::cerr << "User not found: " << username << std::endl;
            return false;
        }
        
        // In a real implementation, we would hash the provided password and compare
        // For this demo, we'll do a simple string comparison
        return it->second.passwordHash == hashPassword(password);
    }
    
    bool assignRole(const std::string& username, UserRole role) {
        auto it = users.find(username);
        if (it == users.end()) {
            std::cerr << "User not found: " << username << std::endl;
            return false;
        }
        
        it->second.role = role;
        std::cout << "Assigned role " << static_cast<int>(role) << " to user: " << username << std::endl;
        return true;
    }
    
    UserRole getUserRole(const std::string& username) const {
        auto it = users.find(username);
        if (it == users.end()) {
            return UserRole::READER; // Default role
        }
        return it->second.role;
    }
    
    bool hasPermission(const std::string& username, Permission permission) const {
        auto it = users.find(username);
        if (it == users.end()) {
            return false;
        }
        
        UserRole role = it->second.role;
        return roleHasPermission(role, permission);
    }
    
    bool roleHasPermission(UserRole role, Permission permission) const {
        auto it = rolePermissions.find(role);
        if (it == rolePermissions.end()) {
            return false;
        }
        
        return it->second.find(permission) != it->second.end();
    }
    
    bool addPermissionToRole(UserRole role, Permission permission) {
        rolePermissions[role].insert(permission);
        std::cout << "Added permission to role " << static_cast<int>(role) << std::endl;
        return true;
    }
    
    bool removePermissionFromRole(UserRole role, Permission permission) {
        auto it = rolePermissions.find(role);
        if (it == rolePermissions.end()) {
            return false;
        }
        
        it->second.erase(permission);
        std::cout << "Removed permission from role " << static_cast<int>(role) << std::endl;
        return true;
    }
    
    std::vector<std::string> listUsers() const {
        std::vector<std::string> userList;
        for (const auto& pair : users) {
            userList.push_back(pair.first);
        }
        return userList;
    }
    
    std::vector<Permission> listUserPermissions(const std::string& username) const {
        std::vector<Permission> permissions;
        
        auto userIt = users.find(username);
        if (userIt == users.end()) {
            return permissions;
        }
        
        UserRole role = userIt->second.role;
        auto roleIt = rolePermissions.find(role);
        if (roleIt == rolePermissions.end()) {
            return permissions;
        }
        
        for (const auto& perm : roleIt->second) {
            permissions.push_back(perm);
        }
        
        return permissions;
    }
    
private:
    std::unordered_map<std::string, UserInfo> users;
    std::unordered_map<UserRole, std::unordered_set<Permission>> rolePermissions;
    
    void initializeDefaultPermissions() {
        // Admin role has all permissions
        std::unordered_set<Permission> adminPermissions = {
            Permission::CREATE_DATABASE,
            Permission::DROP_DATABASE,
            Permission::CREATE_TABLE,
            Permission::DROP_TABLE,
            Permission::INSERT,
            Permission::SELECT,
            Permission::UPDATE,
            Permission::DELETE,
            Permission::ALTER_TABLE,
            Permission::CREATE_INDEX,
            Permission::DROP_INDEX,
            Permission::EXECUTE_QUERY,
            Permission::MANAGE_USERS,
            Permission::MANAGE_ROLES
        };
        rolePermissions[UserRole::ADMIN] = adminPermissions;
        
        // Writer role has read and write permissions
        std::unordered_set<Permission> writerPermissions = {
            Permission::SELECT,
            Permission::INSERT,
            Permission::UPDATE,
            Permission::DELETE,
            Permission::EXECUTE_QUERY
        };
        rolePermissions[UserRole::WRITER] = writerPermissions;
        
        // Reader role has only read permissions
        std::unordered_set<Permission> readerPermissions = {
            Permission::SELECT,
            Permission::EXECUTE_QUERY
        };
        rolePermissions[UserRole::READER] = readerPermissions;
    }
    
    // Simple password hashing function (for demo purposes only)
    std::string hashPassword(const std::string& password) {
        // In a real implementation, use a proper hashing algorithm like bcrypt
        // For this demo, we'll just reverse the string
        std::string hashed = password;
        std::reverse(hashed.begin(), hashed.end());
        return hashed;
    }
};

RBACManager::RBACManager() : pImpl(std::make_unique<Impl>()) {
    std::cout << "RBAC Manager created" << std::endl;
}

RBACManager::~RBACManager() {
    std::cout << "RBAC Manager destroyed" << std::endl;
}

bool RBACManager::initialize() {
    return pImpl->initialize();
}

void RBACManager::shutdown() {
    pImpl->shutdown();
}

bool RBACManager::createUser(const std::string& username, const std::string& password) {
    return pImpl->createUser(username, password);
}

bool RBACManager::authenticateUser(const std::string& username, const std::string& password) {
    return pImpl->authenticateUser(username, password);
}

bool RBACManager::assignRole(const std::string& username, UserRole role) {
    return pImpl->assignRole(username, role);
}

UserRole RBACManager::getUserRole(const std::string& username) const {
    return pImpl->getUserRole(username);
}

bool RBACManager::hasPermission(const std::string& username, Permission permission) const {
    return pImpl->hasPermission(username, permission);
}

bool RBACManager::roleHasPermission(UserRole role, Permission permission) const {
    return pImpl->roleHasPermission(role, permission);
}

bool RBACManager::addPermissionToRole(UserRole role, Permission permission) {
    return pImpl->addPermissionToRole(role, permission);
}

bool RBACManager::removePermissionFromRole(UserRole role, Permission permission) {
    return pImpl->removePermissionFromRole(role, permission);
}

std::vector<std::string> RBACManager::listUsers() const {
    return pImpl->listUsers();
}

std::vector<Permission> RBACManager::listUserPermissions(const std::string& username) const {
    return pImpl->listUserPermissions(username);
}

} // namespace security
} // namespace phantomdb