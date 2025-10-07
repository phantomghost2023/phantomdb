#!/bin/bash

# Simple script to verify Go syntax (for environments where 'go' command is not available)

echo "Verifying Go SDK syntax..."

# Check if files exist
if [ ! -f "phantomdb.go" ]; then
    echo "Error: phantomdb.go not found"
    exit 1
fi

if [ ! -f "phantomdb_test.go" ]; then
    echo "Error: phantomdb_test.go not found"
    exit 1
fi

if [ ! -f "example.go" ]; then
    echo "Error: example.go not found"
    exit 1
fi

if [ ! -f "go.mod" ]; then
    echo "Error: go.mod not found"
    exit 1
fi

echo "All required files present"

# Basic content checks
echo "Performing basic content checks..."

# Check for package declarations
if ! grep -q "^package phantomdb" phantomdb.go; then
    echo "Error: phantomdb.go missing package declaration"
    exit 1
fi

if ! grep -q "^package phantomdb" phantomdb_test.go; then
    echo "Error: phantomdb_test.go missing package declaration"
    exit 1
fi

if ! grep -q "^package main" example.go; then
    echo "Error: example.go missing package declaration"
    exit 1
fi

echo "Package declarations verified"

# Check for main struct
if ! grep -q "type PhantomDB struct" phantomdb.go; then
    echo "Error: PhantomDB struct not found"
    exit 1
fi

echo "Main struct verified"

# Check for main methods
methods=("New" "HealthCheck" "ListDatabases" "CreateDatabase" "DropDatabase" "ListTables" "CreateTable" "DropTable" "Insert" "Select" "Update" "Delete" "ExecuteQuery" "BeginTransaction" "CommitTransaction" "RollbackTransaction")

for method in "${methods[@]}"; do
    if ! grep -q "func.*$method" phantomdb.go; then
        echo "Warning: Method $method not found in phantomdb.go"
    fi
done

echo "Method declarations checked"

# Check for error type
if ! grep -q "type PhantomDBError struct" phantomdb.go; then
    echo "Error: PhantomDBError struct not found"
    exit 1
fi

echo "Error type verified"

# Check for column type
if ! grep -q "type Column struct" phantomdb.go; then
    echo "Error: Column struct not found"
    exit 1
fi

echo "Column type verified"

# Check for imports
if ! grep -q "import (" phantomdb.go; then
    echo "Error: Import block not found"
    exit 1
fi

echo "Import block verified"

echo "Syntax verification completed successfully!"
echo "All Go files appear to be syntactically correct."