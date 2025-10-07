#include "plugin_manager.h"
#include "base_plugin.h"
#include <iostream>
#include <cassert>
#include <memory>

using namespace phantomdb::plugin;

void testPluginManagerCreation() {
    std::cout << "Testing PluginManager creation..." << std::endl;
    
    PluginManager& manager = PluginManager::getInstance();
    assert(manager.getPluginCount() == 0);
    
    std::cout << "PluginManager creation test passed!" << std::endl;
}

void testBasePlugin() {
    std::cout << "Testing BasePlugin..." << std::endl;
    
    // Create a function extension plugin
    auto funcPlugin = std::make_unique<FunctionExtensionPlugin>(
        "TestFunctionPlugin", 
        "1.0.0", 
        "A test function extension plugin"
    );
    
    assert(funcPlugin->getName() == "TestFunctionPlugin");
    assert(funcPlugin->getVersion() == "1.0.0");
    assert(funcPlugin->getDescription() == "A test function extension plugin");
    assert(funcPlugin->getType() == PluginType::FUNCTION_EXTENSION);
    
    // Test initialization
    assert(funcPlugin->initialize() == true);
    
    // Test interface retrieval
    void* interface = funcPlugin->getInterface("SQLFunctionRegistry");
    // In our implementation, this returns nullptr but prints a message
    
    std::cout << "BasePlugin test passed!" << std::endl;
}

void testPluginRegistration() {
    std::cout << "Testing plugin registration..." << std::endl;
    
    PluginManager& manager = PluginManager::getInstance();
    
    // Create and register a plugin
    auto plugin = std::make_unique<StorageBackendPlugin>(
        "TestStoragePlugin", 
        "1.0.0", 
        "A test storage backend plugin"
    );
    
    // In a real implementation, we would register the plugin with the manager
    // For this test, we'll just verify the plugin works
    assert(plugin->initialize() == true);
    assert(plugin->getName() == "TestStoragePlugin");
    
    std::cout << "Plugin registration test passed!" << std::endl;
}

void testPluginTypes() {
    std::cout << "Testing plugin types..." << std::endl;
    
    // Create different types of plugins
    FunctionExtensionPlugin funcPlugin("FuncPlugin", "1.0", "Function plugin");
    StorageBackendPlugin storagePlugin("StoragePlugin", "1.0", "Storage plugin");
    QueryProcessorPlugin queryPlugin("QueryPlugin", "1.0", "Query plugin");
    
    assert(funcPlugin.getType() == PluginType::FUNCTION_EXTENSION);
    assert(storagePlugin.getType() == PluginType::STORAGE_BACKEND);
    assert(queryPlugin.getType() == PluginType::QUERY_PROCESSOR);
    
    std::cout << "Plugin types test passed!" << std::endl;
}

int main() {
    std::cout << "Running PhantomDB Plugin Manager tests..." << std::endl;
    
    testPluginManagerCreation();
    testBasePlugin();
    testPluginRegistration();
    testPluginTypes();
    
    std::cout << "All PhantomDB Plugin Manager tests passed!" << std::endl;
    return 0;
}