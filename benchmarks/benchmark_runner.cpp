#include "benchmark_runner.h"
#include <algorithm>
#include <numeric>
#include <random>

namespace phantomdb {
namespace benchmark {

BenchmarkRunner::BenchmarkRunner(const std::string& benchmark_name)
    : name_(benchmark_name) {
}

BenchmarkResult BenchmarkRunner::run(BenchmarkFunction func, long iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run the benchmark function
    for (long i = 0; i < iterations; ++i) {
        func();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double duration_ms = duration.count() / 1000.0;
    
    BenchmarkResult result(name_, duration_ms, iterations);
    results_.push_back(result);
    
    return result;
}

BenchmarkResult BenchmarkRunner::runWithSetup(
    std::function<void()> setup,
    BenchmarkFunction func,
    std::function<void()> teardown,
    long iterations) {
    
    // Run setup
    if (setup) {
        setup();
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Run the benchmark function
    for (long i = 0; i < iterations; ++i) {
        func();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double duration_ms = duration.count() / 1000.0;
    
    // Run teardown
    if (teardown) {
        teardown();
    }
    
    BenchmarkResult result(name_, duration_ms, iterations);
    results_.push_back(result);
    
    return result;
}

void BenchmarkRunner::printResult(const BenchmarkResult& result) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Benchmark: " << result.name << std::endl;
    std::cout << "  Duration: " << result.duration_ms << " ms" << std::endl;
    std::cout << "  Iterations: " << result.iterations << std::endl;
    std::cout << "  Throughput: " << result.throughput_ops_per_sec << " ops/sec" << std::endl;
    
    if (!result.additional_metrics.empty()) {
        std::cout << "  Additional Metrics:" << std::endl;
        for (const auto& metric : result.additional_metrics) {
            std::cout << "    " << metric.first << ": " << metric.second << std::endl;
        }
    }
    std::cout << std::endl;
}

void BenchmarkRunner::printResults(const std::vector<BenchmarkResult>& results) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "=== Benchmark Results ===" << std::endl;
    
    for (const auto& result : results) {
        printResult(result);
    }
    
    // Summary statistics
    if (!results.empty()) {
        std::vector<double> durations;
        std::vector<double> throughputs;
        
        for (const auto& result : results) {
            durations.push_back(result.duration_ms);
            throughputs.push_back(result.throughput_ops_per_sec);
        }
        
        std::sort(durations.begin(), durations.end());
        std::sort(throughputs.begin(), throughputs.end());
        
        std::cout << "=== Summary ===" << std::endl;
        std::cout << "  Fastest: " << throughputs.back() << " ops/sec" << std::endl;
        std::cout << "  Slowest: " << throughputs.front() << " ops/sec" << std::endl;
        std::cout << "  Average: " << std::accumulate(throughputs.begin(), throughputs.end(), 0.0) / throughputs.size() << " ops/sec" << std::endl;
        std::cout << "  Median: " << throughputs[throughputs.size() / 2] << " ops/sec" << std::endl;
    }
}

// Utility functions implementation
std::vector<std::pair<std::string, std::string>> BenchmarkUtils::generateColumns(int count) {
    std::vector<std::pair<std::string, std::string>> columns;
    for (int i = 0; i < count; ++i) {
        columns.push_back({"col" + std::to_string(i), "string"});
    }
    return columns;
}

std::vector<std::unordered_map<std::string, std::string>> BenchmarkUtils::generateRows(
    const std::vector<std::pair<std::string, std::string>>& columns, 
    int count) {
    
    std::vector<std::unordered_map<std::string, std::string>> rows;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000000);
    
    for (int i = 0; i < count; ++i) {
        std::unordered_map<std::string, std::string> row;
        for (const auto& col : columns) {
            row[col.first] = "value_" + std::to_string(dis(gen));
        }
        rows.push_back(row);
    }
    
    return rows;
}

double BenchmarkUtils::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double BenchmarkUtils::calculateMedian(std::vector<double> values) {
    if (values.empty()) return 0.0;
    
    std::sort(values.begin(), values.end());
    size_t size = values.size();
    
    if (size % 2 == 0) {
        return (values[size / 2 - 1] + values[size / 2]) / 2.0;
    } else {
        return values[size / 2];
    }
}

double BenchmarkUtils::calculatePercentile(std::vector<double> values, double percentile) {
    if (values.empty()) return 0.0;
    
    std::sort(values.begin(), values.end());
    size_t size = values.size();
    double index = (percentile / 100.0) * (size - 1);
    
    if (index <= 0) return values.front();
    if (index >= size - 1) return values.back();
    
    size_t lower_index = static_cast<size_t>(index);
    size_t upper_index = lower_index + 1;
    double weight = index - lower_index;
    
    return values[lower_index] * (1 - weight) + values[upper_index] * weight;
}

} // namespace benchmark
} // namespace phantomdb