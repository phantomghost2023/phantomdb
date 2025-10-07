#include "example_plugin.h"
#include <iostream>

namespace phantomdb {
namespace plugin {

ExampleCustomPlugin::ExampleCustomPlugin()
    : BasePlugin("ExampleCustomPlugin", "1.0.0", "An example custom plugin for PhantomDB", PluginType::FUNCTION_EXTENSION) {
}

bool ExampleCustomPlugin::initialize() {
    if (initialized_) {
        return true;
    }
    
    std::cout << "Initializing ExampleCustomPlugin..." << std::endl;
    
    // Perform custom initialization
    customData_ = "Hello from ExampleCustomPlugin!";
    
    // Call base initialization
    bool result = BasePlugin::initialize();
    
    if (result) {
        std::cout << "ExampleCustomPlugin initialized successfully!" << std::endl;
    } else {
        std::cerr << "Failed to initialize ExampleCustomPlugin!" << std::endl;
    }
    
    return result;
}

void ExampleCustomPlugin::shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "Shutting down ExampleCustomPlugin..." << std::endl;
    
    // Perform custom cleanup
    customData_.clear();
    
    // Call base shutdown
    BasePlugin::shutdown();
    
    std::cout << "ExampleCustomPlugin shut down successfully!" << std::endl;
}

void* ExampleCustomPlugin::getInterface(const std::string& interfaceName) {
    if (interfaceName == "CustomFunctionInterface") {
        std::cout << "Providing CustomFunctionInterface for ExampleCustomPlugin" << std::endl;
        // In a real implementation, this would return a valid pointer to the interface
        return nullptr;
    }
    
    return BasePlugin::getInterface(interfaceName);
}

std::string ExampleCustomPlugin::getCustomData() const {
    return customData_;
}

void ExampleCustomPlugin::setCustomData(const std::string& data) {
    customData_ = data;
}

} // namespace plugin
} // namespace phantomdb