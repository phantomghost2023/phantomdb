#include "init.h"
#include <memory>
#include <iostream>

namespace phantomdb {
namespace observability {

// Global instances
static std::shared_ptr<MetricsRegistry> g_metricsRegistry;
static std::shared_ptr<DatabaseMetricsCollector> g_metricsCollector;
static std::shared_ptr<PrometheusExporter> g_prometheusExporter;

void initializeObservability() {
    if (!g_metricsRegistry) {
        g_metricsRegistry = std::make_shared<MetricsRegistry>();
        g_metricsCollector = std::make_shared<DatabaseMetricsCollector>(g_metricsRegistry);
        g_prometheusExporter = std::make_shared<PrometheusExporter>(g_metricsRegistry);
        std::cout << "Observability system initialized" << std::endl;
    }
}

std::shared_ptr<MetricsRegistry> getMetricsRegistry() {
    if (!g_metricsRegistry) {
        initializeObservability();
    }
    return g_metricsRegistry;
}

std::shared_ptr<DatabaseMetricsCollector> getMetricsCollector() {
    if (!g_metricsCollector) {
        initializeObservability();
    }
    return g_metricsCollector;
}

std::shared_ptr<PrometheusExporter> getPrometheusExporter() {
    if (!g_prometheusExporter) {
        initializeObservability();
    }
    return g_prometheusExporter;
}

} // namespace observability
} // namespace phantomdb