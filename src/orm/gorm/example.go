// Package main demonstrates usage of PhantomDB GORM dialect
package main

import (
	"fmt"
	"log"
	
	"gorm.io/gorm"
	"phantomdb-gorm" // This would be the actual import path
)

// User represents a user entity
type User struct {
	ID        uint   `gorm:"primaryKey"`
	FirstName string
	LastName  string
	Email     string `gorm:"unique"`
	Age       int
	Active    bool `gorm:"default:true"`
	CreatedAt int64
}

// TableName specifies the table name
func (User) TableName() string {
	return "users"
}

// Profile represents a user profile
type Profile struct {
	ID     uint `gorm:"primaryKey"`
	UserID uint
	Bio    string
	Website string
}

// TableName specifies the table name
func (Profile) TableName() string {
	return "profiles"
}

func main() {
	fmt.Println("PhantomDB GORM Dialect Example")
	fmt.Println("=" * 40)
	fmt.Println("\nThis example shows how the integration would work with GORM.")
	
	fmt.Println("\nConfiguration:")
	fmt.Println(`
import (
    "gorm.io/gorm"
    "phantomdb-gorm"
)

// Open database connection with PhantomDB
db, err := gorm.Open(phantomdb.Open("http://localhost:8080"), &gorm.Config{})
if err != nil {
    panic("failed to connect database")
}
	`)
	
	fmt.Println("\nModel definition:")
	fmt.Println(`
type User struct {
    ID        uint   \`gorm:"primaryKey"\`
    FirstName string
    LastName  string
    Email     string \`gorm:"unique"\`
    Age       int
    Active    bool \`gorm:"default:true"\`
    CreatedAt int64
}

type Profile struct {
    ID     uint \`gorm:"primaryKey"\`
    UserID uint
    Bio    string
    Website string
}
	`)
	
	fmt.Println("\nUsage examples:")
	fmt.Println(`
// Migrate the schema
db.AutoMigrate(&User{}, &Profile{})

// Create
user := User{FirstName: "John", LastName: "Doe", Email: "john.doe@example.com", Age: 30}
result := db.Create(&user)

// Read
var user User
db.First(&user, 1)
db.First(&user, "email = ?", "john.doe@example.com")

// Update
db.Model(&user).Where("active = ?", true).Update("Age", 35)

// Delete
db.Delete(&user, 1)

// Transactions
err := db.Transaction(func(tx *gorm.DB) error {
    var sender Account
    if err := tx.First(&sender, fromID).Error; err != nil {
        return err
    }
    
    tx.Model(&sender).Update("Balance", sender.Balance-amount)
    
    var receiver Account
    if err := tx.First(&receiver, toID).Error; err != nil {
        return err
    }
    
    tx.Model(&receiver).Update("Balance", receiver.Balance+amount)
    
    return nil
})
	`)
	
	fmt.Println("\nExample completed successfully!")
	fmt.Println("\nNote: This is a conceptual example. To run it, you would need to:")
	fmt.Println("1. Install GORM and phantomdb-go")
	fmt.Println("2. Configure the PhantomDB GORM dialect")
	fmt.Println("3. Run the example with a PhantomDB server running")
}