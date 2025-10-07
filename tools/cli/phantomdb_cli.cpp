#include "phantomdb_cli.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace phantomdb {
namespace cli {

// CLI implementation
class PhantomDBCLI::Impl {
public:
    Impl() : historyLimit_(100) {
        // Initialize autocomplete keywords
        initializeKeywords();
    }
    
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "PhantomDB CLI v1.0" << std::endl;
        std::cout << "Type 'help' for available commands or 'exit' to quit." << std::endl;
        std::cout << std::endl;
        return true;
    }
    
    void run() {
        std::string input;
        bool running = true;
        
        while (running) {
            std::cout << "phantomdb> ";
            std::getline(std::cin, input);
            
            if (!input.empty()) {
                // Add to history
                addToHistory(input);
                
                // Process command
                if (input == "exit" || input == "quit") {
                    running = false;
                } else if (input == "help") {
                    showHelp();
                } else if (input == "history") {
                    showHistory();
                } else if (input.substr(0, 5) == "clear") {
                    clearHistory();
                } else {
                    // Execute database command
                    executeDatabaseCommand(input);
                }
            }
        }
    }
    
    void shutdown() {
        std::cout << "Goodbye!" << std::endl;
    }
    
    bool executeCommand(const std::string& command) {
        if (!command.empty()) {
            addToHistory(command);
            return executeDatabaseCommand(command);
        }
        return false;
    }
    
    const std::deque<std::string>& getCommandHistory() const {
        return commandHistory_;
    }
    
    std::vector<std::string> getAutocompleteSuggestions(const std::string& partial) const {
        std::vector<std::string> suggestions;
        
        // Find keywords that start with the partial string
        for (const auto& keyword : sqlKeywords_) {
            if (keyword.substr(0, partial.length()) == partial) {
                suggestions.push_back(keyword);
            }
        }
        
        // Sort suggestions alphabetically
        std::sort(suggestions.begin(), suggestions.end());
        
        return suggestions;
    }
    
private:
    void initializeKeywords() {
        // SQL keywords for autocomplete
        sqlKeywords_ = {
            "SELECT", "FROM", "WHERE", "INSERT", "UPDATE", "DELETE",
            "CREATE", "DROP", "ALTER", "TABLE", "INDEX", "VIEW",
            "DATABASE", "USE", "SHOW", "DESCRIBE", "EXPLAIN",
            "BEGIN", "COMMIT", "ROLLBACK", "TRANSACTION",
            "GRANT", "REVOKE", "USER", "ROLE",
            "HELP", "EXIT", "QUIT", "HISTORY", "CLEAR"
        };
    }
    
    void addToHistory(const std::string& command) {
        // Remove if already exists
        commandHistory_.erase(
            std::remove(commandHistory_.begin(), commandHistory_.end(), command),
            commandHistory_.end()
        );
        
        // Add to front
        commandHistory_.push_front(command);
        
        // Limit history size
        if (commandHistory_.size() > historyLimit_) {
            commandHistory_.pop_back();
        }
    }
    
    void showHelp() {
        std::cout << "Available commands:" << std::endl;
        std::cout << "  help        - Show this help message" << std::endl;
        std::cout << "  history     - Show command history" << std::endl;
        std::cout << "  clear       - Clear command history" << std::endl;
        std::cout << "  exit/quit   - Exit the CLI" << std::endl;
        std::cout << "  SQL queries - Any valid SQL statement" << std::endl;
        std::cout << std::endl;
    }
    
    void showHistory() {
        std::cout << "Command History:" << std::endl;
        int index = 1;
        for (const auto& command : commandHistory_) {
            std::cout << "  " << std::setw(3) << index++ << ". " << command << std::endl;
        }
        std::cout << std::endl;
    }
    
    void clearHistory() {
        commandHistory_.clear();
        std::cout << "Command history cleared." << std::endl;
    }
    
    bool executeDatabaseCommand(const std::string& command) {
        // This is where we would integrate with the PhantomDB core
        // For now, we'll just echo the command
        std::cout << "Executing: " << command << std::endl;
        
        // Parse and validate the command
        if (!validateCommand(command)) {
            std::cout << "Error: Invalid command syntax" << std::endl;
            return false;
        }
        
        // In a real implementation, this would connect to the database
        // and execute the command
        std::cout << "Command executed successfully (simulated)." << std::endl;
        return true;
    }
    
    bool validateCommand(const std::string& command) {
        // Simple validation - check if command is not empty
        return !command.empty();
    }
    
    std::deque<std::string> commandHistory_;
    std::vector<std::string> sqlKeywords_;
    size_t historyLimit_;
};

PhantomDBCLI::PhantomDBCLI() : pImpl(std::make_unique<Impl>()) {}

PhantomDBCLI::~PhantomDBCLI() = default;

bool PhantomDBCLI::initialize() {
    return pImpl->initialize();
}

void PhantomDBCLI::run() {
    pImpl->run();
}

void PhantomDBCLI::shutdown() {
    pImpl->shutdown();
}

bool PhantomDBCLI::executeCommand(const std::string& command) {
    return pImpl->executeCommand(command);
}

const std::deque<std::string>& PhantomDBCLI::getCommandHistory() const {
    return pImpl->getCommandHistory();
}

std::vector<std::string> PhantomDBCLI::getAutocompleteSuggestions(const std::string& partial) const {
    return pImpl->getAutocompleteSuggestions(partial);
}

} // namespace cli
} // namespace phantomdb