#!/bin/bash
# Script to set up PhantomDB observability with Prometheus and Grafana

echo "Setting up PhantomDB Observability Stack..."

# Create necessary directories
mkdir -p data
mkdir -p prometheus
mkdir -p grafana/provisioning/dashboards
mkdir -p grafana/provisioning/datasources
mkdir -p grafana/dashboards

echo "Starting observability stack with Docker Compose..."

# Start the observability stack
docker-compose -f docker-compose.observability.yml up -d

if [ $? -ne 0 ]; then
    echo "Error: Failed to start observability stack"
    exit 1
fi

echo "Observability stack started successfully!"
echo ""
echo "Access the services at:"
echo "  PhantomDB REST API: http://localhost:8080"
echo "  Prometheus: http://localhost:9090"
echo "  Grafana: http://localhost:3000 (admin/admin)"
echo ""
echo "Grafana dashboards will be automatically provisioned."
echo "Default credentials: admin/admin"