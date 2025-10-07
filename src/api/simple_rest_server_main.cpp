#include "simple_rest_api.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace phantomdb::api;

// Example route handlers
HttpResponse handleRoot(const HttpRequest& request) {
    HttpResponse response;
    response.setJsonContent("{\"message\": \"Welcome to PhantomDB REST API\"}");
    return response;
}

HttpResponse handleHealth(const HttpRequest& request) {
    HttpResponse response;
    response.setJsonContent("{\"status\": \"healthy\"}");
    return response;
}

HttpResponse handleDatabases(const HttpRequest& request) {
    HttpResponse response;
    response.setJsonContent("{\"databases\": [\"test_db\"]}");
    return response;
}

int main() {
    std::cout << "Starting PhantomDB Simple REST API Server..." << std::endl;
    
    // Create and initialize the REST API
    SimpleRestApi api(8080);
    if (!api.initialize()) {
        std::cout << "Failed to initialize REST API" << std::endl;
        return 1;
    }
    
    // Register some example routes
    api.registerGet("/", handleRoot);
    api.registerGet("/health", handleHealth);
    api.registerGet("/databases", handleDatabases);
    
    std::cout << "PhantomDB Simple REST API Server is running on port " << api.getPort() << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;
    
    // Keep the server running
    while (api.isRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Shutdown the API
    api.shutdown();
    
    std::cout << "PhantomDB Simple REST API Server stopped" << std::endl;
    return 0;
}