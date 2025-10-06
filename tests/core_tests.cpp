#include <gtest/gtest.h>
#include "core/core.h"
#include "storage/storage_engine.h"

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}