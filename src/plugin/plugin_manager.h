#ifndef PHANTOMDB_PLUGIN_MANAGER_H
#define PHANTOMDB_PLUGIN_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace phantomdb {
namespace plugin {

// Forward declarations
class Plugin;

// Plugin type enumeration
enum class PluginType {
    FUNCTION_EXTENSION,
    STORAGE_BACKEND,
    QUERY_PROCESSOR,
    AUTHENTICATION_PROVIDER,
    CUSTOM_DATA_TYPE
};

// Plugin interface
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Get plugin information
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginType getType() const = 0;
    
    // Lifecycle methods
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Plugin-specific functionality
    virtual void* getInterface(const std::string& interfaceName) = 0;
};

// Plugin manager class
class PluginManager {
public:
    static PluginManager& getInstance();
    
    // Load a plugin from a shared library
    bool loadPlugin(const std::string& pluginPath);
    
    // Load all plugins from a directory
    bool loadPluginsFromDirectory(const std::string& directory);
    
    // Unload a plugin
    bool unloadPlugin(const std::string& pluginName);
    
    // Get a loaded plugin by name
    Plugin* getPlugin(const std::string& pluginName) const;
    
    // Get all loaded plugins
    std::vector<Plugin*> getPlugins() const;
    
    // Get plugins by type
    std::vector<Plugin*> getPluginsByType(PluginType type) const;
    
    // Check if a plugin is loaded
    bool isPluginLoaded(const std::string& pluginName) const;
    
    // Get the number of loaded plugins
    size_t getPluginCount() const;
    
private:
    PluginManager();
    ~PluginManager();
    
    // Internal methods
    bool registerPlugin(std::unique_ptr<Plugin> plugin);
    
    // Singleton instance
    static std::unique_ptr<PluginManager> instance_;
    static std::mutex instance_mutex_;
    
    // Plugin storage
    std::unordered_map<std::string, std::unique_ptr<Plugin>> plugins_;
    mutable std::mutex plugins_mutex_;
};

} // namespace plugin
} // namespace phantomdb

#endif // PHANTOMDB_PLUGIN_MANAGER_H