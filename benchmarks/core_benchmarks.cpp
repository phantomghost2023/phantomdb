#include "benchmark_runner.h"
#include "../src/core/database.h"
#include <iostream>
#include <vector>

using namespace phantomdb::benchmark;

int main() {
    std::cout << "Running PhantomDB Core Benchmarks..." << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Benchmark 1: Database creation
    {
        BenchmarkRunner runner("Database Creation");
        auto result = runner.run([]() {
            phantomdb::core::Database db;
            db.createDatabase("benchmark_db");
        }, 100);
        results.push_back(result);
    }
    
    // Benchmark 2: Table creation
    {
        phantomdb::core::Database db;
        db.createDatabase("benchmark_db");
        
        BenchmarkRunner runner("Table Creation");
        auto result = runner.runWithSetup(
            [&db]() {
                // Setup: Create fresh database for each iteration
                // In a real benchmark, we might want to clean up between iterations
            },
            [&db]() {
                std::vector<std::pair<std::string, std::string>> columns = {
                    {"id", "integer"},
                    {"name", "string"},
                    {"email", "string"}
                };
                db.createTable("benchmark_db", "benchmark_table", columns);
            },
            [&db]() {
                // Teardown: Clean up table
                // Note: In a real implementation, we would need a dropTable method
            },
            100
        );
        results.push_back(result);
    }
    
    // Benchmark 3: Data insertion
    {
        phantomdb::core::Database db;
        db.createDatabase("benchmark_db");
        
        std::vector<std::pair<std::string, std::string>> columns = {
            {"id", "integer"},
            {"name", "string"},
            {"email", "string"}
        };
        db.createTable("benchmark_db", "benchmark_table", columns);
        
        BenchmarkRunner runner("Data Insertion (Single Row)");
        auto result = runner.run([&db]() {
            std::unordered_map<std::string, std::string> data = {
                {"id", "1"},
                {"name", "Benchmark User"},
                {"email", "benchmark@example.com"}
            };
            db.insertData("benchmark_db", "benchmark_table", data);
        }, 1000);
        results.push_back(result);
    }
    
    // Benchmark 4: Bulk data insertion
    {
        phantomdb::core::Database db;
        db.createDatabase("benchmark_db");
        
        std::vector<std::pair<std::string, std::string>> columns = {
            {"id", "integer"},
            {"name", "string"},
            {"email", "string"}
        };
        db.createTable("benchmark_db", "benchmark_table_bulk", columns);
        
        // Generate test data
        auto rows = BenchmarkUtils::generateRows(columns, 100);
        
        BenchmarkRunner runner("Bulk Data Insertion (100 Rows)");
        auto result = runner.run([&db, &rows]() {
            for (const auto& row : rows) {
                db.insertData("benchmark_db", "benchmark_table_bulk", row);
            }
        }, 10);
        result.additional_metrics["rows_per_second"] = result.throughput_ops_per_sec * 100;
        results.push_back(result);
    }
    
    // Benchmark 5: Data selection
    {
        phantomdb::core::Database db;
        db.createDatabase("benchmark_db");
        
        std::vector<std::pair<std::string, std::string>> columns = {
            {"id", "integer"},
            {"name", "string"},
            {"email", "string"}
        };
        db.createTable("benchmark_db", "benchmark_table_select", columns);
        
        // Insert some data for selection
        for (int i = 0; i < 100; ++i) {
            std::unordered_map<std::string, std::string> data = {
                {"id", std::to_string(i)},
                {"name", "User " + std::to_string(i)},
                {"email", "user" + std::to_string(i) + "@example.com"}
            };
            db.insertData("benchmark_db", "benchmark_table_select", data);
        }
        
        BenchmarkRunner runner("Data Selection");
        auto result = runner.run([&db]() {
            auto results = db.selectData("benchmark_db", "benchmark_table_select");
            // Process results to ensure they're not optimized away
            volatile size_t count = results.size();
            (void)count;
        }, 1000);
        results.push_back(result);
    }
    
    // Print results
    BenchmarkRunner::printResults(results);
    
    std::cout << "Core benchmarks completed!" << std::endl;
    return 0;
}