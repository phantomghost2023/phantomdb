#include "base_plugin.h"
#include <iostream>

namespace phantomdb {
namespace plugin {

// BasePlugin implementation
BasePlugin::BasePlugin(const std::string& name, const std::string& version, const std::string& description, PluginType type)
    : name_(name), version_(version), description_(description), type_(type), initialized_(false) {
}

std::string BasePlugin::getName() const {
    return name_;
}

std::string BasePlugin::getVersion() const {
    return version_;
}

std::string BasePlugin::getDescription() const {
    return description_;
}

PluginType BasePlugin::getType() const {
    return type_;
}

bool BasePlugin::initialize() {
    if (initialized_) {
        return true;
    }
    
    std::cout << "Initializing plugin: " << name_ << " v" << version_ << std::endl;
    initialized_ = true;
    return true;
}

void BasePlugin::shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "Shutting down plugin: " << name_ << std::endl;
    initialized_ = false;
}

void* BasePlugin::getInterface(const std::string& interfaceName) {
    // Base implementation returns nullptr
    // Derived classes should override this to provide specific interfaces
    std::cout << "Plugin " << name_ << " does not implement interface: " << interfaceName << std::endl;
    return nullptr;
}

// FunctionExtensionPlugin implementation
FunctionExtensionPlugin::FunctionExtensionPlugin(const std::string& name, const std::string& version, const std::string& description)
    : BasePlugin(name, version, description, PluginType::FUNCTION_EXTENSION) {
}

void* FunctionExtensionPlugin::getInterface(const std::string& interfaceName) {
    // Function extension plugins might provide custom SQL functions
    if (interfaceName == "SQLFunctionRegistry") {
        // Return pointer to function registry interface
        std::cout << "Providing SQLFunctionRegistry interface for plugin: " << getName() << std::endl;
        return nullptr; // In a real implementation, this would return a valid pointer
    }
    
    return BasePlugin::getInterface(interfaceName);
}

// StorageBackendPlugin implementation
StorageBackendPlugin::StorageBackendPlugin(const std::string& name, const std::string& version, const std::string& description)
    : BasePlugin(name, version, description, PluginType::STORAGE_BACKEND) {
}

void* StorageBackendPlugin::getInterface(const std::string& interfaceName) {
    // Storage backend plugins might provide custom storage interfaces
    if (interfaceName == "StorageEngine") {
        // Return pointer to storage engine interface
        std::cout << "Providing StorageEngine interface for plugin: " << getName() << std::endl;
        return nullptr; // In a real implementation, this would return a valid pointer
    }
    
    return BasePlugin::getInterface(interfaceName);
}

// QueryProcessorPlugin implementation
QueryProcessorPlugin::QueryProcessorPlugin(const std::string& name, const std::string& version, const std::string& description)
    : BasePlugin(name, version, description, PluginType::QUERY_PROCESSOR) {
}

void* QueryProcessorPlugin::getInterface(const std::string& interfaceName) {
    // Query processor plugins might provide custom query processing interfaces
    if (interfaceName == "QueryOptimizer") {
        // Return pointer to query optimizer interface
        std::cout << "Providing QueryOptimizer interface for plugin: " << getName() << std::endl;
        return nullptr; // In a real implementation, this would return a valid pointer
    }
    
    return BasePlugin::getInterface(interfaceName);
}

} // namespace plugin
} // namespace phantomdb