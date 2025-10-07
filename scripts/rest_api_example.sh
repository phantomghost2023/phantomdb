#!/bin/bash

# Example script demonstrating PhantomDB REST API usage

echo "PhantomDB REST API Example"
echo "=========================="
echo

# Check if curl is available
if ! command -v curl &> /dev/null
then
    echo "curl could not be found. Please install curl to run this example."
    exit 1
fi

# Server URL
SERVER_URL="http://localhost:8080"

echo "1. Checking server health..."
curl -s "$SERVER_URL/health" | jq '.'
echo

echo "2. Listing databases..."
curl -s "$SERVER_URL/databases" | jq '.'
echo

echo "3. Creating a sample database..."
curl -s -X POST "$SERVER_URL/databases" \
  -H "Content-Type: application/json" \
  -d '{"name": "sample_db"}' | jq '.'
echo

echo "4. Listing tables in sample_db..."
curl -s "$SERVER_URL/databases/sample_db/tables" | jq '.'
echo

echo "5. Creating a sample table..."
curl -s -X POST "$SERVER_URL/databases/sample_db/tables" \
  -H "Content-Type: application/json" \
  -d '{"name": "users", "columns": [{"name": "id", "type": "int"}, {"name": "name", "type": "string"}]}' | jq '.'
echo

echo "6. Inserting data into users table..."
curl -s -X POST "$SERVER_URL/databases/sample_db/tables/users" \
  -H "Content-Type: application/json" \
  -d '{"id": "1", "name": "John Doe"}' | jq '.'
echo

echo "7. Querying data from users table..."
curl -s "$SERVER_URL/databases/sample_db/tables/users" | jq '.'
echo

echo "8. Updating data in users table..."
curl -s -X PUT "$SERVER_URL/databases/sample_db/tables/users" \
  -H "Content-Type: application/json" \
  -d '{"id": "1", "name": "Jane Doe"}' | jq '.'
echo

echo "9. Beginning a transaction..."
TXN_ID=$(curl -s -X POST "$SERVER_URL/transactions" | jq -r '.transaction_id')
echo "Transaction ID: $TXN_ID"
echo

echo "10. Committing the transaction..."
curl -s -X POST "$SERVER_URL/transactions/$TXN_ID/commit" | jq '.'
echo

echo "Example completed!"
echo
echo "To try these commands manually, start the PhantomDB REST server and run:"
echo "  scripts/run_rest_api.bat"
echo
echo "Then run this script or individual curl commands."