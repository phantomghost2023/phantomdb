#include <gtest/gtest.h>
#include "core/core.h"
#include "storage/storage_engine.h"
#include "storage/wal_manager.h"

using namespace phantomdb::core;
using namespace phantomdb::storage;

// Core module tests
TEST(CoreTest, VersionTest) {
    Core core;
    EXPECT_EQ(core.getVersion(), "1.0.0");
}

TEST(CoreTest, InitializationTest) {
    Core core;
    EXPECT_TRUE(core.initialize());
}

// Storage module tests
TEST(StorageTest, InitializationTest) {
    StorageEngine engine;
    EXPECT_TRUE(engine.initialize());
}

TEST(StorageTest, StatusTest) {
    StorageEngine engine;
    engine.initialize();
    EXPECT_EQ(engine.getStatus(), "running");
}

// WAL Manager tests
TEST(WALManagerTest, InitializationTest) {
    WALManager walManager;
    EXPECT_TRUE(walManager.initialize());
}

TEST(WALManagerTest, WriteLogEntryTest) {
    WALManager walManager;
    walManager.initialize();
    
    // Write a simple log entry
    EXPECT_TRUE(walManager.writeLogEntry("test log entry"));
}

TEST(WALManagerTest, ReplayLogsTest) {
    WALManager walManager;
    walManager.initialize();
    
    // Write a few log entries
    EXPECT_TRUE(walManager.writeLogEntry("first entry"));
    EXPECT_TRUE(walManager.writeLogEntry("second entry"));
    
    // Replay the logs
    EXPECT_TRUE(walManager.replayLogs());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}