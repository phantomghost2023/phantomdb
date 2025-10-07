#include "rest_api.h"
#include "database_manager.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <thread>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>

namespace phantomdb {
namespace api {

// Request handler for our REST API
class RestApiRequestHandler : public Poco::Net::HTTPRequestHandler {
public:
    RestApiRequestHandler(RestApi* api) : api_(api) {}
    
    void handleRequest(Poco::Net::HTTPServerRequest& request, 
                      Poco::Net::HTTPServerResponse& response) override {
        try {
            // Create our internal request structure
            HttpRequest internalRequest;
            internalRequest.method = convertMethod(request.getMethod());
            internalRequest.uri = request.getURI();
            
            // Parse query parameters
            Poco::URI uri(request.getURI());
            internalRequest.queryString = uri.getQuery();
            
            // Get query parameters
            Poco::URI::QueryParameters queryParams = uri.getQueryParameters();
            for (const auto& param : queryParams) {
                internalRequest.queryParams[param.first] = param.second;
            }
            
            // Get headers
            for (const auto& header : request) {
                internalRequest.headers[header.first] = header.second;
            }
            
            // Read body if present
            if (request.getContentLength() > 0) {
                Poco::StreamCopier::copyToString(request.stream(), internalRequest.body);
            }
            
            // Handle the request using our internal logic
            HttpResponse internalResponse = api_->handleRequest(internalRequest);
            
            // Set response status
            response.setStatus(static_cast<Poco::Net::HTTPResponse::HTTPStatus>(
                static_cast<int>(internalResponse.statusCode)));
            
            // Set headers
            for (const auto& header : internalResponse.headers) {
                response.set(header.first, header.second);
            }
            
            // Send response body
            std::ostream& out = response.send();
            out << internalResponse.body;
        } catch (const std::exception& e) {
            spdlog::error("Exception handling request: {}", e.what());
            response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("application/json");
            std::ostream& out = response.send();
            out << "{\"error\": \"Internal server error\"}";
        }
    }

private:
    HttpMethod convertMethod(const std::string& method) {
        if (method == Poco::Net::HTTPRequest::HTTP_GET) return HttpMethod::GET;
        if (method == Poco::Net::HTTPRequest::HTTP_POST) return HttpMethod::POST;
        if (method == Poco::Net::HTTPRequest::HTTP_PUT) return HttpMethod::PUT;
        if (method == Poco::Net::HTTPRequest::HTTP_DELETE) return HttpMethod::DELETE;
        if (method == Poco::Net::HTTPRequest::HTTP_PATCH) return HttpMethod::PATCH;
        return HttpMethod::GET; // Default
    }
    
    RestApi* api_;
};

// Request handler factory
class RestApiRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    RestApiRequestHandlerFactory(RestApi* api) : api_(api) {}
    
    Poco::Net::HTTPRequestHandler* createRequestHandler(
        const Poco::Net::HTTPServerRequest& request) override {
        return new RestApiRequestHandler(api_);
    }
    
private:
    RestApi* api_;
};

RestApi::RestApi(int port)
    : port_(port), maxRequestBodySize_(1024 * 1024), // 1MB default
      running_(false), databaseManager_(std::make_unique<DatabaseManager>()) {
    std::cout << "Creating RestApi on port " << port << std::endl;
}

RestApi::~RestApi() {
    if (running_) {
        shutdown();
    }
    std::cout << "Destroying RestApi" << std::endl;
}

bool RestApi::initialize() {
    std::cout << "Initializing RestApi on port " << port_ << std::endl;
    
    try {
        // Create server socket
        Poco::Net::ServerSocket socket(port_);
        Poco::Net::HTTPServerParams* params = new Poco::Net::HTTPServerParams;
        params->setMaxThreads(100);
        params->setMaxQueued(1000);
        
        // Create server
        server_ = std::make_unique<Poco::Net::HTTPServer>(
            new RestApiRequestHandlerFactory(this), socket, params);
        
        // Start server
        server_->start();
        running_ = true;
        
        std::cout << "RestApi initialized successfully on port " << port_ << std::endl;
        return true;
    } catch (const Poco::Exception& e) {
        std::cout << "Failed to initialize RestApi: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cout << "Failed to initialize RestApi: " << e.what() << std::endl;
        return false;
    }
}

void RestApi::shutdown() {
    std::cout << "Shutting down RestApi on port " << port_ << std::endl;
    
    running_ = false;
    
    if (server_) {
        server_->stop();
        server_.reset();
    }
    
    std::cout << "RestApi shutdown completed on port " << port_ << std::endl;
}

void RestApi::registerGet(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::GET, path, handler);
    std::cout << "Registered GET route: " << path << std::endl;
}

