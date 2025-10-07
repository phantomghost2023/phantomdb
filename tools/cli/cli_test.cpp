#include "phantomdb_cli.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::cli;

void testCLIInitialization() {
    std::cout << "Testing CLI initialization..." << std::endl;
    
    PhantomDBCLI cli;
    assert(cli.initialize());
    
    std::cout << "CLI initialization test passed!" << std::endl;
}

void testCommandExecution() {
    std::cout << "Testing command execution..." << std::endl;
    
    PhantomDBCLI cli;
    cli.initialize();
    
    // Test simple command execution
    assert(cli.executeCommand("SELECT * FROM users"));
    
    std::cout << "Command execution test passed!" << std::endl;
}

void testCommandHistory() {
    std::cout << "Testing command history..." << std::endl;
    
    PhantomDBCLI cli;
    cli.initialize();
    
    // Execute some commands
    cli.executeCommand("CREATE DATABASE testdb");
    cli.executeCommand("USE testdb");
    cli.executeCommand("CREATE TABLE users (id INT, name VARCHAR(50))");
    
    // Check history
    const auto& history = cli.getCommandHistory();
    assert(history.size() == 3);
    assert(history.front() == "CREATE TABLE users (id INT, name VARCHAR(50))");
    assert(history.back() == "CREATE DATABASE testdb");
    
    std::cout << "Command history test passed!" << std::endl;
}

void testAutocomplete() {
    std::cout << "Testing autocomplete suggestions..." << std::endl;
    
    PhantomDBCLI cli;
    cli.initialize();
    
    // Test autocomplete suggestions
    auto suggestions = cli.getAutocompleteSuggestions("SEL");
    assert(!suggestions.empty());
    assert(suggestions[0] == "SELECT");
    
    suggestions = cli.getAutocompleteSuggestions("CRE");
    assert(!suggestions.empty());
    assert(suggestions[0] == "CREATE");
    
    std::cout << "Autocomplete test passed!" << std::endl;
}

int main() {
    std::cout << "Running PhantomDB CLI tests..." << std::endl;
    
    testCLIInitialization();
    testCommandExecution();
    testCommandHistory();
    testAutocomplete();
    
    std::cout << "All PhantomDB CLI tests passed!" << std::endl;
    return 0;
}