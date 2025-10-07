#include "src/core/database.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <thread>

void test_create_and_drop() {
    phantomdb::core::Database db;
    assert(db.createDatabase("testDB"));
    assert(!db.createDatabase("testDB"));
    assert(db.dropDatabase("testDB"));
    assert(!db.dropDatabase("testDB"));
    std::cout << "Test passed: create_and_drop" << std::endl;
}

void test_table_operations() {
    phantomdb::core::Database db;
    db.createDatabase("testDB");
    std::vector<std::pair<std::string, std::string>> columns = {{"id", "int"}, {"name", "string"}};
    assert(db.createTable("testDB", "users", columns));
    assert(!db.createTable("testDB", "users", columns));

    auto tables = db.listTables("testDB");
    assert(tables.size() == 1);
    assert(tables[0] == "users");

    assert(db.dropTable("testDB", "users"));
    assert(!db.dropTable("testDB", "users"));

    tables = db.listTables("testDB");
    assert(tables.empty());

    std::cout << "Test passed: table_operations" << std::endl;
}

void test_data_operations() {
    phantomdb::core::Database db;
    db.createDatabase("testDB");
    std::vector<std::pair<std::string, std::string>> columns = {{"id", "int"}, {"name", "string"}, {"age", "int"}};
    db.createTable("testDB", "users", columns);

    // Insert
    assert(db.insertData("testDB", "users", {{"id", "1"}, {"name", "Alice"}, {"age", "30"}}));
    assert(db.insertData("testDB", "users", {{"id", "2"}, {"name", "Bob"}, {"age", "25"}}));

    // Select all
    auto all_users = db.selectData("testDB", "users");
    assert(all_users.size() == 2);

    // Select with condition
    auto alice_users = db.selectData("testDB", "users", {{"name", "Alice"}});
    assert(alice_users.size() == 1);
    assert(alice_users[0]["id"] == "1");

    // Update
    assert(db.updateData("testDB", "users", {{"age", "31"}}, {{"name", "Alice"}}));
    auto updated_alice_users = db.selectData("testDB", "users", {{"name", "Alice"}});
    assert(updated_alice_users[0]["age"] == "31");

    // Delete
    assert(db.deleteData("testDB", "users", {{"name", "Bob"}}));
    auto remaining_users = db.selectData("testDB", "users");
    assert(remaining_users.size() == 1);
    assert(remaining_users[0]["name"] == "Alice");

    std::cout << "Test passed: data_operations" << std::endl;
}

void test_concurrency() {
    phantomdb::core::Database db;
    db.createDatabase("testDB");
    std::vector<std::pair<std::string, std::string>> columns = {{"id", "int"}, {"value", "string"}};
    db.createTable("testDB", "concurrent_table", columns);

    auto writer = [&](int start_id) {
        for (int i = 0; i < 100; ++i) {
            db.insertData("testDB", "concurrent_table", {{"id", std::to_string(start_id + i)}, {"value", "some_data"}});
        }
    };

    std::thread t1(writer, 100);
    std::thread t2(writer, 200);

    t1.join();
    t2.join();

    auto results = db.selectData("testDB", "concurrent_table");
    assert(results.size() == 200);
    std::cout << "Test passed: concurrency" << std::endl;
}


int main() {
    test_create_and_drop();
    test_table_operations();
    test_data_operations();
    test_concurrency();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}