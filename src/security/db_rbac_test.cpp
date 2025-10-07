#include "rbac.h"
#include <iostream>
#include <memory>

using namespace phantomdb::security;

// Mock database class to demonstrate RBAC integration
class MockDatabase {
public:
    MockDatabase(std::shared_ptr<RBACManager> rbac) : rbac_(rbac) {}
    
    bool createTable(const std::string& username, const std::string& tableName) {
        if (!rbac_->hasPermission(username, Permission::CREATE_TABLE)) {
            std::cerr << "User " << username << " does not have permission to create tables" << std::endl;
            return false;
        }
        
        std::cout << "User " << username << " created table " << tableName << std::endl;
        return true;
    }
    
    bool insertData(const std::string& username, const std::string& tableName, const std::string& data) {
        if (!rbac_->hasPermission(username, Permission::INSERT)) {
            std::cerr << "User " << username << " does not have permission to insert data" << std::endl;
            return false;
        }
        
        std::cout << "User " << username << " inserted data into table " << tableName << std::endl;
        return true;
    }
    
    bool selectData(const std::string& username, const std::string& tableName) {
        if (!rbac_->hasPermission(username, Permission::SELECT)) {
            std::cerr << "User " << username << " does not have permission to select data" << std::endl;
            return false;
        }
        
        std::cout << "User " << username << " selected data from table " << tableName << std::endl;
        return true;
    }
    
    bool dropTable(const std::string& username, const std::string& tableName) {
        if (!rbac_->hasPermission(username, Permission::DROP_TABLE)) {
            std::cerr << "User " << username << " does not have permission to drop tables" << std::endl;
            return false;
        }
        
        std::cout << "User " << username << " dropped table " << tableName << std::endl;
        return true;
    }

private:
    std::shared_ptr<RBACManager> rbac_;
};

int main() {
    std::cout << "Testing RBAC integration with database operations..." << std::endl;
    
    // Initialize the RBAC manager
    auto rbacManager = std::make_shared<RBACManager>();
    if (!rbacManager->initialize()) {
        std::cerr << "Failed to initialize RBAC manager" << std::endl;
        return 1;
    }
    
    // Create users
    rbacManager->createUser("admin_user", "admin123");
    rbacManager->createUser("reader_user", "reader123");
    rbacManager->createUser("writer_user", "writer123");
    
    // Assign roles
    rbacManager->assignRole("admin_user", UserRole::ADMIN);
    rbacManager->assignRole("reader_user", UserRole::READER);
    rbacManager->assignRole("writer_user", UserRole::WRITER);
    
    // Create mock database
    MockDatabase db(rbacManager);
    
    // Test admin user (should have all permissions)
    std::cout << "\n--- Testing admin_user (ADMIN role) ---" << std::endl;
    db.createTable("admin_user", "users");
    db.insertData("admin_user", "users", "John Doe");
    db.selectData("admin_user", "users");
    db.dropTable("admin_user", "users");
    
    // Test writer user (should have read and write permissions)
    std::cout << "\n--- Testing writer_user (WRITER role) ---" << std::endl;
    db.createTable("writer_user", "products");  // Should fail
    db.insertData("writer_user", "products", "Product 1");
    db.selectData("writer_user", "products");
    db.dropTable("writer_user", "products");    // Should fail
    
    // Test reader user (should have only read permissions)
    std::cout << "\n--- Testing reader_user (READER role) ---" << std::endl;
    db.createTable("reader_user", "orders");    // Should fail
    db.insertData("reader_user", "orders", "Order 1");  // Should fail
    db.selectData("reader_user", "orders");
    db.dropTable("reader_user", "orders");      // Should fail
    
    // Shutdown the RBAC manager
    rbacManager->shutdown();
    
    std::cout << "\nRBAC integration test completed!" << std::endl;
    return 0;
}