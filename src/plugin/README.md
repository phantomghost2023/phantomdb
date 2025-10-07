# PhantomDB Plugin Architecture

Extensible plugin system for PhantomDB that allows users to add custom functions, storage backends, query extensions, and more.

## Features

- Dynamic plugin loading from shared libraries
- Multiple plugin types (functions, storage, query processors, etc.)
- Plugin lifecycle management (initialize/shutdown)
- Interface-based plugin communication
- Thread-safe plugin registry
- Plugin discovery from directories

## Plugin Types

1. **Function Extension Plugins** - Add custom SQL functions
2. **Storage Backend Plugins** - Implement alternative storage engines
3. **Query Processor Plugins** - Provide custom query optimization
4. **Authentication Provider Plugins** - Implement custom authentication
5. **Custom Data Type Plugins** - Add support for new data types

## Architecture

```
+---------------------+
|   Plugin Manager    |
+---------------------+
| - Load/Unload       |
| - Registry          |
| - Lifecycle Mgmt    |
+---------------------+
          |
          v
+---------------------+
|     Base Plugin     |
+---------------------+
| - Common Interface  |
| - Default Behavior  |
+---------------------+
          |
          v
+---------------------+
|  Custom Plugins     |
+---------------------+
| - Function Plugins  |
| - Storage Plugins   |
| - Query Plugins     |
| - Auth Plugins      |
| - Custom Plugins    |
+---------------------+
```

## Creating a Plugin

### 1. Extend BasePlugin

```cpp
#include "base_plugin.h"

class MyCustomPlugin : public BasePlugin {
public:
    MyCustomPlugin() 
        : BasePlugin("MyCustomPlugin", "1.0.0", "My custom plugin", PluginType::FUNCTION_EXTENSION) {
    }
    
    bool initialize() override {
        // Custom initialization
        return BasePlugin::initialize();
    }
    
    void shutdown() override {
        // Custom cleanup
        BasePlugin::shutdown();
    }
    
    void* getInterface(const std::string& interfaceName) override {
        if (interfaceName == "MyCustomInterface") {
            // Return interface pointer
            return nullptr;
        }
        return BasePlugin::getInterface(interfaceName);
    }
};
```

### 2. Implement Plugin Factory (for dynamic loading)

```cpp
extern "C" {
    // Exported function for dynamic loading
    Plugin* createPlugin() {
        return new MyCustomPlugin();
    }
    
    // Exported function for cleanup
    void destroyPlugin(Plugin* plugin) {
        delete plugin;
    }
}
```

## Using Plugins

### Loading Plugins

```cpp
#include "plugin_manager.h"

using namespace phantomdb::plugin;

int main() {
    // Get the plugin manager instance
    PluginManager& manager = PluginManager::getInstance();
    
    // Load a plugin from a shared library
    manager.loadPlugin("./plugins/my_custom_plugin.so");
    
    // Load all plugins from a directory
    manager.loadPluginsFromDirectory("./plugins");
    
    // Get a specific plugin
    Plugin* plugin = manager.getPlugin("MyCustomPlugin");
    if (plugin) {
        // Use the plugin
        void* interface = plugin->getInterface("MyCustomInterface");
    }
    
    return 0;
}
```

### Plugin Registration

In a real implementation, plugins would be registered with the manager:

```cpp
// This would typically be done in the plugin's factory function
auto plugin = std::make_unique<MyCustomPlugin>();
manager.registerPlugin(std::move(plugin));
```

## Building Plugins

### CMakeLists.txt for Plugins

```cmake
# Create a shared library for the plugin
add_library(my_custom_plugin SHARED
    my_custom_plugin.cpp
    my_custom_plugin.h
)

# Link with the plugin library
target_link_libraries(my_custom_plugin plugin)

# Install the plugin
install(TARGETS my_custom_plugin
    LIBRARY DESTINATION plugins
)
```

## Plugin Directory Structure

```
plugins/
├── function_extensions/
│   ├── math_functions.so
│   └── string_functions.so
├── storage_backends/
│   ├── s3_storage.so
│   └── hdfs_storage.so
├── query_processors/
│   └── advanced_optimizer.so
└── authentication/
    └── ldap_auth.so
```

## Future Enhancements

- Plugin dependency management
- Plugin configuration files
- Plugin version compatibility checking
- Hot-reloading of plugins
- Plugin sandboxing for security
- Plugin marketplace integration
- Performance monitoring for plugins