#ifndef PHANTOMDB_REST_API_H
#define PHANTOMDB_REST_API_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Util/ServerApplication.h>

namespace phantomdb {
namespace api {

// Forward declarations
class DatabaseManager;

// HTTP methods
enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH
};

// HTTP status codes
enum class HttpStatusCode {
    OK = 200,
    CREATED = 201,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500
};

// HTTP request
struct HttpRequest {
    HttpMethod method;
    std::string uri;
    std::string queryString;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::unordered_map<std::string, std::string> queryParams;
    std::unordered_map<std::string, std::string> pathParams;
    
    HttpRequest() : method(HttpMethod::GET) {}
};

// HTTP response
struct HttpResponse {
    HttpStatusCode statusCode;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    
    HttpResponse() : statusCode(HttpStatusCode::OK) {}
    
    void setContentType(const std::string& contentType) {
        headers["Content-Type"] = contentType;
    }
    
    void setJsonContent(const std::string& json) {
        setContentType("application/json");
        body = json;
    }
};

// Route handler function type
using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;

// Middleware function type
using Middleware = std::function<HttpResponse(const HttpRequest&, const RouteHandler&)>;

class RestApi {
public:
    RestApi(int port = 8080);
    ~RestApi();
    
    // Initialize the REST API server
    bool initialize();
    
    // Shutdown the REST API server
    void shutdown();
    
    // Register route handlers
    void registerGet(const std::string& path, const RouteHandler& handler);
    void registerPost(const std::string& path, const RouteHandler& handler);
    void registerPut(const std::string& path, const RouteHandler& handler);
    void registerDelete(const std::string& path, const RouteHandler& handler);
    void registerPatch(const std::string& path, const RouteHandler& handler);
    
    // Register middleware
    void registerMiddleware(const Middleware& middleware);
    
    // Get server port
    int getPort() const;
    
    // Check if server is running
    bool isRunning() const;
    
    // Set maximum request body size
    void setMaxRequestBodySize(size_t maxSize);
    
    // Get maximum request body size
    size_t getMaxRequestBodySize() const;
    
    // Handle request (internal method)
    HttpResponse handleRequest(const HttpRequest& request);

private:
    // Server port
    int port_;
    
    // Maximum request body size
    std::atomic<size_t> maxRequestBodySize_;
    
    // Route handlers
    struct Route {
        HttpMethod method;
        std::string path;
        RouteHandler handler;
        
        Route(HttpMethod m, const std::string& p, const RouteHandler& h)
            : method(m), path(p), handler(h) {}
    };
    
    std::vector<Route> routes_;
    std::vector<Middleware> middlewares_;
    
    // Server state
    std::atomic<bool> running_;
    
    // Database manager
    std::unique_ptr<DatabaseManager> databaseManager_;
    
    // Poco HTTP server
    std::unique_ptr<Poco::Net::HTTPServer> server_;
    
    // Mutex for thread safety
    mutable std::mutex apiMutex_;
    
    // Internal methods
    bool matchRoute(const std::string& requestPath, const std::string& routePath, 
                   std::unordered_map<std::string, std::string>& pathParams) const;
    std::vector<std::string> splitPath(const std::string& path) const;
    
    // Friend classes
    friend class RestApiRequestHandler;
    friend class RestApiRequestHandlerFactory;
};

} // namespace api
} // namespace phantomdb

#endif // PHANTOMDB_REST_API_H