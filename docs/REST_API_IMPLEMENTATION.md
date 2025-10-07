# REST API Implementation

This document describes the implementation of the REST API layer for PhantomDB as part of Phase 4: Developer Experience.

## Overview

The REST API provides a web-based interface for interacting with PhantomDB, allowing developers to perform database operations using standard HTTP methods. The implementation uses the Poco C++ libraries for HTTP server functionality.

## Architecture

### Components

1. **RestApi Class**: Main server class that handles HTTP requests and routes them to appropriate handlers
2. **DatabaseManager Class**: Bridge between the REST API and PhantomDB core components
3. **Request Handlers**: Functions that process specific HTTP requests
4. **Middlewares**: Functions that can preprocess requests or postprocess responses

### Key Features

- Support for standard HTTP methods (GET, POST, PUT, DELETE, PATCH)
- Route matching with path parameters
- Middleware support
- JSON request/response handling
- Thread-safe operation
- Graceful shutdown handling

## Implementation Details

### RestApi Class

The [RestApi](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/api/rest_api.h#L47-L145) class is the main entry point for the REST API server:

```cpp
class RestApi {
public:
    RestApi(int port = 8080);
    ~RestApi();
    
    bool initialize();
    void shutdown();
    
    void registerGet(const std::string& path, const RouteHandler& handler);
    void registerPost(const std::string& path, const RouteHandler& handler);
    // ... other methods
};
```

### DatabaseManager Class

The [DatabaseManager](file:///D:/PhantomGhost/Storage/Media/Media/Projects/MyProjects/PhantomDB/src/api/database_manager.h#L11-L70) class provides a simplified interface to PhantomDB core components:

```cpp
class DatabaseManager {
public:
    // Database operations
    bool createDatabase(const std::string& dbName);
    bool dropDatabase(const std::string& dbName);
    std::vector<std::string> listDatabases() const;
    
    // Table operations
    bool createTable(const std::string& dbName, const std::string& tableName, 
                     const std::vector<std::pair<std::string, std::string>>& columns);
    // ... other methods
};
```

### HTTP Request/Response Handling

The API uses custom structures for HTTP requests and responses:

```cpp
struct HttpRequest {
    HttpMethod method;
    std::string uri;
    std::string queryString;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::unordered_map<std::string, std::string> queryParams;
    std::unordered_map<std::string, std::string> pathParams;
};

struct HttpResponse {
    HttpStatusCode statusCode;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};
```

## API Endpoints

### Health Check
- **GET /** - Welcome message
- **GET /health** - Health status

### Database Operations
- **GET /databases** - List all databases
- **POST /databases** - Create a new database
- **DELETE /databases/{name}** - Drop a database

### Table Operations
- **GET /databases/{name}/tables** - List tables in a database
- **POST /databases/{name}/tables** - Create a new table
- **DELETE /databases/{name}/tables/{table}** - Drop a table

### Data Operations
- **GET /databases/{name}/tables/{table}** - Select data from a table
- **POST /databases/{name}/tables/{table}** - Insert data into a table
- **PUT /databases/{name}/tables/{table}** - Update data in a table
- **DELETE /databases/{name}/tables/{table}** - Delete data from a table

### Query Operations
- **POST /databases/{name}/query** - Execute a custom query

### Transaction Operations
- **POST /transactions** - Begin a new transaction
- **POST /transactions/{id}/commit** - Commit a transaction
- **POST /transactions/{id}/rollback** - Rollback a transaction

## Usage Example

To start the REST API server:

```bash
# Build the project
mkdir build
cd build
cmake ..
make

# Run the REST API server
./src/api/rest_server
```

Example API calls:

```bash
# Health check
curl http://localhost:8080/health

# List databases
curl http://localhost:8080/databases

# Create a new database
curl -X POST http://localhost:8080/databases -H "Content-Type: application/json" -d '{"name": "mydb"}'
```

## Future Enhancements

1. **Authentication and Authorization**: Add user authentication and role-based access control
2. **Rate Limiting**: Implement request rate limiting to prevent abuse
3. **Caching**: Add response caching for frequently accessed data
4. **WebSocket Support**: Add real-time notifications for database changes
5. **API Documentation**: Generate interactive API documentation using OpenAPI/Swagger
6. **Request Validation**: Add request schema validation
7. **Logging and Monitoring**: Enhanced logging and monitoring capabilities
8. **Compression**: Add response compression for large payloads

## Testing

The REST API includes unit tests that verify:

- Server initialization and shutdown
- Route registration
- Basic request handling
- Database manager functionality

To run the tests:

```bash
./src/api/rest_api_test
```

## Dependencies

- **Poco**: HTTP server functionality
- **nlohmann/json**: JSON parsing and generation
- **spdlog**: Logging
- **Google Test**: Unit testing

## Conclusion

The REST API implementation provides a solid foundation for web-based access to PhantomDB. The modular design allows for easy extension and customization, while the use of established libraries ensures reliability and performance.