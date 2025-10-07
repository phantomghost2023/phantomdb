// Package phantomdb provides tests for the PhantomDB Go client.
package phantomdb

import (
	"bytes"
	"encoding/json"
	"io"
	"net/http"
	"net/http/httptest"
	"testing"
)

// mockHTTPClient is a mock HTTP client for testing.
type mockHTTPClient struct {
	responseBody []byte
	statusCode   int
	err          error
}

// Do implements the HTTP client interface.
func (m *mockHTTPClient) Do(req *http.Request) (*http.Response, error) {
	if m.err != nil {
		return nil, m.err
	}

	resp := &http.Response{
		StatusCode: m.statusCode,
		Body:       io.NopCloser(bytes.NewBuffer(m.responseBody)),
	}

	return resp, nil
}

func TestNew(t *testing.T) {
	// Test with default URL
	db := New("")
	if db.BaseURL != "http://localhost:8080" {
		t.Errorf("Expected default URL, got %s", db.BaseURL)
	}

	// Test with custom URL
	db = New("http://example.com")
	if db.BaseURL != "http://example.com" {
		t.Errorf("Expected custom URL, got %s", db.BaseURL)
	}
}

func TestHealthCheck(t *testing.T) {
	// Mock response
	response := map[string]interface{}{
		"status": "healthy",
	}
	jsonResponse, _ := json.Marshal(response)

	// Create mock client
	db := &PhantomDB{
		BaseURL: "http://localhost:8080",
		HTTPClient: &mockHTTPClient{
			responseBody: jsonResponse,
			statusCode:   200,
		},
	}

	// Test health check
	result, err := db.HealthCheck()
	if err != nil {
		t.Fatalf("HealthCheck failed: %v", err)
	}

	if result["status"] != "healthy" {
		t.Errorf("Expected status 'healthy', got %v", result["status"])
	}
}

func TestListDatabases(t *testing.T) {
	// Mock response
	response := map[string]interface{}{
		"databases": []string{"db1", "db2"},
	}
	jsonResponse, _ := json.Marshal(response)

	// Create mock client
	db := &PhantomDB{
		BaseURL: "http://localhost:8080",
		HTTPClient: &mockHTTPClient{
			responseBody: jsonResponse,
			statusCode:   200,
		},
	}

	// Test list databases
	databases, err := db.ListDatabases()
	if err != nil {
		t.Fatalf("ListDatabases failed: %v", err)
	}

	if len(databases) != 2 {
		t.Errorf("Expected 2 databases, got %d", len(databases))
	}

	if databases[0] != "db1" || databases[1] != "db2" {
		t.Errorf("Expected [db1, db2], got %v", databases)
	}
}

func TestCreateDatabase(t *testing.T) {
	// Mock response
	response := map[string]interface{}{
		"message": "Database created",
	}
	jsonResponse, _ := json.Marshal(response)

	// Create mock client
	db := &PhantomDB{
		BaseURL: "http://localhost:8080",
		HTTPClient: &mockHTTPClient{
			responseBody: jsonResponse,
			statusCode:   200,
		},
	}

	// Test create database
	result, err := db.CreateDatabase("testdb")
	if err != nil {
		t.Fatalf("CreateDatabase failed: %v", err)
	}

	if result["message"] != "Database created" {
		t.Errorf("Expected message 'Database created', got %v", result["message"])
	}
}

func TestPhantomDBError(t *testing.T) {
	// Test PhantomDBError implementation
	err := &PhantomDBError{Message: "test error"}
	if err.Error() != "test error" {
		t.Errorf("Expected 'test error', got %s", err.Error())
	}
}

func TestHTTPError(t *testing.T) {
	// Create mock client that returns HTTP error
	db := &PhantomDB{
		BaseURL: "http://localhost:8080",
		HTTPClient: &mockHTTPClient{
			statusCode: 500,
		},
	}

	// Test that HTTP errors are handled correctly
	_, err := db.HealthCheck()
	if err == nil {
		t.Fatal("Expected error, got nil")
	}

	if _, ok := err.(*PhantomDBError); !ok {
		t.Errorf("Expected PhantomDBError, got %T", err)
	}
}

// Test server for integration testing
func TestIntegration(t *testing.T) {
	// Create a test server
	server := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		// Simple echo server for testing
		switch r.URL.Path {
		case "/health":
			w.WriteHeader(http.StatusOK)
			json.NewEncoder(w).Encode(map[string]interface{}{
				"status": "healthy",
			})
		case "/databases":
			if r.Method == "GET" {
				w.WriteHeader(http.StatusOK)
				json.NewEncoder(w).Encode(map[string]interface{}{
					"databases": []string{"testdb"},
				})
			} else if r.Method == "POST" {
				w.WriteHeader(http.StatusOK)
				json.NewEncoder(w).Encode(map[string]interface{}{
					"message": "Database created",
				})
			}
		default:
			w.WriteHeader(http.StatusOK)
			json.NewEncoder(w).Encode(map[string]interface{}{
				"success": true,
			})
		}
	}))
	defer server.Close()

	// Create client with test server
	db := New(server.URL)

	// Test health check
	health, err := db.HealthCheck()
	if err != nil {
		t.Fatalf("HealthCheck failed: %v", err)
	}

	if health["status"] != "healthy" {
		t.Errorf("Expected status 'healthy', got %v", health["status"])
	}

	// Test list databases
	databases, err := db.ListDatabases()
	if err != nil {
		t.Fatalf("ListDatabases failed: %v", err)
	}

	if len(databases) != 1 || databases[0] != "testdb" {
		t.Errorf("Expected [testdb], got %v", databases)
	}

	// Test create database
	result, err := db.CreateDatabase("newdb")
	if err != nil {
		t.Fatalf("CreateDatabase failed: %v", err)
	}

	if result["message"] != "Database created" {
		t.Errorf("Expected message 'Database created', got %v", result["message"])
	}
}