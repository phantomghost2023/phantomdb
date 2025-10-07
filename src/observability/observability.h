#ifndef PHANTOMDB_OBSERVABILITY_H
#define PHANTOMDB_OBSERVABILITY_H

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <atomic>
#include <vector>

namespace phantomdb {
namespace observability {

// Forward declarations
class MetricsRegistry;
class PrometheusExporter;

// Metric types
enum class MetricType {
    COUNTER,
    GAUGE,
    HISTOGRAM,
    SUMMARY
};

// Base metric class
class Metric {
public:
    Metric(const std::string& name, const std::string& description, MetricType type);
    virtual ~Metric() = default;
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    MetricType getType() const { return type_; }
    
    virtual std::string serialize() const = 0;
    // Remove JSON method to avoid nlohmann_json dependency
    // virtual nlohmann::json toJson() const = 0;
    
protected:
    std::string name_;
    std::string description_;
    MetricType type_;
};

// Counter metric
class Counter : public Metric {
public:
    Counter(const std::string& name, const std::string& description);
    
    void increment(double value = 1.0);
    double getValue() const;
    
    std::string serialize() const override;
    // Remove JSON method to avoid nlohmann_json dependency
    // nlohmann::json toJson() const override;
    
private:
    std::atomic<double> value_;
    mutable std::mutex mutex_;
};

// Gauge metric
class Gauge : public Metric {
public:
    Gauge(const std::string& name, const std::string& description);
    
    void set(double value);
    void increment(double value = 1.0);
    void decrement(double value = 1.0);
    double getValue() const;
    
    std::string serialize() const override;
    // Remove JSON method to avoid nlohmann_json dependency
    // nlohmann::json toJson() const override;
    
private:
    std::atomic<double> value_;
    mutable std::mutex mutex_;
};

// Histogram metric
class Histogram : public Metric {
public:
    Histogram(const std::string& name, const std::string& description, 
              const std::vector<double>& buckets);
    
    void observe(double value);
    const std::vector<double>& getBuckets() const { return buckets_; }
    const std::vector<std::atomic<long long>>& getBucketCounts() const { return bucket_counts_; }
    uint64_t getCount() const { return static_cast<uint64_t>(count_.load()); }
    double getSum() const { return sum_.load(); }
    
    std::string serialize() const override;
    // Remove JSON method to avoid nlohmann_json dependency
    // nlohmann::json toJson() const override;
    
private:
    std::vector<double> buckets_;
    std::vector<std::atomic<long long>> bucket_counts_;
    std::atomic<long long> count_;
    std::atomic<double> sum_;
    mutable std::mutex mutex_;
};

// Metrics registry
class MetricsRegistry {
public:
    MetricsRegistry();
    ~MetricsRegistry() = default;
    
    // Register metrics
    std::shared_ptr<Counter> registerCounter(const std::string& name, const std::string& description);
    std::shared_ptr<Gauge> registerGauge(const std::string& name, const std::string& description);
    std::shared_ptr<Histogram> registerHistogram(const std::string& name, const std::string& description,
                                                const std::vector<double>& buckets);
    
    // Get metrics
    std::vector<std::shared_ptr<Metric>> getMetrics() const;
    std::shared_ptr<Metric> getMetric(const std::string& name) const;
    
    // Serialization
    std::string serialize() const;
    // Remove JSON method to avoid nlohmann_json dependency
    // nlohmann::json toJson() const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<Metric>> metrics_;
    mutable std::mutex mutex_;
};

// Prometheus exporter
class PrometheusExporter {
public:
    PrometheusExporter(std::shared_ptr<MetricsRegistry> registry);
    ~PrometheusExporter() = default;
    
    std::string exportMetrics() const;
    // Remove JSON method to avoid nlohmann_json dependency
    // nlohmann::json exportJson() const;
    
private:
    std::shared_ptr<MetricsRegistry> registry_;
};

// Database metrics collector
class DatabaseMetricsCollector {
public:
    DatabaseMetricsCollector(std::shared_ptr<MetricsRegistry> registry);
    ~DatabaseMetricsCollector() = default;
    
    void collectMetrics();
    void updateQueryStats(const std::string& query_type, double duration_ms);
    void updateConnectionStats(int active_connections, int total_connections);
    void updateStorageStats(uint64_t used_bytes, uint64_t total_bytes);
    
private:
    std::shared_ptr<MetricsRegistry> registry_;
    
    // Query metrics
    std::shared_ptr<Counter> queries_total_;
    std::shared_ptr<Histogram> query_duration_seconds_;
    
    // Connection metrics
    std::shared_ptr<Gauge> connections_active_;
    std::shared_ptr<Counter> connections_total_;
    
    // Storage metrics
    std::shared_ptr<Gauge> storage_used_bytes_;
    std::shared_ptr<Gauge> storage_total_bytes_;
    
    // System metrics
    std::shared_ptr<Gauge> uptime_seconds_;
    std::shared_ptr<Counter> requests_total_;
};

} // namespace observability
} // namespace phantomdb

#endif // PHANTOMDB_OBSERVABILITY_H