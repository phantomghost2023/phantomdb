# PhantomDB Observability

This document describes the observability features of PhantomDB, including metrics collection, monitoring, and integration with Prometheus and Grafana.

## Overview

PhantomDB includes a comprehensive observability system that provides insights into database performance, resource utilization, and operational health. The system collects metrics in a Prometheus-compatible format and can be easily integrated with monitoring stacks.

## Metrics Collection

PhantomDB collects the following types of metrics:

### Database Metrics
- **Query Performance**: Total queries, query duration histograms
- **Connection Statistics**: Active connections, total connections
- **Storage Usage**: Used storage, total storage capacity
- **System Health**: Uptime, request counts

### Distributed Metrics
- **Cluster Health**: Node status, cluster formation time
- **Replication Metrics**: Replication lag, sync status
- **Consensus Metrics**: Leader elections, term changes
- **Sharding Metrics**: Shard distribution, rebalancing events

### Transaction Metrics
- **Transaction Throughput**: Commits, rollbacks
- **Lock Contention**: Wait times, deadlock occurrences
- **Isolation Metrics**: Snapshot creation, visibility checks

## Prometheus Integration

PhantomDB exposes metrics in the Prometheus text format through its REST API.

### Metrics Endpoint

```
GET /metrics
```

This endpoint returns metrics in the Prometheus exposition format:

```
# HELP phantomdb_queries_total Total number of database queries
# TYPE phantomdb_queries_total counter
phantomdb_queries_total 1234

# HELP phantomdb_query_duration_seconds Query duration in seconds
# TYPE phantomdb_query_duration_seconds histogram
phantomdb_query_duration_seconds_bucket{le="0.001"} 100
phantomdb_query_duration_seconds_bucket{le="0.01"} 500
phantomdb_query_duration_seconds_bucket{le="0.1"} 1000
phantomdb_query_duration_seconds_bucket{le="0.5"} 1200
phantomdb_query_duration_seconds_bucket{le="1.0"} 1230
phantomdb_query_duration_seconds_bucket{le="5.0"} 1234
phantomdb_query_duration_seconds_bucket{le="10.0"} 1234
phantomdb_query_duration_seconds_bucket{le="+Inf"} 1234
phantomdb_query_duration_seconds_count 1234
phantomdb_query_duration_seconds_sum 246.8
```

### Configuration

To enable Prometheus metrics collection, ensure the REST API server is running:

```bash
# Start the REST API server
./rest_server

# Metrics are available at:
# http://localhost:8080/metrics
```

## Grafana Integration

PhantomDB includes pre-built Grafana dashboards for visualizing database metrics.

### Dashboard Features

1. **Overview Dashboard**
   - Database health status
   - Query performance trends
   - Connection statistics
   - Storage utilization

2. **Performance Dashboard**
   - Query latency percentiles
   - Throughput metrics
   - Resource utilization
   - Error rates

3. **Distributed Dashboard**
   - Cluster topology
   - Node health
   - Replication status
   - Sharding distribution

### Installation

1. Import the dashboard JSON files from the `grafana/dashboards` directory
2. Configure the Prometheus data source in Grafana
3. Customize dashboard variables as needed

### Example Queries

Common Prometheus queries for monitoring PhantomDB:

```promql
# Query rate per second
rate(phantomdb_queries_total[5m])

# 95th percentile query latency
histogram_quantile(0.95, rate(phantomdb_query_duration_seconds_bucket[5m]))

# Active connections
phantomdb_connections_active

# Storage utilization percentage
(phantomdb_storage_used_bytes / phantomdb_storage_total_bytes) * 100
```

## Alerting

Recommended alerts for PhantomDB monitoring:

### Critical Alerts
- Database downtime (`absent(phantomdb_uptime_seconds)`)
- High query latency (`histogram_quantile(0.95, rate(phantomdb_query_duration_seconds_bucket[5m])) > 1.0`)
- Storage full (`(phantomdb_storage_used_bytes / phantomdb_storage_total_bytes) > 0.95`)

### Warning Alerts
- High connection count (`phantomdb_connections_active > 1000`)
- Slow query rate (`rate(phantomdb_queries_total[5m]) < 1`)
- Cluster instability (`changes(phantomdb_cluster_leader[1h]) > 5`)

## Custom Metrics

Developers can extend the observability system by registering custom metrics:

```cpp
#include "observability/observability.h"

// Register a custom counter
auto customCounter = registry->registerCounter(
    "myapp_custom_events_total",
    "Total number of custom events"
);

// Increment the counter
customCounter->increment();

// Register a custom gauge
auto customGauge = registry->registerGauge(
    "myapp_processing_queue_size",
    "Current size of processing queue"
);

// Update the gauge
customGauge->set(queueSize);
```

## Performance Impact

The observability system is designed to have minimal performance impact:
- Metrics collection is lock-free where possible
- Atomic operations are used for counters and gauges
- Histograms use efficient bucketing strategies
- Metrics serialization is optimized for the Prometheus format

Typical overhead is less than 1% for normal workloads.

## Troubleshooting

### Common Issues

1. **Metrics not appearing in Prometheus**
   - Verify the REST API server is running
   - Check the `/metrics` endpoint is accessible
   - Ensure Prometheus is configured with the correct scrape target

2. **High cardinality metrics**
   - Avoid creating metrics with unbounded label values
   - Use appropriate aggregation levels
   - Monitor metric count with `count(metric_name)`

3. **Performance degradation**
   - Reduce scrape interval if metrics collection is expensive
   - Disable unnecessary metrics
   - Review custom metric implementations for efficiency

### Debugging

Enable debug logging to troubleshoot observability issues:

```bash
# Set log level to debug
export PHANTOMDB_LOG_LEVEL=debug
```

## Best Practices

1. **Metric Naming**
   - Use descriptive names with units
   - Follow Prometheus naming conventions
   - Use base units (seconds, bytes, etc.)

2. **Label Usage**
   - Use labels for dimensions, not metric names
   - Limit label cardinality
   - Avoid high-cardinality labels

3. **Dashboard Design**
   - Focus on actionable metrics
   - Provide context with annotations
   - Use appropriate visualization types

4. **Alerting**
   - Define clear alert thresholds
   - Include meaningful alert descriptions
   - Test alerting rules with historical data