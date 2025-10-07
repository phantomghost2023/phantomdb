#ifndef PHANTOMDB_EXAMPLE_PLUGIN_H
#define PHANTOMDB_EXAMPLE_PLUGIN_H

#include "base_plugin.h"
#include <string>

namespace phantomdb {
namespace plugin {

// Example custom plugin that extends the base plugin
class ExampleCustomPlugin : public BasePlugin {
public:
    ExampleCustomPlugin();
    virtual ~ExampleCustomPlugin() = default;
    
    // Override base plugin methods
    virtual bool initialize() override;
    virtual void shutdown() override;
    virtual void* getInterface(const std::string& interfaceName) override;
    
    // Custom plugin functionality
    std::string getCustomData() const;
    void setCustomData(const std::string& data);
    
private:
    std::string customData_;
};

} // namespace plugin
} // namespace phantomdb

#endif // PHANTOMDB_EXAMPLE_PLUGIN_H