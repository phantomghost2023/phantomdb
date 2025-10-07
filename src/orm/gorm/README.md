# PhantomDB GORM Integration

This package provides a GORM dialect that allows GORM applications to use PhantomDB as their database backend through its REST API.

## Overview

The PhantomDB GORM dialect enables GORM developers to leverage PhantomDB's advanced features while maintaining the familiar GORM interface. This dialect translates GORM operations into PhantomDB REST API calls.

## Features

- Full GORM compatibility
- Automatic schema translation
- Query translation from GORM to PhantomDB
- Transaction support
- Migration support

## Installation

```bash
go get github.com/phantomdb/phantomdb-gorm
```

## Configuration

To use PhantomDB with GORM, configure your database connection:

```go
import (
    "gorm.io/gorm"
    "phantomdb-gorm"
)

// Open database connection with PhantomDB
db, err := gorm.Open(phantomdb.Open("http://localhost:8080"), &gorm.Config{})
if err != nil {
    panic("failed to connect database")
}
```

## Usage

Once configured, you can use GORM as usual:

```go
type User struct {
    ID        uint   `gorm:"primaryKey"`
    FirstName string
    LastName  string
    Email     string `gorm:"unique"`
    Age       int
    Active    bool `gorm:"default:true"`
}

// Migrate the schema
db.AutoMigrate(&User{})

// Create
user := User{FirstName: "John", LastName: "Doe", Email: "john.doe@example.com", Age: 30}
result := db.Create(&user) // pass pointer of data to Create

// Read
var user User
db.First(&user, 1) // find user with integer primary key
db.First(&user, "email = ?", "john.doe@example.com") // find user with email

// Update
db.Model(&user).Where("active = ?", true).Update("Age", 35)
// Update multiple fields
db.Model(&user).Updates(User{Age: 35, Active: false}) // non-zero fields
db.Model(&user).Updates(map[string]interface{}{"Age": 35, "Active": false})

// Delete
db.Delete(&user, 1)
```

## Data Type Mapping

The dialect automatically maps GORM field types to PhantomDB types:

| GORM Type | PhantomDB Type |
|-----------|----------------|
| bool | BOOLEAN |
| int, int8, int16, int32, int64 | INTEGER, SMALLINT, BIGINT |
| uint, uint8, uint16, uint32, uint64 | INTEGER, SMALLINT, BIGINT |
| float32, float64 | FLOAT, DOUBLE |
| string | VARCHAR(n), TEXT |
| time.Time | TIMESTAMP |
| []byte | BLOB |

## Advanced Features

### Transactions

```go
func TransferMoney(db *gorm.DB, fromID, toID int, amount float64) error {
    return db.Transaction(func(tx *gorm.DB) error {
        // Find and update sender
        var sender Account
        if err := tx.First(&sender, fromID).Error; err != nil {
            return err
        }
        
        if sender.Balance < amount {
            return errors.New("insufficient balance")
        }
        
        tx.Model(&sender).Update("Balance", sender.Balance-amount)
        
        // Find and update receiver
        var receiver Account
        if err := tx.First(&receiver, toID).Error; err != nil {
            return err
        }
        
        tx.Model(&receiver).Update("Balance", receiver.Balance+amount)
        
        return nil
    })
}
```

### Preloading

```go
// Preload associations
var users []User
db.Preload("Orders").Preload("Profile").Find(&users)
```

### Raw SQL

```go
// Execute raw SQL
db.Raw("SELECT * FROM users WHERE age > ?", 25).Scan(&users)

// Raw SQL with Exec
db.Exec("UPDATE users SET active = ? WHERE last_login < ?", false, lastWeek)
```

## Requirements

- Go >= 1.19
- GORM >= 1.25.0
- phantomdb-go >= 1.0.0

## Development

To run tests:

```bash
go test ./...
```

## License

MIT License

## Contributing

Contributions are welcome! Please read our contributing guidelines before submitting pull requests.