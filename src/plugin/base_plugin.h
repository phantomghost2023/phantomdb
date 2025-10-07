#ifndef PHANTOMDB_BASE_PLUGIN_H
#define PHANTOMDB_BASE_PLUGIN_H

#include "plugin_manager.h"
#include <string>

namespace phantomdb {
namespace plugin {

// Base plugin class that provides default implementations
class BasePlugin : public Plugin {
public:
    BasePlugin(const std::string& name, const std::string& version, const std::string& description, PluginType type);
    virtual ~BasePlugin() = default;
    
    // Implement Plugin interface with default behavior
    virtual std::string getName() const override;
    virtual std::string getVersion() const override;
    virtual std::string getDescription() const override;
    virtual PluginType getType() const override;
    
    // Default implementations for lifecycle methods
    virtual bool initialize() override;
    virtual void shutdown() override;
    
    // Default implementation for interface retrieval
    virtual void* getInterface(const std::string& interfaceName) override;
    
protected:
    std::string name_;
    std::string version_;
    std::string description_;
    PluginType type_;
    bool initialized_;
};

// Example plugin types that extend BasePlugin

// Function extension plugin
class FunctionExtensionPlugin : public BasePlugin {
public:
    FunctionExtensionPlugin(const std::string& name, const std::string& version, const std::string& description);
    virtual ~FunctionExtensionPlugin() = default;
    
    // Override to provide custom functionality
    virtual void* getInterface(const std::string& interfaceName) override;
};

// Storage backend plugin
class StorageBackendPlugin : public BasePlugin {
public:
    StorageBackendPlugin(const std::string& name, const std::string& version, const std::string& description);
    virtual ~StorageBackendPlugin() = default;
    
    // Override to provide custom functionality
    virtual void* getInterface(const std::string& interfaceName) override;
};

// Query processor plugin
class QueryProcessorPlugin : public BasePlugin {
public:
    QueryProcessorPlugin(const std::string& name, const std::string& version, const std::string& description);
    virtual ~QueryProcessorPlugin() = default;
    
    // Override to provide custom functionality
    virtual void* getInterface(const std::string& interfaceName) override;
};

} // namespace plugin
} // namespace phantomdb

#endif // PHANTOMDB_BASE_PLUGIN_H