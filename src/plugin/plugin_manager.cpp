#include "plugin_manager.h"
#include <iostream>
#include <filesystem>

namespace phantomdb {
namespace plugin {

// Static member definitions
std::unique_ptr<PluginManager> PluginManager::instance_;
std::mutex PluginManager::instance_mutex_;

// PluginManager implementation
PluginManager::PluginManager() {
    std::cout << "Initializing Plugin Manager..." << std::endl;
}

PluginManager::~PluginManager() {
    std::cout << "Shutting down Plugin Manager..." << std::endl;
    // Unload all plugins
    plugins_.clear();
}

PluginManager& PluginManager::getInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<PluginManager>(new PluginManager());
    }
    return *instance_;
}

bool PluginManager::loadPlugin(const std::string& pluginPath) {
    std::lock_guard<std::mutex> lock(plugins_mutex_);
    
    // In a real implementation, this would load a shared library
    // and instantiate the plugin class
    std::cout << "Loading plugin from: " << pluginPath << std::endl;
    
    // For demonstration purposes, we'll just simulate loading
    // A real implementation would use dlopen/dlsym on Unix or LoadLibrary/GetProcAddress on Windows
    
    // Check if file exists
    if (!std::filesystem::exists(pluginPath)) {
        std::cerr << "Plugin file not found: " << pluginPath << std::endl;
        return false;
    }
    
    // In a real implementation, we would:
    // 1. Load the shared library
    // 2. Look for a plugin factory function
    // 3. Create an instance of the plugin
    // 4. Register it with the manager
    
    std::cout << "Plugin loaded successfully (simulated): " << pluginPath << std::endl;
    return true;
}

bool PluginManager::loadPluginsFromDirectory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(plugins_mutex_);
    
    std::cout << "Loading plugins from directory: " << directory << std::endl;
    
    // Check if directory exists
    if (!std::filesystem::exists(directory)) {
        std::cerr << "Plugin directory not found: " << directory << std::endl;
        return false;
    }
    
    if (!std::filesystem::is_directory(directory)) {
        std::cerr << "Path is not a directory: " << directory << std::endl;
        return false;
    }
    
    // Iterate through files in directory
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                const std::string& path = entry.path().string();
                
                // Check if it's a shared library file
                #ifdef _WIN32
                if (path.substr(path.find_last_of(".") + 1) == "dll") {
                    loadPlugin(path);
                }
                #else
                if (path.substr(path.find_last_of(".") + 1) == "so") {
                    loadPlugin(path);
                }
                #endif
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading plugins from directory: " << e.what() << std::endl;
        return false;
    }
    
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(plugins_mutex_);
    
    auto it = plugins_.find(pluginName);
    if (it != plugins_.end()) {
        std::cout << "Unloading plugin: " << pluginName << std::endl;
        
        // Shutdown the plugin
        it->second->shutdown();
        
        // Remove from registry
        plugins_.erase(it);
        
        std::cout << "Plugin unloaded successfully: " << pluginName << std::endl;
        return true;
    }
    
    std::cerr << "Plugin not found: " << pluginName << std::endl;
    return false;
}

Plugin* PluginManager::getPlugin(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(plugins_mutex_);
    
    auto it = plugins_.find(pluginName);
    if (it != plugins_.end()) {
        return it->second.get();
    }
    
    return nullptr;
}

std::vector<Plugin*> PluginManager::getPlugins() const {
    std::lock_guard<std::mutex> lock(plugins_mutex_);
    
    std::vector<Plugin*> result;
    result.reserve(plugins_.size());
    
    for (const auto& pair : plugins_) {
        result.push_back(pair.second.get());
    }
    
    return result;
}

std::vector<Plugin*> PluginManager::getPluginsByType(PluginType type) const {
    std::lock_guard<std::mutex> lock(plugins_mutex_);
    
    std::vector<Plugin*> result;
    
    for (const auto& pair : plugins_) {
        if (pair.second->getType() == type) {
            result.push_back(pair.second.get());
        }
    }
    
    return result;
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(plugins_mutex_);
    return plugins_.find(pluginName) != plugins_.end();
}

size_t PluginManager::getPluginCount() const {
    std::lock_guard<std::mutex> lock(plugins_mutex_);
    return plugins_.size();
}

bool PluginManager::registerPlugin(std::unique_ptr<Plugin> plugin) {
    if (!plugin) {
        return false;
    }
    
    std::string name = plugin->getName();
    
    std::lock_guard<std::mutex> lock(plugins_mutex_);
    
    // Check if plugin with this name already exists
    if (plugins_.find(name) != plugins_.end()) {
        std::cerr << "Plugin with name '" << name << "' already loaded" << std::endl;
        return false;
    }
    
    // Initialize the plugin
    if (!plugin->initialize()) {
        std::cerr << "Failed to initialize plugin: " << name << std::endl;
        return false;
    }
    
    // Add to registry
    plugins_[name] = std::move(plugin);
    
    std::cout << "Plugin registered successfully: " << name << std::endl;
    return true;
}

} // namespace plugin
} // namespace phantomdb