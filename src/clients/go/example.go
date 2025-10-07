// Package main demonstrates usage of the PhantomDB Go client.
package main

import (
	"fmt"
	"log"

	"github.com/phantomdb/phantomdb-go"
)

func main() {
	// Connect to PhantomDB server
	db := phantomdb.New("http://localhost:8080")

	// Check server health
	fmt.Println("Checking server health...")
	health, err := db.HealthCheck()
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Server health: %+v\n", health)

	// Create a database
	fmt.Println("\nCreating database 'example_db'...")
	result, err := db.CreateDatabase("example_db")
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Create database result: %+v\n", result)

	// List databases
	fmt.Println("\nListing databases...")
	databases, err := db.ListDatabases()
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Databases: %+v\n", databases)

	// Create a table
	fmt.Println("\nCreating table 'users'...")
	columns := []phantomdb.Column{
		{Name: "id", Type: "int"},
		{Name: "name", Type: "string"},
		{Name: "email", Type: "string"},
	}

	result, err = db.CreateTable("example_db", "users", columns)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Create table result: %+v\n", result)

	// List tables
	fmt.Println("\nListing tables...")
	tables, err := db.ListTables("example_db")
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Tables: %+v\n", tables)

	// Insert data
	fmt.Println("\nInserting data...")
	userData := map[string]interface{}{
		"id":    1,
		"name":  "John Doe",
		"email": "john@example.com",
	}

	result, err = db.Insert("example_db", "users", userData)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Insert result: %+v\n", result)

	// Insert another record
	userData2 := map[string]interface{}{
		"id":    2,
		"name":  "Jane Smith",
		"email": "jane@example.com",
	}

	result, err = db.Insert("example_db", "users", userData2)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Insert result: %+v\n", result)

	// Query data
	fmt.Println("\nQuerying data...")
	users, err := db.Select("example_db", "users", nil)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Users: %+v\n", users)

	// Update data
	fmt.Println("\nUpdating data...")
	updateData := map[string]interface{}{
		"name": "Jane Doe",
	}

	condition := "id = 2"
	result, err = db.Update("example_db", "users", updateData, &condition)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Update result: %+v\n", result)

	// Query updated data
	fmt.Println("\nQuerying updated data...")
	users, err = db.Select("example_db", "users", nil)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Users: %+v\n", users)

	// Delete data
	fmt.Println("\nDeleting data...")
	result, err = db.Delete("example_db", "users", &condition)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Delete result: %+v\n", result)

	// Query remaining data
	fmt.Println("\nQuerying remaining data...")
	users, err = db.Select("example_db", "users", nil)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Users: %+v\n", users)

	// Demonstrate transactions
	fmt.Println("\nDemonstrating transactions...")
	txnID, err := db.BeginTransaction()
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Transaction ID: %s\n", txnID)

	// Insert data within transaction
	userData3 := map[string]interface{}{
		"id":    3,
		"name":  "Bob Johnson",
		"email": "bob@example.com",
	}

	_, err = db.Insert("example_db", "users", userData3)
	if err != nil {
		// Rollback on error
		fmt.Printf("Error occurred, rolling back: %s\n", err.Error())
		rollbackResult, rollbackErr := db.RollbackTransaction(txnID)
		if rollbackErr != nil {
			log.Fatal(rollbackErr)
		}
		fmt.Printf("Rollback result: %+v\n", rollbackResult)
		return
	}

	// Commit the transaction
	result, err = db.CommitTransaction(txnID)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Commit result: %+v\n", result)

	// Final query
	fmt.Println("\nFinal data query...")
	users, err = db.Select("example_db", "users", nil)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Users: %+v\n", users)

	// Drop table
	fmt.Println("\nDropping table 'users'...")
	result, err = db.DropTable("example_db", "users")
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Drop table result: %+v\n", result)

	// Drop database
	fmt.Println("\nDropping database 'example_db'...")
	result, err = db.DropDatabase("example_db")
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Drop database result: %+v\n", result)
}