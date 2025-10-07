@echo off
REM Script to set up PhantomDB observability with Prometheus and Grafana

echo Setting up PhantomDB Observability Stack...

REM Create necessary directories
if not exist data mkdir data
if not exist prometheus mkdir prometheus
if not exist grafana\provisioning\dashboards mkdir grafana\provisioning\dashboards
if not exist grafana\provisioning\datasources mkdir grafana\provisioning\datasources
if not exist grafana\dashboards mkdir grafana\dashboards

echo Starting observability stack with Docker Compose...

REM Start the observability stack
docker-compose -f docker-compose.observability.yml up -d

if %errorlevel% neq 0 (
    echo Error: Failed to start observability stack
    exit /b 1
)

echo Observability stack started successfully!
echo.
echo Access the services at:
echo   PhantomDB REST API: http://localhost:8080
echo   Prometheus: http://localhost:9090
echo   Grafana: http://localhost:3000 (admin/admin)
echo.
echo Grafana dashboards will be automatically provisioned.
echo Default credentials: admin/admin