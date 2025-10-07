#ifndef PHANTOMDB_OBSERVABILITY_INIT_H
#define PHANTOMDB_OBSERVABILITY_INIT_H

#include "observability.h"

namespace phantomdb {
namespace observability {

// Initialize observability system
void initializeObservability();

// Get global metrics registry
std::shared_ptr<MetricsRegistry> getMetricsRegistry();

// Get global metrics collector
std::shared_ptr<DatabaseMetricsCollector> getMetricsCollector();

// Get global Prometheus exporter
std::shared_ptr<PrometheusExporter> getPrometheusExporter();

} // namespace observability
} // namespace phantomdb

#endif // PHANTOMDB_OBSERVABILITY_INIT_H