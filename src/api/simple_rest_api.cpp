#include "simple_rest_api.h"
#include "database_manager.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <thread>
#include <chrono>

namespace phantomdb {
namespace api {

SimpleRestApi::SimpleRestApi(int port)
    : port_(port), maxRequestBodySize_(1024 * 1024), // 1MB default
      running_(false), databaseManager_(std::make_unique<DatabaseManager>()) {
    std::cout << "Creating SimpleRestApi on port " << port << std::endl;
}

SimpleRestApi::~SimpleRestApi() {
    if (running_) {
        shutdown();
    }
    std::cout << "Destroying SimpleRestApi" << std::endl;
}

bool SimpleRestApi::initialize() {
    std::cout << "Initializing SimpleRestApi on port " << port_ << std::endl;
    
    // In a real implementation, we would start a server here
    // For now, we'll just set the running flag
    running_ = true;
    
    std::cout << "SimpleRestApi initialized successfully on port " << port_ << std::endl;
    return true;
}

void SimpleRestApi::shutdown() {
    std::cout << "Shutting down SimpleRestApi on port " << port_ << std::endl;
    
    running_ = false;
    
    std::cout << "SimpleRestApi shutdown completed on port " << port_ << std::endl;
}

void SimpleRestApi::registerGet(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::GET, path, handler);
    std::cout << "Registered GET route: " << path << std::endl;
}

void SimpleRestApi::registerPost(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::POST, path, handler);
    std::cout << "Registered POST route: " << path << std::endl;
}

void SimpleRestApi::registerPut(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::PUT, path, handler);
    std::cout << "Registered PUT route: " << path << std::endl;
}

void SimpleRestApi::registerDelete(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::DELETE, path, handler);
    std::cout << "Registered DELETE route: " << path << std::endl;
}

void SimpleRestApi::registerPatch(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::PATCH, path, handler);
    std::cout << "Registered PATCH route: " << path << std::endl;
}

void SimpleRestApi::registerMiddleware(const Middleware& middleware) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    middlewares_.push_back(middleware);
    std::cout << "Registered middleware" << std::endl;
}

int SimpleRestApi::getPort() const {
    return port_;
}

bool SimpleRestApi::isRunning() const {
    return running_.load();
}

void SimpleRestApi::setMaxRequestBodySize(size_t maxSize) {
    maxRequestBodySize_.store(maxSize);
    std::cout << "Set maximum request body size to " << maxSize << " bytes" << std::endl;
}

size_t SimpleRestApi::getMaxRequestBodySize() const {
    return maxRequestBodySize_.load();
}

HttpResponse SimpleRestApi::handleRequest(const HttpRequest& request) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    
    std::cout << "Handling " << static_cast<int>(request.method) << " request to " << request.uri << std::endl;
    
    // Try to match the request to a route
    for (const auto& route : routes_) {
        if (route.method == request.method) {
            std::unordered_map<std::string, std::string> pathParams;
            if (matchRoute(request.uri, route.path, pathParams)) {
                // Create a request with path parameters
                HttpRequest requestWithParams = request;
                requestWithParams.pathParams = pathParams;
                
                // Apply middleware if any
                RouteHandler finalHandler = route.handler;
                for (auto it = middlewares_.rbegin(); it != middlewares_.rend(); ++it) {
                    const Middleware& middleware = *it;
                    RouteHandler currentHandler = finalHandler;
                    finalHandler = [middleware, currentHandler](const HttpRequest& req) {
                        return middleware(req, currentHandler);
                    };
                }
                
                // Call the handler
                return finalHandler(requestWithParams);
            }
        }
    }
    
    // No route matched
    HttpResponse response;
    response.statusCode = HttpStatusCode::NOT_FOUND;
    response.setJsonContent("{\"error\": \"Route not found\"}");
    return response;
}

bool SimpleRestApi::matchRoute(const std::string& requestPath, const std::string& routePath, 
                              std::unordered_map<std::string, std::string>& pathParams) const {
    auto requestParts = splitPath(requestPath);
    auto routeParts = splitPath(routePath);
    
    if (requestParts.size() != routeParts.size()) {
        return false;
    }
    
    pathParams.clear();
    
    for (size_t i = 0; i < requestParts.size(); ++i) {
        const std::string& requestPart = requestParts[i];
        const std::string& routePart = routeParts[i];
        
        if (routePart.empty()) {
            continue;
        }
        
        if (routePart[0] == ':') {
            // This is a path parameter
            std::string paramName = routePart.substr(1);
            pathParams[paramName] = requestPart;
        } else if (requestPart != routePart) {
            // Regular path part doesn't match
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> SimpleRestApi::splitPath(const std::string& path) const {
    std::vector<std::string> parts;
    
    if (path.empty()) {
        return parts;
    }
    
    size_t start = 0;
    if (path[0] == '/') {
        start = 1;
    }
    
    size_t pos = start;
    while (pos < path.length()) {
        size_t nextSlash = path.find('/', pos);
        if (nextSlash == std::string::npos) {
            nextSlash = path.length();
        }
        
        if (nextSlash > pos) {
            parts.push_back(path.substr(pos, nextSlash - pos));
        }
        
        pos = nextSlash + 1;
    }
    
    return parts;
}

} // namespace api
} // namespace phantomdb