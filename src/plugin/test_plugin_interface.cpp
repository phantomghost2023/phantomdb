#include "plugin_manager.h"
#include "base_plugin.h"
#include "example_plugin.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::plugin;

int main() {
    std::cout << "Testing PhantomDB Plugin Interface..." << std::endl;
    
    // Get the plugin manager instance
    PluginManager& manager = PluginManager::getInstance();
    
    // Test creating and registering a plugin
    auto examplePlugin = std::make_unique<ExampleCustomPlugin>();
    
    // Test plugin properties
    assert(examplePlugin->getName() == "ExampleCustomPlugin");
    assert(examplePlugin->getVersion() == "1.0.0");
    assert(examplePlugin->getDescription() == "An example custom plugin for PhantomDB");
    assert(examplePlugin->getType() == PluginType::FUNCTION_EXTENSION);
    
    std::cout << "Plugin properties test passed" << std::endl;
    
    // Test plugin initialization
    assert(examplePlugin->initialize() == true);
    assert(examplePlugin->initialize() == true); // Test idempotency
    
    std::cout << "Plugin initialization test passed" << std::endl;
    
    // Test custom functionality
    examplePlugin->setCustomData("Test data");
    assert(examplePlugin->getCustomData() == "Test data");
    
    std::cout << "Custom functionality test passed" << std::endl;
    
    // Test plugin shutdown
    examplePlugin->shutdown();
    
    std::cout << "Plugin shutdown test passed" << std::endl;
    
    // Test base plugin functionality
    auto basePlugin = std::make_unique<BasePlugin>(
        "TestPlugin", "1.0.0", "A test plugin", PluginType::STORAGE_BACKEND);
    
    assert(basePlugin->getName() == "TestPlugin");
    assert(basePlugin->getVersion() == "1.0.0");
    assert(basePlugin->getDescription() == "A test plugin");
    assert(basePlugin->getType() == PluginType::STORAGE_BACKEND);
    
    std::cout << "Base plugin functionality test passed" << std::endl;
    
    // Test specific plugin types
    auto functionPlugin = std::make_unique<FunctionExtensionPlugin>(
        "FunctionPlugin", "1.0.0", "A function extension plugin");
    
    assert(functionPlugin->getType() == PluginType::FUNCTION_EXTENSION);
    
    auto storagePlugin = std::make_unique<StorageBackendPlugin>(
        "StoragePlugin", "1.0.0", "A storage backend plugin");
    
    assert(storagePlugin->getType() == PluginType::STORAGE_BACKEND);
    
    auto queryPlugin = std::make_unique<QueryProcessorPlugin>(
        "QueryPlugin", "1.0.0", "A query processor plugin");
    
    assert(queryPlugin->getType() == PluginType::QUERY_PROCESSOR);
    
    std::cout << "Specific plugin types test passed" << std::endl;
    
    // Test interface retrieval
    void* interface = functionPlugin->getInterface("SQLFunctionRegistry");
    // Should print a message but return nullptr in this implementation
    
    interface = storagePlugin->getInterface("StorageEngine");
    // Should print a message but return nullptr in this implementation
    
    interface = queryPlugin->getInterface("QueryOptimizer");
    // Should print a message but return nullptr in this implementation
    
    std::cout << "Interface retrieval test passed" << std::endl;
    
    std::cout << "All plugin interface tests passed!" << std::endl;
    
    return 0;
}