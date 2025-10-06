#ifndef PHANTOMDB_CORE_H
#define PHANTOMDB_CORE_H

#include <string>
#include <memory>

namespace phantomdb {
namespace core {

class Core {
public:
    Core();
    ~Core();
    
    // Get the version of the database
    std::string getVersion() const;
    
    // Initialize the core components
    bool initialize();
    
    // Shutdown the core components
    void shutdown();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace core
} // namespace phantomdb

#endif // PHANTOMDB_CORE_H