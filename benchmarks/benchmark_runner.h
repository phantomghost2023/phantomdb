#ifndef PHANTOMDB_BENCHMARK_RUNNER_H
#define PHANTOMDB_BENCHMARK_RUNNER_H

#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <iostream>
#include <iomanip>
#include <map>
#include <unordered_map>

namespace phantomdb {
namespace benchmark {

// Benchmark result structure
struct BenchmarkResult {
    std::string name;
    double duration_ms;
    long iterations;
    double throughput_ops_per_sec;
    std::map<std::string, double> additional_metrics;
    
    BenchmarkResult(const std::string& n, double duration, long iter)
        : name(n), duration_ms(duration), iterations(iter) {
        throughput_ops_per_sec = (iterations / duration_ms) * 1000.0;
    }
};

// Benchmark runner class
class BenchmarkRunner {
public:
    using BenchmarkFunction = std::function<void()>;
    
    BenchmarkRunner(const std::string& benchmark_name);
    
    // Run a benchmark function
    BenchmarkResult run(BenchmarkFunction func, long iterations = 1);
    
    // Run a benchmark function with setup and teardown
    BenchmarkResult runWithSetup(
        std::function<void()> setup,
        BenchmarkFunction func,
        std::function<void()> teardown,
        long iterations = 1
    );
    
    // Print benchmark results
    static void printResult(const BenchmarkResult& result);
    static void printResults(const std::vector<BenchmarkResult>& results);
    
    // Get benchmark name
    const std::string& getName() const { return name_; }
    
private:
    std::string name_;
    std::vector<BenchmarkResult> results_;
};

// Utility functions for benchmarking
class BenchmarkUtils {
public:
    // Generate test data
    static std::vector<std::pair<std::string, std::string>> generateColumns(int count);
    static std::vector<std::unordered_map<std::string, std::string>> generateRows(
        const std::vector<std::pair<std::string, std::string>>& columns, 
        int count
    );
    
    // Time measurement utilities
    template<typename Func>
    static double measureTime(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0; // Convert to milliseconds
    }
    
    // Calculate statistics
    static double calculateMean(const std::vector<double>& values);
    static double calculateMedian(std::vector<double> values);
    static double calculatePercentile(std::vector<double> values, double percentile);
};

} // namespace benchmark
} // namespace phantomdb

#endif // PHANTOMDB_BENCHMARK_RUNNER_H