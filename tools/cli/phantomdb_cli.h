#ifndef PHANTOMDB_CLI_H
#define PHANTOMDB_CLI_H

#include <string>
#include <vector>
#include <memory>
#include <deque>

namespace phantomdb {
namespace cli {

class PhantomDBCLI {
public:
    PhantomDBCLI();
    ~PhantomDBCLI();
    
    // Initialize the CLI
    bool initialize();
    
    // Run the CLI loop
    void run();
    
    // Shutdown the CLI
    void shutdown();
    
    // Execute a single command
    bool executeCommand(const std::string& command);
    
    // Get command history
    const std::deque<std::string>& getCommandHistory() const;
    
    // Get autocomplete suggestions
    std::vector<std::string> getAutocompleteSuggestions(const std::string& partial) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace cli
} // namespace phantomdb

#endif // PHANTOMDB_CLI_H