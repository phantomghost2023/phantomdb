#include "observability.h"
#include "init.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace phantomdb::observability;

int main() {
    std::cout << "Testing PhantomDB Observability Module\n";
    std::cout << "=====================================\n";
    
    // Initialize observability
    initializeObservability();
    
    // Get registry and collector
    auto registry = getMetricsRegistry();
    auto collector = getMetricsCollector();
    
    // Test counter
    auto counter = registry->registerCounter("test_counter", "A test counter");
    counter->increment(5.0);
    counter->increment(3.0);
    
    std::cout << "Counter value: " << counter->getValue() << std::endl;
    
    // Test gauge
    auto gauge = registry->registerGauge("test_gauge", "A test gauge");
    gauge->set(42.0);
    gauge->increment(8.0);
    gauge->decrement(2.0);
    
    std::cout << "Gauge value: " << gauge->getValue() << std::endl;
    
    // Test histogram
    std::vector<double> buckets = {1.0, 5.0, 10.0, 50.0, 100.0};
    auto histogram = registry->registerHistogram("test_histogram", "A test histogram", buckets);
    
    // Add some observations
    histogram->observe(0.5);
    histogram->observe(3.2);
    histogram->observe(7.8);
    histogram->observe(45.1);
    histogram->observe(120.0);
    
    std::cout << "Histogram count: " << histogram->getCount() << std::endl;
    std::cout << "Histogram sum: " << histogram->getSum() << std::endl;
    
    // Test metrics collection
    collector->updateQueryStats("SELECT", 15.5);
    collector->updateQueryStats("INSERT", 8.2);
    collector->updateConnectionStats(5, 10);
    collector->updateStorageStats(1024000, 1048576);
    
    // Serialize metrics
    std::cout << "\nSerialized Metrics:\n";
    std::cout << registry->serialize() << std::endl;
    
    // Test Prometheus exporter
    auto exporter = getPrometheusExporter();
    std::cout << "Prometheus Export:\n";
    std::cout << exporter->exportMetrics() << std::endl;
    
    std::cout << "All tests completed successfully!\n";
    return 0;
}