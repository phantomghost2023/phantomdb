// Package phantomdb provides a Go client for interacting with the PhantomDB REST API.
package phantomdb

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"time"
)

// PhantomDBError represents a custom error for PhantomDB client operations.
type PhantomDBError struct {
	Message string
}

func (e *PhantomDBError) Error() string {
	return e.Message
}

// PhantomDB represents a client for interacting with PhantomDB through its REST API.
type PhantomDB struct {
	BaseURL    string
	HTTPClient *http.Client
}

// New creates a new PhantomDB client.
func New(baseURL string) *PhantomDB {
	if baseURL == "" {
		baseURL = "http://localhost:8080"
	}
	
	return &PhantomDB{
		BaseURL: baseURL,
		HTTPClient: &http.Client{
			Timeout: 30 * time.Second,
		},
	}
}

// makeRequest makes an HTTP request to the PhantomDB REST API.
func (p *PhantomDB) makeRequest(method, endpoint string, data interface{}) ([]byte, error) {
	url := p.BaseURL + endpoint
	
	var req *http.Request
	var err error
	
	if data != nil {
		jsonData, err := json.Marshal(data)
		if err != nil {
			return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to marshal data: %v", err)}
		}
		
		req, err = http.NewRequest(method, url, bytes.NewBuffer(jsonData))
		if err != nil {
			return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to create request: %v", err)}
		}
		req.Header.Set("Content-Type", "application/json")
	} else {
		req, err = http.NewRequest(method, url, nil)
		if err != nil {
			return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to create request: %v", err)}
		}
	}
	
	resp, err := p.HTTPClient.Do(req)
	if err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Request failed: %v", err)}
	}
	defer resp.Body.Close()
	
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to read response body: %v", err)}
	}
	
	if resp.StatusCode < 200 || resp.StatusCode >= 300 {
		return nil, &PhantomDBError{Message: fmt.Sprintf("HTTP error: %d %s", resp.StatusCode, resp.Status)}
	}
	
	return body, nil
}

// HealthCheck checks the health of the PhantomDB server.
func (p *PhantomDB) HealthCheck() (map[string]interface{}, error) {
	body, err := p.makeRequest("GET", "/health", nil)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// ListDatabases lists all databases in PhantomDB.
func (p *PhantomDB) ListDatabases() ([]string, error) {
	body, err := p.makeRequest("GET", "/databases", nil)
	if err != nil {
		return nil, err
	}
	
	var result struct {
		Databases []string `json:"databases"`
	}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result.Databases, nil
}

// CreateDatabase creates a new database.
func (p *PhantomDB) CreateDatabase(name string) (map[string]interface{}, error) {
	data := map[string]interface{}{
		"name": name,
	}
	
	body, err := p.makeRequest("POST", "/databases", data)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// DropDatabase drops a database.
func (p *PhantomDB) DropDatabase(name string) (map[string]interface{}, error) {
	body, err := p.makeRequest("DELETE", "/databases/"+name, nil)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// ListTables lists all tables in a database.
func (p *PhantomDB) ListTables(database string) ([]string, error) {
	body, err := p.makeRequest("GET", "/databases/"+database+"/tables", nil)
	if err != nil {
		return nil, err
	}
	
	var result struct {
		Tables []string `json:"tables"`
	}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result.Tables, nil
}

// Column represents a table column definition.
type Column struct {
	Name string `json:"name"`
	Type string `json:"type"`
}

// CreateTable creates a new table in a database.
func (p *PhantomDB) CreateTable(database, name string, columns []Column) (map[string]interface{}, error) {
	data := map[string]interface{}{
		"name":    name,
		"columns": columns,
	}
	
	body, err := p.makeRequest("POST", "/databases/"+database+"/tables", data)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// DropTable drops a table from a database.
func (p *PhantomDB) DropTable(database, name string) (map[string]interface{}, error) {
	body, err := p.makeRequest("DELETE", "/databases/"+database+"/tables/"+name, nil)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// Insert inserts data into a table.
func (p *PhantomDB) Insert(database, table string, data map[string]interface{}) (map[string]interface{}, error) {
	body, err := p.makeRequest("POST", "/databases/"+database+"/tables/"+table, data)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// Select selects data from a table.
func (p *PhantomDB) Select(database, table string, condition *string) ([]map[string]interface{}, error) {
	endpoint := "/databases/" + database + "/tables/" + table
	if condition != nil {
		endpoint += "?condition=" + *condition
	}
	
	body, err := p.makeRequest("GET", endpoint, nil)
	if err != nil {
		return nil, err
	}
	
	var result struct {
		Records []map[string]interface{} `json:"records"`
	}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result.Records, nil
}

// Update updates data in a table.
func (p *PhantomDB) Update(database, table string, data map[string]interface{}, condition *string) (map[string]interface{}, error) {
	payload := map[string]interface{}{
		"data": data,
	}
	
	if condition != nil {
		payload["condition"] = *condition
	}
	
	body, err := p.makeRequest("PUT", "/databases/"+database+"/tables/"+table, payload)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// Delete deletes data from a table.
func (p *PhantomDB) Delete(database, table string, condition *string) (map[string]interface{}, error) {
	endpoint := "/databases/" + database + "/tables/" + table
	if condition != nil {
		endpoint += "?condition=" + *condition
	}
	
	body, err := p.makeRequest("DELETE", endpoint, nil)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// ExecuteQuery executes a custom SQL query.
func (p *PhantomDB) ExecuteQuery(database, query string) (map[string]interface{}, error) {
	data := map[string]interface{}{
		"query": query,
	}
	
	body, err := p.makeRequest("POST", "/databases/"+database+"/query", data)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// BeginTransaction begins a new transaction.
func (p *PhantomDB) BeginTransaction() (string, error) {
	body, err := p.makeRequest("POST", "/transactions", nil)
	if err != nil {
		return "", err
	}
	
	var result struct {
		TransactionID string `json:"transaction_id"`
	}
	if err := json.Unmarshal(body, &result); err != nil {
		return "", &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result.TransactionID, nil
}

// CommitTransaction commits a transaction.
func (p *PhantomDB) CommitTransaction(transactionID string) (map[string]interface{}, error) {
	body, err := p.makeRequest("POST", "/transactions/"+transactionID+"/commit", nil)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}

// RollbackTransaction rolls back a transaction.
func (p *PhantomDB) RollbackTransaction(transactionID string) (map[string]interface{}, error) {
	body, err := p.makeRequest("POST", "/transactions/"+transactionID+"/rollback", nil)
	if err != nil {
		return nil, err
	}
	
	var result map[string]interface{}
	if err := json.Unmarshal(body, &result); err != nil {
		return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
	}
	
	return result, nil
}