#include "observability.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iostream>

namespace phantomdb {
namespace observability {

// Metric implementation
Metric::Metric(const std::string& name, const std::string& description, MetricType type)
    : name_(name), description_(description), type_(type) {
}

// Counter implementation
Counter::Counter(const std::string& name, const std::string& description)
    : Metric(name, description, MetricType::COUNTER), value_(0.0) {
}

void Counter::increment(double value) {
    value_.fetch_add(value);
}

double Counter::getValue() const {
    return value_.load();
}

std::string Counter::serialize() const {
    std::ostringstream oss;
    oss << "# HELP " << name_ << " " << description_ << "\n";
    oss << "# TYPE " << name_ << " counter\n";
    oss << name_ << " " << std::fixed << std::setprecision(6) << value_.load() << "\n";
    return oss.str();
}

// Gauge implementation
Gauge::Gauge(const std::string& name, const std::string& description)
    : Metric(name, description, MetricType::GAUGE), value_(0.0) {
}

void Gauge::set(double value) {
    value_.store(value);
}

void Gauge::increment(double value) {
    value_.fetch_add(value);
}

void Gauge::decrement(double value) {
    value_.fetch_sub(value);
}

double Gauge::getValue() const {
    return value_.load();
}

std::string Gauge::serialize() const {
    std::ostringstream oss;
    oss << "# HELP " << name_ << " " << description_ << "\n";
    oss << "# TYPE " << name_ << " gauge\n";
    oss << name_ << " " << std::fixed << std::setprecision(6) << value_.load() << "\n";
    return oss.str();
}

// Histogram implementation
Histogram::Histogram(const std::string& name, const std::string& description,
                     const std::vector<double>& buckets)
    : Metric(name, description, MetricType::HISTOGRAM),
      buckets_(buckets),
      bucket_counts_(buckets.size() + 1),
      count_(0),
      sum_(0.0) {
    // Initialize atomic counters
    for (auto& count : bucket_counts_) {
        count.store(0);
    }
}

void Histogram::observe(double value) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Increment the count
    count_.fetch_add(1);
    
    // Add to sum
    sum_.fetch_add(value);
    
    // Find the appropriate bucket
    auto it = std::upper_bound(buckets_.begin(), buckets_.end(), value);
    size_t bucket_index = std::distance(buckets_.begin(), it);
    
    // Increment the bucket count
    if (bucket_index < bucket_counts_.size()) {
        bucket_counts_[bucket_index].fetch_add(1);
    }
}

std::string Histogram::serialize() const {
    std::ostringstream oss;
    oss << "# HELP " << name_ << " " << description_ << "\n";
    oss << "# TYPE " << name_ << " histogram\n";
    
    // Output bucket counts
    for (size_t i = 0; i < buckets_.size(); ++i) {
        oss << name_ << "_bucket{le=\"" << std::fixed << std::setprecision(6) << buckets_[i] << "\"} "
            << bucket_counts_[i].load() << "\n";
    }
    
    // Output +Inf bucket (total count)
    oss << name_ << "_bucket{le=\"+Inf\"} " << count_.load() << "\n";
    
    // Output count and sum
    oss << name_ << "_count " << count_.load() << "\n";
    oss << name_ << "_sum " << std::fixed << std::setprecision(6) << sum_.load() << "\n";
    
    return oss.str();
}

// MetricsRegistry implementation
MetricsRegistry::MetricsRegistry() {
}

std::shared_ptr<Counter> MetricsRegistry::registerCounter(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto counter = std::make_shared<Counter>(name, description);
    metrics_[name] = counter;
    return counter;
}

std::shared_ptr<Gauge> MetricsRegistry::registerGauge(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto gauge = std::make_shared<Gauge>(name, description);
    metrics_[name] = gauge;
    return gauge;
}

std::shared_ptr<Histogram> MetricsRegistry::registerHistogram(const std::string& name, const std::string& description,
                                                             const std::vector<double>& buckets) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto histogram = std::make_shared<Histogram>(name, description, buckets);
    metrics_[name] = histogram;
    return histogram;
}

std::vector<std::shared_ptr<Metric>> MetricsRegistry::getMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::shared_ptr<Metric>> result;
    for (const auto& pair : metrics_) {
        result.push_back(pair.second);
    }
    return result;
}

std::shared_ptr<Metric> MetricsRegistry::getMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metrics_.find(name);
    if (it != metrics_.end()) {
        return it->second;
    }
    return nullptr;
}

std::string MetricsRegistry::serialize() const {
    std::ostringstream oss;
    auto metrics = getMetrics();
    for (const auto& metric : metrics) {
        oss << metric->serialize() << "\n";
    }
    return oss.str();
}

// PrometheusExporter implementation
PrometheusExporter::PrometheusExporter(std::shared_ptr<MetricsRegistry> registry)
    : registry_(registry) {
}

std::string PrometheusExporter::exportMetrics() const {
    if (registry_) {
        return registry_->serialize();
    }
    return "";
}

// DatabaseMetricsCollector implementation
DatabaseMetricsCollector::DatabaseMetricsCollector(std::shared_ptr<MetricsRegistry> registry)
    : registry_(registry) {
    
    // Register metrics
    queries_total_ = registry_->registerCounter(
        "phantomdb_queries_total", 
        "Total number of database queries"
    );
    
    query_duration_seconds_ = registry_->registerHistogram(
        "phantomdb_query_duration_seconds",
        "Query duration in seconds",
        {0.001, 0.01, 0.1, 0.5, 1.0, 5.0, 10.0}
    );
    
    connections_active_ = registry_->registerGauge(
        "phantomdb_connections_active",
        "Number of active database connections"
    );
    
    connections_total_ = registry_->registerCounter(
        "phantomdb_connections_total",
        "Total number of database connections"
    );
    
    storage_used_bytes_ = registry_->registerGauge(
        "phantomdb_storage_used_bytes",
        "Used storage in bytes"
    );
    
    storage_total_bytes_ = registry_->registerGauge(
        "phantomdb_storage_total_bytes",
        "Total storage in bytes"
    );
    
    uptime_seconds_ = registry_->registerGauge(
        "phantomdb_uptime_seconds",
        "Database uptime in seconds"
    );
    
    requests_total_ = registry_->registerCounter(
        "phantomdb_requests_total",
        "Total number of HTTP requests"
    );
}

void DatabaseMetricsCollector::collectMetrics() {
    // This would typically collect metrics from the database system
    // For now, we'll just log that collection is happening
    std::cout << "Collecting database metrics" << std::endl;
}

void DatabaseMetricsCollector::updateQueryStats(const std::string& query_type, double duration_ms) {
    queries_total_->increment(1.0);
    query_duration_seconds_->observe(duration_ms / 1000.0); // Convert to seconds
}

void DatabaseMetricsCollector::updateConnectionStats(int active_connections, int total_connections) {
    connections_active_->set(active_connections);
    // Only increment total connections if it's actually increasing
    static int last_total = 0;
    if (total_connections > last_total) {
        connections_total_->increment(total_connections - last_total);
        last_total = total_connections;
    }
}

void DatabaseMetricsCollector::updateStorageStats(uint64_t used_bytes, uint64_t total_bytes) {
    storage_used_bytes_->set(static_cast<double>(used_bytes));
    storage_total_bytes_->set(static_cast<double>(total_bytes));
}

} // namespace observability
} // namespace phantomdb