#include "phantomdb_cli.h"
#include <iostream>

using namespace phantomdb::cli;

int main(int argc, char* argv[]) {
    try {
        PhantomDBCLI cli;
        
        if (!cli.initialize()) {
            std::cerr << "Failed to initialize PhantomDB CLI" << std::endl;
            return 1;
        }
        
        cli.run();
        cli.shutdown();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}