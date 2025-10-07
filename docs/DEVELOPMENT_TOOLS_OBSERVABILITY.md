# PhantomDB Observability Tools

## Overview

The PhantomDB observability system provides comprehensive monitoring and metrics collection capabilities for the database. It implements a Prometheus-compatible metrics exposition format and includes a Python client for easy integration with monitoring systems.

## Architecture

The observability system consists of several key components:

1. **Metrics Classes** - Core metric types (Counter, Gauge, Histogram)
2. **Metrics Registry** - Central registry for all metrics
3. **Prometheus Exporter** - Exports metrics in Prometheus format
4. **Database Metrics Collector** - Collects database-specific metrics
5. **Initialization System** - Global initialization and access points
6. **Python Client** - Client library for external monitoring tools

## Core Components

### Metrics Classes

#### Counter
A monotonically increasing metric that can only be incremented.

```cpp
auto counter = registry->registerCounter("http_requests_total", "Total HTTP requests");
counter->increment(1.0);
```

#### Gauge
A metric that can arbitrarily go up and down.

```cpp
auto gauge = registry->registerGauge("memory_usage_bytes", "Current memory usage");
gauge->set(1024.0);
gauge->increment(512.0);
gauge->decrement(256.0);
```

#### Histogram
A metric that samples observations and counts them in configurable buckets.

```cpp
std::vector<double> buckets = {0.1, 0.5, 1.0, 5.0, 10.0};
auto histogram = registry->registerHistogram("request_duration_seconds", "Request duration", buckets);
histogram->observe(0.75);
```

### Metrics Registry

The central registry manages all metrics and provides serialization capabilities.

```cpp
auto registry = phantomdb::observability::getMetricsRegistry();
std::string prometheusText = registry->serialize();
```

### Database Metrics Collector

Collects database-specific metrics such as query performance, connection statistics, and storage usage.

```cpp
auto collector = phantomdb::observability::getMetricsCollector();
collector->updateQueryStats("SELECT", 15.5);  // 15.5ms query duration
collector->updateConnectionStats(5, 10);      // 5 active, 10 total connections
```

## Integration with REST API

The observability system is integrated with the REST API server, providing two endpoints:

- `/metrics` - Prometheus metrics in text format
- `/stats` - Database statistics in JSON format

## Python Client

A Python client is provided for easy integration with external monitoring tools:

```python
from phantomdb_observability import PhantomDBObservabilityClient

client = PhantomDBObservabilityClient("http://localhost:8080")
metrics = client.get_metrics()
parsed_metrics = client.get_metrics_as_dict()
```

## Usage Examples

### C++ Integration

```cpp
#include "observability/init.h"
#include "observability/observability.h"

// Initialize observability system
phantomdb::observability::initializeObservability();

// Get registry and collector
auto registry = phantomdb::observability::getMetricsRegistry();
auto collector = phantomdb::observability::getMetricsCollector();

// Register and use metrics
auto queryCounter = registry->registerCounter("queries_total", "Total queries");
queryCounter->increment();

// Export metrics
std::string metrics = registry->serialize();
```

### Python Client Usage

```python
from phantomdb_observability import PhantomDBObservabilityClient

# Create client
client = PhantomDBObservabilityClient("http://localhost:8080")

# Get metrics
metrics_text = client.get_metrics()
print(metrics_text)

# Get parsed metrics
metrics_dict = client.get_metrics_as_dict()
for name, value in metrics_dict.items():
    print(f"{name}: {value}")
```

## Testing

The observability module includes a comprehensive test suite that validates all metric types and functionality:

```bash
cd build
make test_observability
./src/observability/test_observability
```

## Grafana Integration

To visualize PhantomDB metrics in Grafana:

1. Configure Prometheus to scrape metrics from `http://localhost:8080/metrics`
2. Import the PhantomDB dashboard JSON into Grafana
3. Create custom dashboards using the available metrics

## Available Metrics

The following metrics are automatically collected by the DatabaseMetricsCollector:

- `phantomdb_queries_total` - Counter of total database queries
- `phantomdb_query_duration_seconds` - Histogram of query durations
- `phantomdb_connections_active` - Gauge of active connections
- `phantomdb_connections_total` - Counter of total connections
- `phantomdb_storage_used_bytes` - Gauge of used storage
- `phantomdb_storage_total_bytes` - Gauge of total storage
- `phantomdb_uptime_seconds` - Gauge of database uptime
- `phantomdb_requests_total` - Counter of HTTP requests

## Extending Observability

To add custom metrics to your application:

```cpp
// Register a new counter
auto customCounter = registry->registerCounter("custom_metric_total", "Description");

// Register a new gauge
auto customGauge = registry->registerGauge("custom_gauge", "Description");

// Register a new histogram
std::vector<double> buckets = {1.0, 10.0, 100.0};
auto customHistogram = registry->registerHistogram("custom_histogram", "Description", buckets);
```

## Best Practices

1. **Metric Naming**: Use descriptive names with units (e.g., `_seconds`, `_bytes`)
2. **Label Usage**: Use labels sparingly to avoid high cardinality
3. **Metric Types**: Choose the appropriate metric type for your use case
4. **Performance**: Metric collection should have minimal performance impact
5. **Documentation**: Document all custom metrics with clear descriptions

## Troubleshooting

### No Metrics Exported
- Ensure the observability system is initialized
- Check that metrics are registered before being used
- Verify the REST API server is running

### Incorrect Values
- Check metric update logic
- Verify thread safety for concurrent access
- Ensure proper unit conversions (e.g., milliseconds to seconds)

### Prometheus Scraping Issues
- Verify the `/metrics` endpoint is accessible
- Check that the response format is correct
- Ensure Prometheus is configured with the correct endpoint