void RestApi::registerPost(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::POST, path, handler);
    std::cout << "Registered POST route: " << path << std::endl;
}

void RestApi::registerPut(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::PUT, path, handler);
    std::cout << "Registered PUT route: " << path << std::endl;
}

void RestApi::registerDelete(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::DELETE, path, handler);
    std::cout << "Registered DELETE route: " << path << std::endl;
}

void RestApi::registerPatch(const std::string& path, const RouteHandler& handler) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    routes_.emplace_back(HttpMethod::PATCH, path, handler);
    std::cout << "Registered PATCH route: " << path << std::endl;
}

void RestApi::registerMiddleware(const Middleware& middleware) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    middlewares_.push_back(middleware);
    std::cout << "Registered middleware" << std::endl;
}

int RestApi::getPort() const {
    return port_;
}

bool RestApi::isRunning() const {
    return running_.load();
}

void RestApi::setMaxRequestBodySize(size_t maxSize) {
    maxRequestBodySize_.store(maxSize);
    std::cout << "Set maximum request body size to " << maxSize << " bytes" << std::endl;
}

size_t RestApi::getMaxRequestBodySize() const {
    return maxRequestBodySize_.load();
}

HttpResponse RestApi::handleRequest(const HttpRequest& request) {
    std::lock_guard<std::mutex> lock(apiMutex_);
    
    std::cout << "Handling " << 
                 (request.method == HttpMethod::GET) ? "GET" :
                 (request.method == HttpMethod::POST) ? "POST" :
                 (request.method == HttpMethod::PUT) ? "PUT" :
                 (request.method == HttpMethod::DELETE) ? "DELETE" : "PATCH" << 
                 " request to " << request.uri << std::endl;
    
    // Apply middlewares
    RouteHandler finalHandler = [this](const HttpRequest& req) -> HttpResponse {
        // Find matching route
        for (const auto& route : routes_) {
            if (route.method == req.method) {
                std::unordered_map<std::string, std::string> pathParams;
                if (matchRoute(req.uri, route.path, pathParams)) {
                    HttpRequest modifiedReq = req;
                    modifiedReq.pathParams = pathParams;
                    return route.handler(modifiedReq);
                }
            }
        }
        
        // No matching route found
        HttpResponse response;
        response.statusCode = HttpStatusCode::NOT_FOUND;
        response.setJsonContent("{\"error\": \"Route not found\"}");
        return response;
    };
    
    // Apply middlewares in reverse order
    for (auto it = middlewares_.rbegin(); it != middlewares_.rend(); ++it) {
        RouteHandler prevHandler = finalHandler;
        finalHandler = [this, &it, &prevHandler](const HttpRequest& req) -> HttpResponse {
            return (*it)(req, prevHandler);
        };
    }
    
    return finalHandler(request);
}

bool RestApi::matchRoute(const std::string& requestPath, const std::string& routePath, 
                        std::unordered_map<std::string, std::string>& pathParams) const {
    auto requestParts = splitPath(requestPath);
    auto routeParts = splitPath(routePath);
    
    if (requestParts.size() != routeParts.size()) {
        return false;
    }
    
    pathParams.clear();
    
    for (size_t i = 0; i < requestParts.size(); i++) {
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
            // Literal path segment doesn't match
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> RestApi::splitPath(const std::string& path) const {
    std::vector<std::string> parts;
    
    if (path.empty() || path == "/") {
        parts.push_back("");
        return parts;
    }
    
    std::istringstream iss(path);
    std::string part;
    
    // Skip leading slash
    if (path[0] == '/') {
        iss.ignore(1);
    }
    
    while (std::getline(iss, part, '/')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

} // namespace api
} // namespace phantomdb