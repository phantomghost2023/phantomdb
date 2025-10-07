#include "rest_api.h"
#include "database_manager.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

using namespace phantomdb::api;

class RestApiTest : public ::testing::Test {
protected:
    void SetUp() override {
        restApi = std::make_unique<RestApi>(8081); // Use different port for testing
    }
    
    void TearDown() override {
        if (restApi) {
            restApi->shutdown();
        }
    }
    
    std::unique_ptr<RestApi> restApi;
};

TEST_F(RestApiTest, InitializationTest) {
    EXPECT_FALSE(restApi->isRunning());
    EXPECT_TRUE(restApi->initialize());
    EXPECT_TRUE(restApi->isRunning());
    
    // Give the server a moment to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    restApi->shutdown();
    EXPECT_FALSE(restApi->isRunning());
}

TEST_F(RestApiTest, RouteRegistrationTest) {
    // Register a simple route
    restApi->registerGet("/test", [](const HttpRequest& request) -> HttpResponse {
        HttpResponse response;
        response.setJsonContent("{\"message\": \"test\"}");
        return response;
    });
    
    // Verify route registration doesn't crash
    EXPECT_NO_THROW(restApi->registerPost("/test", [](const HttpRequest& request) -> HttpResponse {
        HttpResponse response;
        response.statusCode = HttpStatusCode::CREATED;
        return response;
    }));
}

TEST_F(RestApiTest, DatabaseManagerTest) {
    DatabaseManager dbManager;
    
    // Test basic operations
    EXPECT_TRUE(dbManager.isHealthy());
    
    auto databases = dbManager.listDatabases();
    EXPECT_FALSE(databases.empty());
    
    auto tables = dbManager.listTables("default");
    EXPECT_FALSE(tables.empty());
    
    // Test stats
    std::string stats = dbManager.getStats();
    EXPECT_FALSE(stats.empty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}