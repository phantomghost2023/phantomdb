@echo off
REM Example script demonstrating PhantomDB REST API usage

echo PhantomDB REST API Example
echo ==========================
echo.

REM Check if curl is available
curl --version >nul 2>&1
if %errorlevel% neq 0 (
    echo curl could not be found. Please install curl to run this example.
    exit /b 1
)

REM Server URL
set SERVER_URL=http://localhost:8080

echo 1. Checking server health...
curl -s %SERVER_URL%/health
echo.

echo 2. Listing databases...
curl -s %SERVER_URL%/databases
echo.

echo 3. Creating a sample database...
curl -s -X POST %SERVER_URL%/databases -H "Content-Type: application/json" -d "{\"name\": \"sample_db\"}"
echo.

echo 4. Listing tables in sample_db...
curl -s %SERVER_URL%/databases/sample_db/tables
echo.

echo 5. Creating a sample table...
curl -s -X POST %SERVER_URL%/databases/sample_db/tables -H "Content-Type: application/json" -d "{\"name\": \"users\", \"columns\": [{\"name\": \"id\", \"type\": \"int\"}, {\"name\": \"name\", \"type\": \"string\"}]}"
echo.

echo 6. Inserting data into users table...
curl -s -X POST %SERVER_URL%/databases/sample_db/tables/users -H "Content-Type: application/json" -d "{\"id\": \"1\", \"name\": \"John Doe\"}"
echo.

echo 7. Querying data from users table...
curl -s %SERVER_URL%/databases/sample_db/tables/users
echo.

echo 8. Updating data in users table...
curl -s -X PUT %SERVER_URL%/databases/sample_db/tables/users -H "Content-Type: application/json" -d "{\"id\": \"1\", \"name\": \"Jane Doe\"}"
echo.

echo 9. Beginning a transaction...
REM Note: In a real implementation, this would return a transaction ID
echo Transaction ID: txn_12345
echo.

echo 10. Committing the transaction...
curl -s -X POST %SERVER_URL%/transactions/txn_12345/commit
echo.

echo Example completed!
echo.
echo To try these commands manually, start the PhantomDB REST server and run:
echo   scripts\run_rest_api.bat
echo.
echo Then run this script or individual curl commands.