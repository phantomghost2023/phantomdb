#include "rbac.h"
#include <iostream>
#include <vector>

using namespace phantomdb::security;

int main() {
    std::cout << "Testing RBAC functionality..." << std::endl;
    
    // Initialize the RBAC manager
    RBACManager rbacManager;
    if (!rbacManager.initialize()) {
        std::cerr << "Failed to initialize RBAC manager" << std::endl;
        return 1;
    }
    
    // Test 1: Create users
    std::cout << "\n--- Test 1: Create users ---" << std::endl;
    if (rbacManager.createUser("alice", "password123")) {
        std::cout << "Created user alice" << std::endl;
    } else {
        std::cerr << "Failed to create user alice" << std::endl;
        return 1;
    }
    
    if (rbacManager.createUser("bob", "password456")) {
        std::cout << "Created user bob" << std::endl;
    } else {
        std::cerr << "Failed to create user bob" << std::endl;
        return 1;
    }
    
    // Test 2: Authenticate users
    std::cout << "\n--- Test 2: Authenticate users ---" << std::endl;
    if (rbacManager.authenticateUser("alice", "password123")) {
        std::cout << "Authenticated alice successfully" << std::endl;
    } else {
        std::cerr << "Failed to authenticate alice" << std::endl;
        return 1;
    }
    
    if (rbacManager.authenticateUser("bob", "password456")) {
        std::cout << "Authenticated bob successfully" << std::endl;
    } else {
        std::cerr << "Failed to authenticate bob" << std::endl;
        return 1;
    }
    
    // Test 3: Assign roles
    std::cout << "\n--- Test 3: Assign roles ---" << std::endl;
    if (rbacManager.assignRole("alice", UserRole::ADMIN)) {
        std::cout << "Assigned ADMIN role to alice" << std::endl;
    } else {
        std::cerr << "Failed to assign ADMIN role to alice" << std::endl;
        return 1;
    }
    
    if (rbacManager.assignRole("bob", UserRole::WRITER)) {
        std::cout << "Assigned WRITER role to bob" << std::endl;
    } else {
        std::cerr << "Failed to assign WRITER role to bob" << std::endl;
        return 1;
    }
    
    // Test 4: Check user roles
    std::cout << "\n--- Test 4: Check user roles ---" << std::endl;
    UserRole aliceRole = rbacManager.getUserRole("alice");
    std::cout << "Alice's role: " << static_cast<int>(aliceRole) << std::endl;
    
    UserRole bobRole = rbacManager.getUserRole("bob");
    std::cout << "Bob's role: " << static_cast<int>(bobRole) << std::endl;
    
    // Test 5: Check permissions
    std::cout << "\n--- Test 5: Check permissions ---" << std::endl;
    if (rbacManager.hasPermission("alice", Permission::CREATE_DATABASE)) {
        std::cout << "Alice has CREATE_DATABASE permission" << std::endl;
    } else {
        std::cerr << "Alice does not have CREATE_DATABASE permission" << std::endl;
        return 1;
    }
    
    if (rbacManager.hasPermission("bob", Permission::INSERT)) {
        std::cout << "Bob has INSERT permission" << std::endl;
    } else {
        std::cerr << "Bob does not have INSERT permission" << std::endl;
        return 1;
    }
    
    if (!rbacManager.hasPermission("bob", Permission::MANAGE_USERS)) {
        std::cout << "Bob does not have MANAGE_USERS permission (as expected)" << std::endl;
    } else {
        std::cerr << "Bob has MANAGE_USERS permission (unexpected)" << std::endl;
        return 1;
    }
    
    // Test 6: List users
    std::cout << "\n--- Test 6: List users ---" << std::endl;
    std::vector<std::string> users = rbacManager.listUsers();
    std::cout << "Users in system:" << std::endl;
    for (const auto& user : users) {
        std::cout << "  - " << user << std::endl;
    }
    
    // Test 7: List user permissions
    std::cout << "\n--- Test 7: List user permissions ---" << std::endl;
    std::vector<Permission> alicePermissions = rbacManager.listUserPermissions("alice");
    std::cout << "Alice's permissions:" << std::endl;
    for (const auto& perm : alicePermissions) {
        std::cout << "  - " << static_cast<int>(perm) << std::endl;
    }
    
    std::vector<Permission> bobPermissions = rbacManager.listUserPermissions("bob");
    std::cout << "Bob's permissions:" << std::endl;
    for (const auto& perm : bobPermissions) {
        std::cout << "  - " << static_cast<int>(perm) << std::endl;
    }
    
    // Shutdown the RBAC manager
    rbacManager.shutdown();
    
    std::cout << "\nAll RBAC tests passed!" << std::endl;
    return 0;
}