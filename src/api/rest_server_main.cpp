#include "rest_api.h"
#include "database_manager.h"
#include <iostream>
#include <csignal>
#include <chrono>
#include <memory>

using namespace phantomdb::api;

// Global pointer to RestApi instance for signal handling
static RestApi* g_restApi = nullptr;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    if (g_restApi) {
        g_restApi->shutdown();
    }
}

int main(int argc, char* argv[]) {
    try {
        // Set up signal handlers for graceful shutdown
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        
        // Create and initialize REST API server
        int port = 8080;
        if (argc > 1) {
            port = std::stoi(argv[1]);
        }
        
        RestApi restApi(port);
        g_restApi = &restApi;
        
        // Create database manager
        auto dbManager = std::make_shared<DatabaseManager>();
        
        // Register example routes
        restApi.registerGet("/", [dbManager](const HttpRequest& request) -> HttpResponse {
            HttpResponse response;
            response.setJsonContent("{\"message\": \"Welcome to PhantomDB REST API\"}");
            return response;
        });
        
        restApi.registerGet("/health", [dbManager](const HttpRequest& request) -> HttpResponse {
            HttpResponse response;
            bool healthy = dbManager->isHealthy();
            response.setJsonContent("{\"status\": \"" + std::string(healthy ? "healthy" : "unhealthy") + "\"}");
            return response;
        });
        
        restApi.registerGet("/metrics", [dbManager](const HttpRequest& request) -> HttpResponse {
            HttpResponse response;
            response.setContentType("text/plain; version=0.0.4");
            std::string metrics = dbManager->getMetrics();
            response.body = metrics;
            return response;
        });
        
        restApi.registerGet("/stats", [dbManager](const HttpRequest& request) -> HttpResponse {
            HttpResponse response;
            response.setContentType("application/json");
            response.body = dbManager->getStats();
            return response;
        });
        
        // Initialize the server
        if (!restApi.initialize()) {
            std::cout << "Failed to initialize REST API server" << std::endl;
            return 1;
        }
        
        std::cout << "PhantomDB REST API server started on port " << port << std::endl;
        std::cout << "Endpoints available:" << std::endl;
        std::cout << "  - GET /          - Welcome message" << std::endl;
        std::cout << "  - GET /health    - Health check" << std::endl;
        std::cout << "  - GET /metrics   - Prometheus metrics" << std::endl;
        std::cout << "  - GET /stats     - Statistics in JSON format" << std::endl;
        std::cout << "Press Ctrl+C to stop the server" << std::endl;
        
        // Wait for shutdown signal
        while (restApi.isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "PhantomDB REST API server stopped" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Exception in main: " << e.what() << std::endl;
        return 1;
    }
}